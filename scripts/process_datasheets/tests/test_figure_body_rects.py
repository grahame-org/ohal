"""Tests for _figure_body_rects() and figure-body exclusion in page_to_markdown."""

from hamcrest import assert_that, equal_to, contains_string, not_, has_length

from process_datasheets.pdf_to_markdown import _figure_body_rects


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def _make_block(text: str, y0: float, y1: float, bold: bool = False) -> dict:
	"""Build a minimal text block dict that _figure_body_rects can consume."""
	flags = 16 if bold else 0
	return {
		"type": 0,
		"bbox": [0, y0, 400, y1],
		"lines": [
			{
				"spans": [
					{"text": text, "flags": flags},
				]
			}
		],
	}


def _make_image_block(y0: float, y1: float) -> dict:
	return {"type": 1, "bbox": [0, y0, 400, y1]}


# ---------------------------------------------------------------------------
# No figures
# ---------------------------------------------------------------------------


def test_no_figure_no_exclusion():
	blocks = [
		_make_block("Some prose text.", 50, 65),
		_make_block("More prose.", 80, 95),
	]
	assert_that(_figure_body_rects(blocks), equal_to([]))


def test_caption_without_stamp_produces_no_exclusion():
	"""A caption with no following stamp leaves no exclusion band."""
	blocks = [
		_make_block("Figure 1. Some figure", 100, 115, bold=True),
		_make_block("body label text", 120, 130),
	]
	assert_that(_figure_body_rects(blocks), equal_to([]))


# ---------------------------------------------------------------------------
# Single figure with stamp
# ---------------------------------------------------------------------------


def test_single_figure_exclusion_band():
	blocks = [
		_make_block("Some prose.", 50, 65),
		_make_block("Figure 5. Example of disabling core debug access", 165, 177, bold=True),
		_make_block("SEC_PROT = 1", 196, 211),
		_make_block("timeline", 298, 307),
		_make_block("MSv42192V1", 339, 346),
		_make_block("3.5.6 Forcing boot from main flash memory", 361, 375),
	]
	bands = _figure_body_rects(blocks)
	assert_that(bands, has_length(1))
	y_start, y_end = bands[0]
	# Band starts at the bottom of the caption block.
	assert_that(y_start, equal_to(177))
	# Band ends at the bottom of the stamp block.
	assert_that(y_end, equal_to(346))


def test_figure_body_blocks_fall_inside_band():
	"""Body blocks (SEC_PROT lines, timeline) have y0 inside the band."""
	blocks = [
		_make_block("Figure 5. Example", 165, 177, bold=True),
		_make_block("SEC_PROT = 1", 196, 211),
		_make_block("timeline", 298, 307),
		_make_block("MSv42192V1", 339, 346),
		_make_block("3.5.6 Next section", 361, 375),
	]
	bands = _figure_body_rects(blocks)
	y_start, y_end = bands[0]

	body_ys = [196, 298, 339]  # SEC_PROT, timeline, stamp
	for y in body_ys:
		assert y_start <= y <= y_end, f"y0={y} not in [{y_start}, {y_end}]"

	# Content after the stamp must not be excluded.
	assert not (y_start <= 361 <= y_end)


# ---------------------------------------------------------------------------
# Multiple figures on the same page
# ---------------------------------------------------------------------------


def test_two_figures_on_same_page():
	blocks = [
		_make_block("Figure 1. First figure", 100, 115, bold=True),
		_make_block("label A", 120, 130),
		_make_block("MS11111V1", 140, 150),
		_make_block("Prose between figures.", 160, 170),
		_make_block("Figure 2. Second figure", 180, 195, bold=True),
		_make_block("label B", 200, 210),
		_make_block("MS22222V2", 220, 230),
		_make_block("Prose after figures.", 240, 255),
	]
	bands = _figure_body_rects(blocks)
	assert_that(bands, has_length(2))
	assert_that(bands[0], equal_to((115, 150)))
	assert_that(bands[1], equal_to((195, 230)))


# ---------------------------------------------------------------------------
# Image blocks are ignored
# ---------------------------------------------------------------------------


def test_image_block_ignored():
	blocks = [
		_make_block("Figure 3. Waveform", 100, 115, bold=True),
		_make_image_block(120, 200),
		_make_block("MS33333V3", 210, 220),
		_make_block("Prose after.", 230, 245),
	]
	bands = _figure_body_rects(blocks)
	assert_that(bands, has_length(1))
	assert_that(bands[0], equal_to((115, 220)))


# ---------------------------------------------------------------------------
# strip_header_footer removes version stamps (belt-and-suspenders)
# ---------------------------------------------------------------------------


def test_strip_removes_figure_version_stamp():
	from process_datasheets.pdf_to_markdown import strip_header_footer
	text = "Some prose.\n\nMSv42192V1\n\nMore prose."
	result = strip_header_footer(text)
	assert_that(result, not_(contains_string("MSv42192V1")))
	assert_that(result, contains_string("Some prose."))
	assert_that(result, contains_string("More prose."))


def test_strip_removes_ms_style_stamp():
	from process_datasheets.pdf_to_markdown import strip_header_footer
	text = "Before.\n\nMS31444V5\n\nAfter."
	result = strip_header_footer(text)
	assert_that(result, not_(contains_string("MS31444V5")))


def test_strip_does_not_remove_register_names_with_letters_digits():
	"""Register names like RCC_CFGR must not match the stamp pattern."""
	from process_datasheets.pdf_to_markdown import strip_header_footer
	text = "The RCC_CFGR register controls clocks."
	result = strip_header_footer(text)
	assert_that(result, contains_string("RCC_CFGR"))
