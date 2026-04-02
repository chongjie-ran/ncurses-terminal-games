#include <emscripten.h>
#include "game.h"

EMSCRIPTEN_KEEPALIVE
void init_game_wasm(void) {
    init_game();
}

EMSCRIPTEN_KEEPALIVE
int place_stone_wasm(int x, int y) {
    return place_stone(x, y);
}

EMSCRIPTEN_KEEPALIVE
int check_winner_wasm(void) {
    return check_winner();
}

EMSCRIPTEN_KEEPALIVE
int get_cell_wasm(int x, int y) {
    return get_cell(x, y);
}

EMSCRIPTEN_KEEPALIVE
int get_current_player_wasm(void) {
    return get_current_player();
}

EMSCRIPTEN_KEEPALIVE
int get_game_over_wasm(void) {
    return get_game_over();
}

EMSCRIPTEN_KEEPALIVE
int get_win_start_x_wasm(void) { return get_win_start_x(); }
EMSCRIPTEN_KEEPALIVE
int get_win_start_y_wasm(void) { return get_win_start_y(); }
EMSCRIPTEN_KEEPALIVE
int get_win_end_x_wasm(void) { return get_win_end_x(); }
EMSCRIPTEN_KEEPALIVE
int get_win_end_y_wasm(void) { return get_win_end_y(); }

EMSCRIPTEN_KEEPALIVE
int get_last_x_wasm(void) { return get_last_x(); }
EMSCRIPTEN_KEEPALIVE
int get_last_y_wasm(void) { return get_last_y(); }
