#include <emscripten.h>
#include "game.h"

EMSCRIPTEN_KEEPALIVE void cf_init_game(void) { cf_init_game_impl(); }
EMSCRIPTEN_KEEPALIVE void cf_start_game(void) { cf_start_game_impl(); }
EMSCRIPTEN_KEEPALIVE void cf_start_game_ex(int ai_mode, int ai_difficulty) { cf_start_game_ex_impl(ai_mode, ai_difficulty); }
EMSCRIPTEN_KEEPALIVE int cf_drop_disc(int col) { return cf_drop_disc_impl(col); }
EMSCRIPTEN_KEEPALIVE int cf_get_cell(int row, int col) { return cf_get_cell_impl(row, col); }
EMSCRIPTEN_KEEPALIVE int cf_can_drop(int col) { return cf_can_drop_impl(col); }
EMSCRIPTEN_KEEPALIVE int cf_get_state(void) { return cf_get_state_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_player(void) { return cf_get_player_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_winner(void) { return cf_get_winner_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_move_count(void) { return cf_get_move_count_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_win_x1(void) { return cf_get_win_x1_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_win_y1(void) { return cf_get_win_y1_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_win_x2(void) { return cf_get_win_x2_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_win_y2(void) { return cf_get_win_y2_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_win_type(void) { return cf_get_win_type_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_last_col(void) { return cf_get_last_col_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_last_row(void) { return cf_get_last_row_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_ai_mode(void) { return cf_get_ai_mode_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_get_ai_difficulty(void) { return cf_get_ai_difficulty_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_is_ai_thinking(void) { return cf_is_ai_thinking_impl(); }
EMSCRIPTEN_KEEPALIVE int cf_ai_move(void) { return cf_ai_move_impl(); }
