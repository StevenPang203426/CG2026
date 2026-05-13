#!/bin/bash
# Run script for Project 2 - generate all scene images
# Usage: bash run.sh [size] [output_dir]
#   size       - render resolution, default 800
#   output_dir - where to save PNGs, default ./output

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
EXE="$SCRIPT_DIR/build/a2"
SIZE=${1:-800}
OUT_DIR="${2:-$SCRIPT_DIR/output}"

# Check executable
if [ ! -f "$EXE" ]; then
    echo "Error: $EXE not found. Run build.sh first."
    exit 1
fi

# Create output directory
mkdir -p "$OUT_DIR"

echo "=== Project 2 Renderer ==="
echo "Resolution: ${SIZE}x${SIZE}"
echo "Output dir: $OUT_DIR"
echo ""

# Scene 1-5: ray casting (no bounces needed)
for i in 01 02 03 04 05; do
    SCENE="data/scene${i}_*.txt"
    SCENE_FILE=$(ls $SCRIPT_DIR/$SCENE 2>/dev/null | head -1)
    if [ -z "$SCENE_FILE" ]; then
        echo "[Scene $i] SKIP - scene file not found"
        continue
    fi
    echo -n "[Scene $i] Rendering... "
    "$EXE" \
        -input "$SCENE_FILE" \
        -output "$OUT_DIR/a${i}.png" \
        -normals "$OUT_DIR/a${i}n.png" \
        -depth "$OUT_DIR/a${i}d.png" 8 12 \
        -size "$SIZE" "$SIZE"
    echo "done"
done

# Scene 6: bunny with ray tracing (bounces=4)
echo -n "[Scene 06] Rendering (ray tracing)... "
"$EXE" \
    -input "$SCRIPT_DIR/data/scene06_bunny_1k.txt" \
    -output "$OUT_DIR/a06.png" \
    -normals "$OUT_DIR/a06n.png" \
    -depth "$OUT_DIR/a06d.png" 8 12 \
    -size "$SIZE" "$SIZE" \
    -bounces 4
echo "done"

# Scene 7: arch with shadows + ray tracing (bounces=4)
echo -n "[Scene 07] Rendering (shadows + ray tracing)... "
"$EXE" \
    -input "$SCRIPT_DIR/data/scene07_arch.txt" \
    -output "$OUT_DIR/a07.png" \
    -normals "$OUT_DIR/a07n.png" \
    -depth "$OUT_DIR/a07d.png" 8 12 \
    -size "$SIZE" "$SIZE" \
    -shadows -bounces 4
echo "done"

echo ""
echo "=== All scenes rendered ==="
echo "Images saved to: $OUT_DIR/"
ls -lh "$OUT_DIR"/*.png 2>/dev/null | awk '{print "  "$NF, $5}'
