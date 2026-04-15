#include <emscripten.h>
#include "game.h"

static Game game;
static int keys[256] = {0};
static int left_button = 0, right_button = 0;

EMSCRIPTEN_KEEPALIVE
void game_reset() {
    initGame(&game);
}

EMSCRIPTEN_KEEPALIVE
void game_tick(float dt) {
    // Handle flipper input
    game.leftFlipper.targetAngle = left_button ? 0.0f : 0.4f;
    game.rightFlipper.targetAngle = right_button ? 0.0f : 0.4f;
    
    // Launch ball with space
    if (keys[32] && game.state == STATE_IDLE) {
        launchBall(&game);
    }
    
    // Paddle movement
    if (keys[37]) game.paddleX -= PADDLE_SPEED;
    if (keys[39]) game.paddleX += PADDLE_SPEED;
    if (game.paddleX < PADDLE_WIDTH/2) game.paddleX = PADDLE_WIDTH/2;
    if (game.paddleX > CANVAS_WIDTH - PADDLE_WIDTH/2) game.paddleX = CANVAS_WIDTH - PADDLE_WIDTH/2;
    
    updateGame(&game, dt);
}

EMSCRIPTEN_KEEPALIVE
void set_key(int code, int pressed) {
    if (code >= 0 && code < 256) keys[code] = pressed;
}

EMSCRIPTEN_KEEPALIVE
void set_left_flipper(int pressed) { left_button = pressed; }
EMSCRIPTEN_KEEPALIVE
void set_right_flipper(int pressed) { right_button = pressed; }

// Getters
EMSCRIPTEN_KEEPALIVE
int get_game_state() { return game.state; }
EMSCRIPTEN_KEEPALIVE
int get_score() { return game.score; }
EMSCRIPTEN_KEEPALIVE
int get_lives() { return game.lives; }
EMSCRIPTEN_KEEPALIVE
float get_ball_x() { return game.ball.x; }
EMSCRIPTEN_KEEPALIVE
float get_ball_y() { return game.ball.y; }
EMSCRIPTEN_KEEPALIVE
int get_ball_active() { return game.ball.active; }
EMSCRIPTEN_KEEPALIVE
float get_paddle_x() { return game.paddleX; }
EMSCRIPTEN_KEEPALIVE
int get_ball_radius() { return BALL_RADIUS; }
EMSCRIPTEN_KEEPALIVE
int get_paddle_width() { return PADDLE_WIDTH; }
EMSCRIPTEN_KEEPALIVE
int get_paddle_height() { return PADDLE_HEIGHT; }
EMSCRIPTEN_KEEPALIVE
int get_canvas_width() { return CANVAS_WIDTH; }
EMSCRIPTEN_KEEPALIVE
int get_canvas_height() { return CANVAS_HEIGHT; }

// Bumper getters
EMSCRIPTEN_KEEPALIVE
int get_bumper_count() { return NUM_BUMPERS; }
EMSCRIPTEN_KEEPALIVE
float get_bumper_x(int i) { return i >= 0 && i < NUM_BUMPERS ? game.bumpers[i].x : 0; }
EMSCRIPTEN_KEEPALIVE
float get_bumper_y(int i) { return i >= 0 && i < NUM_BUMPERS ? game.bumpers[i].y : 0; }
EMSCRIPTEN_KEEPALIVE
int get_bumper_radius_i() { return BUMPER_RADIUS; }
EMSCRIPTEN_KEEPALIVE
int get_bumper_hit(int i) { return i >= 0 && i < NUM_BUMPERS ? game.bumpers[i].hit : 0; }

// Flipper getters
EMSCRIPTEN_KEEPALIVE
float get_left_flipper_x() { return game.leftFlipper.x; }
EMSCRIPTEN_KEEPALIVE
float get_left_flipper_angle() { return game.leftFlipper.angle; }
