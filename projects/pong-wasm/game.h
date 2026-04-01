#ifndef PONG_GAME_H
#define PONG_GAME_H

#define SCREEN_W 640
#define SCREEN_H 480
#define PADDLE_W 10
#define PADDLE_H 80
#define PADDLE_SPEED 7.0f
#define BALL_R 8
#define BALL_SPEED 5.0f
#define AI_SPEED 4.5f

typedef struct {
    float x, y;
    float dx, dy;
    float radius;
} Ball;

typedef struct {
    float x, y;
    int score;
} Paddle;

typedef struct {
    Ball ball;
    Paddle left;
    Paddle right;
    int state;
    int win_score;
    int menu_selection;
} PongGame;

extern PongGame g_state;

void game_init(PongGame *g);
void game_update(PongGame *g, float dt);
void game_launch_ball(PongGame *g);

#endif
