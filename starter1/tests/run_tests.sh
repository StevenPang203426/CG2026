#!/usr/bin/env bash
# Run each swp example with your build and the sample solution side-by-side.
# Usage: cd starter1 && bash tests/run_tests.sh [FILTER]
#   FILTER (optional): only run tests whose name contains this string
#   e.g.  bash tests/run_tests.sh florus

set -euo pipefail

MY_BIN="./build/a1"
REF_BIN="./sample_solution/athena/a1"
SWP_DIR="./swp"

# Colour codes
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
CYAN="\033[0;36m"
RESET="\033[0m"

FILTER="${1:-}"

# Collect swp files
SWP_FILES=("$SWP_DIR"/*.swp)

if [ ${#SWP_FILES[@]} -eq 0 ]; then
    echo "No .swp files found in $SWP_DIR"
    exit 1
fi

echo ""
echo "========================================"
echo "  Assignment 1 – Test Runner"
echo "========================================"
echo "  My binary : $MY_BIN"
echo "  Reference : $REF_BIN"
echo "  SWP dir   : $SWP_DIR"
echo "========================================"
echo ""

for swp in "${SWP_FILES[@]}"; do
    name=$(basename "$swp" .swp)

    # Apply optional filter
    if [ -n "$FILTER" ] && [[ "$name" != *"$FILTER"* ]]; then
        continue
    fi

    echo -e "${CYAN}──────────────────────────────────────${RESET}"
    echo -e "${CYAN}  Test: $name${RESET}"
    echo -e "${CYAN}──────────────────────────────────────${RESET}"

    # ── Your implementation ──────────────────────────────────────────────────
    echo -e "${YELLOW}[YOUR BUILD]  $MY_BIN $swp${RESET}"
    if [ -x "$MY_BIN" ]; then
        "$MY_BIN" "$swp"
    else
        echo "  ✗  Binary not found or not executable: $MY_BIN"
    fi

    echo ""

    # ── Reference / sample solution ──────────────────────────────────────────
    echo -e "${GREEN}[REFERENCE]   $REF_BIN $swp${RESET}"
    if [ -x "$REF_BIN" ]; then
        "$REF_BIN" "$swp"
    else
        echo "  ✗  Reference binary not found or not executable: $REF_BIN"
    fi

    echo ""
done

echo -e "${CYAN}========================================"
echo -e "  All tests done."
echo -e "========================================${RESET}"
echo ""
