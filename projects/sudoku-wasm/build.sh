#!/bin/bash
source ~/emsdk/emsdk_env.sh 2>/dev/null || { echo "Emscripten not found"; exit 1; }

GAME="sudoku"
OUT="sudoku"

emcc -O3 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="SudokuModule" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=64MB \
  -s EXPORTED_FUNCTIONS='["_sudoku_start_game","_sudoku_select_cell","_sudoku_input_digit","_sudoku_erase","_sudoku_toggle_candidates","_sudoku_hint","_sudoku_solve","_sudoku_reset","_sudoku_get_cell","_sudoku_get_fixed","_sudoku_get_candidates","_sudoku_get_state","_sudoku_get_selected_row","_sudoku_get_selected_col","_sudoku_get_selected_num","_sudoku_get_error_count","_sudoku_get_hints_used","_sudoku_get_difficulty","_sudoku_get_timer","_sudoku_set_timer","_sudoku_get_show_candidates","_sudoku_check_valid","_sudoku_is_solved"]' \
  --no-entry -o ${OUT}.js game.c wasm_main.c

echo "Build complete: ${OUT}.js + ${OUT}.wasm"
ls -la ${OUT}.*
