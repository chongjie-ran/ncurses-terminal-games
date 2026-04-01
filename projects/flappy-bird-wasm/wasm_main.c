#include "game.h"
#include <emscripten/emscripten.h>

// Forward declarations
void flappy_init(FlappyGame *g, int best_score);
void flappy_jump(FlappyGame *g);
void flappy_update(FlappyGame *g, int dt_ms);

// Global game instance
static FlappyGame g_game;

// WASM exports

EMSCRIPTEN_KEEPALIVE
void wasm_init(int best_score) {
    srand((unsigned int)42);  // deterministic
    flappy_init(&g_game, best_score);
}

EMSCRIPTEN_KEEPALIVE
void wasm_jump(void) {
    flappy_jump(&g_game);
}

EMSCRIPTEN_KEEPALIVE
void wasm_update(int dt_ms) {
    flappy_update(&g_game, dt_ms);
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_state(void) {
    return (int)g_game.state;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_bird_x(void) {
    return g_game.bird.x;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_bird_y(void) {
    return g_game.bird.y;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_bird_vy(void) {
    return g_game.bird.vy;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_bird_rot(void) {
    return g_game.bird_rot;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_score(void) {
    return g_game.score;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_best_score(void) {
    return g_game.best_score;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_pipe_count(void) {
    return g_game.pipe_count;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_pipe_x(int idx) {
    if (idx < 0 || idx >= g_game.pipe_count) return -9999.0f;
    return g_game.pipes[idx].x;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_pipe_gap_y(int idx) {
    if (idx < 0 || idx >= g_game.pipe_count) return 0.0f;
    return g_game.pipes[idx].gap_y;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_screen_w(void) { return SCREEN_W; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_screen_h(void) { return SCREEN_H; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_ground_h(void) { return GROUND_H; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_bird_radius(void) { return BIRD_RADIUS; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_pipe_width(void) { return PIPE_WIDTH; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_pipe_gap(void) { return PIPE_GAP; }
