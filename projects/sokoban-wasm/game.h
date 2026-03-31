#ifndef GAME_H
#define GAME_H

#include <stdint.h>

// Cell types
#define CELL_FLOOR   0
#define CELL_WALL    1
#define CELL_BOX     2
#define CELL_TARGET  3
#define CELL_BOX_ON_TARGET 4
#define CELL_PLAYER  5
#define CELL_PLAYER_ON_TARGET 6

// Game states
#define GAME_STATE_PLAYING  0
#define GAME_STATE_WON      1
#define GAME_STATE_RESTART  2

// Level format: 0=floor, 1=wall, 2=box, 3=target, 4=box+target, 5=player, 6=player+target
// We'll use a simpler encoding for the C side

extern int grid_width;
extern int grid_height;
extern int player_x;
extern int player_y;
extern int moves;
extern int pushes;
extern int game_state;
extern int current_level;

// Functions
void init_level(int level);
int wasm_tick(void);
int wasm_click_move(int dx, int dy);
void wasm_restart(void);
int wasm_next_level(void);
int wasm_get_cell(int x, int y);
int wasm_get_grid_width(void);
int wasm_get_grid_height(void);
int wasm_get_player_x(void);
int wasm_get_player_y(void);
int wasm_get_moves(void);
int wasm_get_pushes(void);
int wasm_get_state(void);
int wasm_get_current_level(void);
int wasm_get_total_levels(void);

#endif // GAME_H
