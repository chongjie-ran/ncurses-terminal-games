#!/bin/bash
source ~/emsdk/emsdk_env.sh 2>/dev/null || { echo "Emscripten not found"; exit 1; }
set -e
echo "=== Building Battleship WASM ==="
emcc game.c wasm_main.c \
  -o battleship.js \
  -s MODULARIZE=1 \
  -s "EXPORTED_FUNCTIONS=['_battleship_init','_battleship_reset','_battleship_get_cell','_battleship_get_game_state','_battleship_get_ships_sunk','_battleship_get_hit_count','_battleship_get_miss_count','_battleship_get_ship_count','_battleship_place_ship','_battleship_shoot']" \
  -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap','getValue']" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s INITIAL_MEMORY=16777216 \
  -s TOTAL_STACK=2097152 \
  -s WASM=1 \
  -O2 \
  -std=c99
echo "Build successful: battleship.js + battleship.wasm"
