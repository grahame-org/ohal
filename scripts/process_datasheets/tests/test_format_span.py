"""Tests for _format_span inline-formatting conventions.

Prettier 3.x normalises:
  - italic  → _text_   (not *text*)
  - bold    → **text** (unchanged)
  - bold+italic → **_text_**  (not ***text***)
  - monospace   → `text`

These tests verify that _format_span produces output that is already in
Prettier's canonical form so that running ``prettier --check`` on the
generated Markdown files produces no reformatting diff.
"""

from __future__ import annotations

import pytest
from hamcrest import assert_that, equal_to

from process_datasheets.pdf_to_markdown import _format_span

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
	"""Italic spans must use _underscores_, matching Prettier 3.x output."""

	def test_italic_wraps_with_underscores(self):
		assert_that(_format_span("word", _ITALIC_FLAG, "Arial"), equal_to("_word_"))

	def test_italic_whitespace_placed_outside_markers(self):
		# Whitespace is preserved *outside* the markers, not stripped away.
		assert_that(_format_span("  word  ", _ITALIC_FLAG, "Arial"), equal_to("  _word_  "))

	def test_italic_does_not_use_asterisks(self):
		result = _format_span("word", _ITALIC_FLAG, "Arial")
		assert "*" not in result, f"Expected no asterisks but got: {result!r}"

	@pytest.mark.parametrize("text", ["register name", "See Table 3", "μs"])
	def test_italic_multi_word_content(self, text):
		assert_that(_format_span(text, _ITALIC_FLAG, "Arial"), equal_to(f"_{text}_"))


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
	"""Bold+italic spans must use **_text_**, matching Prettier 3.x output."""

	def test_bold_italic_uses_prettier_convention(self):
		result = _format_span("word", _BOLD_FLAG | _ITALIC_FLAG, "Arial")
		assert_that(result, equal_to("**_word_**"))

	def test_bold_italic_does_not_use_triple_asterisks(self):
		result = _format_span("word", _BOLD_FLAG | _ITALIC_FLAG, "Arial")
		assert "***" not in result, f"Expected no triple-asterisks but got: {result!r}"

	def test_bold_italic_whitespace_placed_outside_markers(self):
		assert_that(_format_span("  word  ", _BOLD_FLAG | _ITALIC_FLAG, "Arial"), equal_to("  **_word_**  "))

	@pytest.mark.parametrize("text", ["Section 1.2", "Important Note"])
	def test_bold_italic_multi_word_content(self, text):
		assert_that(_format_span(text, _BOLD_FLAG | _ITALIC_FLAG, "Arial"), equal_to(f"**_{text}_**"))


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

	CommonMark forbids ``_`` from opening emphasis when immediately preceded by
	a Unicode alphanumeric.  By keeping the PDF's original inter-span spacing
	outside the markers the natural separation prevents ``word_italic_``
	adjacency, which Prettier would rewrite to ``word*italic*``.
	"""

	def test_leading_space_placed_outside_italic_markers(self):
		# PDF span: " Section 1.5" (italic, leading space)
		# Joined to previous plain span "Refer to" -> "Refer to _Section 1.5_"
		assert_that(_format_span(" Section 1.5", _ITALIC_FLAG, "Arial"), equal_to(" _Section 1.5_"))

	def test_trailing_space_placed_outside_italic_markers(self):
		assert_that(_format_span("italic ", _ITALIC_FLAG, "Arial"), equal_to("_italic_ "))

	def test_both_spaces_placed_outside_italic_markers(self):
		assert_that(_format_span(" italic ", _ITALIC_FLAG, "Arial"), equal_to(" _italic_ "))

	def test_leading_space_placed_outside_bold_markers(self):
		assert_that(_format_span(" important", _BOLD_FLAG, "Arial"), equal_to(" **important**"))

	def test_leading_space_placed_outside_bold_italic_markers(self):
		assert_that(_format_span(" phrase", _BOLD_FLAG | _ITALIC_FLAG, "Arial"), equal_to(" **_phrase_**"))

	def test_leading_space_placed_outside_monospace_markers(self):
		assert_that(_format_span(" 0xFF", 0, "Courier"), equal_to(" `0xFF`"))

	def test_no_space_in_raw_markers_wrap_content_only(self):
		# When the PDF provides no space, no space is added here — the
		# safety-net regex in _apply_regex_postprocessing handles it.
		assert_that(_format_span("italic", _ITALIC_FLAG, "Arial"), equal_to("_italic_"))

	def test_multiple_leading_spaces_all_placed_outside(self):
		assert_that(_format_span("  word", _ITALIC_FLAG, "Arial"), equal_to("  _word_"))
