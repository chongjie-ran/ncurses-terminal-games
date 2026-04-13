#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define SCREEN_W 800
#define SCREEN_H 600

#define PADDLE_W 120
#define PADDLE_H 16
#define PADDLE_Y (SCREEN_H - 50)

#define BALL_R 8
#define BALL_SPEED 6.0f
#define BALL_DY_INITIAL (-BALL_SPEED)

#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_W 70
#define BRICK_H 24
#define BRICK_GAP 6
#define BRICK_TOP 80
#define BRICK_LEFT 30

#define MAX_BALLS 5

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_WIN,
    STATE_GAMEOVER,
    STATE_NEXT_LEVEL,
    STATE_PAUSED
} GameState;

typedef struct {
    float x, y;
    float dx, dy;
    bool active;
} Ball;

typedef struct {
    float x, y;
    int w, h;
} Paddle;

typedef struct {
    int row, col;
    bool alive;
    int color_idx;
} Brick;

typedef struct {
    GameState state;
    Paddle paddle;
    Ball balls[MAX_BALLS];
    int ball_count;
    int lives;
    int score;
    int level;
    Brick bricks[BRICK_ROWS][BRICK_COLS];
    int total_bricks;
    int bricks_remaining;
    float serve_timer;
    bool launch_ready;
    float next_ball_timer;
} Game;

void init_game(Game* g);
void reset_ball(Game* g, int ball_idx);
void update_game(Game* g, float dt);
bool rect_circle_hit(float rx, float ry, int rw, int rh, float cx, float cy, float cr);
void launch_ball(Game* g);

#endif
