# -*- coding: utf-8 -*-
"""Document-specific post-extraction fixups keyed by PDF SHA-256.

Some defects in extracted Markdown originate in the source PDF itself rather
than in the extraction logic — for example, a Table of Contents that omits an
entry that does appear in the body of the document.  These cannot be corrected
by the generic extraction pipeline.

This module provides a registry of such fixups.  Each entry is keyed by the
SHA-256 hex digest of the exact PDF file it was authored against; fixups will
only be applied when the digest matches, so they can never corrupt output
produced from a different (e.g. revised) edition of the document.

Registry format
---------------
FIXUPS : dict[str, list[tuple[int, Callable[[str], str]]]]
    Maps a PDF SHA-256 hex digest to an ordered list of (page_number, patch)
    pairs.  ``page_number`` is 1-based (matching the PDF page numbering used
    throughout the tool).  ``patch`` is a callable that receives the current
    Markdown text of that page and returns the corrected text.
"""

from __future__ import annotations

import hashlib
from pathlib import Path
from typing import Callable


# ---------------------------------------------------------------------------
# Patch helpers
# ---------------------------------------------------------------------------


def _insert_after(text: str, anchor: str, insertion: str) -> str:
    """Return *text* with *insertion* placed on the line immediately after *anchor*.

    If *anchor* is not found the text is returned unchanged.
    """
    idx = text.find(anchor)
    if idx == -1:
        return text
    end = text.find("\n", idx)
    if end == -1:
        return text + "\n" + insertion
    return text[: end + 1] + insertion + "\n" + text[end + 1 :]


def _insert_before(text: str, anchor: str, insertion: str) -> str:
    """Return *text* with *insertion* placed on the line immediately before *anchor*.

    If *anchor* is not found the text is returned unchanged.
    """
    idx = text.find(anchor)
    if idx == -1:
        return text
    # Walk back to the start of the anchor line.
    line_start = text.rfind("\n", 0, idx)
    insert_pos = line_start + 1 if line_start != -1 else 0
    return text[:insert_pos] + insertion + "\n" + text[insert_pos:]


# ---------------------------------------------------------------------------
# RM0503 Rev 4  (ST reference manual for STM32U0 series)
# SHA-256 of docs/rm0503-stm32u0-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
# Obtained via: sha256sum <file>
# ---------------------------------------------------------------------------

_RM0503_SHA256 = "782e1a3bb5a83cdc15f8c6544345911f47ef8fd3c9ca7212cac80cfe50243edf"

# ---------------------------------------------------------------------------
# RM0503 page 37 fixup: insert the missing Table 16 ToC entry.
#
# The source PDF omits "Table 16. WRP protection" from its Table of Contents
# even though the table appears correctly on page 83.  The entry belongs
# between Table 15 (page 80) and Table 17 (page 84).
# ---------------------------------------------------------------------------

_TABLE16_TOC_LINE = "Table 16. WRP protection ................................................................ 83"


def _patch_page_37_insert_table16_toc(text: str) -> str:
    """Insert the missing Table 16 ToC entry between Table 15 and Table 17."""
    # Only insert if Table 16 is not already present (idempotency guard).
    if "Table 16." in text:
        return text
    anchor = "Table 17."
    return _insert_before(text, anchor, _TABLE16_TOC_LINE)


# ---------------------------------------------------------------------------
# RM0503 page 93 fixup: protect NBOOT_SEL from Prettier italic-span corruption.
#
# The Bit 25 paragraph contains "NBOOT_SEL option bit" immediately before an
# italic cross-reference "_Section 2.5: Boot configuration_".  Prettier
# interprets the "_S" in "NBOOT_SEL" as an italic-open delimiter and the
# "_" in "_Section" as its close, corrupting "NBOOT_SEL" to "NBOOT*SEL" and
# mangling the cross-reference.  Switching the cross-reference to "*...*"
# notation pre-empts Prettier's ambiguous parse while preserving the italic
# rendering.
# ---------------------------------------------------------------------------


def _patch_page_93_protect_nboot_sel_italic(text: str) -> str:
    """Replace the long-form Section cross-ref that Prettier mis-parses."""
    old = "_Section 2.5: Boot configuration_"
    new = "*Section 2.5: Boot configuration*"
    if new in text or old not in text:
        return text
    return text.replace(old, new)


# ---------------------------------------------------------------------------
# Registry
# ---------------------------------------------------------------------------

FIXUPS: dict[str, list[tuple[int, Callable[[str], str]]]] = {
    _RM0503_SHA256: [
        (37, _patch_page_37_insert_table16_toc),
        (93, _patch_page_93_protect_nboot_sel_italic),
    ],
}


# ---------------------------------------------------------------------------
# Public API
# ---------------------------------------------------------------------------


def compute_pdf_sha256(pdf_path: Path) -> str:
    """Return the SHA-256 hex digest of the file at *pdf_path*."""
    h = hashlib.sha256()
    with pdf_path.open("rb") as fh:
        for chunk in iter(lambda: fh.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def apply_fixups(pdf_sha256: str, out_dir: Path) -> int:
    """Apply any registered fixups for *pdf_sha256* to files in *out_dir*.

    Returns the number of fixups applied.  Pages whose output file does not
    exist in *out_dir* are silently skipped (the page may not have been
    extracted in this run).
    """
    patches = FIXUPS.get(pdf_sha256, [])
    applied = 0
    for page_number, patch in patches:
        out_file = out_dir / f"page_{page_number:04d}.md"
        if not out_file.exists():
            continue
        original = out_file.read_text(encoding="utf-8")
        patched = patch(original)
        if patched != original:
            out_file.write_text(patched, encoding="utf-8")
            applied += 1
    return applied
