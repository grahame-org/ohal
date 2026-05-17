"""Tests for strip_header_footer."""

import re

import pytest
from hamcrest import assert_that, equal_to, matches_regexp, not_, contains_string

from process_datasheets.pdf_to_markdown import strip_header_footer, _TOC_WIDTH


# ---------------------------------------------------------------------------
# Header removal
# ---------------------------------------------------------------------------


def test_strip_removes_bare_rm0503_header():
    text = "**RM0503**\n**Flash memory**\n\nSome content."
    assert_that(strip_header_footer(text), not_(contains_string("**RM0503**")))


def test_strip_removes_bold_section_name_preceding_rm0503_header():
    # e.g. page 2 has "**Contents**" immediately before "**RM0503**" in the same block.
    text = "**Contents**\n**RM0503**\n**Contents**\n\nReal content."
    result = strip_header_footer(text)
    assert_that(result, equal_to("Real content."))


def test_strip_removes_heading_prefixed_rm0503_header():
    text = "#### **RM0503**\n#### **Flash memory**\n\nSome content."
    assert_that(strip_header_footer(text), not_(contains_string("**RM0503**")))


def test_strip_preserves_content_after_header():
    text = "**RM0503**\n**Flash memory**\n\nSome content."
    assert_that(strip_header_footer(text), contains_string("Some content."))


def test_strip_removes_rm0503_header_without_following_section_title():
    text = "**RM0503**\n\nSome content."
    assert_that(strip_header_footer(text), not_(contains_string("**RM0503**")))


# ---------------------------------------------------------------------------
# Footer removal
# ---------------------------------------------------------------------------


@pytest.mark.parametrize(
    "footer_line",
    [
        "RM0503 Rev 4",
        "RM0503 Rev 12",
    ],
)
def test_strip_removes_revision_footer(footer_line):
    text = f"Content.\n\n{footer_line}"
    assert_that(strip_header_footer(text), not_(contains_string("RM0503 Rev")))


@pytest.mark.parametrize(
    "footer_line",
    [
        "1/1333",
        "42/1333",
        "1332/1333",
    ],
)
def test_strip_removes_page_count_footer(footer_line):
    text = f"Content.\n\n{footer_line}"
    assert_that(strip_header_footer(text), not_(contains_string(footer_line)))


@pytest.mark.parametrize(
    "month",
    ["January", "June", "July", "December"],
)
def test_strip_removes_date_stamp(month):
    text = f"Content.\n\n{month} 2025"
    assert_that(strip_header_footer(text), not_(contains_string(month)))


@pytest.mark.parametrize(
    "url_line",
    ["www.st.com", "*www.st.com*"],
)
def test_strip_removes_st_url(url_line):
    text = f"Content.\n\n{url_line}"
    assert_that(strip_header_footer(text), not_(contains_string("www.st.com")))


def test_strip_removes_stray_lone_integer():
    text = "Content.\n\n36\n\nMore content."
    assert_that(strip_header_footer(text), not_(contains_string("\n36\n")))


def test_strip_removes_stray_lone_alpha_char():
    text = "Reset value: 0x0000 0000\n\ns\n\n| col1 | col2 |"
    assert_that(strip_header_footer(text), not_(contains_string("\ns\n")))


def test_strip_preserves_content_with_embedded_number():
    text = "STM32U031xx and STM32U073/83xx errata sheets."
    assert_that(strip_header_footer(text), contains_string("STM32U031xx"))


# ---------------------------------------------------------------------------
# List joining and collapsing
# ---------------------------------------------------------------------------


def test_strip_joins_orphaned_unordered_marker_with_next_line():
    text = "-\nItem text"
    assert_that(strip_header_footer(text), equal_to("- Item text"))


def test_strip_joins_orphaned_unordered_marker_across_blank_lines():
    text = "-\n\nItem text"
    assert_that(strip_header_footer(text), equal_to("- Item text"))


def test_strip_joins_orphaned_ordered_marker_with_next_line():
    text = "1.\nFirst item text"
    assert_that(strip_header_footer(text), equal_to("1. First item text"))


def test_strip_collapses_blank_line_between_two_unordered_items():
    text = "- First item\n\n- Second item"
    assert_that(strip_header_footer(text), equal_to("- First item\n- Second item"))


def test_strip_collapses_blank_lines_between_three_unordered_items():
    text = "- First\n\n- Second\n\n- Third"
    assert_that(strip_header_footer(text), equal_to("- First\n- Second\n- Third"))


def test_strip_collapses_blank_line_between_two_ordered_items():
    text = "1. First item\n\n2. Second item"
    assert_that(strip_header_footer(text), equal_to("1. First item\n2. Second item"))


def test_strip_collapses_ordered_list_with_wrapped_items():
    # Items whose text wraps onto a continuation line must still be collapsed into
    # a single tight list, not split into separate lists at each blank line.
    text = (
        "1. Check that the lock bit is not set, or that the readout protection is\n"
        "at level 0.\n"
        "\n"
        "2. Write a 128-bit key to the register.\n"
        "3. Launch the option modification by setting the OPTSTRT bit, and check that\n"
        "OEMOPTWERR is not set.\n"
        "\n"
        "4. Set OBL_LAUNCH or perform a power-on reset.\n"
        "5. Check that the lock bit is set."
    )
    result = strip_header_footer(text)
    # No blank lines should remain between any consecutive numbered items.
    assert_that(result, not_(matches_regexp(r"(?m)^\d+\. .*\n\n\d+\. ")))
    # All five items must be present.
    for marker in ("1.", "2.", "3.", "4.", "5."):
        assert_that(result, contains_string(marker))
    # Continuation lines must be indented by 3 spaces.
    assert_that(result, contains_string("\n   at level 0."))
    assert_that(result, contains_string("\n   OEMOPTWERR is not set."))


def test_strip_indents_continuation_lines_of_ordered_items():
    text = "1. This is a long item that\ncontinues here.\n2. Second item."
    result = strip_header_footer(text)
    assert_that(result, equal_to("1. This is a long item that\n   continues here.\n2. Second item."))


def test_strip_indents_continuation_lines_of_unordered_items():
    text = "- This is a long bullet that\ncontinues here.\n- Second bullet."
    result = strip_header_footer(text)
    # '- ' marker: content at col 2 → 2-space indent for continuations
    assert_that(result, equal_to("- This is a long bullet that\n  continues here.\n- Second bullet."))


def test_strip_does_not_indent_paragraph_after_blank_line():
    # A blank line terminates the list item; the following paragraph must not be indented.
    text = "1. Item one.\n\nThis is a paragraph."
    result = strip_header_footer(text)
    assert_that(result, equal_to("1. Item one.\n\nThis is a paragraph."))


def test_strip_collapses_unordered_list_with_wrapped_items():
    text = "- First item that\nwraps here.\n\n- Second item.\n\n- Third item."
    result = strip_header_footer(text)
    assert_that(result, not_(matches_regexp(r"(?m)^- .*\n\n- ")))
    assert_that(result, contains_string("\n  wraps here."))


def test_strip_preserves_blank_line_between_list_item_and_paragraph():
    text = "- List item\n\nThis is a paragraph."
    assert_that(strip_header_footer(text), equal_to("- List item\n\nThis is a paragraph."))


def test_strip_preserves_blank_line_between_paragraph_and_list_item():
    text = "This is a paragraph.\n\n- List item"
    assert_that(strip_header_footer(text), equal_to("This is a paragraph.\n\n- List item"))


def test_strip_indents_continuation_of_two_digit_ordered_item():
    # "10. item" → content at col 4 → 4-space continuation indent
    text = "10. Some long item that\ncontinues here.\n11. Next item."
    result = strip_header_footer(text)
    assert_that(result, equal_to("10. Some long item that\n    continues here.\n11. Next item."))


def test_strip_indents_continuation_of_nested_unordered_item():
    # "  - item" → 2 leading + 2 marker = 4-space continuation indent.
    # The continuation line arrives with no indentation from the PDF.
    text = "- Outer item\n  - Nested item that\nwraps here.\n- Next outer."
    result = strip_header_footer(text)
    # Continuation under "  - " should be indented by 4 spaces (2+2)
    assert_that(result, contains_string("    wraps here."))


def test_strip_joins_orphaned_indented_bullet_marker():
    # An indented orphaned marker "  -\ncontent" must join to "  - content"
    # (the nesting indent must be preserved).
    text = "- Outer.\n  -\nNested item.\n- Next outer."
    result = strip_header_footer(text)
    assert_that(result, contains_string("  - Nested item."))
    assert_that(result, not_(contains_string("\n  -\n")))


def test_strip_joins_bold_heading_paren_continuation():
    # PDF renders "RegName\n(REG_ACRONYM)" as two bold lines → two headings.
    # After bold-stripping, the paren-continuation join must fire.
    text = "### **TIM1 DMA/interrupt enable register**\n### **(TIM1_DIER)**"
    result = strip_header_footer(text)
    assert_that(result, equal_to("### TIM1 DMA/interrupt enable register (TIM1_DIER)"))


def test_strip_demotes_bold_footnote_number_with_body():
    # PDF renders a footnote index "1." and its text both in bold, producing two
    # heading stubs.  They should be joined and demoted to plain body text.
    text = "### 1.\n### TRGi are mapped at product level."
    result = strip_header_footer(text)
    assert_that(result, equal_to("1. TRGi are mapped at product level."))


def test_strip_demotes_lone_bold_footnote_number():
    # A footnote index with no immediately following bold body line should still
    # be demoted from a heading to plain text.
    text = "Some text.\n\n### 1.\n\nFootnote body."
    result = strip_header_footer(text)
    assert_that(result, not_(contains_string("### 1.")))
    assert_that(result, contains_string("1."))


def test_strip_does_not_join_hex_tail_as_ordered_list_marker():
    # "0000." at end of a wrapped hex literal must NOT be treated as a list marker.
    text = (
        "- Double word value 0x0000 0000 0000\n"
        "0000.\n"
        "- Option bytes for user configuration."
    )
    result = strip_header_footer(text)
    # The "0000." line must not be joined with "- Option bytes"
    assert_that(result, not_(contains_string("0000. - Option bytes")))
    assert_that(result, contains_string("- Option bytes for user configuration."))


# ---------------------------------------------------------------------------
# ToC section-number / title joining
# ---------------------------------------------------------------------------


@pytest.mark.parametrize(
    "level",
    ["#", "##", "###", "####"],
)
def test_strip_joins_toc_section_number_with_following_title(level):
    text = f"{level} **3**\n{level} **Section title . . . 42**"
    result = strip_header_footer(text)
    # Section number and title are both plain; dot-leader is normalised.
    # #### ToC chapter entries are further normalised to ### (see heading-level
    # normalisation step), so the expected level for #### input is ###.
    expected_level = "###" if level == "####" else level
    assert_that(result, matches_regexp(rf"^\{expected_level} 3 Section title \.+ 42$"))


def test_strip_does_not_join_section_number_when_heading_levels_differ():
    text = "#### **3**\n### **Different level title**"
    result = strip_header_footer(text)
    # Join does not fire (different levels) but bold is stripped from both lines.
    assert_that(result, contains_string("#### 3"))


def test_strip_does_not_join_section_number_when_next_line_is_body_text():
    text = "#### **3**\nSome body paragraph text."
    result = strip_header_footer(text)
    # Bold is stripped; the heading line contains the plain number.
    assert_that(result, contains_string("#### 3"))


# ---------------------------------------------------------------------------
# ToC sub-section number / title joining  (plain dotted numbers, no heading prefix)
# ---------------------------------------------------------------------------


@pytest.mark.parametrize(
    "number",
    ["1.1", "2.5", "2.5.3", "10.3.2"],
)
def test_strip_joins_toc_subsection_number_with_following_title(number):
    title = "General information  . . . . . 51"
    text = f"{number}\n{title}"
    result = strip_header_footer(text)
    # Line is prefixed with "- ", section number and title, dot-leader, page number at _TOC_WIDTH
    assert_that(result, matches_regexp(rf"^- {re.escape(number)} General information \.+ 51$"))


def test_strip_does_not_join_plain_integer_with_following_line():
    # Plain integers are stripped as stray artefacts, not joined.
    text = "3\nSome title"
    assert_that(strip_header_footer(text), not_(contains_string("3\nSome title")))


def test_strip_does_not_join_ordered_list_marker_as_subsection_number():
    # "1." is an ordered list marker, not a dotted section number.
    text = "1.\nFirst item"
    assert_that(strip_header_footer(text), equal_to("1. First item"))


# ---------------------------------------------------------------------------
# Bold dot-leader stripping on ToC lines
# ---------------------------------------------------------------------------


def test_strip_removes_bold_markers_from_toc_dot_leader_and_page_number():
    text = "3.3.6 FLASH main memory erase sequences**. . . . . . . . . . . . . . . . . . . . . . . . 69**"
    result = strip_header_footer(text)
    # Bold markers removed, line is a list item, ends with page number 69 right-aligned
    assert_that(result, matches_regexp(r"^- 3\.3\.6 FLASH main memory erase sequences \.+ 69$"))


def test_strip_does_not_remove_bold_from_regular_text():
    text = "Some **important** word in a sentence."
    assert_that(strip_header_footer(text), contains_string("**important**"))


# ---------------------------------------------------------------------------
# _align_toc_page_numbers: page numbers land at column _TOC_WIDTH
# ---------------------------------------------------------------------------


def test_align_toc_page_numbers_right_justifies_page_number():
    # Dot count fills to exactly _TOC_WIDTH: title + " " + dots + " " + page == _TOC_WIDTH.
    text = "1.1 General information . . . 51"
    result = strip_header_footer(text)
    line = [l for l in result.splitlines() if l.startswith("- 1.1")][0]
    assert_that(len(line), equal_to(_TOC_WIDTH))


def test_align_toc_page_numbers_right_justifies_top_level_heading():
    # Supply a line that has already had the heading-join applied (plain number).
    # #### ToC chapter headings are normalised to ### so the expected prefix is ###.
    text = "### 2 Memory and bus architecture . . . . . 53"
    result = strip_header_footer(text)
    line = result.splitlines()[0]
    assert_that(len(line), equal_to(_TOC_WIDTH))


# ---------------------------------------------------------------------------
# ToC sub-section list: blank-line collapse between consecutive items
# ---------------------------------------------------------------------------


def test_strip_collapses_blank_lines_between_toc_subsection_items():
    text = "1.1 General information . . . 51\n\n1.2 List of abbreviations . . . 51"
    result = strip_header_footer(text)
    # Two consecutive list items with no blank line between them
    assert_that(result, matches_regexp(r"(?s)^- 1\.1 General information \.+ 51\n- 1\.2 List of abbreviations \.+ 51$"))


def test_strip_preserves_blank_line_between_top_level_heading_and_first_subsection():
    text = "#### **1** Documentation conventions . . . 51\n\n1.1 General information . . . 51"
    result = strip_header_footer(text)
    assert_that(result, contains_string("\n\n- 1.1 "))


# ---------------------------------------------------------------------------
# Standalone bold title duplicate (e.g. "**Contents**" above "## Contents")
# ---------------------------------------------------------------------------


def test_strip_removes_standalone_bold_title_before_heading():
    text = "**Contents**\n## Contents\n\nSome text."
    assert_that(strip_header_footer(text), not_(matches_regexp(r"(?m)^\*\*Contents\*\*$")))


def test_strip_removes_standalone_bold_title_separated_from_heading_by_blank_line():
    text = "**Contents**\n\n## Contents\n\nSome text."
    assert_that(strip_header_footer(text), not_(matches_regexp(r"(?m)^\*\*Contents\*\*$")))


def test_strip_preserves_bold_title_not_followed_by_heading():
    text = "**Important Note**\n\nSome paragraph text."
    assert_that(strip_header_footer(text), contains_string("**Important Note**"))


# ---------------------------------------------------------------------------
# Wrapped ToC title continuation (e.g. 4.2.1 title split across two PDF lines)
# ---------------------------------------------------------------------------


def test_strip_joins_wrapped_toc_title_continuation():
    text = "4.2.1\nPower-on reset (POR) / power-down reset (PDR) / brown-out reset\n(BOR) . . . . . . . 110"
    result = strip_header_footer(text)
    assert_that(result, matches_regexp(r"^- 4\.2\.1 Power-on reset.+brown-out reset \(BOR\) \.+ 110$"))


# ---------------------------------------------------------------------------
# Section heading number + title join on body pages (no dot-leader)
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("level", ["##", "###", "####"])
def test_strip_joins_body_heading_number_with_title(level):
    text = f"{level} 4.3\n{level} Low-power modes"
    assert_that(strip_header_footer(text), equal_to(f"{level} 4.3 Low-power modes"))


def test_strip_joins_body_heading_number_with_title_different_levels():
    # PDF sometimes emits the section number at level 1 and the title at level 2.
    text = "# 15.3\n## DAC implementation\n\nSome text."
    assert_that(strip_header_footer(text), contains_string("# 15.3 DAC implementation"))


def test_strip_joins_body_heading_with_wrapped_parenthesised_suffix():
    text = "### APB peripheral clock enable in Sleep/Stop mode register 1\n### (RCC_APBSMENR1)"
    assert_that(
        strip_header_footer(text),
        equal_to("### APB peripheral clock enable in Sleep/Stop mode register 1 (RCC_APBSMENR1)"),
    )


# ---------------------------------------------------------------------------
# Mojibake replacement
# ---------------------------------------------------------------------------


@pytest.mark.parametrize(
    "raw, expected",
    [
        ("Cortexﾂｮ-M0+", "Cortex®-M0+"),
        ("sensor窶冱 position", "sensor's position"),
        ("3 ﾃ・ 4096", "3 × 4096"),
    ],
)
def test_strip_replaces_mojibake(raw, expected):
    assert_that(strip_header_footer(raw), equal_to(expected))


def test_strip_removes_garbled_only_line():
    # A line whose content is entirely garbled mojibake symbols should be dropped.
    text = "Normal text.\n\nﾃ・\n\nMore text."
    assert_that(strip_header_footer(text), not_(contains_string("ﾃ")))


# ---------------------------------------------------------------------------
# Missing space before inline bold/italic markers
# ---------------------------------------------------------------------------


def test_strip_inserts_space_before_inline_bold():
    text = "Refer to**Section 1.5: Availability**."
    assert_that(strip_header_footer(text), contains_string("to **Section"))


# ---------------------------------------------------------------------------
# Double list marker collapse
# ---------------------------------------------------------------------------


def test_strip_collapses_double_list_marker():
    text = "- - 1.5 stop bits when transmitting"
    assert_that(strip_header_footer(text), equal_to("- 1.5 stop bits when transmitting"))


# ---------------------------------------------------------------------------
# Heading-wrapped footer lines (### 156/1333  and  ### RM0503 Rev 4)
# ---------------------------------------------------------------------------


def test_strip_removes_heading_wrapped_page_count_footer():
    text = "Some text.\n\n### 156/1333\n### RM0503 Rev 4"
    result = strip_header_footer(text)
    assert_that(result, not_(contains_string("156/1333")))
    assert_that(result, not_(contains_string("RM0503 Rev 4")))


# ---------------------------------------------------------------------------
# Heading number + title separated by a blank line
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("level", ["##", "###", "####"])
def test_strip_joins_body_heading_number_with_title_across_blank_line(level):
    text = f"{level} 15.3\n\n{level} DAC implementation\n\nSome text."
    assert_that(strip_header_footer(text), contains_string(f"{level} 15.3 DAC implementation"))


# ---------------------------------------------------------------------------
# BOLD: figure-legend headings
# ---------------------------------------------------------------------------


def test_strip_removes_bold_colon_figure_legend_heading():
    text = "Some figure.\n\n#### BOLD:clock origin\n\nMore text."
    result = strip_header_footer(text)
    assert_that(result, not_(contains_string("BOLD:")))


# ---------------------------------------------------------------------------
# Enum bit values promoted to headings (should be demoted to plain text)
# ---------------------------------------------------------------------------


@pytest.mark.parametrize(
    "enum_prefix",
    ["0:", "1:", "00:", "01:", "10:", "11:", "0x1F:"],
)
def test_strip_demotes_bit_value_enum_headings(enum_prefix):
    text = f"#### {enum_prefix} Some description"
    result = strip_header_footer(text)
    assert_that(result, not_(matches_regexp(r"^#{1,6} ")))
    assert_that(result, contains_string(f"{enum_prefix} Some description"))


# ---------------------------------------------------------------------------
# Bold-wrapped dotted section number orphan join (e.g. ### **1.1** / ### **Title**)
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("level", ["##", "###", "####"])
def test_strip_joins_bold_wrapped_dotted_number_with_following_title(level):
    text = f"{level} **1.1**\n{level} **General information**"
    result = strip_header_footer(text)
    assert_that(result, equal_to(f"{level} 1.1 General information"))


# ---------------------------------------------------------------------------
# Table / Figure caption split across two PDF lines
# ---------------------------------------------------------------------------


@pytest.mark.parametrize(
    "label",
    ["Table 1.", "Table 42.", "Figure 5.", "Figure 100."],
)
def test_strip_joins_split_table_figure_caption(label):
    text = f"{label}\nSome description of the table or figure."
    result = strip_header_footer(text)
    assert_that(result, equal_to(f"{label} Some description of the table or figure."))


# ---------------------------------------------------------------------------
# Heading demotion — "Example:" and "Refer to" (bold in PDF → heading prefix)
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("level", range(1, 7))
def test_strip_demotes_example_colon_heading(level):
    prefix = "#" * level
    text = f"{prefix} Example: Let us consider the following transfer."
    result = strip_header_footer(text)
    assert_that(result, equal_to("Example: Let us consider the following transfer."))
    assert_that(result, not_(contains_string("#")))


@pytest.mark.parametrize("level", range(1, 7))
def test_strip_demotes_refer_to_heading(level):
    prefix = "#" * level
    text = f"{prefix} Refer to *Section 2.2 on page 55* for the register boundary addresses"
    result = strip_header_footer(text)
    assert_that(result, not_(contains_string("#")))
    assert_that(result, contains_string("Refer to"))


# ---------------------------------------------------------------------------
# Mojibake — sequences that NFKC normalisation would otherwise destroy
# ---------------------------------------------------------------------------


def test_mojibake_registered_trademark_in_cell_text():
    """ﾂｮ must be replaced with ® even when it appears in table-like text."""
    from process_datasheets.pdf_to_markdown import _normalise_cell

    assert_that(_normalise_cell("Armﾂｮ JEDEC code"), equal_to("Arm® JEDEC code"))


def test_mojibake_apostrophe_t_in_cell_text():
    from process_datasheets.pdf_to_markdown import _normalise_cell

    assert_that(_normalise_cell("don窶冲 care"), equal_to("don't care"))


def test_mojibake_span_registered_trademark():
    """ﾂｮ must be replaced with ® in span text before NFKC normalisation."""
    from process_datasheets.pdf_to_markdown import _normalise_span_text

    assert_that(_normalise_span_text("Armﾂｮ JEDEC code"), equal_to("Arm® JEDEC code"))


# ---------------------------------------------------------------------------
# Reserved bit-field lines promoted to #### headings
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("line", [
    "Bits 31:24 Reserved, must be kept at reset value.",
    "Bits 15:8 Reserved, must be kept at reset value.",
    "Bit 7 Reserved, must be kept at reset value.",
    "Bits 3:2 Reserved, must be kept at reset value.",
])
def test_strip_promotes_reserved_bits_to_heading(line):
    result = strip_header_footer(line)
    assert_that(result, equal_to(f"#### {line}"))


def test_strip_reserved_bits_matches_named_field_heading_level():
    """Reserved and named fields in the same register should both be ####."""
    text = (
        "Bits 31:24 Reserved, must be kept at reset value.\n\n"
        "#### Bits 23:16 MYFIELD[7:0]: some field\n"
        "Description text.\n\n"
        "Bits 15:8 Reserved, must be kept at reset value."
    )
    result = strip_header_footer(text)
    assert_that(result, contains_string("#### Bits 31:24 Reserved"))
    assert_that(result, contains_string("#### Bits 15:8 Reserved"))
    assert_that(result, contains_string("#### Bits 23:16 MYFIELD"))


# ---------------------------------------------------------------------------
# Prose-sentence bold lines demoted from headings
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("opener", [
    "The elapsed time between the start",
    "This flag is set by hardware",
    "If channel CC1 is configured as output",
    "When a data is received",
    "In this mode the counter",
    "After a system reset",
    "For each channel",
    "A similar procedure must be followed",
    "An interrupt is generated",
    "Set by software and cleared by hardware",
    "These bits are updated",
    "Each change of the RTC_CR register",
])
def test_strip_demotes_prose_sentence_heading(opener):
    """Bold sentence-fragment lines must not become headings."""
    for level in range(1, 7):
        prefix = "#" * level
        result = strip_header_footer(f"{prefix} {opener} here.")
        assert_that(result, not_(matches_regexp(r"^#{1,6} ")),
                    reason=f"level {level}: {opener!r} should be plain text")


def test_strip_demotes_lowercase_continuation_heading():
    """Wrapped continuation lines that start lowercase must not be headings."""
    text = "### the configured sampling time plus the successive approximation time"
    result = strip_header_footer(text)
    assert_that(result, not_(contains_string("#")))
    assert_that(result, contains_string("the configured sampling"))


# ---------------------------------------------------------------------------
# Mojibake — μ (micro sign)
# ---------------------------------------------------------------------------


def test_mojibake_micro_sign_in_cell_text():
    from process_datasheets.pdf_to_markdown import _normalise_cell

    assert_that(_normalise_cell("0.400 ﾎｼs"), equal_to("0.400 μs"))


def test_mojibake_micro_sign_in_span_text():
    from process_datasheets.pdf_to_markdown import _normalise_span_text

    assert_that(_normalise_span_text("42 ﾎｼs"), equal_to("42 μs"))


# ---------------------------------------------------------------------------
# ToC chapter heading level normalisation (#### → ###)
# ---------------------------------------------------------------------------


def test_toc_chapter_heading_promoted_from_level4_to_level3():
    """#### chapter entries on ToC pages must be normalised to ###."""
    text = (
        "## Contents\n\n"
        "#### 1 Documentation conventions ................................................................ 51\n\n"
        "- 1.1 General information ....................................................................... 51\n\n"
        "#### 2 Memory and bus architecture .............................................................. 53\n\n"
        "- 2.1 System architecture ....................................................................... 53"
    )
    result = strip_header_footer(text)
    assert_that(result, not_(contains_string("#### 1")))
    assert_that(result, not_(contains_string("#### 2")))
    assert_that(result, contains_string("### 1 Documentation conventions"))
    assert_that(result, contains_string("### 2 Memory and bus architecture"))


def test_toc_chapter_heading_normalisation_preserves_subsection_list():
    """Sub-section list items (- N.M ...) are unchanged by the normalisation."""
    text = (
        "#### 3 Embedded flash memory (FLASH) ............................................................ 64\n\n"
        "- 3.1 FLASH introduction ....................................................................... 64\n"
        "- 3.2 FLASH main features ....................................................................... 64"
    )
    result = strip_header_footer(text)
    assert_that(result, contains_string("- 3.1 FLASH introduction"))
    assert_that(result, contains_string("- 3.2 FLASH main features"))


def test_non_toc_level4_heading_not_affected():
    """#### headings that are not ToC entries (no dot-leader) are left unchanged."""
    text = "#### Bits 31:8 Reserved, must be kept at reset value."
    result = strip_header_footer(text)
    assert_that(result, contains_string("#### Bits 31:8 Reserved"))


# ---------------------------------------------------------------------------
# ToC chapter heading — wrapped title joining
# ---------------------------------------------------------------------------


def test_toc_chapter_heading_wrapped_title_is_joined():
    """A chapter heading whose title wraps onto the next line must be joined.

    Long chapter titles in the PDF are sometimes split across two lines so that
    the heading line carries the first part only (no dot-leader) and the
    continuation carries the rest of the title plus the dot-leader and page
    number.  Both lines must be merged into a single ### heading line.

    Real example: sections 34 (USART/UART) and 35 (LPUART) in RM0503.
    """
    text = (
        "### 34 Universal synchronous/asynchronous receiver\n"
        "transmitter (USART/UART) ...................................................................... 1016\n\n"
        "- 34.1 Introduction ........................................................................... 1016"
    )
    result = strip_header_footer(text)
    first_line = result.splitlines()[0]
    assert_that(first_line, contains_string("### 34 Universal synchronous/asynchronous receiver transmitter (USART/UART)"))
    assert_that(first_line, contains_string("1016"))
    # Must be a single line — no newline within the heading
    assert_that(result, not_(contains_string("### 34 Universal synchronous/asynchronous receiver\n")))


def test_toc_chapter_heading_wrapped_title_subsections_preserved():
    """Sub-section list items following a wrapped chapter heading are not affected."""
    text = (
        "### 35 Low-power universal asynchronous receiver\n"
        "transmitter (LPUART) .......................................................................... 1106\n\n"
        "- 35.1 Introduction ........................................................................... 1106\n"
        "- 35.2 LPUART main features ................................................................... 1106"
    )
    result = strip_header_footer(text)
    assert_that(result, contains_string("- 35.1 Introduction"))
    assert_that(result, contains_string("- 35.2 LPUART main features"))

