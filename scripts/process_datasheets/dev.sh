#!/usr/bin/env bash
# dev.sh — bootstrap the process-datasheets development environment.
#
# Usage (from any directory):
#   bash scripts/process_datasheets/dev.sh [-- <pytest-args>…]
#
# What it does:
#   1. Locates the script's own directory (works regardless of cwd).
#   2. Creates a virtual environment at <script-dir>/.venv if one does not
#      already exist.
#   3. Installs / upgrades the package and its dev dependencies into the venv.
#   4. Runs the test suite (passes any extra arguments through to pytest).
#
# To activate the venv manually afterwards:
#   source scripts/process_datasheets/.venv/bin/activate   # Linux/macOS
#   scripts/process_datasheets/.venv/Scripts/activate      # Windows Git-Bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$SCRIPT_DIR/.venv"

# ---------------------------------------------------------------------------
# 1. Resolve python interpreter
# ---------------------------------------------------------------------------
if command -v python3 &>/dev/null; then
    PYTHON=python3
elif command -v python &>/dev/null; then
    PYTHON=python
else
    echo "ERROR: no python3 or python found on PATH" >&2
    exit 1
fi

# Require at least Python 3.11 (matches pyproject.toml requires-python)
PYTHON_VERSION=$("$PYTHON" -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")
REQUIRED_MAJOR=3
REQUIRED_MINOR=11
IFS='.' read -r py_major py_minor <<<"$PYTHON_VERSION"
if (( py_major < REQUIRED_MAJOR || (py_major == REQUIRED_MAJOR && py_minor < REQUIRED_MINOR) )); then
    echo "ERROR: Python $REQUIRED_MAJOR.$REQUIRED_MINOR+ required, found $PYTHON_VERSION" >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# 2. Create venv if missing
# ---------------------------------------------------------------------------
if [[ ! -d "$VENV_DIR" ]]; then
    echo "Creating virtual environment at $VENV_DIR …"
    "$PYTHON" -m venv "$VENV_DIR"
fi

# Activate the venv for the remainder of this script
if [[ -f "$VENV_DIR/Scripts/activate" ]]; then
    # shellcheck source=/dev/null
    source "$VENV_DIR/Scripts/activate"   # Windows (Git-Bash / MSYS2)
else
    # shellcheck source=/dev/null
    source "$VENV_DIR/bin/activate"       # Linux / macOS
fi

# ---------------------------------------------------------------------------
# 3. Install / upgrade package + dev dependencies
# ---------------------------------------------------------------------------
echo "Installing process-datasheets[dev] …"
python -m pip install --quiet --upgrade pip
python -m pip install --quiet -e "$SCRIPT_DIR"
python -m pip install --quiet "PyHamcrest>=2" "pyfakefs>=5" "pytest>=8"

# ---------------------------------------------------------------------------
# 4. Run tests (forward any extra args supplied after --)
# ---------------------------------------------------------------------------
# Collect arguments that appear after an optional "--" separator.
pytest_args=()
collecting=false
for arg in "$@"; do
    if [[ "$arg" == "--" ]]; then
        collecting=true
        continue
    fi
    if $collecting; then
        pytest_args+=("$arg")
    fi
done

echo ""
echo "Running tests …"
pytest "$SCRIPT_DIR/tests" "${pytest_args[@]}"
