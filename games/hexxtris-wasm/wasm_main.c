// wasm_main.c - Hextris Emscripten exports
#include "game.h"
#include <emscripten/emscripten.h>

// Static game instance shared across all calls
static HextrisGame g_game;

EMSCRIPTEN_KEEPALIVE
void wasm_init_game_ptr(void) {
    init_game(&g_game);
}

EMSCRIPTEN_KEEPALIVE
void wasm_start_game_ptr(void) {
    start_game(&g_game);
}

EMSCRIPTEN_KEEPALIVE
void wasm_tick_game_ptr(void) {
    tick_game(&g_game);
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_grid_cell(int col, int row) {
    if (col < 0 || col >= GRID_W || row < 0 || row >= GRID_H) return 0;
    return g_game.grid[col][row];
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_score(void) { return g_game.score; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_lines(void) { return g_game.lines_cleared; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_game_over(void) { return g_game.game_over; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_piece_x(void) { return g_game.piece_x; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_piece_y(void) { return g_game.piece_y; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_piece_dir(void) { return g_game.piece_dir; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_piece_color(void) { return g_game.current_piece.color; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_piece_num_blocks(void) { return g_game.current_piece.num_blocks; }

EMSCRIPTEN_KEEPALIVE
int wasm_get_piece_block_col(int idx) {
    if (idx < 0 || idx >= g_game.current_piece.num_blocks) return -1;
    return g_game.piece_x + g_game.current_piece.blocks[idx].col;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_piece_block_row(int idx) {
    if (idx < 0 || idx >= g_game.current_piece.num_blocks) return -1;
    return g_game.piece_y + g_game.current_piece.blocks[idx].row;
}

EMSCRIPTEN_KEEPALIVE
int wasm_rotate(void) { return set_direction(&g_game, 1) ? 1 : 0; }

EMSCRIPTEN_KEEPALIVE
int wasm_move_left(void) {
    int dcol, drow;
    get_dir_delta(DIR_LEFT, g_game.piece_x, &dcol, &drow);
    return move_piece(&g_game, dcol, drow) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int wasm_move_right(void) {
    int dcol, drow;
    get_dir_delta(DIR_RIGHT, g_game.piece_x, &dcol, &drow);
    return move_piece(&g_game, dcol, drow) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int wasm_move_down(void) {
    int dcol, drow;
    get_dir_delta(DIR_DOWNRIGHT, g_game.piece_x, &dcol, &drow);
    return move_piece(&g_game, dcol, drow) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int wasm_hard_drop(void) {
    hard_drop_fn(&g_game);
    return 1;
}

EMSCRIPTEN_KEEPALIVE
int wasm_restart(void) {
    start_game(&g_game);
    return 1;
}
