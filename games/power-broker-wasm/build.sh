#!/bin/bash
set -e
cd "$(dirname "$0")"

echo "=== Building Power Broker WASM ==="

# Compile C to WASM
emcc -o power_broker.js \
  game.c wasm_main.c \
  -Os \
  -s WASM=1 \
  -s EXPORTED_FUNCTIONS='["_init","_new_game","_get_state","_get_day","_get_cash","_get_power","_get_price","_get_profit","_get_total_assets","_get_win_streak","_get_boss","_get_ach_count","_ach_unlocked","_get_event_name","_get_event_desc","_get_event_impact","_player_action","_end_day","_back_to_menu"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","memory"]' \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s INITIAL_MEMORY=16MB \
  -s MODULARIZE=0 \
  -s STRICT=0 \
  --no-entry \
  2>&1

echo "✅ WASM built: power_broker.wasm + power_broker.js"
ls -lh power_broker.wasm power_broker.js
