"""Tests for _format_span inline-formatting conventions.

Prettier 3.x normalises:
  - bold    → **text** (unchanged)
  - monospace   → `text`

Italic formatting is stripped (PDF italic spans are emitted as plain text)
to avoid underscore/asterisk collisions with register identifiers.

These tests verify that _format_span produces output that is already in
Prettier's canonical form so that running ``prettier --check`` on the
generated Markdown files produces no reformatting diff.
"""

from __future__ import annotations

import pytest
from hamcrest import assert_that, equal_to

from process_datasheets.pdf_to_markdown import _fix_asterisk_artifacts, _format_span

# Font flag bit masks (as used by PyMuPDF).
_ITALIC_FLAG = 2
_BOLD_FLAG = 16


class TestPlainText:
	def test_plain_text_is_returned_unchanged(self):
		assert_that(_format_span("hello", 0, "Arial"), equal_to("hello"))

	def test_plain_text_with_whitespace_is_returned_unchanged(self):
		# No formatting applied – leading/trailing space preserved as-is.
		assert_that(_format_span("  hello  ", 0, "Arial"), equal_to("  hello  "))


class TestItalic:
	"""Italic spans are stripped — plain text is returned."""

	def test_italic_returns_plain_text(self):
		assert_that(_format_span("word", _ITALIC_FLAG, "Arial"), equal_to("word"))

	def test_italic_whitespace_returned_unchanged(self):
		assert_that(_format_span("  word  ", _ITALIC_FLAG, "Arial"), equal_to("  word  "))

	def test_italic_does_not_use_underscores(self):
		result = _format_span("word", _ITALIC_FLAG, "Arial")
		assert "_" not in result, f"Expected no underscores but got: {result!r}"

	@pytest.mark.parametrize("text", ["register name", "See Table 3", "μs"])
	def test_italic_multi_word_content(self, text):
		assert_that(_format_span(text, _ITALIC_FLAG, "Arial"), equal_to(text))


class TestBold:
	"""Bold spans must use **double asterisks** — already Prettier-canonical."""

	def test_bold_wraps_with_double_asterisks(self):
		assert_that(_format_span("word", _BOLD_FLAG, "Arial"), equal_to("**word**"))

	def test_bold_whitespace_placed_outside_markers(self):
		assert_that(_format_span("  word  ", _BOLD_FLAG, "Arial"), equal_to("  **word**  "))

	@pytest.mark.parametrize("text", ["RCC_CR", "Reset value: 0x0000", "Important note"])
	def test_bold_multi_word_content(self, text):
		assert_that(_format_span(text, _BOLD_FLAG, "Arial"), equal_to(f"**{text}**"))


class TestBoldItalic:
	"""Bold+italic spans are treated as bold (italic is stripped)."""

	def test_bold_italic_uses_bold_only(self):
		result = _format_span("word", _BOLD_FLAG | _ITALIC_FLAG, "Arial")
		assert_that(result, equal_to("**word**"))

	def test_bold_italic_does_not_use_underscores(self):
		result = _format_span("word", _BOLD_FLAG | _ITALIC_FLAG, "Arial")
		assert "_" not in result, f"Expected no underscores but got: {result!r}"

	def test_bold_italic_whitespace_placed_outside_markers(self):
		assert_that(_format_span("  word  ", _BOLD_FLAG | _ITALIC_FLAG, "Arial"), equal_to("  **word**  "))

	@pytest.mark.parametrize("text", ["Section 1.2", "Important Note"])
	def test_bold_italic_multi_word_content(self, text):
		assert_that(_format_span(text, _BOLD_FLAG | _ITALIC_FLAG, "Arial"), equal_to(f"**{text}**"))


class TestMonospace:
	"""Monospace spans must use backtick code spans regardless of other flags."""

	@pytest.mark.parametrize(
		"font",
		["Courier", "CourierNew", "Consolas", "SomeMono", "DejaVuSansMono"],
	)
	def test_monospace_font_wraps_with_backticks(self, font):
		assert_that(_format_span("0xFF", 0, font), equal_to("`0xFF`"))

	def test_monospace_takes_priority_over_bold(self):
		# When a span is both monospace and bold, the monospace wins.
		assert_that(_format_span("0xFF", _BOLD_FLAG, "Courier"), equal_to("`0xFF`"))

	def test_monospace_takes_priority_over_italic(self):
		assert_that(_format_span("0xFF", _ITALIC_FLAG, "Courier"), equal_to("`0xFF`"))

	def test_monospace_takes_priority_over_bold_italic(self):
		assert_that(_format_span("0xFF", _BOLD_FLAG | _ITALIC_FLAG, "Courier"), equal_to("`0xFF`"))

	def test_monospace_whitespace_placed_outside_markers(self):
		assert_that(_format_span("  0xFF  ", 0, "Courier"), equal_to("  `0xFF`  "))


class TestEmptyAndWhitespaceOnly:
	"""Edge cases: empty and whitespace-only spans return the raw text unchanged."""

	def test_empty_string_plain(self):
		assert_that(_format_span("", 0, "Arial"), equal_to(""))

	def test_whitespace_only_italic_returns_raw(self):
		# No content after stripping — return raw rather than producing "__".
		assert_that(_format_span("   ", _ITALIC_FLAG, "Arial"), equal_to("   "))

	def test_whitespace_only_bold_returns_raw(self):
		assert_that(_format_span("   ", _BOLD_FLAG, "Arial"), equal_to("   "))

	def test_empty_string_italic_returns_raw(self):
		assert_that(_format_span("", _ITALIC_FLAG, "Arial"), equal_to(""))


class TestWhitespacePreservation:
	"""Leading/trailing whitespace must be placed *outside* emphasis markers.

	For bold and monospace spans the PDF's original inter-span spacing is
	preserved outside the markers.
	"""

	def test_italic_with_leading_space_returns_plain_text_with_space(self):
		# Italic is stripped — leading space is preserved as-is.
		assert_that(_format_span(" Section 1.5", _ITALIC_FLAG, "Arial"), equal_to(" Section 1.5"))

	def test_leading_space_placed_outside_bold_markers(self):
		assert_that(_format_span(" important", _BOLD_FLAG, "Arial"), equal_to(" **important**"))

	def test_leading_space_placed_outside_bold_italic_markers(self):
		# bold+italic → bold only; space still outside markers.
		assert_that(_format_span(" phrase", _BOLD_FLAG | _ITALIC_FLAG, "Arial"), equal_to(" **phrase**"))

	def test_leading_space_placed_outside_monospace_markers(self):
		assert_that(_format_span(" 0xFF", 0, "Courier"), equal_to(" `0xFF`"))


class TestFixAsteriskArtifacts:
	"""Tests for _fix_asterisk_artifacts post-processing of asterisk artefacts."""

	def test_bold_adjacency_asterisk_fix(self):
		# "TIMx*BDTR" → "TIMx_BDTR" (space inserted then bare * converted to _)
		assert_that(_fix_asterisk_artifacts("TIMx*BDTR"), equal_to("TIMx_BDTR"))

	def test_multiplication_asterisk_escaped(self):
		# "0x004 * x" → "0x004 \* x" (pre-empt Prettier italic interpretation)
		assert_that(_fix_asterisk_artifacts("0x004 * x"), equal_to(r"0x004 \* x"))

	def test_identifier_underscore_unchanged(self):
		# Without italic spans in the output, plain identifiers are never escaped.
		assert_that(_fix_asterisk_artifacts("the CRS_ISR register."), equal_to("the CRS_ISR register."))
