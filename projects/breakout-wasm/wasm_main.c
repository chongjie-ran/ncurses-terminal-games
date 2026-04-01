#include "../breakout-wasm/game.h"
#include <emscripten/emscripten.h>

static Game g;

// ---- game lifecycle ----
EMSCRIPTEN_KEEPALIVE
void wasm_init() {
    init_game(&g);
    g.state = STATE_PLAYING;
    g.launch_ready = true;
}

EMSCRIPTEN_KEEPALIVE
void wasm_update(float dt) {
    update_game(&g, dt);
}

EMSCRIPTEN_KEEPALIVE
void wasm_launch() {
    launch_ball(&g);
}

EMSCRIPTEN_KEEPALIVE
void wasm_restart() {
    init_game(&g);
    g.state = STATE_PLAYING;
    g.launch_ready = true;
}

// ---- paddle ----
EMSCRIPTEN_KEEPALIVE
void wasm_move_paddle(float dx) {
    g.paddle.x += dx;
    if (g.paddle.x < 0) g.paddle.x = 0;
    if (g.paddle.x + g.paddle.w > SCREEN_W) g.paddle.x = SCREEN_W - g.paddle.w;
}

EMSCRIPTEN_KEEPALIVE
float wasm_get_paddle_x() { return g.paddle.x; }
EMSCRIPTEN_KEEPALIVE
float wasm_get_paddle_y() { return g.paddle.y; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_paddle_w() { return g.paddle.w; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_paddle_h() { return g.paddle.h; }

// ---- ball ----
EMSCRIPTEN_KEEPALIVE
float wasm_get_ball_x() { return g.balls[0].x; }
EMSCRIPTEN_KEEPALIVE
float wasm_get_ball_y() { return g.balls[0].y; }
EMSCRIPTEN_KEEPALIVE
float wasm_get_ball_r() { return BALL_R; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_ball_active() { return g.balls[0].active ? 1 : 0; }

// ---- bricks ----
EMSCRIPTEN_KEEPALIVE
int wasm_get_brick_rows() { return BRICK_ROWS; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_brick_cols() { return BRICK_COLS; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_brick_alive(int row, int col) {
    if (row < 0 || row >= BRICK_ROWS || col < 0 || col >= BRICK_COLS) return 0;
    return g.bricks[row][col].alive ? 1 : 0;
}
EMSCRIPTEN_KEEPALIVE
int wasm_get_brick_color_idx(int row, int col) {
    if (row < 0 || row >= BRICK_ROWS || col < 0 || col >= BRICK_COLS) return 0;
    return g.bricks[row][col].color_idx;
}
EMSCRIPTEN_KEEPALIVE
float wasm_get_brick_x(int row, int col) {
    if (row < 0 || row >= BRICK_ROWS || col < 0 || col >= BRICK_COLS) return 0;
    return BRICK_LEFT + col * (BRICK_W + BRICK_GAP);
}
EMSCRIPTEN_KEEPALIVE
float wasm_get_brick_y(int row, int col) {
    if (row < 0 || row >= BRICK_ROWS || col < 0 || col >= BRICK_COLS) return 0;
    return BRICK_TOP + row * (BRICK_H + BRICK_GAP);
}
EMSCRIPTEN_KEEPALIVE
int wasm_get_brick_w() { return BRICK_W; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_brick_h() { return BRICK_H; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_bricks_remaining() { return g.bricks_remaining; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_total_bricks() { return g.total_bricks; }

// ---- game state ----
EMSCRIPTEN_KEEPALIVE
int wasm_get_state() { return (int)g.state; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_lives() { return g.lives; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_score() { return g.score; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_level() { return g.level; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_launch_ready() { return g.launch_ready ? 1 : 0; }

// ---- screen ----
EMSCRIPTEN_KEEPALIVE
int wasm_get_screen_w() { return SCREEN_W; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_screen_h() { return SCREEN_H; }
