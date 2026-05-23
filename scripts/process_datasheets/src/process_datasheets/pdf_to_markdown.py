# -*- coding: utf-8 -*-
"""Core PDF-to-Markdown conversion logic."""

from __future__ import annotations

import re
import unicodedata
from collections import Counter

try:
    import pymupdf
except ImportError:
    import fitz as pymupdf  # type: ignore[no-redef]


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

# Bullet/dash glyphs used as list markers in PDF output.
_BULLET_GLYPHS: frozenset[str] = frozenset({
    "\u2022",  # •  bullet
    "\u2013",  # –  en-dash
    "\u2014",  # —  em-dash
    "\u25e6",  # ◦  white bullet
    "\u00b7",  # ·  middle dot
})

# Matches the opening of a Markdown list item; used by _indent_list_continuations.
# Group 1 = leading spaces, group 2 = marker ("-" or "N.")
_LIST_ITEM_START = re.compile(r"^( *)(-|\d+\.) ")

# Bit-value enum lines: "0:", "1:", "00:", "0x1F:", etc.
# These are top-level body lines and must not be treated as list continuations.
_BIT_VALUE_LINE = re.compile(r"^[01x][0-9A-Fa-fx]*:")

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
    # ST figure version stamps, e.g. "MSv42192V1", "MS31444V5".  These are
    # artefacts of the drawing tool embedded in figure artwork.  The pattern
    # handles both "MS12345V1" and "MSv12345V1" (optional lowercase v).
    re.compile(r"[A-Z]{2,}v?\d+[Vv]\d+"),
)

# Running header: "**RM0503**" optionally preceded by a heading marker.
_RUNNING_HEADER_RE = re.compile(r"(?:#{1,6}\s+)?\*\*RM0503\*\*")
# Bold-only line: any text wrapped in **…** (optionally heading-prefixed).
_BOLD_SECTION_TITLE_RE = re.compile(r"(?:#{1,6}\s+)?\*\*[^*]+\*\*")
# Standalone bold title without heading prefix.
_STANDALONE_BOLD_RE = re.compile(r"\*\*[^*]+\*\*")
# Matches any Markdown heading line.
_HEADING_LINE_RE = re.compile(r"#{1,6} ")

_PAGE_RANGE_RE = re.compile(r"^(\d+)-(\d+)$")


def parse_page_ranges(spec: str) -> list[int]:
    """Parse a page-range spec like '1,5-10,20' into a sorted list of 1-based page numbers."""
    pages: set[int] = set()
    for part in spec.split(","):
        part = part.strip()
        if not part:
            continue
        m = _PAGE_RANGE_RE.match(part)
        if m:
            start, end = int(m.group(1)), int(m.group(2))
            if start > end:
                raise ValueError(f"Invalid range '{part}': start > end")
            pages.update(range(start, end + 1))
        elif part.isdecimal():
            pages.add(int(part))
        else:
            raise ValueError(f"Cannot parse page spec '{part}'")
    return sorted(pages)


_MONO_HINTS: tuple[str, ...] = ("courier", "mono", "consolas", "inconsolata", "lucidaconsole", "cour")


def is_monospace(font_name: str) -> bool:
    """Return True if the font name looks like a monospace / code font."""
    return any(h in font_name.lower() for h in _MONO_HINTS)


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
        if _RUNNING_HEADER_RE.fullmatch(stripped):
            if filtered and _BOLD_SECTION_TITLE_RE.fullmatch(filtered[-1].strip()):
                filtered.pop()
            if i + 1 < len(lines) and _BOLD_SECTION_TITLE_RE.fullmatch(
                lines[i + 1].strip()
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
        # Exclude figure/table captions (e.g. "**Figure 5. …**", "**Table 18. …**") which are
        # legitimate bold captions that should be kept even when a heading follows them.
        if _STANDALONE_BOLD_RE.fullmatch(stripped) and not re.match(
            r"\*\*(?:Figure|Table)\s+\d+[\.\:]", stripped
        ):
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


def _normalise_text(raw: str) -> str:
    """Apply mojibake fixes and Unicode NFKC normalisation to *raw*.

    This combines the two steps that are always applied together when
    cleaning text extracted directly from PDF spans or table cells.
    """
    return unicodedata.normalize("NFKC", _apply_mojibake_fixes(raw))


def _strip_heading_bold(text: str) -> str:
    """Remove all ``**`` bold markers from heading lines."""
    out = []
    for line in text.splitlines():
        if _HEADING_LINE_RE.match(line):
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
    lines = text.splitlines()
    out: list[str] = []
    cont_indent: int = 0   # spaces to prepend to continuation lines
    in_item = False
    for line in lines:
        m = _LIST_ITEM_START.match(line)
        if m:
            leading = len(m.group(1))
            marker = m.group(2)          # "-" or "N."
            cont_indent = leading + len(marker) + 1   # +1 for the space after marker
            in_item = True
            out.append(line)
        elif in_item and line and not line[0].isspace() and not _BIT_VALUE_LINE.match(line):
            # Non-blank, non-indented continuation of the current list item.
            # Bit-value enum lines (0:, 1:, 00:, 01: …) are excluded: they are
            # top-level body text that happens to follow a bullet list.
            out.append(" " * cont_indent + line)
        else:
            if not line.strip():
                in_item = False
            out.append(line)
    return "\n".join(out)


def _merge_paren_heading(m: re.Match) -> str:  # type: ignore[type-arg]
    """Merge a same-level heading continuation that starts with '(' into the preceding line.

    E.g. ``### TIM1 DMA/interrupt enable register\\n### (TIM1_DIER)``
    becomes ``### TIM1 DMA/interrupt enable register (TIM1_DIER)``.
    Only merges when both lines share the same heading level prefix.
    """
    if m.group(1).startswith(m.group(2)):
        return m.group(1) + " " + m.group(3)
    return m.group(0)


def _fix_headings(text: str) -> str:
    """Normalise heading levels and clean up mis-promoted / mis-demoted headings."""
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
    text = re.sub(
        r"(?m)^(#{1,6} .+)\n(#{1,6}) (\(.+)",
        _merge_paren_heading,
        text,
    )

    # Join ToC bold-integer orphans: "#### **3**\n#### Title" -> "#### 3 Title"
    text = re.sub(
        r"(?m)^(#{1,6}) \*\*(\d+)\*\*\n\n?\1 (.+)",
        r"\1 \2 \3",
        text,
    )

    # Demote headings that are really register bit-value enum entries.
    text = re.sub(r"(?m)^#{1,6} ([01x]+:|0x[0-9A-Fa-f]+:) ", r"\1 ", text)

    # Demote headings that start with "Example:" or "Refer to" — inline prose.
    text = re.sub(r"(?m)^#{1,6} (Example:)", r"\1", text)
    text = re.sub(r"(?m)^#{1,6} (Refer to )", r"\1", text)

    # Demote headings that are really body sentences (bold prose words).
    text = re.sub(
        r"(?m)^#{1,6} "
        r"((?:The |This |If |When |In |After |Before |For |A |An |Set |These |Each |Note[^:]))",
        r"\1",
        text,
    )

    # Demote headings starting with a lowercase letter (wrapped continuation lines).
    text = re.sub(r"(?m)^#{1,6} ([a-z])", r"\1", text)

    # Promote reserved-bit range lines to #### so they match named bit-field headings.
    text = re.sub(r"(?m)^(Bits? \d+(?::\d+)? Reserved\b)", r"#### \1", text)

    # Demote footnote-number headings and join with their body text.
    text = re.sub(r"(?m)^#{1,6} (\d+\.)\n#{1,6} (.+)", r"\1 \2", text)
    text = re.sub(r"(?m)^#{1,6} (\d+\.)\s*$", r"\1", text)

    # Join section-heading number orphans: "### 1.1\n### Title" -> "### 1.1 Title"
    text = re.sub(
        r"(?m)^(#{1,6}) (\d+(?:\.\d+)*)\n\n?#{1,6} (.+)",
        r"\1 \2 \3",
        text,
    )

    # Join Table/Figure caption split across two lines.
    text = re.sub(r"(?m)^((?:Table|Figure) \d+\.)\n(.+)", r"\1 \2", text)

    return text


def _fix_asterisk_artifacts(text: str) -> str:
    """Fix stray asterisk artefacts left by PDF extraction.

    Italic formatting is stripped entirely (see ``_format_span``), so this
    function only corrects artefacts unrelated to italic emphasis:

    1. Bold-adjacency fix – insert a space before ``**`` / ``*`` markers that
       directly follow a word character (e.g. ``TIMx*BDTR`` → ``TIMx *BDTR``).
    2. Bare ``*`` → ``_`` fix – converts the space-separated lone ``*`` from
       pass 1 back to an underscore for register names.
    3. Multiplication ``*`` escape – pre-empt Prettier's interpretation of
       ``0x004 * x`` as an italic-open delimiter.
    4. Table-cell trailing-underscore cleanup – remove spurious ``\\_`` / ``_``
       artefacts at the end of register-name cells.
    """
    # Ensure a space before inline ** or * markers when they directly follow a
    # word character (e.g. "TIMx*BDTR" → "TIMx *BDTR").
    text = re.sub(r"(\w)(\*+)(?=\w)", r"\1 \2", text)

    # Replace a bare * used as an underscore artefact within register/signal names.
    # The space-insertion pass above separates "TIMx*BDTR" into "TIMx *BDTR";
    # this pass then converts the lone " *" back to "_".
    text = re.sub(r"(?<=[A-Za-z0-9\]]) \*(?=[A-Za-z0-9\[])", "_", text)

    # Escape bare "*" used as a multiplication operator in address-offset
    # formulas (e.g. "0x004 * x").  Prettier interprets "space * letter" as an
    # italic-open delimiter and escapes it to "\*".  We pre-empt this by
    # escaping such occurrences ourselves so the raw output already matches
    # what Prettier would produce.
    text = re.sub(r"(0x[0-9A-Fa-f]+) \* ([a-zA-Z])", r"\1 \* \2", text)

    # Remove spurious escaped-underscore artifacts trailing register names in table
    # cells: "WRP1x END \_ \_" → "WRP1x END".
    text = re.sub(r"(?<=\w)( \\_)+(?=\s*(?:\||$))", "", text)

    # Remove spurious bare-underscore artifacts in table cells (pre-prettier form).
    text = re.sub(r"(?<=\w)( _)+(?=\s*(?:\||$))", "", text)

    return text


def _fix_toc(text: str) -> str:
    """Fix and normalise Table of Contents structure."""
    # Join ToC sub-section number orphans: "1.1\nTitle" → "1.1 Title"
    text = re.sub(r"(?m)^(\d+(?:\.\d+)+)\n(.+)", r"\1 \2", text)

    # Join wrapped ToC chapter headings whose title spills onto the next line.
    text = re.sub(
        r"(?m)^(#{1,6} \d.+[^.\d\s])\n([^#\-\n].+\.{3,}.+\d+\s*)$",
        r"\1 \2",
        text,
    )

    # Join wrapped ToC sub-section titles whose continuation carries dot-leader + page.
    text = re.sub(r"(?m)^(\d+(?:\.\d+)+ .+[^\d])\n([^\d#-].+)", r"\1 \2", text)

    # Strip spurious bold markers wrapping a ToC dot-leader + page number.
    text = re.sub(r"\*\*([\s.]+\d+)\*\*", r"\1", text)

    # Prefix ToC dotted sub-section lines with "- " to form a Markdown list.
    text = re.sub(r"(?m)^(\d+(?:\.\d+)+) ", r"- \1 ", text)

    # Right-justify page numbers in ToC lines.
    text = _align_toc_page_numbers(text)

    # Normalise ToC chapter-level headings from #### to ###.
    text = re.sub(r"(?m)^#### (\d.+ \.{3,} \d+\s*)$", r"### \1", text)

    return text


def _collapse_spaces_outside_tables(text: str) -> str:
    """Collapse multiple consecutive spaces within non-table, non-code lines.

    Preserves leading indentation and skips table rows (``|``-prefixed) and
    fenced code blocks so that intentional column padding is not destroyed.
    """
    result_lines: list[str] = []
    in_fence = False
    for line in text.splitlines():
        if line.startswith("```"):
            in_fence = not in_fence
        if not in_fence and not line.startswith("|"):
            # Preserve leading whitespace; collapse only internal runs.
            stripped = line.lstrip()
            indent = line[: len(line) - len(stripped)]
            stripped = re.sub(r"  +", " ", stripped)
            line = indent + stripped
        result_lines.append(line)
    return "\n".join(result_lines)


def _fix_lists(text: str) -> str:
    """Fix list structure: collapse markers, join orphans, indent continuations."""
    # Collapse double list markers: "- - item" → "- item"
    text = re.sub(r"(?m)^- - ", "- ", text)

    # Join orphaned unordered list markers with next content line.
    # Use [ \t] (horizontal whitespace only) to avoid catastrophic backtracking.
    text = re.sub(r"(?m)^( *)-[ \t]*\n(?:[ \t]*\n)*[ \t]*(.+)", r"\1- \2", text)

    # Join orphaned ordered list markers with next content line.
    # Limit to 1-3 digits so hex tails like "0000." are not matched.
    text = re.sub(r"(?m)^(\d{1,3}\.)[ \t]*\n(?:[ \t]*\n)*[ \t]*(.+)", r"\1 \2", text)

    text = _collapse_list_items(text)
    text = _indent_list_continuations(text)

    # Ensure a blank line after each heading when it is immediately followed by
    # non-blank, non-heading content.  This happens when the heading span and
    # the following paragraph are in the same PDF text block and are therefore
    # joined with only a single newline in output_items.
    text = re.sub(r"(?m)^(#{1,6} .+)\n(?!\n|#{1,6} )", r"\1\n\n", text)

    # Ensure a blank line before each heading when it is immediately preceded by
    # non-blank content (same-block extraction artefact, symmetric to the above).
    text = re.sub(r"(?m)(?<!\n)\n(#{1,6} )", r"\n\n\1", text)

    # Ensure a blank line before bold Note paragraphs (*Note:) that directly
    # follow body text without one.  Without this separation, prettier may
    # misinterpret identifier underscores (e.g. USART_CR1) in preceding lines.
    text = re.sub(r"(?m)(?<!\n)\n(\*Note:)", r"\n\n\1", text)

    # Collapse 3+ consecutive blank lines to a single blank line.
    # never produces more than one blank line between blocks, so any run of
    # two or more empty lines in the output is an extraction artefact.
    text = re.sub(r"\n{3,}", "\n\n", text)

    text = _collapse_spaces_outside_tables(text)

    return text


def _apply_regex_postprocessing(text: str) -> str:
    """Apply structural regex fixes to merged page text after line filtering.

    Delegates to four focused helpers applied in order:
    1. ``_fix_headings``          – heading level normalisation and join/demote passes
    2. ``_fix_asterisk_artifacts`` – asterisk artefacts from PDF bold-span extraction
    3. ``_fix_toc``               – Table of Contents structure
    4. ``_fix_lists``       – list marker joining and continuation indentation
    """
    text = _fix_headings(text)
    text = _fix_asterisk_artifacts(text)
    text = _fix_toc(text)
    text = _fix_lists(text)
    return text


def strip_header_footer(text: str) -> str:
    """Remove running headers/footers and apply structural fixes to page text."""
    filtered = _filter_header_footer_lines(text.splitlines())
    result = "\n".join(filtered)
    result = _apply_mojibake_fixes(result)
    result = _apply_regex_postprocessing(result)
    # Replace any tab characters with spaces so the output is consistently
    # space-indented regardless of what the PDF extractor emits.
    result = result.expandtabs(4)
    return result.strip()


def _dominant_body_size(blocks: list[dict]) -> float:
    """Return the modal font size among *blocks*, weighted by character count."""
    size_chars: Counter[int] = Counter()
    for block in blocks:
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


# Module-level helpers used by _join_register_cell_lines.
_LONE_UNDERSCORE_RE = re.compile(r"^[_ ]+$")
_IDENT_FRAGMENT_RE = re.compile(r"^[A-Z0-9\[\]:.]+$")
_OPERATOR_CHAR_RE = re.compile(r"[=<>+\-*/]")


def _spaces_to_underscores(s: str) -> tuple[str, int]:
    """Replace spaces with ``_`` unless either neighbour is an operator character.

    Returns the converted string and the number of replacements made.
    """
    result = list(s)
    count = 0
    for i, ch in enumerate(s):
        if ch == " " and i > 0 and i < len(s) - 1:
            if not _OPERATOR_CHAR_RE.fullmatch(s[i - 1]) and not _OPERATOR_CHAR_RE.fullmatch(s[i + 1]):
                result[i] = "_"
                count += 1
    return "".join(result), count


def _join_register_cell_lines(text: str) -> str:
    """Reassemble a multi-line register-header cell into a single identifier.

    pymupdf's table extractor renders vertically-stacked register field names
    as multiple text lines, with underscores either appearing on their own
    separator lines or being stripped from embedded spans and placed on adjacent
    lines.  The observed patterns and their expected outputs are::

        "NRST\\n_\\nSHDW"                            -> "NRST_SHDW"
        "NRST MODE\\n_\\n[1:0]"                       -> "NRST_MODE[1:0]"
        "N\\nBOOT\\n0"                                -> "NBOOT0"
        "NBOOT\\nSEL\\n_"                             -> "NBOOT_SEL"
        "BKPSRAM\\nHW\\n_ _\\nERASE\\n_\\nDISABLE"   -> "BKPSRAM_HW_ERASE_DISABLE"
        "BOR LEV[1:0]\\n_"                            -> "BOR_LEV[1:0]"

    The algorithm:

    1. Split on ``\\n`` and strip each line.
    2. Classify lines as word segments or lone-underscore separator lines
       (lines whose stripped content consists entirely of ``_`` and spaces).
    3. Convert internal spaces within word segments to ``_`` (the extractor
       splits ``NRST_MODE`` into ``NRST MODE`` plus a lone ``_`` separator line;
       the space marks the original underscore position).  Track how many
       spaces were converted (``spaces_used``).
    4. Count the total number of ``_`` chars across all separator lines
       (``sep_chars``).  The underscores available to join word segments are
       ``sep_chars - spaces_used``.
    5. When the available-separator count exactly equals the number of gaps
       between word segments (``len(words) - 1``), join every pair with ``_``.
       Otherwise the segments are concatenated directly.

    If no lone-underscore line is present but every line looks like an
    all-uppercase register-identifier fragment (letters, digits, brackets),
    the lines are concatenated directly (handles ``N\\nBOOT\\n0`` → ``NBOOT0``).
    """
    raw_lines = [ln.strip() for ln in text.split("\n")]
    lines = [ln for ln in raw_lines if ln]
    if len(lines) <= 1:
        return text

    has_separator = any(_LONE_UNDERSCORE_RE.fullmatch(ln) for ln in lines)

    if not has_separator:
        # No separator lines: concatenate directly only when every fragment
        # looks like part of an all-caps identifier.
        if all(_IDENT_FRAGMENT_RE.fullmatch(ln) for ln in lines):
            return "".join(lines)
        return text

    # Separate word segments from separator lines; convert internal spaces to _
    # in word segments (each space is an underscore extracted by the table parser).
    # Only convert a space if neither the preceding nor the following character is
    # an operator (=, <, >, +, -, *, /) or a space.  This preserves the space
    # around comparison operators in cells like "WRP1x STRT = WRP1x END"
    # (which should become "WRP1x_STRT = WRP1x_END", not "WRP1x_STRT_=_WRP1x_END").

    words: list[str] = []
    sep_count = 0   # total underscore separators available
    spaces_used = 0
    for ln in lines:
        if _LONE_UNDERSCORE_RE.fullmatch(ln):
            # Count the number of "_" characters: a line like "_ _" represents
            # two underscore separators (one between the previous segment and the
            # next, plus one trailing/leading from the adjacent span).
            sep_count += ln.count("_")
        else:
            converted, n_spaces = _spaces_to_underscores(ln)
            spaces_used += n_spaces
            words.append(converted)

    if not words:
        return text
    if len(words) == 1:
        # Only one word segment; all separator _s were either internal
        # (converted from spaces) or border artefacts.
        return words[0]

    # Available underscores for joining word segments.
    # sep_count counts raw "_" chars in separator lines; spaces_used accounts
    # for underscores already embedded within word segments (from internal spaces).
    available = sep_count - spaces_used
    gaps = len(words) - 1
    if available == gaps:
        return "_".join(words)
    if available > 0:
        # available < gaps: some inter-word gaps have a separator line between
        # them (use "_") and some do not (concatenate directly).
        # Example: "VC\n_\nHARDE\nRR" -> VC gets "_" before HARDE (sep line),
        # then HARDE and RR are concatenated directly -> "VC_HARDERR".
        # Replay the original line sequence, consuming the sep budget only
        # when a lone-underscore line directly precedes the next word.
        result_tokens: list[str] = []
        word_iter = iter(words)
        prev_was_sep = False
        sep_budget = available
        for ln in lines:
            if _LONE_UNDERSCORE_RE.fullmatch(ln):
                prev_was_sep = True
            else:
                word = next(word_iter, None)
                if word is None:
                    break
                if result_tokens and prev_was_sep and sep_budget > 0:
                    result_tokens.append("_")
                    sep_budget -= 1
                result_tokens.append(word)
                prev_was_sep = False
        return "".join(result_tokens)
    # Fallback: concatenate (shouldn't occur in practice for well-formed cells).
    return "".join(words)


def _normalise_cell(value: object, collapse_newlines: bool = True) -> str:
    """Normalise a single table cell value to a plain string."""
    text = _normalise_text(str(value)) if value is not None else ""
    if collapse_newlines:
        # Attempt intelligent reassembly of vertically-stacked register field
        # names before falling back to simple space-joining.
        text = _join_register_cell_lines(text)
        # If the cell still contains newlines (e.g. ordinary multi-line prose
        # not handled by the register-name heuristic), collapse them to spaces.
        text = text.replace("\n", " ")
    # Strip trailing standalone-underscore PDF artefacts.  pymupdf sometimes
    # extracts a register-map cell as e.g. "REV ID\n_" or "FLASH SIZE\n_" where
    # the lone "_" on its own line is a table-border glyph, not part of the name.
    # After newline collapsing this becomes "REV ID _" / "FLASH SIZE _".  Remove
    # any trailing " _" (space + lone underscore) that is not an embedded
    # identifier underscore (i.e. the _ has a space before it).
    text = re.sub(r"(?<=\w)( _)+$", "", text)
    # Escape underscore characters that are not embedded between two word characters
    # (i.e. not identifier underscores like FLASH_ITF).  Prettier escapes standalone _
    # in table cells because they can act as italic/emphasis delimiters in Markdown.
    # A negative lookbehind (?<!\w) or negative lookahead (?!\w) identifies non-embedded _.
    text = re.sub(r"(?<!\w)_|_(?!\w)", r"\\_", text)
    return text


def _fmt_table_row(cells: list[str], widths: list[int]) -> str:
    """Render one GFM table row, padding each cell to its column width."""
    return "| " + " | ".join(cell.ljust(w) for cell, w in zip(cells, widths)) + " |"


def _format_md_table(rows: list[list[str]]) -> str:
    """Format a list of rows (header first) as a Prettier-aligned GFM table.

    Prettier pads every cell with trailing spaces so that all cells in the same
    column share the same rendered width, and stretches the separator row with
    dashes to match.  Minimum column width is 3 (the length of ``---``).
    """
    if not rows:
        return ""
    col_count = max(len(r) for r in rows)
    # Pad every row to the same number of columns.
    padded = [r + [""] * (col_count - len(r)) for r in rows]
    # Compute the display width of each column.
    widths = [
        max(3, max(len(padded[r][c]) for r in range(len(padded))))
        for c in range(col_count)
    ]
    lines = [_fmt_table_row(padded[0], widths)]
    lines.append("| " + " | ".join("-" * w for w in widths) + " |")
    for row in padded[1:]:
        lines.append(_fmt_table_row(row, widths))
    return "\n".join(lines)


def _table_rects(page: "pymupdf.Page") -> list[tuple[object, str]]:
    """Return (bbox, markdown_text) pairs for all tables found on the page."""
    results = []
    try:
        tabs = page.find_tables()
        for tab in tabs.tables:
            rows = tab.extract()
            if not rows:
                continue
            str_rows = [[_normalise_cell(c) for c in row] for row in rows]
            results.append((tab.bbox, _format_md_table(str_rows)))
    except Exception:
        # Table extraction is best-effort; failures are intentionally ignored
        # so that page conversion can continue without the table content.
        pass
    return results


def _normalise_span_text(raw: str) -> str:
    """Apply Unicode normalisation and replace PDF bullet/dash characters."""
    return (
        _normalise_text(raw)
        .replace("\u2022", "-")  # bullet •
        .replace("\u2013", "-")  # en-dash –
    )


def _format_span(raw: str, flags: int, font: str) -> str:
    """Apply Markdown inline formatting to a span's text based on its font flags.

    Italic spans are emitted as plain text (italic formatting is stripped to
    avoid underscore/asterisk collisions with register identifiers).  Bold and
    monospace spans are marked up as ``**text**`` and `` `text` `` respectively.

    Leading and trailing whitespace is preserved *outside* the emphasis/code
    markers so that the natural inter-span spacing from the PDF is not lost.
    """
    bold = bool(flags & 16)
    mono = is_monospace(font)
    content = raw.strip()
    lead = raw[: len(raw) - len(raw.lstrip())]
    trail = raw[len(raw.rstrip()):]
    if not content:
        return raw
    if mono:
        marked = f"`{content}`"
    elif bold:
        marked = f"**{content}**"
    else:
        return raw
    return lead + marked + trail


def _rect_overlaps_any(rect: object, rect_list: list) -> bool:
    """Return True if *rect* overlaps any rectangle in *rect_list*."""
    return any(abs(rect & tr) > 0 for tr in rect_list)


def _first_line_text(block: dict) -> str:
    """Return the stripped text of the first line of a text block."""
    lines = block.get("lines", [])
    if not lines:
        return ""
    return "".join(s["text"] for s in lines[0].get("spans", [])).strip()


def _bullet_nesting_level(block: dict, min_bullet_x: float) -> int:
    """Return the nesting depth (0 = top-level) of a bullet block based on x-offset.

    The first line of a bullet block contains only the glyph (•, –, etc.).
    Subsequent lines contain the content, indented further.  We use the block's
    own x0 (which equals the glyph x-position) relative to the minimum bullet
    x-position seen on the page to infer the nesting level.  A difference of
    ~20 pt (typical PDF indent step) maps to one nesting level.
    """
    lines = block.get("lines", [])
    if not lines:
        return 0
    first_text = _first_line_text(block)
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
    xs: list[float] = []
    for block in blocks:
        if block.get("type") != 0:
            continue
        if not block.get("lines"):
            continue
        first_text = _first_line_text(block)
        if first_text in _BULLET_GLYPHS:
            xs.append(block["bbox"][0])
    return min(xs) if xs else 0.0


# Matches the version stamp that ST's drawing tool embeds at the bottom of each
# figure, e.g. "MSv42192V1" or "MS31444V5".  Optional lowercase v between the
# letter prefix and the numeric ID.
_FIGURE_STAMP_RE = re.compile(r"^[A-Z]{2,}v?\d+[Vv]\d+$")

# Matches a bold figure caption line: "Figure N." optionally followed by a title.
_FIGURE_CAPTION_RE = re.compile(r"^Figure\s+\d+[\.\:]")


def _collect_block_spans(block: dict) -> tuple[str, bool]:
    """Return (block_text, all_bold) for the text spans in *block*.

    ``block_text`` is the concatenation of all span texts, stripped.
    ``all_bold`` is True when every non-empty span in the block has the bold flag set.
    Returns ("", False) for blocks with no spans.
    """
    lines = block.get("lines", [])
    spans = [s for ln in lines for s in ln.get("spans", [])]
    if not spans:
        return ("", False)
    block_text = "".join(s.get("text", "") for s in spans).strip()
    all_bold = all(bool(s.get("flags", 0) & 16) for s in spans if s.get("text", "").strip())
    return (block_text, all_bold)


def _figure_body_rects(blocks: list[dict]) -> list[tuple[float, float]]:
    """Return (y_start, y_end) exclusion bands for figure-body content on a page.

    The strategy mirrors ST's PDF figure structure: every embedded figure has a
    bold caption block (``Figure N. title``) whose bottom edge marks the start of
    the figure body, and a version stamp block (``MSv…V…``) whose bottom edge
    marks the end.  Any text block whose y0 lies strictly inside such a band is
    figure-body artwork text and must be excluded from the Markdown output.

    The stamp block itself is also included in the exclusion band (its y0 is >=
    the caption y1 and <= stamp y1) so it is suppressed along with the rest.
    """
    # First pass: collect caption y1 values and stamp positions, in y order.
    # Blocks arrive sorted by y0 from get_text("dict", sort=True).
    pending_caption_y1: float | None = None
    exclusion_bands: list[tuple[float, float]] = []

    for block in blocks:
        if block.get("type") != 0:
            continue
        y0: float = block["bbox"][1]
        y1: float = block["bbox"][3]
        if not block.get("lines"):
            continue

        block_text, all_bold = _collect_block_spans(block)

        # Detect a figure caption: bold block whose text starts with "Figure N."
        if all_bold and _FIGURE_CAPTION_RE.match(block_text):
            pending_caption_y1 = y1
            continue

        # Detect the closing stamp for the most recently seen caption.
        if pending_caption_y1 is not None and _FIGURE_STAMP_RE.match(block_text):
            exclusion_bands.append((pending_caption_y1, y1))
            pending_caption_y1 = None

    return exclusion_bands


def _in_figure_band(y: float, exclusions: list[tuple[float, float]]) -> bool:
    """Return True if *y* falls within any of the figure-body exclusion bands."""
    return any(y_start <= y <= y_end for y_start, y_end in exclusions)


def _render_text_block(block: dict, body_size: float, min_bx: float) -> str | None:
    """Convert one PDF text block to a Markdown string, or None if it is empty.

    Handles heading detection, bullet nesting, and span-level inline formatting.
    Returns None when the block produces no visible output lines.
    """
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

    return "\n".join(para_lines) if para_lines else None


def page_to_markdown(page: "pymupdf.Page") -> str:
    """Convert a single PDF page to structured Markdown using font metadata and table detection."""
    table_entries = _table_rects(page)
    table_rect_list = [pymupdf.Rect(r) for r, _ in table_entries]

    data = page.get_text("dict", sort=True)
    all_blocks = data.get("blocks", [])
    # Compute the dominant body size from blocks that do not overlap any
    # detected table rectangle.  Register-map-heavy pages have far more
    # characters in the tiny (7 pt) bit-field table cells than in the 9 pt
    # body prose; including those cells skews the modal size downward and
    # causes "Address offset:" / "Reset value:" lines (10 pt) and section
    # headings (9 pt) to be incorrectly promoted to higher heading levels.
    non_table_blocks = [
        b for b in all_blocks
        if not _rect_overlaps_any(pymupdf.Rect(b["bbox"]), table_rect_list)
    ]
    body_size = _dominant_body_size(non_table_blocks) or _dominant_body_size(all_blocks)
    min_bx = _min_bullet_x(all_blocks)
    figure_exclusions = _figure_body_rects(all_blocks)
    output_items: list[tuple[float, str]] = []

    for block in all_blocks:
        if block.get("type") != 0:
            continue

        block_rect = pymupdf.Rect(block["bbox"])
        if _rect_overlaps_any(block_rect, table_rect_list):
            continue

        block_y0: float = block["bbox"][1]
        if _in_figure_band(block_y0, figure_exclusions):
            continue

        text = _render_text_block(block, body_size, min_bx)
        if text is not None:
            output_items.append((block_y0, text))

    for bbox, md_table in table_entries:
        y0 = bbox[1] if isinstance(bbox, (list, tuple)) else bbox.y0
        if _in_figure_band(y0, figure_exclusions):
            continue
        output_items.append((y0, md_table))

    output_items.sort(key=lambda x: x[0])
    parts = [text for _, text in output_items]
    return strip_header_footer("\n\n".join(parts))
