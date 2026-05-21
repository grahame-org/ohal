#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Extract selected pages from rm0503-stm32u0-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
and write each page as a Markdown file under claude/rm0503/.

Usage:
    python extract_pdf_pages.py <pdf_path> <page_ranges>

    <page_ranges> is a comma-separated list of individual pages or inclusive ranges, e.g.:
        42,43,44
        42-44
        1,5-10,20,30-35

    Pages are 1-based (matching the physical PDF page numbers).

Output:
    claude/rm0503/page_<NNNN>.md  for each extracted page.

Example:
    python extract_pdf_pages.py \
        rm0503-stm32u0-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf \
        42-44,100,200-205
"""

import argparse
import io
import re
import sys
import unicodedata
from pathlib import Path

# Ensure stdout/stderr use UTF-8 on Windows where the default console
# codepage is often CP1252 or CP932, which cannot represent all Unicode
# characters that may appear in PDF text.
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
if hasattr(sys.stderr, "reconfigure"):
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

try:
    import pymupdf  # PyMuPDF >= 1.24 uses "pymupdf" as the top-level package
except ImportError:
    import fitz as pymupdf  # older installs expose it as "fitz"


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


def _is_monospace(font_name: str) -> bool:
    """Return True if the font name looks like a monospace / code font."""
    mono_hints = ("courier", "mono", "consolas", "inconsolata", "lucidaconsole", "cour")
    name = font_name.lower()
    return any(h in name for h in mono_hints)


def _dominant_body_size(page: "pymupdf.Page") -> float:
    """Return the most common (modal) font size on the page — used as the body baseline."""
    from collections import Counter

    size_chars: Counter = Counter()
    data = page.get_text("dict", sort=True)
    for block in data.get("blocks", []):
        if block.get("type") != 0:
            continue
        for line in block.get("lines", []):
            for span in line.get("spans", []):
                text = span.get("text", "").strip()
                if text:
                    size = round(span["size"])
                    size_chars[size] += len(text)
    if not size_chars:
        return 10.0
    return size_chars.most_common(1)[0][0]


def _heading_level(size: float, body_size: float, bold: bool) -> int | None:
    """
    Map a font size to a Markdown heading level (1-4), or None for body text.
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


def _table_rects(page: "pymupdf.Page") -> list[tuple["pymupdf.Rect", str]]:
    """Return (rect, markdown_text) pairs for all tables found on the page."""
    results = []
    try:
        tabs = page.find_tables()
        for tab in tabs.tables:
            md_lines = []
            rows = tab.extract()
            if not rows:
                continue
            # Header row
            header = [unicodedata.normalize("NFKC", str(c)) if c is not None else "" for c in rows[0]]
            md_lines.append("| " + " | ".join(header) + " |")
            md_lines.append("| " + " | ".join("---" for _ in header) + " |")
            for row in rows[1:]:
                cells = [unicodedata.normalize("NFKC", str(c)).replace("\n", " ") if c is not None else "" for c in row]
                md_lines.append("| " + " | ".join(cells) + " |")
            results.append((tab.bbox, "\n".join(md_lines)))
    except Exception:
        pass
    return results


def _strip_header_footer(text: str) -> str:
    """Remove the running header and footer that ST prints on every RM0503 page.

    Header pattern (first block on most pages):
        **RM0503**
        **<Section title>**

    Footer patterns (last few lines):
        RM0503 Rev <N>
        <page>/<total>      e.g. "42/1333"
        <lone integer>      stray section/chapter number artefact
    """
    lines = text.splitlines()
    filtered: list[str] = []
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # Running header: bold "RM0503" optionally followed on the next line by a bold section
        # title. Matches bare '**RM0503**' and heading-prefixed forms like '#### **RM0503**'.
        if re.fullmatch(r"(?:#{1,6}\s+)?\*\*RM0503\*\*", stripped):
            # Consume this line and the next if it is also a bold-only (optionally headed) line.
            if i + 1 < len(lines) and re.fullmatch(r"(?:#{1,6}\s+)?\*\*[^*]+\*\*", lines[i + 1].strip()):
                i += 2
            else:
                i += 1
            continue

        # Footer: "RM0503 Rev N"
        if re.fullmatch(r"RM0503\s+Rev\s+\d+", stripped):
            i += 1
            continue

        # Footer: page-count stamp  e.g. "42/1333"
        if re.fullmatch(r"\d+/\d+", stripped):
            i += 1
            continue

        # Stray lone integer artefact (chapter/section bookmark number from PDF)
        if re.fullmatch(r"\d+", stripped):
            i += 1
            continue

        # Stray single non-space character artefact (e.g. 's' access-type label from register
        # tables). Exclude '-' (orphaned list marker, handled by the join pass below) and digits.
        if len(stripped) == 1 and stripped.isalpha():
            i += 1
            continue

        # Footer: date stamp e.g. "July 2025", "January 2024"
        if re.fullmatch(r"(?:January|February|March|April|May|June|July|August|September|October|November|December)\s+\d{4}", stripped):
            i += 1
            continue

        # Footer: ST website URL (plain or italic markdown)
        if re.fullmatch(r"\*?www\.st\.com\*?", stripped):
            i += 1
            continue

        filtered.append(line)
        i += 1

    # Trim leading/trailing blank lines that remain after removal.
    result = "\n".join(filtered)

    # Join orphaned unordered list markers: a bare "-" line merged with the next content line.
    # Use [ \t]* instead of \s* to avoid newlines being matched by the space-consuming
    # quantifiers, which would create overlapping alternatives and risk catastrophic backtracking.
    result = re.sub(r"(?m)^-[ \t]*\n(?:[ \t]*\n)*[ \t]*(.+)", r"- \1", result)

    # Join orphaned ordered list markers: e.g. "1.\n" merged with the next content line.
    result = re.sub(r"(?m)^(\d+\.)[ \t]*\n(?:[ \t]*\n)*[ \t]*(.+)", r"\1 \2", result)

    # Collapse blank lines between consecutive list items (both unordered and ordered) so
    # they form a single tight list.  Only remove the blank when BOTH neighbours are list
    # items; genuinely separate lists divided by non-list content are left intact.
    # Run repeatedly until stable: re.sub does not re-examine replacement text so a run
    # of 3+ items needs multiple passes.
    _ul_collapse = re.compile(r"(?m)^(- .+)\n(?:\n)+(- )")
    _ol_collapse = re.compile(r"(?m)^(\d+\. .+)\n(?:\n)+(\d+\. )")
    for _pat in (_ul_collapse, _ol_collapse):
        while True:
            result, n = _pat.subn(r"\1\n\2", result)
            if n == 0:
                break

    return result.strip()


def page_to_markdown(page: "pymupdf.Page") -> str:
    """Convert a single PDF page to structured Markdown using font metadata and table detection."""
    import pymupdf as _mu

    body_size = _dominant_body_size(page)

    # Collect table regions so we can skip text that falls inside a table.
    table_entries = _table_rects(page)
    table_rects = [_mu.Rect(r) for r, _ in table_entries]

    def _inside_table(rect: "pymupdf.Rect") -> bool:
        for tr in table_rects:
            if abs(rect & tr) > 0:
                return True
        return False

    # Walk blocks in reading order, emitting Markdown.
    data = page.get_text("dict", sort=True)
    output_items: list[tuple[float, str]] = []  # (y0, markdown_text)

    for block in data.get("blocks", []):
        if block.get("type") != 0:
            continue

        block_rect = _mu.Rect(block["bbox"])
        if _inside_table(block_rect):
            continue

        block_y0: float = block["bbox"][1]
        para_lines: list[str] = []

        for line in block.get("lines", []):
            line_parts: list[str] = []
            line_is_heading: int | None = None

            for span in line.get("spans", []):
                raw = unicodedata.normalize("NFKC", span.get("text", "")).replace("•", "-").replace("\u2013", "-")
                if not raw.strip():
                    line_parts.append(raw)
                    continue

                size = span["size"]
                flags = span.get("flags", 0)
                bold = bool(flags & 16)  # bit 4 = bold in PyMuPDF
                italic = bool(flags & 2)  # bit 1 = italic
                mono = _is_monospace(span.get("font", ""))

                level = _heading_level(size, body_size, bold)
                if level is not None and line_is_heading is None:
                    line_is_heading = level

                text = raw
                if mono:
                    text = f"`{text.strip()}`"
                elif bold and italic:
                    text = f"***{text.strip()}***"
                elif bold:
                    text = f"**{text.strip()}**"
                elif italic:
                    text = f"*{text.strip()}*"

                line_parts.append(text)

            line_text = "".join(line_parts).strip()
            if not line_text:
                continue

            if line_is_heading is not None:
                prefix = "#" * line_is_heading
                para_lines.append(f"{prefix} {line_text}")
            else:
                para_lines.append(line_text)

        if para_lines:
            output_items.append((block_y0, "\n".join(para_lines)))

    # Insert tables at their natural vertical position.
    for (bbox, md_table) in table_entries:
        y0 = bbox[1] if isinstance(bbox, (list, tuple)) else bbox.y0
        output_items.append((y0, md_table))

    # Sort all items top-to-bottom.
    output_items.sort(key=lambda x: x[0])

    parts = [text for _, text in output_items]
    return _strip_header_footer("\n\n".join(parts))


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Extract PDF pages to Markdown files under claude/rm0503/."
    )
    parser.add_argument(
        "pdf",
        help="Path to the RM0503 PDF file.",
    )
    parser.add_argument(
        "pages",
        help=(
            "Comma-separated page numbers or ranges (1-based), "
            "e.g. '42-44,100,200-205'."
        ),
    )
    parser.add_argument(
        "--output-dir",
        default=None,
        help=(
            "Directory to write Markdown files into. "
            "Defaults to claude/rm0503/ relative to this script."
        ),
    )
    args = parser.parse_args()

    pdf_path = Path(args.pdf).resolve()
    if not pdf_path.is_file():
        print(f"ERROR: PDF not found: {pdf_path}", file=sys.stderr)
        sys.exit(1)

    try:
        page_numbers = parse_page_ranges(args.pages)
    except ValueError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)

    if args.output_dir:
        out_dir = Path(args.output_dir).resolve()
    else:
        out_dir = Path(__file__).parent / "rm0503"
    out_dir.mkdir(parents=True, exist_ok=True)

    doc: pymupdf.Document = pymupdf.open(str(pdf_path))
    total_pages = doc.page_count

    print(f"PDF has {total_pages} pages.")
    print(f"Extracting {len(page_numbers)} page(s) → {out_dir}")

    skipped = 0
    for pnum in page_numbers:
        if pnum < 1 or pnum > total_pages:
            print(f"  SKIP page {pnum}: out of range (1-{total_pages})")
            skipped += 1
            continue

        page = doc.load_page(pnum - 1)  # PyMuPDF is 0-based internally
        md_text = page_to_markdown(page)

        out_file = out_dir / f"page_{pnum:04d}.md"
        out_file.write_text(
            f"<!-- RM0503 page {pnum} -->\n\n{md_text}\n",
            encoding="utf-8",
        )
        print(f"  wrote {out_file.name}")

    doc.close()

    extracted = len(page_numbers) - skipped
    print(f"\nDone. {extracted} file(s) written to {out_dir}")
    if skipped:
        print(f"  ({skipped} page(s) skipped — out of range)")


if __name__ == "__main__":
    main()
