#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define CANVAS_W 480
#define CANVAS_H 640
#define PADDLE_W 80
#define PADDLE_H 12
#define BALL_R 8
#define BRICK_ROWS 6
#define BRICK_COLS 10
#define BRICK_W 44
#define BRICK_H 16
#define BRICK_GAP 4
#define BRICK_TOP 60
#define BRICK_LEFT 8
#define BALL_SPEED 5
#define PADDLE_SPEED 6
#define MAX_SPEED 8

typedef struct {
    float x, y;
    float dx, dy;
    float speed;
} Ball;

typedef struct {
    float x, y;
    int w, h;
} Paddle;

typedef struct {
    int row, col;
    int hits;
    int color;
} Brick;

typedef struct {
    Ball ball;
    Paddle paddle;
    Brick bricks[BRICK_ROWS * BRICK_COLS];
    int score;
    int lives;
    int state;   // 0=MENU, 1=PLAYING, 2=GAME_OVER, 3=WIN
    int paused;
    int frame_count;
    int paddle_dir;
} GameState;

extern GameState g_game;

void game_init(void);
void game_reset(void);
void game_tick(void);

#endif
