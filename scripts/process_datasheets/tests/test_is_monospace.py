"""Tests for is_monospace."""

import pytest
from hamcrest import assert_that, equal_to

from process_datasheets.pdf_to_markdown import is_monospace


@pytest.mark.parametrize(
    "font_name",
    [
        "Courier",
        "CourierNew",
        "COUR",
        "Consolas",
        "Inconsolata",
        "LucidaConsole",
        "SomeMono",
        "DejaVuSansMono",
    ],
)
def test_is_monospace_returns_true_for_monospace_fonts(font_name):
    assert_that(is_monospace(font_name), equal_to(True))


@pytest.mark.parametrize(
    "font_name",
    [
        "Arial",
        "Times New Roman",
        "Helvetica",
        "Calibri",
        "Georgia",
    ],
)
def test_is_monospace_returns_false_for_proportional_fonts(font_name):
    assert_that(is_monospace(font_name), equal_to(False))
