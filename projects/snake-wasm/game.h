#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <stdbool.h>

// Grid dimensions
#define GRID_W 20
#define GRID_H 15
#define CELL_SIZE 24

// Snake max length (enough for full grid)
#define MAX_SNAKE (GRID_W * GRID_H)

// Game states
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_PAUSED 2
#define STATE_GAME_OVER 3

// Directions
#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

// Snake segment
typedef struct {
    int x;
    int y;
} Segment;

// Snake game state
typedef struct {
    Segment snake[MAX_SNAKE];
    int snake_len;
    int dir;
    int next_dir;
    int food_x;
    int food_y;
    int score;
    int state;
    int speed;
    int move_timer;
    int move_interval;
    int dir_timer;
} SnakeGame;

// External state (defined in game.c)
extern SnakeGame g_game;

// Game functions
void wasm_init_game(void);
void wasm_start_game(void);
void wasm_tick_game(void);
void wasm_set_dir_game(int dir);
void wasm_toggle_pause_game(void);
void wasm_restart_game(void);

// Getters
int wasm_get_state_game(void);
int wasm_get_score_game(void);
int wasm_get_speed_game(void);
int wasm_get_snake_len_game(void);
int wasm_get_snake_x_game(int i);
int wasm_get_snake_y_game(int i);
int wasm_get_food_x_game(void);
int wasm_get_food_y_game(void);

#endif // SNAKE_GAME_H
