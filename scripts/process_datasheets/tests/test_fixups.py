"""Tests for the document-specific fixup registry."""

from __future__ import annotations

import pytest
from hamcrest import assert_that, contains_string, equal_to, not_

from process_datasheets.fixups import (
    FIXUPS,
    _RM0503_SHA256,
    _TABLE16_TOC_LINE,
    _insert_after,
    _insert_before,
    _patch_page_37_insert_table16_toc,
    apply_fixups,
    compute_pdf_sha256,
)


# ---------------------------------------------------------------------------
# _insert_after helper
# ---------------------------------------------------------------------------


def test_insert_after_places_text_on_line_after_anchor():
    text = "line one\nanchor line\nline three\n"
    result = _insert_after(text, "anchor line", "inserted")
    assert_that(result, equal_to("line one\nanchor line\ninserted\nline three\n"))


def test_insert_after_returns_unchanged_when_anchor_missing():
    text = "line one\nline two\n"
    assert_that(_insert_after(text, "not present", "inserted"), equal_to(text))


def test_insert_after_handles_text_with_no_trailing_newline():
    text = "first\nanchor"
    result = _insert_after(text, "anchor", "appended")
    assert_that(result, equal_to("first\nanchor\nappended"))


# ---------------------------------------------------------------------------
# _insert_before helper
# ---------------------------------------------------------------------------


def test_insert_before_places_text_on_line_before_anchor():
    text = "line one\nanchor line\nline three\n"
    result = _insert_before(text, "anchor line", "inserted")
    assert_that(result, equal_to("line one\ninserted\nanchor line\nline three\n"))


def test_insert_before_returns_unchanged_when_anchor_missing():
    text = "line one\nline two\n"
    assert_that(_insert_before(text, "not present", "inserted"), equal_to(text))


def test_insert_before_at_start_of_text():
    text = "anchor line\nrest\n"
    result = _insert_before(text, "anchor line", "inserted")
    assert_that(result, equal_to("inserted\nanchor line\nrest\n"))


# ---------------------------------------------------------------------------
# _patch_page_37_insert_table16_toc
# ---------------------------------------------------------------------------

_TOC_SNIPPET = (
    "Table 15. Access status versus protection level and execution modes ............................. 80\n"
    "Table 17. HDP extension protection .............................................................. 84\n"
    "Table 18. FLASH interrupt requests .............................................................. 85\n"
)


def test_patch_page_37_inserts_table16_between_15_and_17():
    result = _patch_page_37_insert_table16_toc(_TOC_SNIPPET)
    lines = result.splitlines()
    idx15 = next(i for i, l in enumerate(lines) if l.startswith("Table 15."))
    idx16 = next(i for i, l in enumerate(lines) if l.startswith("Table 16."))
    idx17 = next(i for i, l in enumerate(lines) if l.startswith("Table 17."))
    assert_that(idx15 < idx16 < idx17, equal_to(True))


def test_patch_page_37_table16_contains_correct_title_and_page():
    result = _patch_page_37_insert_table16_toc(_TOC_SNIPPET)
    assert_that(result, contains_string("Table 16. WRP protection"))
    assert_that(result, contains_string("83"))


def test_patch_page_37_uses_canonical_toc_line():
    result = _patch_page_37_insert_table16_toc(_TOC_SNIPPET)
    assert_that(result, contains_string(_TABLE16_TOC_LINE))


def test_patch_page_37_is_idempotent():
    once = _patch_page_37_insert_table16_toc(_TOC_SNIPPET)
    twice = _patch_page_37_insert_table16_toc(once)
    # Second application must not insert a duplicate.
    assert_that(twice.count("Table 16."), equal_to(1))


def test_patch_page_37_returns_unchanged_when_no_table17_anchor():
    text = "Table 15. Some entry ......................... 80\n"
    result = _patch_page_37_insert_table16_toc(text)
    assert_that(result, equal_to(text))


def test_patch_page_37_preserves_all_other_entries():
    result = _patch_page_37_insert_table16_toc(_TOC_SNIPPET)
    assert_that(result, contains_string("Table 15."))
    assert_that(result, contains_string("Table 17."))
    assert_that(result, contains_string("Table 18."))


# ---------------------------------------------------------------------------
# apply_fixups
# ---------------------------------------------------------------------------


def test_apply_fixups_returns_zero_for_unknown_sha256(tmp_path):
    (tmp_path / "page_0037.md").write_text(_TOC_SNIPPET, encoding="utf-8")
    count = apply_fixups("unknown-sha256", tmp_path)
    assert_that(count, equal_to(0))


def test_apply_fixups_skips_missing_page_file(tmp_path):
    """If the target page was not extracted, apply_fixups silently skips it."""
    fake_sha = "a" * 64
    sentinel_called = []

    def _patch(text):
        sentinel_called.append(True)
        return text

    original = FIXUPS.copy()
    FIXUPS[fake_sha] = [(9999, _patch)]
    try:
        count = apply_fixups(fake_sha, tmp_path)
    finally:
        FIXUPS.clear()
        FIXUPS.update(original)

    assert_that(count, equal_to(0))
    assert_that(len(sentinel_called), equal_to(0))


def test_apply_fixups_patches_matching_page(tmp_path):
    """apply_fixups rewrites the file when the patch changes its content."""
    fake_sha = "b" * 64
    page_file = tmp_path / "page_0001.md"
    page_file.write_text("original content\n", encoding="utf-8")

    original = FIXUPS.copy()
    FIXUPS[fake_sha] = [(1, lambda t: t + "patched\n")]
    try:
        count = apply_fixups(fake_sha, tmp_path)
    finally:
        FIXUPS.clear()
        FIXUPS.update(original)

    assert_that(count, equal_to(1))
    assert_that(page_file.read_text(encoding="utf-8"), contains_string("patched"))


def test_apply_fixups_does_not_rewrite_when_patch_is_noop(tmp_path):
    """apply_fixups does not count a fixup that returns the text unchanged."""
    fake_sha = "c" * 64
    page_file = tmp_path / "page_0001.md"
    page_file.write_text("content\n", encoding="utf-8")

    original = FIXUPS.copy()
    FIXUPS[fake_sha] = [(1, lambda t: t)]
    try:
        count = apply_fixups(fake_sha, tmp_path)
    finally:
        FIXUPS.clear()
        FIXUPS.update(original)

    assert_that(count, equal_to(0))


def test_apply_fixups_with_real_rm0503_sha_and_toc_page(tmp_path):
    """Integration: the real RM0503 hash triggers the Table 16 fixup on page 37."""
    page_file = tmp_path / "page_0037.md"
    page_file.write_text(_TOC_SNIPPET, encoding="utf-8")

    count = apply_fixups(_RM0503_SHA256, tmp_path)

    assert_that(count, equal_to(1))
    result = page_file.read_text(encoding="utf-8")
    assert_that(result, contains_string("Table 16. WRP protection"))


# ---------------------------------------------------------------------------
# compute_pdf_sha256
# ---------------------------------------------------------------------------


def test_compute_pdf_sha256_returns_hex_string_of_correct_length(tmp_path):
    f = tmp_path / "dummy.pdf"
    f.write_bytes(b"dummy pdf content")
    digest = compute_pdf_sha256(f)
    assert_that(len(digest), equal_to(64))
    assert_that(all(c in "0123456789abcdef" for c in digest), equal_to(True))


def test_compute_pdf_sha256_is_deterministic(tmp_path):
    f = tmp_path / "dummy.pdf"
    f.write_bytes(b"stable content")
    assert_that(compute_pdf_sha256(f), equal_to(compute_pdf_sha256(f)))


def test_compute_pdf_sha256_differs_for_different_content(tmp_path):
    a = tmp_path / "a.pdf"
    b = tmp_path / "b.pdf"
    a.write_bytes(b"content A")
    b.write_bytes(b"content B")
    assert_that(compute_pdf_sha256(a), not_(equal_to(compute_pdf_sha256(b))))
