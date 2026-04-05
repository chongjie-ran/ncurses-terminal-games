#!/bin/bash
cd "$(dirname "$0")"
source ~/emsdk/emsdk_env.sh 2>/dev/null || true

emcc -O3 \
  -s MODULARIZE=0 \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=64MB \
  -s EXPORTED_FUNCTIONS='["_nono_start_game","_nono_handle_click","_nono_get_grid_cell","_nono_get_game_rows","_nono_get_game_cols","_nono_get_game_won","_nono_fetch_row_clue","_nono_fetch_row_clue_count","_nono_fetch_col_clue","_nono_fetch_col_clue_count"]' \
  --no-entry -o nonogram.js game.c wasm_main.c

echo "Build complete: nonogram.js + nonogram.wasm"
ls -la nonogram.*
