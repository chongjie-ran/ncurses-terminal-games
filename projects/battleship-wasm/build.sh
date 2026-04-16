#!/bin/bash
set -e
cd "$(dirname "$0")"
echo "=== Compiling battleship WASM ==="
emcc game.c wasm_main.c \
  -o battleship.js \
  -s STANDALONE_WASM=1 \
  -s EXPORTED_FUNCTIONS='["_battleship_init","_battleship_reset","_battleship_get_cell","_battleship_get_game_state","_battleship_get_ships_sunk","_battleship_get_computer_ships_sunk","_battleship_get_hit_count","_battleship_get_miss_count","_battleship_get_ship_count","_battleship_place_ship","_battleship_shoot","_battleship_computer_turn","_malloc","_free"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue"]' \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="BattleshipModule" \
  -O2 \
  --no-entry
echo "=== Done ==="
ls -la battleship.wasm battleship.js 2>/dev/null
