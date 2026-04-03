#!/bin/bash
set -e
cd "$(dirname "$0")"
source ~/emsdk/emsdk_env.sh
emcc -O3 -s MODULARIZE=1 -s EXPORT_NAME="Connect4AIModule" -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=64MB -s EXPORTED_FUNCTIONS="[_cf_init_game,_cf_start_game,_cf_start_game_ex,_cf_drop_disc,_cf_get_cell,_cf_can_drop,_cf_get_state,_cf_get_player,_cf_get_winner,_cf_get_win_x1,_cf_get_win_y1,_cf_get_win_x2,_cf_get_win_y2,_cf_get_win_type,_cf_get_last_col,_cf_get_last_row,_cf_get_ai_mode,_cf_is_ai_thinking,_cf_ai_move]" --no-entry -o connect4ai.js game.c wasm_main.c
echo "Build OK"
