#include <emscripten/emscripten.h>
#include "game.h"

EMSCRIPTEN_KEEPALIVE
void nono_start_game(void) {
    nono_init_game();
}

EMSCRIPTEN_KEEPALIVE
void nono_handle_click(int row, int col) {
    nono_click(row, col);
}

EMSCRIPTEN_KEEPALIVE
int nono_get_grid_cell(int row, int col) {
    return nono_get_cell(row, col);
}

EMSCRIPTEN_KEEPALIVE
int nono_get_game_rows(void) {
    return nono_get_rows();
}

EMSCRIPTEN_KEEPALIVE
int nono_get_game_cols(void) {
    return nono_get_cols();
}

EMSCRIPTEN_KEEPALIVE
int nono_get_game_won(void) {
    return nono_get_won();
}

EMSCRIPTEN_KEEPALIVE
int nono_fetch_row_clue(int row, int idx) {
    return nono_get_row_clue(row, idx);
}

EMSCRIPTEN_KEEPALIVE
int nono_fetch_row_clue_count(int row) {
    return nono_get_row_clue_count(row);
}

EMSCRIPTEN_KEEPALIVE
int nono_fetch_col_clue(int col, int idx) {
    return nono_get_col_clue(col, idx);
}

EMSCRIPTEN_KEEPALIVE
int nono_fetch_col_clue_count(int col) {
    return nono_get_col_clue_count(col);
}
