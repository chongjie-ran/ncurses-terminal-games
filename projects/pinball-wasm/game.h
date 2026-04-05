#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_W 400
#define SCREEN_H 700

#define BALL_R 8
#define FLIPPER_W 70
#define FLIPPER_H 14
#define FLIPPER_BOUNCE 1.3f
#define BALL_SPEED_MAX 15.0f
#define GRAVITY 0.15f
#define FRICTION 0.998f
#define BOUNCE_DAMPING 0.75f

#define BUMPERS 5
#define BUMPER_R 22
#define BUMPER_SCORE 100

#define MAX_BALLS 3

typedef enum {
    STATE_READY,
    STATE_PLAYING,
    STATE_DRAIN,
    STATE_GAMEOVER
} GameState;

typedef struct {
    float x, y;
    float dx, dy;
    bool active;
} Ball;

typedef struct {
    float x, y;       // pivot point
    float angle;      // current angle (radians)
    float target_angle;
    float len;
    bool is_left;
} Flipper;

typedef struct {
    float x, y;
    float r;
    int color_idx;
    float hit_timer;
} Bumper;

typedef struct {
    GameState state;
    Ball ball;
    Flipper left_flipper;
    Flipper right_flipper;
    Bumper bumpers[BUMPERS];
    int score;
    int balls_remaining;
    float drain_timer;
    float launch_power;
    float bumper_timers[BUMPERS];
} Game;

void init_game(Game* g);
void update_game(Game* g, float dt);
void launch_ball(Game* g);
void flipper_up(Flipper* f);
void flipper_down(Flipper* f);
void update_flipper(Flipper* f, float dt);
bool circle_circle_hit(float x1, float y1, float r1, float x2, float y2, float r2);
bool point_in_rect(float px, float py, float rx, float ry, float rw, float rh);

#endif
