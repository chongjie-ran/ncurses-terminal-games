#!/bin/bash
set -e
cd "$(dirname "$0")"
echo "=== Building Model League WASM ==="
emcc -c game.c -o game.o -O3
emcc -c wasm_main.c -o wasm_main.o -O3
emcc game.o wasm_main.o -o model_league.js -s WASM=1 \
  -s "EXPORTED_FUNCTIONS=['_wasm_init','_wasm_new_game','_wasm_get_state','_wasm_get_gold','_wasm_get_exp','_wasm_get_level','_wasm_get_cycle','_wasm_get_faction','_wasm_get_card_count','_wasm_card_owned','_wasm_card_atk','_wasm_card_def','_wasm_card_spd','_wasm_card_cost','_wasm_card_rarity','_wasm_card_skill','_wasm_card_lv','_wasm_card_name','_wasm_deck_size','_wasm_deck_card','_wasm_shop_buy','_wasm_deck_toggle','_wasm_upgrade_card','_wasm_set_faction','_wasm_battle_fight','_wasm_mine_gold','_wasm_next_cycle','_wasm_navigate','_wasm_back','_wasm_get_msg']" \
  -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=16MB -s MODULARIZE=0 -s STRICT=0 -s RESERVED_FUNCTION_POINTERS=20
rm -f game.o wasm_main.o
ls -lh model_league.wasm model_league.js
echo "Done"
