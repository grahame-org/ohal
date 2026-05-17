#!/usr/bin/env bash
# extract-datasheet-pages.sh — extract pages from a datasheet PDF to Markdown.
#
# Usage:
#   bash extract-datasheet-pages.sh <pdf> <pages> [--output-dir <dir>]
#
# Arguments:
#   <pdf>     Path to the PDF file.
#   <pages>   Comma-separated page numbers or inclusive ranges (1-based),
#             e.g. '42-44,100,200-205'.
#
# Options:
#   --output-dir <dir>   Directory for output .md files.
#                        Defaults to claude/rm0503/ inside the repo root.
#
# Example:
#   bash extract-datasheet-pages.sh \
#       docs/rm0503-stm32u0-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf \
#       42-44,100 \
#       --output-dir claude/rm0503

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEV_SCRIPT="$REPO_ROOT/scripts/process_datasheets/dev.sh"
VENV_DIR="$REPO_ROOT/scripts/process_datasheets/.venv"

# ---------------------------------------------------------------------------
# Arguments
# ---------------------------------------------------------------------------
if [[ $# -lt 2 ]]; then
    echo "Usage: bash extract-datasheet-pages.sh <pdf> <pages> [--output-dir <dir>]" >&2
    exit 1
fi

PDF="$1"
PAGES="$2"
shift 2

# Default output directory; may be overridden by --output-dir below.
OUTPUT_DIR="$REPO_ROOT/claude/rm0503"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --output-dir)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        *)
            echo "ERROR: unknown argument '$1'" >&2
            exit 1
            ;;
    esac
done

# ---------------------------------------------------------------------------
# Bootstrap: create venv and install dependencies if not already done
# ---------------------------------------------------------------------------
if [[ ! -d "$VENV_DIR" ]]; then
    echo "venv not found — running bootstrap …"
    bash "$DEV_SCRIPT"
fi

# Activate the venv
if [[ -f "$VENV_DIR/Scripts/activate" ]]; then
    # shellcheck source=/dev/null
    source "$VENV_DIR/Scripts/activate"   # Windows (Git-Bash / MSYS2)
else
    # shellcheck source=/dev/null
    source "$VENV_DIR/bin/activate"       # Linux / macOS
fi

# ---------------------------------------------------------------------------
# Extract pages
# ---------------------------------------------------------------------------
process-datasheets "$PDF" "$PAGES" --output-dir "$OUTPUT_DIR"
