#!/bin/bash
source ~/emsdk/emsdk_env.sh 2>/dev/null || { echo "Emscripten not found"; exit 1; }

GAME="connect-four"
OUT="connect4"

emcc -O3 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="Connect4Module" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=64MB \
  -s EXPORTED_FUNCTIONS='["_cf_start_game","_cf_drop_disc","_cf_get_cell","_cf_can_drop","_cf_get_state","_cf_get_player","_cf_get_winner","_cf_get_win_x1","_cf_get_win_y1","_cf_get_win_x2","_cf_get_win_y2"]' \
  --no-entry -o ${OUT}.js game.c wasm_main.c

echo "Build complete: ${OUT}.js + ${OUT}.wasm"
ls -la ${OUT}.*
