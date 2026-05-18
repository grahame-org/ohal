"""Integration tests for the CLI entry point using pyfakefs."""

from __future__ import annotations

import sys
import pathlib
import pytest
from unittest.mock import MagicMock, patch
from hamcrest import assert_that, equal_to, contains_string

from process_datasheets.cli import main


def _make_fake_doc(pages: dict[int, str]):
    """Return a mock pymupdf.Document whose pages produce given markdown text."""
    doc = MagicMock()
    doc.page_count = max(pages.keys()) + 1

    def load_page(idx):
        page = MagicMock()
        # page_to_markdown is patched; store index so patch can look it up
        page._test_page_index = idx
        return page

    doc.load_page.side_effect = load_page
    return doc, pages


@pytest.fixture()
def fake_pdf_setup(fs, tmp_path):
    """Provide a pyfakefs filesystem with a dummy PDF path and patched pymupdf."""
    # pyfakefs controls the real fs; we still need real imports, so pause for them
    fs.pause()
    import pathlib

    fs.resume()

    pdf_path = "/fake/datasheet.pdf"
    fs.create_file(pdf_path, contents=b"")
    return pdf_path


def test_cli_writes_output_file_for_valid_page(fs):
    pdf_path = "/fake/ds.pdf"
    fs.create_file(pdf_path, contents=b"")
    out_dir = "/out"

    mock_doc = MagicMock()
    mock_doc.page_count = 10
    mock_page = MagicMock()
    mock_doc.load_page.return_value = mock_page

    with (
        patch("process_datasheets.cli.pymupdf") as mock_mu,
        patch("process_datasheets.cli.page_to_markdown", return_value="# Heading\n\nBody text."),
    ):
        mock_mu.open.return_value = mock_doc
        main([pdf_path, "3", "--output-dir", out_dir])

    out_file = pathlib.Path(out_dir) / "page_0003.md"
    assert_that(out_file.exists(), equal_to(True))


def test_cli_output_file_contains_page_comment(fs):
    pdf_path = "/fake/ds.pdf"
    fs.create_file(pdf_path, contents=b"")
    out_dir = "/out"

    mock_doc = MagicMock()
    mock_doc.page_count = 10
    mock_doc.load_page.return_value = MagicMock()

    with (
        patch("process_datasheets.cli.pymupdf") as mock_mu,
        patch("process_datasheets.cli.page_to_markdown", return_value="Body."),
    ):
        mock_mu.open.return_value = mock_doc
        main([pdf_path, "5", "--output-dir", out_dir])

    content = (pathlib.Path(out_dir) / "page_0005.md").read_text(encoding="utf-8")
    assert_that(content, contains_string("<!-- RM0503 page 5 -->"))


def test_cli_output_file_contains_markdown_body(fs):
    pdf_path = "/fake/ds.pdf"
    fs.create_file(pdf_path, contents=b"")
    out_dir = "/out"

    mock_doc = MagicMock()
    mock_doc.page_count = 10
    mock_doc.load_page.return_value = MagicMock()
    expected_body = "# Section\n\nSome register description."

    with (
        patch("process_datasheets.cli.pymupdf") as mock_mu,
        patch("process_datasheets.cli.page_to_markdown", return_value=expected_body),
    ):
        mock_mu.open.return_value = mock_doc
        main([pdf_path, "2", "--output-dir", out_dir])

    content = (pathlib.Path(out_dir) / "page_0002.md").read_text(encoding="utf-8")
    assert_that(content, contains_string(expected_body))


def test_cli_skips_out_of_range_page(fs, capsys):
    pdf_path = "/fake/ds.pdf"
    fs.create_file(pdf_path, contents=b"")
    out_dir = "/out"

    mock_doc = MagicMock()
    mock_doc.page_count = 5

    with (
        patch("process_datasheets.cli.pymupdf") as mock_mu,
        patch("process_datasheets.cli.page_to_markdown", return_value=""),
    ):
        mock_mu.open.return_value = mock_doc
        main([pdf_path, "99", "--output-dir", out_dir])

    captured = capsys.readouterr()
    assert_that(captured.out, contains_string("SKIP"))


def test_cli_exits_when_pdf_not_found(fs):
    with pytest.raises(SystemExit) as exc_info:
        main(["/no/such/file.pdf", "1"])
    assert_that(exc_info.value.code, equal_to(1))


def test_cli_exits_on_invalid_page_spec(fs):
    pdf_path = "/fake/ds.pdf"
    fs.create_file(pdf_path, contents=b"")

    with pytest.raises(SystemExit) as exc_info:
        main([pdf_path, "abc"])
    assert_that(exc_info.value.code, equal_to(1))
