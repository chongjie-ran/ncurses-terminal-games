#include <emscripten.h>
#include <string.h>
#include "game.h"

Game g_state;

EMSCRIPTEN_KEEPALIVE
void wasm_init(void) {
    init_game(&g_state);
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_state(void) {
    return g_state.state;
}

EMSCRIPTEN_KEEPALIVE
void wasm_launch(void) {
    if (g_state.state == STATE_READY) {
        launch_ball(&g_state);
    }
}

EMSCRIPTEN_KEEPALIVE
void wasm_update(float dt) {
    update_game(&g_state, dt);
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_score(void) {
    return g_state.score;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_balls_remaining(void) {
    return g_state.balls_remaining;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_ball_x(void) {
    return g_state.ball.x;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_ball_y(void) {
    return g_state.ball.y;
}

EMSCRIPTEN_KEEPALIVE
int wasm_ball_active(void) {
    return g_state.ball.active ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_flipper_angle(int is_right) {
    Flipper* f = is_right ? &g_state.right_flipper : &g_state.left_flipper;
    return f->angle;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_flipper_x(int is_right) {
    Flipper* f = is_right ? &g_state.right_flipper : &g_state.left_flipper;
    return f->x;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_flipper_y(void) {
    return g_state.left_flipper.y;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_flipper_len(void) {
    return g_state.left_flipper.len;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_bumper_x(int i) {
    if (i < 0 || i >= BUMPERS) return 0;
    return (int)g_state.bumpers[i].x;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_bumper_y(int i) {
    if (i < 0 || i >= BUMPERS) return 0;
    return (int)g_state.bumpers[i].y;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_bumper_r(void) {
    return BUMPER_R;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_bumper_hit(int i) {
    if (i < 0 || i >= BUMPERS) return 0;
    return g_state.bumper_timers[i] > 0 ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
void wasm_flipper_up(int is_right) {
    Flipper* f = is_right ? &g_state.right_flipper : &g_state.left_flipper;
    flipper_up(f);
}

EMSCRIPTEN_KEEPALIVE
void wasm_flipper_down(int is_right) {
    Flipper* f = is_right ? &g_state.right_flipper : &g_state.left_flipper;
    flipper_down(f);
}

EMSCRIPTEN_KEEPALIVE
void wasm_restart(void) {
    init_game(&g_state);
}

EMSCRIPTEN_KEEPALIVE
void wasm_launch_power(float power) {
    g_state.launch_power = power;
}
