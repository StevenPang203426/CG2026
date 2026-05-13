#!/bin/bash
# Build script for Project 2
# Usage: bash build.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

echo "=== Project 2 Build ==="

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake
echo "[1/2] Running CMake..."
cmake ..

# Compile
echo "[2/2] Compiling..."
make -j$(nproc)

echo ""
echo "=== Build successful! ==="
echo "Executable: $BUILD_DIR/a2"
