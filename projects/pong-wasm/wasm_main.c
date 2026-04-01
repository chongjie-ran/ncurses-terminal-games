#include <emscripten.h>
#include "game.h"

// These are defined in game.c
extern void game_launch_ball(PongGame *g);

EMSCRIPTEN_KEEPALIVE
void wasm_init(void) {
    game_init(&g_state);
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_state(void) {
    return g_state.state;
}

EMSCRIPTEN_KEEPALIVE
void wasm_launch(void) {
    if (g_state.state == 0 || g_state.state == 2 || g_state.state == 3) {
        g_state.left.score = 0;
        g_state.right.score = 0;
        g_state.left.y = SCREEN_H / 2.0f - PADDLE_H / 2.0f;
        g_state.right.y = SCREEN_H / 2.0f - PADDLE_H / 2.0f;
        g_state.state = 1;
        game_launch_ball(&g_state);
    }
}

EMSCRIPTEN_KEEPALIVE
void wasm_update(float dt) {
    game_update(&g_state, dt);
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_left_score(void) { return g_state.left.score; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_right_score(void) { return g_state.right.score; }
EMSCRIPTEN_KEEPALIVE
float wasm_get_ball_x(void) { return g_state.ball.x; }
EMSCRIPTEN_KEEPALIVE
float wasm_get_ball_y(void) { return g_state.ball.y; }
EMSCRIPTEN_KEEPALIVE
float wasm_get_ball_radius(void) { return g_state.ball.radius; }
EMSCRIPTEN_KEEPALIVE
float wasm_get_paddle_y(int is_right) {
    return is_right ? g_state.right.y : g_state.left.y;
}
EMSCRIPTEN_KEEPALIVE
int wasm_get_menu_selection(void) { return g_state.menu_selection; }
EMSCRIPTEN_KEEPALIVE
void wasm_menu_select(int sel) { g_state.menu_selection = sel; }
EMSCRIPTEN_KEEPALIVE
void wasm_restart(void) { game_init(&g_state); }
EMSCRIPTEN_KEEPALIVE
void wasm_paddle_move(int is_right, int dir) {
    Paddle *p = is_right ? &g_state.right : &g_state.left;
    float speed = PADDLE_SPEED;
    if (dir == -1) p->y -= speed;
    else if (dir == 1) p->y += speed;
    if (p->y < 0) p->y = 0;
    if (p->y > SCREEN_H - PADDLE_H) p->y = SCREEN_H - PADDLE_H;
}
