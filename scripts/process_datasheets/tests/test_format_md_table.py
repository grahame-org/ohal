"""Tests for _format_md_table column-aligned GFM table output.

Prettier 3.x reformats GFM tables so that every cell is padded with trailing
spaces to the maximum content width of its column, and the separator row uses
dashes stretched to the same width.  Minimum column width is 3 (``---``).

These tests verify that ``_format_md_table`` already produces output in that
canonical form so that ``prettier --check`` generates no diff.
"""

from __future__ import annotations

from hamcrest import assert_that, contains_string, equal_to

from process_datasheets.pdf_to_markdown import (
    _format_md_table,
    _join_register_cell_lines,
    _normalise_cell,
)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def _parse_table(text: str) -> list[list[str]]:
	"""Parse a Markdown table back into a list of rows (cells stripped).

	The separator row (the ``| --- |`` line) is excluded from the result.
	"""
	lines = [l for l in text.splitlines() if l.strip()]
	# Remove separator line (second row).
	rows = [lines[0]] + lines[2:]
	return [
		[cell.strip() for cell in row.strip("|").split("|")]
		for row in rows
	]


def _column_widths(text: str) -> list[int]:
	"""Return the rendered width of each column (excluding ``| `` delimiters)."""
	lines = text.splitlines()
	sep = lines[1]  # ``| --- | --- |`` line
	return [len(part) for part in sep.strip("|").split("|")]


# ---------------------------------------------------------------------------
# Basic structure
# ---------------------------------------------------------------------------


def test_format_md_table_produces_three_sections():
	rows = [["H1", "H2"], ["a", "b"]]
	result = _format_md_table(rows)
	lines = result.splitlines()
	# header, separator, data row
	assert_that(len(lines), equal_to(3))


def test_format_md_table_header_starts_and_ends_with_pipe():
	rows = [["Name", "Value"], ["x", "y"]]
	result = _format_md_table(rows)
	header = result.splitlines()[0]
	assert_that(header.startswith("|"), equal_to(True))
	assert_that(header.endswith("|"), equal_to(True))


def test_format_md_table_separator_contains_only_dashes_pipes_spaces():
	rows = [["Name", "Value"], ["x", "y"]]
	result = _format_md_table(rows)
	sep = result.splitlines()[1]
	import re
	assert re.fullmatch(r"[|\- ]+", sep), f"Unexpected separator: {sep!r}"


# ---------------------------------------------------------------------------
# Column alignment (Prettier canonical form)
# ---------------------------------------------------------------------------


def test_format_md_table_all_columns_equal_width_when_uniform():
	rows = [["AB", "CD"], ["EF", "GH"]]
	result = _format_md_table(rows)
	widths = _column_widths(result)
	assert_that(widths[0], equal_to(widths[1]))


def test_format_md_table_column_width_driven_by_widest_cell():
	rows = [["Name", "Value"], ["SHORT", "x"], ["LONGER_NAME", "y"]]
	result = _format_md_table(rows)
	# First column: max("Name"=4, "SHORT"=5, "LONGER_NAME"=11) = 11
	# Separator width includes a leading and trailing space: " " + dashes + " "
	# _column_widths splits on "|" so each part is " dashes " → strip → len
	# Actual cell in row:  "| LONGER_NAME |" → part = " LONGER_NAME " → width 13
	# We want the dash count to equal the max content width.
	lines = result.splitlines()
	sep_parts = [p.strip() for p in lines[1].strip("|").split("|")]
	assert_that(len(sep_parts[0]), equal_to(11))  # "LONGER_NAME"


def test_format_md_table_header_driven_width():
	rows = [["Long Header", "V"], ["a", "b"]]
	result = _format_md_table(rows)
	lines = result.splitlines()
	sep_parts = [p.strip() for p in lines[1].strip("|").split("|")]
	assert_that(len(sep_parts[0]), equal_to(11))  # "Long Header"


# ---------------------------------------------------------------------------
# Minimum column width of 3 (for the ``---`` separator)
# ---------------------------------------------------------------------------


def test_format_md_table_minimum_column_width_is_three():
	rows = [["A", "B"], ["x", "y"]]
	result = _format_md_table(rows)
	lines = result.splitlines()
	sep_parts = [p.strip() for p in lines[1].strip("|").split("|")]
	for part in sep_parts:
		assert len(part) >= 3, f"Separator part {part!r} is shorter than 3 dashes"


def test_format_md_table_single_char_header_produces_three_dash_separator():
	rows = [["A"], ["x"]]
	result = _format_md_table(rows)
	sep = result.splitlines()[1]
	assert_that(sep.strip(), equal_to("| --- |"))


# ---------------------------------------------------------------------------
# Content round-trip: cell values are preserved
# ---------------------------------------------------------------------------


def test_format_md_table_preserves_cell_content():
	rows = [["Register", "Offset", "Reset"], ["RCC_CR", "0x00", "0x0000 0063"]]
	result = _format_md_table(rows)
	parsed = _parse_table(result)
	assert_that(parsed[0], equal_to(["Register", "Offset", "Reset"]))
	assert_that(parsed[1], equal_to(["RCC_CR", "0x00", "0x0000 0063"]))


def test_format_md_table_many_data_rows():
	header = ["Bit", "Field", "Access", "Reset", "Description"]
	data_rows = [
		["31:24", "Reserved", "r", "0", "Reserved"],
		["23:16", "HSITRIM[7:0]", "rw", "0x20", "HSI clock trimming"],
		["15:8", "HSICAL[7:0]", "r", "0x00", "HSI clock calibration"],
	]
	result = _format_md_table([header] + data_rows)
	parsed = _parse_table(result)
	assert_that(len(parsed), equal_to(4))  # header + 3 data rows
	assert_that(parsed[0], equal_to(header))
	for i, row in enumerate(data_rows):
		assert_that(parsed[i + 1], equal_to(row))


# ---------------------------------------------------------------------------
# Ragged rows: short rows are padded to full column count
# ---------------------------------------------------------------------------


def test_format_md_table_ragged_rows_padded_to_column_count():
	# Row 2 has fewer cells than the header.
	rows = [["A", "B", "C"], ["only one"]]
	result = _format_md_table(rows)
	lines = result.splitlines()
	# Every row must have the same number of ``|`` separators.
	pipe_counts = [line.count("|") for line in lines]
	assert len(set(pipe_counts)) == 1, f"Unequal pipe counts: {pipe_counts}"


# ---------------------------------------------------------------------------
# Empty input
# ---------------------------------------------------------------------------


def test_format_md_table_empty_rows_returns_empty_string():
	assert_that(_format_md_table([]), equal_to(""))


# ---------------------------------------------------------------------------
# Prettier-stable: output must not change when re-run through Prettier logic
# ---------------------------------------------------------------------------


def test_format_md_table_is_prettier_stable_uniform_column():
	"""A table where all columns have uniform width must already be Prettier-stable."""
	rows = [["Name", "Val"], ["FOO ", "0x1"], ["BAR ", "0x2"]]
	result = _format_md_table(rows)
	# Re-parse and re-format; must be identical.
	parsed = _parse_table(result)
	# Re-build rows list: header = parsed[0], data = parsed[1:]
	reformatted = _format_md_table(parsed)
	assert_that(reformatted, equal_to(result))


def test_format_md_table_is_prettier_stable_varying_columns():
	"""A table with varying column widths must already be Prettier-stable."""
	rows = [
		["Name", "Value", "Description"],
		["FOO", "0x1", "Short"],
		["LONGER_NAME", "0xFF", "Something else entirely"],
	]
	result = _format_md_table(rows)
	parsed = _parse_table(result)
	reformatted = _format_md_table(parsed)
	assert_that(reformatted, equal_to(result))


# ---------------------------------------------------------------------------
# Single-column and single-row tables
# ---------------------------------------------------------------------------


def test_format_md_table_single_column():
	rows = [["Header"], ["value one"], ["value two and more"]]
	result = _format_md_table(rows)
	lines = result.splitlines()
	assert_that(len(lines), equal_to(4))  # header + sep + 2 data


def test_format_md_table_header_only():
	rows = [["Just a header"]]
	result = _format_md_table(rows)
	lines = result.splitlines()
	assert_that(len(lines), equal_to(2))  # header + separator only


# ---------------------------------------------------------------------------
# Underscore escaping in table cells (Prettier parity)
# ---------------------------------------------------------------------------


def test_normalise_cell_strips_trailing_space_underscore():
	# A trailing standalone _ is a PDF border artefact and must be stripped entirely.
	# e.g. pymupdf emits "REV ID\n_" -> collapse -> "REV ID _" -> strip -> "REV ID".
	assert_that(_normalise_cell("FLASH ITF _"), equal_to("FLASH ITF"))
	assert_that(_normalise_cell("REV ID _"), equal_to("REV ID"))
	assert_that(_normalise_cell("FLASH SIZE _"), equal_to("FLASH SIZE"))


def test_normalise_cell_escapes_leading_underscore():
	assert_that(_normalise_cell("_ FOO"), equal_to(r"\_ FOO"))


def test_normalise_cell_escapes_space_separated_underscore():
	assert_that(_normalise_cell("FOO _ BAR"), equal_to(r"FOO \_ BAR"))


def test_normalise_cell_does_not_escape_identifier_underscore():
	# Embedded underscores in identifiers (FLASH_ITF) must NOT be escaped.
	assert_that(_normalise_cell("FLASH_ITF"), equal_to("FLASH_ITF"))
	assert_that(_normalise_cell("TIM1_DIER"), equal_to("TIM1_DIER"))


def test_format_md_table_with_escaped_cell_uses_correct_column_width():
	# Mid-string standalone _ is escaped as \_ by _normalise_cell; _format_md_table
	# must use the display width of the escaped form for column alignment.
	escaped_rows = [
		[_normalise_cell(c) for c in ["Res.", "Res.", "FOO _ BAR", "FOO _ BAZ"]],
		[_normalise_cell(c) for c in ["", "", "r", "r"]],
	]
	result = _format_md_table(escaped_rows)
	assert_that(result, contains_string(r"FOO \_ BAR"))
	assert_that(result, contains_string(r"FOO \_ BAZ"))
	# Column width should be 10 (len("FOO \_ BAR") = 10), giving "----------"
	assert_that(result, contains_string("----------"))


# ---------------------------------------------------------------------------
# Register cell line reassembly (_join_register_cell_lines)
# ---------------------------------------------------------------------------


def test_join_register_cell_nrst_shdw():
	"""Lone-_ separator line between two word fragments produces underscore join."""
	assert_that(_join_register_cell_lines("NRST\n_\nSHDW"), equal_to("NRST_SHDW"))


def test_join_register_cell_nrst_stdby():
	assert_that(_join_register_cell_lines("NRST\n_\nSTDBY"), equal_to("NRST_STDBY"))


def test_join_register_cell_nrst_stop():
	assert_that(_join_register_cell_lines("NRST\n_\nSTOP"), equal_to("NRST_STOP"))


def test_join_register_cell_nrst_mode_with_subscript():
	"""Space in word segment counts as one underscore; separator _ accounts for
	the other, giving NRST_MODE[1:0] (no spare underscore to join [1:0])."""
	assert_that(_join_register_cell_lines("NRST MODE\n_\n[1:0]"), equal_to("NRST_MODE[1:0]"))


def test_join_register_cell_nboot0():
	"""All-caps fragments with no separator are concatenated directly."""
	assert_that(_join_register_cell_lines("N\nBOOT\n0"), equal_to("NBOOT0"))


def test_join_register_cell_nboot1():
	assert_that(_join_register_cell_lines("N\nBOOT\n1"), equal_to("NBOOT1"))


def test_join_register_cell_nboot_sel_trailing_separator():
	"""Trailing lone-_ separator is used to join the two preceding word segments."""
	assert_that(_join_register_cell_lines("NBOOT\nSEL\n_"), equal_to("NBOOT_SEL"))


def test_join_register_cell_bkpsram_hw_erase_disable():
	"""'_ _' separator line carries two underscore chars for two join points."""
	assert_that(
		_join_register_cell_lines("BKPSRAM\nHW\n_ _\nERASE\n_\nDISABLE"),
		equal_to("BKPSRAM_HW_ERASE_DISABLE"),
	)


def test_join_register_cell_ram_parity_check():
	assert_that(
		_join_register_cell_lines("RAM\n_\nPARITY\n_\nCHECK"),
		equal_to("RAM_PARITY_CHECK"),
	)


def test_join_register_cell_wwdg_sw():
	assert_that(_join_register_cell_lines("WWDG\nSW\n_"), equal_to("WWDG_SW"))


def test_join_register_cell_iwdg_stdby():
	assert_that(_join_register_cell_lines("IWDG\nSTDBY\n_"), equal_to("IWDG_STDBY"))


def test_join_register_cell_bor_lev():
	"""Space inside 'BOR LEV[1:0]' converts to _ consuming the only separator."""
	assert_that(_join_register_cell_lines("BOR LEV[1:0]\n_"), equal_to("BOR_LEV[1:0]"))


def test_join_register_cell_bor_en():
	assert_that(_join_register_cell_lines("BOR\n_\nEN"), equal_to("BOR_EN"))


def test_join_register_cell_single_line_unchanged():
	"""A single-line cell is returned verbatim."""
	assert_that(_join_register_cell_lines("IRHEN"), equal_to("IRHEN"))


def test_join_register_cell_prose_unchanged():
	"""Multi-line prose cells without lone-_ lines are returned unchanged."""
	assert_that(
		_join_register_cell_lines("Some\nlong\ntext"),
		equal_to("Some\nlong\ntext"),
	)


def test_normalise_cell_reassembles_multiline_register_name():
	"""_normalise_cell applies the reassembly heuristic before collapsing newlines."""
	assert_that(_normalise_cell("NRST\n_\nSHDW"), equal_to("NRST_SHDW"))
	assert_that(_normalise_cell("WWDG\nSW\n_"), equal_to("WWDG_SW"))


def test_join_register_cell_vc_harderr():
	"""One separator with the second part split across two lines (VC_HARDERR, page 1302)."""
	assert_that(_join_register_cell_lines("VC\n_\nHARDE\nRR"), equal_to("VC_HARDERR"))


def test_join_register_cell_vc_corereset():
	"""Same one-separator / multi-fragment pattern for VC_CORERESET."""
	assert_that(_join_register_cell_lines("VC\n_\nCORE\nRESET"), equal_to("VC_CORERESET"))

def test_join_register_cell_vc_harderr():
	"""One separator line with the second part split across two lines (VC_HARDERR on page 1302)."""
	assert_that(_join_register_cell_lines("VC\n_\nHARDE\nRR"), equal_to("VC_HARDERR"))


def test_join_register_cell_vc_corereset():
	"""Same pattern as VC_HARDERR but for VC_CORERESET."""
	assert_that(_join_register_cell_lines("VC\n_\nCORE\nRESET"), equal_to("VC_CORERESET"))

