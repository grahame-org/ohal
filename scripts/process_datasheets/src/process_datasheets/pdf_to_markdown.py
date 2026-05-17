# -*- coding: utf-8 -*-
"""Core PDF-to-Markdown conversion logic."""

from __future__ import annotations

import re
import unicodedata
from collections import Counter
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    try:
        import pymupdf
    except ImportError:
        import fitz as pymupdf  # type: ignore[no-redef]


def _import_pymupdf():
    """Import pymupdf (or the legacy fitz alias) and return the module."""
    try:
        import pymupdf as _mu
    except ImportError:
        import fitz as _mu  # type: ignore[no-redef]
    return _mu


# Mojibake substitutions for common CP1252/Latin-1 bytes mis-decoded as
# multi-byte encodings (Shift-JIS etc.).  Applied in order after line filtering.
_MOJIBAKE_REPLACEMENTS: tuple[tuple[str, str], ...] = (
    ("\uff66", ""),      # half-width katakana middle dot used as × artefact
    ("\uff65", ""),      # half-width katakana middle dot variant
    ("ﾂｮ", "®"),        # ® sign
    ("ﾂｻ", "»"),        # right double angle quotation
    ("ﾂｫ", "«"),        # left double angle quotation
    ("ﾎｼ", "μ"),        # μ (micro sign, CP1252 0xB5)
    ("窶冱", "'s"),      # possessive apostrophe
    ("窶冲", "'t"),      # contraction apostrophe
    ("窶堋ｫ", "«"),
    ("窶堋ｻ", "»"),
    ("窶", "\u2014"),    # em dash fallback
    ("ﾃ・", "×"),       # multiplication sign
)

# Pre-compiled patterns reused across strip_header_footer calls.
# The item body captures the first content line plus any wrapped continuation
# lines (non-blank lines that don't start a new list item), so that multi-line
# items like:
#   "1. Some long text\n   that wraps here.\n\n2. Next item"
# are collapsed into a tight list just like single-line items.
_ITEM_BODY = r".+(?:\n(?!\n| *- |\d+\. ).+)*"
_UL_COLLAPSE = re.compile(rf"(?m)^( *- {_ITEM_BODY})\n(?:\n)+( *- )")
_OL_COLLAPSE = re.compile(rf"(?m)^(\d+\. {_ITEM_BODY})\n(?:\n)+(\d+\. )")

# Simple whole-line patterns whose match means the line is a header/footer
# artefact and should be dropped.  Checked via fullmatch against stripped text.
_SKIP_LINE_PATTERNS: tuple[re.Pattern, ...] = (
    re.compile(r"(?:#{1,6}\s+)?RM0503\s+Rev\s+\d+"),
    re.compile(r"(?:#{1,6}\s+)?\d+/\d+"),
    re.compile(r"\d+"),
    re.compile(
        r"(?:January|February|March|April|May|June|July"
        r"|August|September|October|November|December)\s+\d{4}"
    ),
    re.compile(r"\*?www\.st\.com\*?"),
)


def parse_page_ranges(spec: str) -> list[int]:
    """Parse a page-range spec like '1,5-10,20' into a sorted list of 1-based page numbers."""
    pages: set[int] = set()
    for part in spec.split(","):
        part = part.strip()
        if not part:
            continue
        m = re.fullmatch(r"(\d+)-(\d+)", part)
        if m:
            start, end = int(m.group(1)), int(m.group(2))
            if start > end:
                raise ValueError(f"Invalid range '{part}': start > end")
            pages.update(range(start, end + 1))
        elif re.fullmatch(r"\d+", part):
            pages.add(int(part))
        else:
            raise ValueError(f"Cannot parse page spec '{part}'")
    return sorted(pages)


def is_monospace(font_name: str) -> bool:
    """Return True if the font name looks like a monospace / code font."""
    mono_hints = ("courier", "mono", "consolas", "inconsolata", "lucidaconsole", "cour")
    return any(h in font_name.lower() for h in mono_hints)


def heading_level(size: float, body_size: float, bold: bool) -> int | None:
    """Map a font size to a Markdown heading level (1-4), or None for body text.

    Thresholds are relative to the page's body size.
    """
    ratio = size / body_size if body_size else 1.0
    if ratio >= 1.8:
        return 1
    if ratio >= 1.4:
        return 2
    if ratio >= 1.15:
        return 3
    if ratio >= 1.0 and bold:
        return 4
    return None


# Width (in characters) at which the page number is right-aligned in ToC lines.
_TOC_WIDTH = 100

# Matches a ToC entry whose trailing content is a dot-leader followed by a page number.
# Group 1 – everything before the dots (the section title / heading prefix)
# Group 2 – the page number
_TOC_LINE_RE = re.compile(r"^(.+?)\s*[\s.]{2,}(\d+)\s*$")


def _align_toc_page_numbers(text: str) -> str:
    """Reformat ToC dot-leaders so every page number lands at column _TOC_WIDTH.

    Works on both top-level heading lines (e.g. ``#### **1** Title . . . 51``)
    and plain sub-section lines (e.g. ``- 2.5.3 Title.....63``).
    Lines that do not match the dot-leader pattern are left untouched.
    """
    out_lines: list[str] = []
    for line in text.splitlines():
        m = _TOC_LINE_RE.match(line)
        if m:
            title, page = m.group(1), m.group(2)
            # Use solid dots with a single space either side.
            # Minimum 3 dots so it still looks like a leader on very long titles.
            dots_needed = max(3, _TOC_WIDTH - len(title) - len(page) - 2)
            out_lines.append(f"{title} {'.' * dots_needed} {page}")
        else:
            out_lines.append(line)
    return "\n".join(out_lines)


def _filter_header_footer_lines(lines: list[str]) -> list[str]:
    """Remove running header / footer lines from a list of text lines.

    Handles:
    - Bold ``**RM0503**`` running header (bare and heading-prefixed), optionally
      preceded by a bold section name and followed by a bold section title.
    - ``RM0503 Rev N`` revision footer.
    - ``N/total`` page-count stamps.
    - Stray lone integers (chapter/section bookmark artefacts).
    - Stray single alpha characters (access-type label leakage from register tables).
    - Month YYYY date stamps on cover pages.
    - ``www.st.com`` URL (plain or italic).
    - Standalone bold section title that duplicates the heading immediately below it.
    - Garbled / replacement-character lines (mojibake math formula fragments).
    """
    filtered: list[str] = []
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # Running header: bold "RM0503" optionally preceded by a bold section name and
        # optionally followed on the next line by another bold section title.
        if re.fullmatch(r"(?:#{1,6}\s+)?\*\*RM0503\*\*", stripped):
            if filtered and re.fullmatch(r"(?:#{1,6}\s+)?\*\*[^*]+\*\*", filtered[-1].strip()):
                filtered.pop()
            if i + 1 < len(lines) and re.fullmatch(
                r"(?:#{1,6}\s+)?\*\*[^*]+\*\*", lines[i + 1].strip()
            ):
                i += 2
            else:
                i += 1
            continue

        # Stray single alpha character (e.g. 's' access-type label leaked from register tables).
        if len(stripped) == 1 and stripped.isalpha():
            i += 1
            continue

        # Footer lines matched by a fixed set of whole-line patterns.
        if any(p.fullmatch(stripped) for p in _SKIP_LINE_PATTERNS):
            i += 1
            continue

        # Standalone bold section/chapter title that duplicates a heading immediately below it.
        if re.fullmatch(r"\*\*[^*]+\*\*", stripped):
            j = i + 1
            while j < len(lines) and not lines[j].strip():
                j += 1
            if j < len(lines) and lines[j].strip().startswith("#"):
                i += 1
                continue

        # Garbled / replacement-character lines (e.g. mojibake math formula fragments).
        if stripped and all(
            ord(c) > 0xFF or unicodedata.category(c) == "So"
            for c in stripped
            if not c.isspace()
        ):
            i += 1
            continue

        filtered.append(line)
        i += 1

    return filtered


def _apply_mojibake_fixes(text: str) -> str:
    """Replace known mojibake sequences arising from mis-decoded PDF encodings."""
    for bad, good in _MOJIBAKE_REPLACEMENTS:
        text = text.replace(bad, good)
    return text


def _strip_heading_bold(text: str) -> str:
    """Remove all ``**`` bold markers from heading lines."""
    out = []
    for line in text.splitlines():
        if re.match(r"#{1,6} ", line):
            line = line.replace("**", "")
        out.append(line)
    return "\n".join(out)


def _collapse_list_items(text: str) -> str:
    """Collapse blank lines between consecutive list items to form tight lists."""
    for pat in (_UL_COLLAPSE, _OL_COLLAPSE):
        while True:
            text, n = pat.subn(r"\1\n\2", text)
            if n == 0:
                break
    return text


def _indent_list_continuations(text: str) -> str:
    """Indent wrapped continuation lines so they align under the item's content.

    After PDF extraction, a list item whose text was too long to fit on one PDF
    line appears as two (or more) consecutive lines with no blank between them::

        - Main memory block containing up to 128 pages of 2 Kbytes, each page
        with eight rows of 256 bytes

        1. Check that the lock bit is not set, or that the readout protection is
        at level 0.
        2. Write the key.

    CommonMark requires continuation lines to be indented by at least as many
    spaces as the item's content column:

    * ``- item``    → content starts at col 2 → indent continuation by 2
    * ``1. item``   → content starts at col 3 → indent continuation by 3
    * ``10. item``  → content starts at col 4 → indent continuation by 4
    * ``  - item``  → already-indented nested item → 2 + 2 = 4 spaces
    * ``   1. item``→ already-indented nested item → 3 + 3 = 6 spaces
    """
    # Matches leading spaces + marker; group 1 = leading spaces, group 2 = marker
    _LIST_START = re.compile(r"^( *)(-|\d+\.) ")
    lines = text.splitlines()
    out: list[str] = []
    cont_indent: int = 0   # spaces to prepend to continuation lines
    in_item = False
    for line in lines:
        m = _LIST_START.match(line)
        if m:
            leading = len(m.group(1))
            marker = m.group(2)          # "-" or "N."
            cont_indent = leading + len(marker) + 1   # +1 for the space after marker
            in_item = True
            out.append(line)
        elif in_item and line and not line[0].isspace():
            # Non-blank, non-indented continuation of the current list item.
            out.append(" " * cont_indent + line)
        else:
            if not line.strip():
                in_item = False
            out.append(line)
    return "\n".join(out)


def _apply_regex_postprocessing(text: str) -> str:
    """Apply structural regex fixes to merged page text after line filtering."""
    # Drop figure/diagram legend headings.  Two surface forms exist:
    #   "#### BOLD:label"        – span text already plain
    #   "## **BOLD:**label"      – bold markers still present
    # A single pattern handles both before bold-stripping is applied.
    text = re.sub(r"(?m)^#{1,6} (?:\*\*)?BOLD:.+\n?", "", text)

    # Strip bold/italic markers from heading lines before any structural joins so
    # that downstream patterns work on plain text.
    text = _strip_heading_bold(text)

    # Merge same-level heading continuation lines that start with "(" — e.g.:
    #   "### TIM1 DMA/interrupt enable register\n### (TIM1_DIER)"
    # Bold markers are already stripped above, so "**(...)**" is no longer an issue.
    text = re.sub(
        r"(?m)^(#{1,6} .+)\n(#{1,6}) (\(.+)",
        lambda m: m.group(1) + " " + m.group(3) if m.group(1).startswith(m.group(2)) else m.group(0),
        text,
    )

    # Join ToC bold-integer orphans: "#### **3**\n#### Title" -> "#### 3 Title"
    # (bold stripped above, but keep for any residual ** from other passes)
    text = re.sub(
        r"(?m)^(#{1,6}) \*\*(\d+)\*\*\n\n?\1 (.+)",
        r"\1 \2 \3",
        text,
    )

    # Demote headings that are really register bit-value enum entries.
    # The PDF renders "0: Disable", "01: Input mode", "0x1F: some value" in bold,
    # which causes heading_level() to assign them a heading prefix.  These are body
    # text and should carry no heading prefix.
    text = re.sub(
        r"(?m)^#{1,6} ([01x]+:|0x[0-9A-Fa-f]+:) ",
        r"\1 ",
        text,
    )

    # Demote headings that start with "Example:" — these are bold inline examples
    # in register descriptions, not structural headings.
    text = re.sub(r"(?m)^#{1,6} (Example:)", r"\1", text)

    # Demote headings that are cross-reference sentences starting with "Refer to".
    text = re.sub(r"(?m)^#{1,6} (Refer to )", r"\1", text)

    # Demote headings that are really body sentences promoted because the PDF
    # renders them in bold (e.g. long descriptive sentences, conditional clauses).
    # Real section headings never start with these common prose words.
    text = re.sub(
        r"(?m)^#{1,6} "
        r"((?:The |This |If |When |In |After |Before |For |A |An |Set |These |Each |Note[^:]))",
        r"\1",
        text,
    )

    # Demote headings that start with a lowercase letter — these are wrapped
    # continuation lines of a bold sentence that the PDF split across lines.
    # Genuine section headings always start with an uppercase letter or a digit.
    text = re.sub(r"(?m)^#{1,6} ([a-z])", r"\1", text)

    # Promote reserved-bit range lines to #### headings so they are rendered at
    # the same level as named bit-field headings.  In the PDF these lines are not
    # bold, so heading_level() leaves them as plain text, while neighbouring named
    # fields (which are bold) get a #### prefix.
    # Matches: "Bit 7 Reserved, ..."  and  "Bits 31:24 Reserved, ..."
    text = re.sub(
        r"(?m)^(Bits? \d+(?::\d+)? Reserved\b)",
        r"#### \1",
        text,
    )

    # Demote bare footnote-number headings joined with their footnote body.
    # The PDF renders both the footnote index ("1.") and its text in bold, so
    # heading_level() promotes each to a heading.  Join them and demote to plain
    # body text: "### 1.\n### TRGi are mapped…" → "1. TRGi are mapped…"
    text = re.sub(
        r"(?m)^#{1,6} (\d+\.)\n#{1,6} (.+)",
        r"\1 \2",
        text,
    )
    # Also demote any remaining lone footnote-number heading with no following body.
    text = re.sub(r"(?m)^#{1,6} (\d+\.)\s*$", r"\1", text)

    # Join section-heading number orphans after bold-stripping so the plain form
    # "### 1.1\n### Title" (previously "### **1.1**\n### **Title**") is matched.
    text = re.sub(
        r"(?m)^(#{1,6}) (\d+(?:\.\d+)*)\n\n?#{1,6} (.+)",
        r"\1 \2 \3",
        text,
    )

    # Join Table/Figure caption label split across two lines:
    # "Table 12.\nSome description . . . 45" -> "Table 12. Some description . . . 45"
    # "Figure 5.\nSome diagram caption"       -> "Figure 5. Some diagram caption"
    text = re.sub(
        r"(?m)^((?:Table|Figure) \d+\.)\n(.+)",
        r"\1 \2",
        text,
    )

    # Ensure a space before inline ** or * markers when they directly follow a word character
    text = re.sub(r"(\w)(\*+)(?=\w)", r"\1 \2", text)

    # Collapse double list markers: "- - item" -> "- item"
    text = re.sub(r"(?m)^- - ", "- ", text)

    # Join ToC sub-section number orphans: "1.1\nTitle" -> "1.1 Title"
    text = re.sub(
        r"(?m)^(\d+(?:\.\d+)+)\n(.+)",
        r"\1 \2",
        text,
    )

    # Join wrapped ToC titles whose continuation carries the dot-leader + page number
    text = re.sub(
        r"(?m)^(\d+(?:\.\d+)+ .+[^\d])\n([^\d#-].+)",
        r"\1 \2",
        text,
    )

    # Strip spurious bold markers wrapping a ToC dot-leader + page number
    text = re.sub(r"\*\*([\s.]+\d+)\*\*", r"\1", text)

    # Prefix ToC dotted sub-section lines with "- " to form a markdown list
    text = re.sub(r"(?m)^(\d+(?:\.\d+)+) ", r"- \1 ", text)

    # Right-justify page numbers in ToC lines
    text = _align_toc_page_numbers(text)

    # Join orphaned unordered list markers with next content line.
    # Use [ \t] (horizontal whitespace only) so the quantifiers never overlap
    # with \n, which would cause catastrophic backtracking on non-matching input.
    # The leading ( *) captures any nesting indent so it is preserved.
    text = re.sub(r"(?m)^( *)-[ \t]*\n(?:[ \t]*\n)*[ \t]*(.+)", r"\1- \2", text)

    # Join orphaned ordered list markers with next content line.
    # Limit to 1-3 digits so hex value tails like "0000." are not matched.
    text = re.sub(r"(?m)^(\d{1,3}\.)[ \t]*\n(?:[ \t]*\n)*[ \t]*(.+)", r"\1 \2", text)

    text = _collapse_list_items(text)
    text = _indent_list_continuations(text)

    return text


def strip_header_footer(text: str) -> str:
    """Remove running headers/footers and apply structural fixes to page text."""
    filtered = _filter_header_footer_lines(text.splitlines())
    result = "\n".join(filtered)
    result = _apply_mojibake_fixes(result)
    result = _apply_regex_postprocessing(result)
    return result.strip()


def _dominant_body_size(page: "pymupdf.Page") -> float:
    """Return the modal font size on the page, weighted by character count."""
    size_chars: Counter[int] = Counter()
    data = page.get_text("dict", sort=True)
    for block in data.get("blocks", []):
        if block.get("type") != 0:
            continue
        for line in block.get("lines", []):
            for span in line.get("spans", []):
                text = span.get("text", "").strip()
                if text:
                    size_chars[round(span["size"])] += len(text)
    if not size_chars:
        return 10.0
    return size_chars.most_common(1)[0][0]


def _normalise_cell(value: object, collapse_newlines: bool = True) -> str:
    """Normalise a single table cell value to a plain string."""
    text = _apply_mojibake_fixes(str(value)) if value is not None else ""
    text = unicodedata.normalize("NFKC", text)
    if collapse_newlines:
        text = text.replace("\n", " ")
    return text


def _table_rects(page: "pymupdf.Page") -> list[tuple[object, str]]:
    """Return (bbox, markdown_text) pairs for all tables found on the page."""
    results = []
    try:
        tabs = page.find_tables()
        for tab in tabs.tables:
            rows = tab.extract()
            if not rows:
                continue
            header = [_normalise_cell(c) for c in rows[0]]
            md_lines = [
                "| " + " | ".join(header) + " |",
                "| " + " | ".join("---" for _ in header) + " |",
            ]
            for row in rows[1:]:
                cells = [_normalise_cell(c) for c in row]
                md_lines.append("| " + " | ".join(cells) + " |")
            results.append((tab.bbox, "\n".join(md_lines)))
    except Exception:
        pass
    return results


def _normalise_span_text(raw: str) -> str:
    """Apply Unicode normalisation and replace PDF bullet/dash characters."""
    text = _apply_mojibake_fixes(raw)
    return (
        unicodedata.normalize("NFKC", text)
        .replace("\u2022", "-")  # bullet •
        .replace("\u2013", "-")  # en-dash –
    )


def _format_span(raw: str, flags: int, font: str) -> str:
    """Apply Markdown inline formatting to a span's text based on its font flags."""
    bold = bool(flags & 16)
    italic = bool(flags & 2)
    mono = is_monospace(font)
    text = raw
    if mono:
        text = f"`{text.strip()}`"
    elif bold and italic:
        text = f"***{text.strip()}***"
    elif bold:
        text = f"**{text.strip()}**"
    elif italic:
        text = f"*{text.strip()}*"
    return text


def _rect_overlaps_any(rect: object, rect_list: list) -> bool:
    """Return True if *rect* overlaps any rectangle in *rect_list*."""
    for tr in rect_list:
        if abs(rect & tr) > 0:
            return True
    return False


def _bullet_nesting_level(block: dict, min_bullet_x: float) -> int:
    """Return the nesting depth (0 = top-level) of a bullet block based on x-offset.

    The first line of a bullet block contains only the glyph (•, –, etc.).
    Subsequent lines contain the content, indented further.  We use the block's
    own x0 (which equals the glyph x-position) relative to the minimum bullet
    x-position seen on the page to infer the nesting level.  A difference of
    ~20 pt (typical PDF indent step) maps to one nesting level.
    """
    _BULLET_GLYPHS = {"\u2022", "\u2013", "\u2014", "\u25e6", "\u00b7"}
    lines = block.get("lines", [])
    if not lines:
        return 0
    first_text = "".join(s["text"] for s in lines[0].get("spans", [])).strip()
    if first_text not in _BULLET_GLYPHS:
        return 0
    x0 = block["bbox"][0]
    delta = x0 - min_bullet_x
    if delta < 5:
        return 0
    # Each nesting level is approximately 20 pt; use 10 pt as the threshold to
    # avoid rounding noise producing false levels.
    return max(1, round(delta / 20))


def _min_bullet_x(blocks: list[dict]) -> float:
    """Return the minimum x0 among all bullet-marker blocks on the page."""
    _BULLET_GLYPHS = {"\u2022", "\u2013", "\u2014", "\u25e6", "\u00b7"}
    xs: list[float] = []
    for block in blocks:
        if block.get("type") != 0:
            continue
        lines = block.get("lines", [])
        if not lines:
            continue
        first_text = "".join(s["text"] for s in lines[0].get("spans", [])).strip()
        if first_text in _BULLET_GLYPHS:
            xs.append(block["bbox"][0])
    return min(xs) if xs else 0.0


def page_to_markdown(page: "pymupdf.Page") -> str:
    """Convert a single PDF page to structured Markdown using font metadata and table detection."""
    _mu = _import_pymupdf()

    body_size = _dominant_body_size(page)

    table_entries = _table_rects(page)
    table_rect_list = [_mu.Rect(r) for r, _ in table_entries]

    data = page.get_text("dict", sort=True)
    all_blocks = data.get("blocks", [])
    min_bx = _min_bullet_x(all_blocks)
    output_items: list[tuple[float, str]] = []

    for block in all_blocks:
        if block.get("type") != 0:
            continue

        block_rect = _mu.Rect(block["bbox"])
        if _rect_overlaps_any(block_rect, table_rect_list):
            continue

        block_y0: float = block["bbox"][1]
        nesting = _bullet_nesting_level(block, min_bx)
        nest_prefix = "  " * nesting
        para_lines: list[str] = []
        is_first_line = True

        for line in block.get("lines", []):
            line_parts: list[str] = []
            line_is_heading: int | None = None

            for span in line.get("spans", []):
                raw = _normalise_span_text(span.get("text", ""))
                if not raw.strip():
                    line_parts.append(raw)
                    continue

                size = span["size"]
                flags = span.get("flags", 0)
                font = span.get("font", "")

                level = heading_level(size, body_size, bool(flags & 16))
                if level is not None and line_is_heading is None:
                    line_is_heading = level

                line_parts.append(_format_span(raw, flags, font))

            line_text = "".join(line_parts).strip()
            if not line_text:
                continue

            if line_is_heading is not None:
                prefix = "#" * line_is_heading
                para_lines.append(f"{prefix} {line_text}")
            else:
                # For nested bullet blocks, prefix only the first (glyph) line.
                # Continuation lines are left un-prefixed so that
                # _indent_list_continuations can align them correctly.
                if nest_prefix and is_first_line:
                    para_lines.append(nest_prefix + line_text)
                else:
                    para_lines.append(line_text)
            is_first_line = False

        if para_lines:
            output_items.append((block_y0, "\n".join(para_lines)))

    for bbox, md_table in table_entries:
        y0 = bbox[1] if isinstance(bbox, (list, tuple)) else bbox.y0
        output_items.append((y0, md_table))

    output_items.sort(key=lambda x: x[0])
    parts = [text for _, text in output_items]
    return strip_header_footer("\n\n".join(parts))
