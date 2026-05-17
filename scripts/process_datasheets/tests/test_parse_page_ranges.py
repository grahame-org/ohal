"""Tests for parse_page_ranges."""

import pytest
from hamcrest import assert_that, equal_to, contains_exactly

from process_datasheets.pdf_to_markdown import parse_page_ranges


@pytest.mark.parametrize(
    "spec, expected",
    [
        ("1", [1]),
        ("42", [42]),
        ("1,2,3", [1, 2, 3]),
        ("5-8", [5, 6, 7, 8]),
        ("1,5-7,10", [1, 5, 6, 7, 10]),
        ("10,1,5", [1, 5, 10]),          # sorted output
        ("3-3", [3]),                     # single-element range
        (" 4 , 6 ", [4, 6]),             # whitespace tolerance
        ("1-3,2-4", [1, 2, 3, 4]),       # overlapping ranges deduplicated
    ],
)
def test_parse_page_ranges_valid(spec, expected):
    assert_that(parse_page_ranges(spec), equal_to(expected))


@pytest.mark.parametrize(
    "spec",
    [
        "10-5",       # start > end
        "abc",        # non-numeric
        "1,abc,3",    # non-numeric in list
        "1-2-3",      # too many dashes
    ],
)
def test_parse_page_ranges_invalid_raises(spec):
    with pytest.raises(ValueError):
        parse_page_ranges(spec)


def test_parse_page_ranges_empty_parts_ignored():
    assert_that(parse_page_ranges("1,,3"), equal_to([1, 3]))
