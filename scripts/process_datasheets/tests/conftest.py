"""Shared pytest fixtures for process_datasheets tests."""

from unittest.mock import MagicMock, patch

import pytest


@pytest.fixture(autouse=True)
def _mock_prettier_subprocess():
    """Suppress the prettier subprocess call in all tests.

    The prettier post-processing step in cli.py calls ``subprocess.run``
    to invoke ``npx prettier --write``.  In unit/integration tests we do
    not want to spawn a real process (npx may not be available, and
    pyfakefs intercepts filesystem calls that subprocess would need).
    This autouse fixture replaces ``subprocess.run`` with a no-op mock
    for the duration of every test.
    """
    with patch("process_datasheets.cli.subprocess") as mock_sp:
        mock_result = MagicMock()
        mock_result.returncode = 0
        mock_sp.run.return_value = mock_result
        yield mock_sp
