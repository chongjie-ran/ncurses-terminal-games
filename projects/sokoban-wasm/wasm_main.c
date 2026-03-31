#include "game.h"
#include <emscripten.h>

// These wrappers call the game.c functions and get exported
EMSCRIPTEN_KEEPALIVE
void wasm_init(int level) {
    init_level(level);
}

EMSCRIPTEN_KEEPALIVE
int wasm_move(int dx, int dy) {
    return wasm_click_move(dx, dy);
}

EMSCRIPTEN_KEEPALIVE
void wasm_restart_wrapper(void) {
    wasm_restart();
}

EMSCRIPTEN_KEEPALIVE
int wasm_next_level_wrapper(void) {
    return wasm_next_level();
}

// The following are defined in game.c and exported via EMSCRIPTEN_KEEPALIVE there:
// wasm_tick, wasm_get_cell, wasm_get_grid_width, wasm_get_grid_height,
// wasm_get_player_x, wasm_get_player_y, wasm_get_moves, wasm_get_pushes,
// wasm_get_state, wasm_get_current_level, wasm_get_total_levels
