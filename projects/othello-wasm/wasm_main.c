#include <emscripten/emscripten.h>
#include "othello.h"
#include <string.h>

static OthelloGame game;

EMSCRIPTEN_KEEPALIVE
void othello_js_init() {
    othello_init(&game);
}

EMSCRIPTEN_KEEPALIVE
int othello_js_get_cell(int row, int col) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return -1;
    return game.board[row][col];
}

EMSCRIPTEN_KEEPALIVE
int othello_js_get_current_player() { return game.current_player; }
EMSCRIPTEN_KEEPALIVE
int othello_js_get_black_count() { return game.black_count; }
EMSCRIPTEN_KEEPALIVE
int othello_js_get_white_count() { return game.white_count; }
EMSCRIPTEN_KEEPALIVE
int othello_js_get_game_over() { return game.game_over; }
EMSCRIPTEN_KEEPALIVE
int othello_js_get_winner() { return game.winner; }
EMSCRIPTEN_KEEPALIVE
int othello_js_get_valid_count() { return game.valid_count; }
EMSCRIPTEN_KEEPALIVE
int othello_js_get_valid_move(int idx) {
    if (idx < 0 || idx >= game.valid_count) return -1;
    return game.valid_moves[idx];
}

EMSCRIPTEN_KEEPALIVE
int othello_js_make_move(int row, int col) {
    return othello_make_move(&game, row, col);
}
