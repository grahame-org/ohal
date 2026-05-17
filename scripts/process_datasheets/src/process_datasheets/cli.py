# -*- coding: utf-8 -*-
"""Command-line entry point for process-datasheets."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

# Ensure stdout/stderr use UTF-8 on Windows where the default console
# codepage is often CP1252 or CP932.
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
if hasattr(sys.stderr, "reconfigure"):
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

try:
    import pymupdf
except ImportError:
    import fitz as pymupdf  # type: ignore[no-redef]

from process_datasheets.pdf_to_markdown import page_to_markdown, parse_page_ranges


def main(argv: list[str] | None = None) -> None:
    parser = argparse.ArgumentParser(
        description="Extract PDF pages to Markdown files.",
    )
    parser.add_argument("pdf", help="Path to the PDF file.")
    parser.add_argument(
        "pages",
        help="Comma-separated page numbers or ranges (1-based), e.g. '42-44,100,200-205'.",
    )
    parser.add_argument(
        "--output-dir",
        default=None,
        help="Directory to write Markdown files into. Defaults to rm0503/ next to this script.",
    )
    args = parser.parse_args(argv)

    pdf_path = Path(args.pdf).resolve()
    if not pdf_path.is_file():
        print(f"ERROR: PDF not found: {pdf_path}", file=sys.stderr)
        sys.exit(1)

    try:
        page_numbers = parse_page_ranges(args.pages)
    except ValueError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)

    out_dir = Path(args.output_dir).resolve() if args.output_dir else Path("rm0503")
    out_dir.mkdir(parents=True, exist_ok=True)

    doc: pymupdf.Document = pymupdf.open(str(pdf_path))
    total_pages = doc.page_count

    print(f"PDF has {total_pages} pages.")
    print(f"Extracting {len(page_numbers)} page(s) -> {out_dir}")

    skipped = 0
    for pnum in page_numbers:
        if pnum < 1 or pnum > total_pages:
            print(f"  SKIP page {pnum}: out of range (1-{total_pages})")
            skipped += 1
            continue

        page = doc.load_page(pnum - 1)
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
        print(f"  ({skipped} page(s) skipped - out of range)")


if __name__ == "__main__":
    main()
