"""Tests for heading_level."""

import pytest
from hamcrest import assert_that, equal_to, none

from process_datasheets.pdf_to_markdown import heading_level

BODY = 10.0


@pytest.mark.parametrize(
    "size, bold, expected",
    [
        # Ratios >= 1.8 -> H1
        (18.0, False, 1),
        (20.0, True, 1),
        # Ratios >= 1.4 but < 1.8 -> H2
        (14.0, False, 2),
        (17.9, False, 2),
        # Ratios >= 1.15 but < 1.4 -> H3
        (11.5, False, 3),
        (13.9, False, 3),
        # Exactly body size but bold -> H4
        (10.0, True, 4),
        (11.4, True, 4),
        # Body size, not bold -> None
        (10.0, False, None),
        # Below body size -> None
        (8.0, False, None),
        (9.9, True, None),
    ],
)
def test_heading_level(size, bold, expected):
    result = heading_level(size, BODY, bold)
    if expected is None:
        assert_that(result, none())
    else:
        assert_that(result, equal_to(expected))


def test_heading_level_zero_body_size_does_not_raise():
    assert_that(heading_level(10.0, 0.0, False), none())
