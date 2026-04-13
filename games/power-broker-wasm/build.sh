#!/bin/bash
set -e
cd "$(dirname "$0")"
echo "=== Building Power Broker WASM ==="
emcc -c game.c -o game.o -O3
emcc -c wasm_main.c -o wasm_main.o -O3
emcc game.o wasm_main.o -o power_broker.js -s WASM=1 \
  -s "EXPORTED_FUNCTIONS=['_wasm_init','_wasm_new_game','_wasm_get_state','_wasm_get_day','_wasm_get_cash','_wasm_get_power','_wasm_get_price','_wasm_get_profit','_wasm_get_total_assets','_wasm_get_win_streak','_wasm_is_boss_day','_wasm_get_ach_count','_wasm_get_ach_unlocked','_wasm_get_event_name','_wasm_get_event_desc','_wasm_get_event_impact','_wasm_player_action','_wasm_end_day','_wasm_return_to_menu']" \
  -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=16MB -s MODULARIZE=0 -s STRICT=0 -s RESERVED_FUNCTION_POINTERS=20
rm -f game.o wasm_main.o
ls -lh power_broker.wasm power_broker.js
echo "Done"
