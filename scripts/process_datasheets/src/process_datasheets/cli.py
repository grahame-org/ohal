# -*- coding: utf-8 -*-
"""Command-line entry point for process-datasheets."""

from __future__ import annotations

import argparse
import subprocess
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
from process_datasheets.fixups import apply_fixups, compute_pdf_sha256


def _extract_pages(
    doc: "pymupdf.Document",
    page_numbers: list[int],
    out_dir: Path,
) -> tuple[int, int]:
    """Extract *page_numbers* from *doc* and write one Markdown file per page.

    Returns ``(extracted, skipped)`` counts.
    """
    total_pages = doc.page_count
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

    extracted = len(page_numbers) - skipped
    return extracted, skipped


def _run_prettier(out_dir: Path) -> None:
    """Run prettier over every Markdown file in *out_dir*.

    Passes ``--config`` pointing at the repo-root ``.prettierrc.json`` so that
    the same settings apply regardless of where *out_dir* is on disk.  Uses a
    glob pattern to avoid Windows command-line length limits.
    """
    md_files = sorted(out_dir.glob("*.md"))
    if not md_files:
        return
    print(f"\nRunning prettier on {len(md_files)} file(s) in {out_dir} …")
    _repo_root = Path(__file__).parents[4]
    _prettier_config = _repo_root / ".prettierrc.json"
    glob_pattern = str(out_dir / "*.md")
    result = subprocess.run(
        [
            "npx",
            "--yes",
            "prettier",
            "--config",
            str(_prettier_config),
            "--write",
            glob_pattern,
        ],
        check=False,
        shell=(sys.platform == "win32"),
    )
    if result.returncode != 0:
        print("WARNING: prettier exited with a non-zero status.", file=sys.stderr)


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
    pdf_sha256 = compute_pdf_sha256(pdf_path)
    print(f"PDF SHA-256: {pdf_sha256}")
    print(f"PDF has {doc.page_count} pages.")
    print(f"Extracting {len(page_numbers)} page(s) -> {out_dir}")

    extracted, skipped = _extract_pages(doc, page_numbers, out_dir)
    doc.close()

    print(f"\nDone. {extracted} file(s) written to {out_dir}")
    if skipped:
        print(f"  ({skipped} page(s) skipped - out of range)")

    # Apply document-specific fixups keyed to the PDF's SHA-256 digest.
    fixup_count = apply_fixups(pdf_sha256, out_dir)
    if fixup_count:
        print(f"Applied {fixup_count} document-specific fixup(s).")

    _run_prettier(out_dir)


if __name__ == "__main__":
    main()
