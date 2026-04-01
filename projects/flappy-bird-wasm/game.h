#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

// Canvas dimensions (logical)
#define SCREEN_W 288
#define SCREEN_H 512
#define CELL 32  // for compatibility

// Physics
#define GRAVITY 0.45f
#define JUMP_VELOCITY -7.5f

// Bird
#define BIRD_X 60
#define BIRD_RADIUS 15

// Pipes
#define PIPE_WIDTH 52
#define PIPE_GAP 120
#define PIPE_SPEED 2.0f
#define PIPE_INTERVAL 180   // pixels between pipe pairs
#define PIPE_CAP 10

// Ground
#define GROUND_H 112

// Game states
typedef enum { STATE_MENU, STATE_PLAYING, STATE_GAMEOVER } GameState;

// Bird
typedef struct {
    float x;
    float y;
    float vy;
} Bird;

// Pipe (top and bottom of one pair share same x)
typedef struct {
    float x;         // x position of pipe pair
    float gap_y;     // y of TOP of gap opening
    bool passed;     // has bird passed this pipe?
} Pipe;

// Game
typedef struct {
    Bird bird;
    Pipe pipes[PIPE_CAP];
    int pipe_count;
    int score;
    int best_score;
    GameState state;
    float scroll_x;
    float bird_rot;   // rotation angle for rendering
} FlappyGame;

#endif
