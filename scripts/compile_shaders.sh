#!/bin/bash
set -e

GLSLC="/Users/danielcho/VulkanSDK/1.4.313.0/macOS/bin/glslc"
SRC_DIR="src/shaders"
OUT_DIR="resources/shaders"

mkdir -p "$OUT_DIR"

# Find all .vert and .frag shaders recursively
find "$SRC_DIR" -type f \( -name "*.vert" -o -name "*.frag" \) | while read -r shader; do
    filename="$(basename "$shader")"
    name="${filename%.*}"        # shader
    ext="${filename##*.}"        # vert or frag

    output="$OUT_DIR/${name}_${ext}.spv"

    echo "Compiling $shader → $output"
    "$GLSLC" "$shader" -o "$output"
done
