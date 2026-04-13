#include <emscripten/emscripten.h>
#include <stdbool.h>
#include "../src/game.h"

// Global game instance
Game g_game;

// C call: init game
EMSCRIPTEN_KEEPALIVE
void _init_game() {
    init_game(&g_game);
    g_game.state = STATE_PLAYING;
    g_game.serve_timer = 0;
    g_game.launch_ready = true;
}

// C call: update game
EMSCRIPTEN_KEEPALIVE
void _update_game(float dt) {
    update_game(&g_game, dt);
}

// C call: move paddle left/right
EMSCRIPTEN_KEEPALIVE
void _move_paddle(float dx) {
    g_game.paddle.x += dx;
    if (g_game.paddle.x < 0) g_game.paddle.x = 0;
    if (g_game.paddle.x + g_game.paddle.w > SCREEN_W)
        g_game.paddle.x = SCREEN_W - g_game.paddle.w;
}

// C call: launch ball
EMSCRIPTEN_KEEPALIVE
void _launch_ball() {
    launch_ball(&g_game);
}

// C call: reset game
EMSCRIPTEN_KEEPALIVE
void _reset_game() {
    init_game(&g_game);
    g_game.state = STATE_PLAYING;
    g_game.serve_timer = 0;
    g_game.launch_ready = true;
}

// Getters: game state
EMSCRIPTEN_KEEPALIVE
int _get_game_state() { return (int)g_game.state; }
EMSCRIPTEN_KEEPALIVE
int _get_lives() { return g_game.lives; }
EMSCRIPTEN_KEEPALIVE
int _get_score() { return g_game.score; }
EMSCRIPTEN_KEEPALIVE
int _get_level() { return g_game.level; }
EMSCRIPTEN_KEEPALIVE
int _get_bricks_remaining() { return g_game.bricks_remaining; }
EMSCRIPTEN_KEEPALIVE
int _get_total_bricks() { return g_game.total_bricks; }
EMSCRIPTEN_KEEPALIVE
int _get_launch_ready() { return g_game.launch_ready ? 1 : 0; }

// Getters: paddle
EMSCRIPTEN_KEEPALIVE
float _get_paddle_x() { return g_game.paddle.x; }
EMSCRIPTEN_KEEPALIVE
float _get_paddle_y() { return g_game.paddle.y; }
EMSCRIPTEN_KEEPALIVE
int   _get_paddle_w() { return g_game.paddle.w; }
EMSCRIPTEN_KEEPALIVE
int   _get_paddle_h() { return g_game.paddle.h; }

// Getters: ball (only ball 0 for simplicity)
EMSCRIPTEN_KEEPALIVE
float _get_ball_x() { return g_game.balls[0].x; }
EMSCRIPTEN_KEEPALIVE
float _get_ball_y() { return g_game.balls[0].y; }
EMSCRIPTEN_KEEPALIVE
int   _get_ball_active() { return g_game.balls[0].active ? 1 : 0; }
EMSCRIPTEN_KEEPALIVE
int   _get_ball_count() { return g_game.ball_count; }

// Getters: brick
EMSCRIPTEN_KEEPALIVE
int _get_brick_alive(int row, int col) {
    if (row < 0 || row >= BRICK_ROWS || col < 0 || col >= BRICK_COLS) return 0;
    return g_game.bricks[row][col].alive ? 1 : 0;
}
EMSCRIPTEN_KEEPALIVE
int _get_brick_color_idx(int row, int col) {
    if (row < 0 || row >= BRICK_ROWS || col < 0 || col >= BRICK_COLS) return 0;
    return g_game.bricks[row][col].color_idx;
}

// Brick geometry
EMSCRIPTEN_KEEPALIVE
float _get_brick_x(int row, int col) {
    return BRICK_LEFT + col * (BRICK_W + BRICK_GAP);
}
EMSCRIPTEN_KEEPALIVE
float _get_brick_y(int row, int col) {
    return BRICK_TOP + row * (BRICK_H + BRICK_GAP);
}
EMSCRIPTEN_KEEPALIVE
int _get_brick_w() { return BRICK_W; }
EMSCRIPTEN_KEEPALIVE
int _get_brick_h() { return BRICK_H; }

// Screen / geometry constants
EMSCRIPTEN_KEEPALIVE
int _get_screen_w() { return SCREEN_W; }
EMSCRIPTEN_KEEPALIVE
int _get_screen_h() { return SCREEN_H; }
EMSCRIPTEN_KEEPALIVE
int _get_brick_rows() { return BRICK_ROWS; }
EMSCRIPTEN_KEEPALIVE
int _get_brick_cols() { return BRICK_COLS; }
EMSCRIPTEN_KEEPALIVE
float _get_ball_r() { return BALL_R; }
EMSCRIPTEN_KEEPALIVE
int _get_paddle_y_val() { return PADDLE_Y; }
