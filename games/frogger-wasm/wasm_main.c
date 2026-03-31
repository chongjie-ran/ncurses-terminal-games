#include <stdio.h>
#include <emscripten.h>
#include "game.h"

EMSCRIPTEN_KEEPALIVE
void wasm_init(void) {
    game_init();
}

EMSCRIPTEN_KEEPALIVE
int wasm_tick(void) {
    return game_tick();
}

EMSCRIPTEN_KEEPALIVE
int wasm_click(int grid_x, int grid_y) {
    return game_click(grid_x, grid_y);
}

EMSCRIPTEN_KEEPALIVE
void wasm_restart(void) {
    game_restart();
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_player_grid_x(void) { return get_player_grid_x(); }
EMSCRIPTEN_KEEPALIVE
int wasm_get_player_grid_y(void) { return get_player_grid_y(); }
EMSCRIPTEN_KEEPALIVE
int wasm_get_player_lives(void) { return get_player_lives(); }
EMSCRIPTEN_KEEPALIVE
int wasm_get_player_score(void) { return get_player_score(); }
EMSCRIPTEN_KEEPALIVE
int wasm_get_player_level(void) { return get_player_level(); }
EMSCRIPTEN_KEEPALIVE
int wasm_get_player_state(void) { return get_player_state(); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_obstacle_count(void) { return get_obstacle_count(); }
EMSCRIPTEN_KEEPALIVE
float wasm_get_obstacle_x(int idx) { return get_obstacle_x(idx); }
EMSCRIPTEN_KEEPALIVE
float wasm_get_obstacle_y(int idx) { return get_obstacle_y(idx); }
EMSCRIPTEN_KEEPALIVE
int wasm_get_obstacle_type(int idx) { return get_obstacle_type(idx); }
EMSCRIPTEN_KEEPALIVE
int wasm_get_obstacle_width(int idx) { return get_obstacle_width(idx); }
EMSCRIPTEN_KEEPALIVE
int wasm_get_obstacle_dir(int idx) { return get_obstacle_dir(idx); }
