/*
 * Brick Breaker WASM - Classic Breakout Game
 * Pure C + Emscripten + Canvas 2D
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <emscripten.h>

#define WIDTH 480
#define HEIGHT 640
#define PADDLE_W 80
#define PADDLE_H 12
#define BALL_R 8
#define BRICK_ROWS 6
#define BRICK_COLS 8
#define BRICK_W 54
#define BRICK_H 20
#define BRICK_GAP 4
#define BRICK_TOP 60
#define MAX_SPEED 8

// Game state
static float paddle_x;
static float ball_x, ball_y, ball_dx, ball_dy;
static int bricks_alive[BRICK_ROWS][BRICK_COLS];
static int bricks_color[BRICK_ROWS][BRICK_COLS];
static int score;
static int lives;
static int game_over;
static int game_won;
static int keys_pressed;

void init_game() {
    paddle_x = WIDTH / 2.0f;
    ball_x = WIDTH / 2.0f;
    ball_y = HEIGHT - 80;
    ball_dx = 3.0f;
    ball_dy = -3.0f;
    score = 0;
    lives = 3;
    game_over = 0;
    game_won = 0;
    keys_pressed = 0;
    
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            bricks_alive[r][c] = 1;
            bricks_color[r][c] = r;
        }
    }
}

void reset_ball() {
    ball_x = paddle_x;
    ball_y = HEIGHT - 80;
    float angle = ((float)rand() / (float)RAND_MAX) * 0.5f - 0.25f;
    ball_dx = sinf(angle) * 4.0f;
    ball_dy = -4.0f;
}

EMSCRIPTEN_KEEPALIVE
void restart() {
    init_game();
}

EMSCRIPTEN_KEEPALIVE
void set_key(int key, int value) {
    if (key == 1) keys_pressed = (keys_pressed & ~1) | (value & 1);
    if (key == 2) keys_pressed = (keys_pressed & ~2) | ((value << 1) & 2);
}

EMSCRIPTEN_KEEPALIVE
void set_paddle_x(float x) {
    paddle_x = x;
    if (paddle_x < PADDLE_W / 2) paddle_x = PADDLE_W / 2;
    if (paddle_x > WIDTH - PADDLE_W / 2) paddle_x = WIDTH - PADDLE_W / 2;
}

EMSCRIPTEN_KEEPALIVE
int get_ball_x() { return (int)ball_x; }
EMSCRIPTEN_KEEPALIVE
int get_ball_y() { return (int)ball_y; }
EMSCRIPTEN_KEEPALIVE
float get_paddle_x() { return paddle_x; }
EMSCRIPTEN_KEEPALIVE
int get_paddle_w() { return PADDLE_W; }
EMSCRIPTEN_KEEPALIVE
int get_paddle_h() { return PADDLE_H; }
EMSCRIPTEN_KEEPALIVE
int get_ball_r() { return BALL_R; }
EMSCRIPTEN_KEEPALIVE
int get_brick_w() { return BRICK_W; }
EMSCRIPTEN_KEEPALIVE
int get_brick_h() { return BRICK_H; }
EMSCRIPTEN_KEEPALIVE
int get_brick_gap() { return BRICK_GAP; }
EMSCRIPTEN_KEEPALIVE
int get_brick_top() { return BRICK_TOP; }
EMSCRIPTEN_KEEPALIVE
int is_brick_alive(int r, int c) { return bricks_alive[r][c]; }
EMSCRIPTEN_KEEPALIVE
int get_brick_color(int r, int c) { return bricks_color[r][c]; }
EMSCRIPTEN_KEEPALIVE
int get_score() { return score; }
EMSCRIPTEN_KEEPALIVE
int get_lives() { return lives; }
EMSCRIPTEN_KEEPALIVE
int is_game_over() { return game_over; }
EMSCRIPTEN_KEEPALIVE
int is_game_won() { return game_won; }

EMSCRIPTEN_KEEPALIVE
void update_game() {
    if (game_over || game_won) return;
    
    if (keys_pressed & 1) paddle_x -= 6;
    if (keys_pressed & 2) paddle_x += 6;
    if (paddle_x < PADDLE_W / 2) paddle_x = PADDLE_W / 2;
    if (paddle_x > WIDTH - PADDLE_W / 2) paddle_x = WIDTH - PADDLE_W / 2;
    
    ball_x += ball_dx;
    ball_y += ball_dy;
    
    if (ball_x - BALL_R < 0) { ball_x = BALL_R; ball_dx = -ball_dx; }
    if (ball_x + BALL_R > WIDTH) { ball_x = WIDTH - BALL_R; ball_dx = -ball_dx; }
    if (ball_y - BALL_R < 0) { ball_y = BALL_R; ball_dy = -ball_dy; }
    
    if (ball_y + BALL_R > HEIGHT) {
        lives--;
        if (lives <= 0) game_over = 1;
        else reset_ball();
        return;
    }
    
    if (ball_dy > 0 &&
        ball_y + BALL_R > HEIGHT - 40 - PADDLE_H / 2 &&
        ball_y - BALL_R < HEIGHT - 40 + PADDLE_H / 2 &&
        ball_x > paddle_x - PADDLE_W / 2 &&
        ball_x < paddle_x + PADDLE_W / 2) {
        
        ball_y = HEIGHT - 40 - PADDLE_H / 2 - BALL_R;
        float hit_pos = (ball_x - paddle_x) / (PADDLE_W / 2);
        float angle = hit_pos * 1.2f;
        float speed = sqrtf(ball_dx * ball_dx + ball_dy * ball_dy);
        speed = speed * 1.005f;
        if (speed > MAX_SPEED) speed = MAX_SPEED;
        ball_dx = sinf(angle) * speed;
        ball_dy = -cosf(angle) * speed;
    }
    
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            if (!bricks_alive[r][c]) continue;
            
            float bx = c * (BRICK_W + BRICK_GAP) + BRICK_GAP + BRICK_W / 2;
            float by = BRICK_TOP + r * (BRICK_H + BRICK_GAP) + BRICK_H / 2;
            
            if (ball_x + BALL_R > bx - BRICK_W / 2 &&
                ball_x - BALL_R < bx + BRICK_W / 2 &&
                ball_y + BALL_R > by - BRICK_H / 2 &&
                ball_y - BALL_R < by + BRICK_H / 2) {
                
                bricks_alive[r][c] = 0;
                score += (BRICK_ROWS - r) * 10;
                
                float ol = (ball_x + BALL_R) - (bx - BRICK_W / 2);
                float or_ = (bx + BRICK_W / 2) - (ball_x - BALL_R);
                float ot = (ball_y + BALL_R) - (by - BRICK_H / 2);
                float ob = (by + BRICK_H / 2) - (ball_y - BALL_R);
                
                float min_o = ol;
                if (or_ < min_o) min_o = or_;
                if (ot < min_o) min_o = ot;
                if (ob < min_o) min_o = ob;
                
                if (min_o == ol || min_o == or_) ball_dx = -ball_dx;
                else ball_dy = -ball_dy;
                break;
            }
        }
    }
    
    int bricks_left = 0;
    for (int r = 0; r < BRICK_ROWS; r++)
        for (int c = 0; c < BRICK_COLS; c++)
            if (bricks_alive[r][c]) bricks_left++;
    if (bricks_left == 0) game_won = 1;
}
