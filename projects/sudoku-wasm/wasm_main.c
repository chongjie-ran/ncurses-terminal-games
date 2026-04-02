#include "game.h"
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
void sudoku_start_game(int diff) { sudoku_start_game_impl(diff); }

EMSCRIPTEN_KEEPALIVE
void sudoku_select_cell(int row, int col) { sudoku_select_cell_impl(row, col); }

EMSCRIPTEN_KEEPALIVE
void sudoku_input_digit(int digit) { sudoku_input_digit_impl(digit); }

EMSCRIPTEN_KEEPALIVE
void sudoku_erase(void) { sudoku_erase_impl(); }

EMSCRIPTEN_KEEPALIVE
void sudoku_toggle_candidates(void) { sudoku_toggle_candidates_impl(); }

EMSCRIPTEN_KEEPALIVE
void sudoku_hint(void) { sudoku_hint_impl(); }

EMSCRIPTEN_KEEPALIVE
void sudoku_solve(void) { sudoku_solve_impl(); }

EMSCRIPTEN_KEEPALIVE
void sudoku_reset(void) { sudoku_reset_impl(); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_cell(int row, int col) { return sudoku_get_cell_impl(row, col); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_fixed(int row, int col) { return sudoku_get_fixed_impl(row, col); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_candidates(int row, int col) { return sudoku_get_candidates_impl(row, col); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_state(void) { return sudoku_get_state_impl(); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_selected_row(void) { return sudoku_get_selected_row_impl(); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_selected_col(void) { return sudoku_get_selected_col_impl(); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_selected_num(void) { return sudoku_get_selected_num_impl(); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_error_count(void) { return sudoku_get_error_count_impl(); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_hints_used(void) { return sudoku_get_hints_used_impl(); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_difficulty(void) { return sudoku_get_difficulty_impl(); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_timer(void) { return sudoku_get_timer_impl(); }

EMSCRIPTEN_KEEPALIVE
void sudoku_set_timer(int seconds) { sudoku_set_timer_impl(seconds); }

EMSCRIPTEN_KEEPALIVE
int sudoku_get_show_candidates(void) { return sudoku_get_show_candidates_impl(); }

EMSCRIPTEN_KEEPALIVE
int sudoku_check_valid(int row, int col, int digit) { return sudoku_check_valid_impl(row, col, digit) ? 1 : 0; }

EMSCRIPTEN_KEEPALIVE
int sudoku_is_solved(void) { return sudoku_is_solved_impl() ? 1 : 0; }
