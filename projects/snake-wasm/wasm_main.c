#include <emscripten.h>
#include "game.h"

EMSCRIPTEN_KEEPALIVE
void wasm_init(void) {
    wasm_init_game();
}

EMSCRIPTEN_KEEPALIVE
void wasm_start(void) {
    wasm_start_game();
}

EMSCRIPTEN_KEEPALIVE
void wasm_tick(void) {
    wasm_tick_game();
}

EMSCRIPTEN_KEEPALIVE
void wasm_set_dir(int dir) {
    wasm_set_dir_game(dir);
}

EMSCRIPTEN_KEEPALIVE
void wasm_toggle_pause(void) {
    wasm_toggle_pause_game();
}

EMSCRIPTEN_KEEPALIVE
void wasm_restart(void) {
    wasm_restart_game();
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_state(void) { return wasm_get_state_game(); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_score(void) { return wasm_get_score_game(); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_speed(void) { return wasm_get_speed_game(); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_snake_len(void) { return wasm_get_snake_len_game(); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_snake_x(int i) { return wasm_get_snake_x_game(i); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_snake_y(int i) { return wasm_get_snake_y_game(i); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_food_x(void) { return wasm_get_food_x_game(); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_food_y(void) { return wasm_get_food_y_game(); }
