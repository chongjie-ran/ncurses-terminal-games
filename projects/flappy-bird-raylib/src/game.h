#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define SCREEN_W 480
#define SCREEN_H 640
#define GRAVITY 0.45f
#define JUMP_VELOCITY -7.5f
#define PIPE_WIDTH 60
#define PIPE_GAP 150
#define PIPE_SPEED 2.5f
#define PIPE_INTERVAL 200   // pixels between pipe pairs
#define BIRD_RADIUS 15

typedef enum { STATE_MENU, STATE_PLAYING, STATE_GAMEOVER } GameState;

typedef struct {
    float x;
    float y;
    float vy;
} Bird;

typedef struct {
    float x;
    float gap_y;      // y of TOP of gap
    bool passed;
} Pipe;

typedef struct {
    Bird bird;
    Pipe *pipes;
    int pipe_count;
    int pipe_cap;
    int score;
    int best_score;
    GameState state;
    float scroll_x;   // accumulated scroll offset
} FlappyGame;

void game_init(FlappyGame *g);
void game_free(FlappyGame *g);
void game_jump(FlappyGame *g);
void game_update(FlappyGame *g);
void game_draw(FlappyGame *g);
float get_pipe_x(FlappyGame *g, int idx);
int  best_score_load(void);
void best_score_save(int s);

#endif
