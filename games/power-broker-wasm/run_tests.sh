#!/bin/bash
# Run C unit tests (native compile, no WASM needed)
set -e
cd "$(dirname "$0")"
echo "=== Compiling C unit tests ==="
gcc -o game_test game_test.c game.c -lm -O0
echo "=== Running unit tests ==="
./game_test
rm -f game_test
echo "=== All C unit tests passed ==="
