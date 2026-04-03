#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define GRID_ROWS 16
#define GRID_COLS 16
#define NUM_MINES 40

#define CELL_HIDDEN 0
#define CELL_MINE   9
#define CELL_FLAG   10

typedef struct {
    uint8_t cells[GRID_ROWS][GRID_COLS];  // 0-8: adjacent count, 9: mine, 10: flag
    uint8_t revealed[GRID_ROWS][GRID_COLS];
    uint8_t game_state;   // 0: playing, 1: won, 2: lost
    uint8_t first_click;
    int cells_left;
} GameState;

void game_init(GameState* gs);
void game_place_mines(GameState* gs, int click_row, int click_col);
int  game_reveal(GameState* gs, int row, int col);
int  game_toggle_flag(GameState* gs, int row, int col);
int  game_get_cell(GameState* gs, int row, int col);  // returns display value
int  game_is_revealed(GameState* gs, int row, int col);

#endif
