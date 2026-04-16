#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define BOARD_SIZE 10
#define NUM_SHIPS 5

// Cell states
#define CELL_EMPTY 0
#define CELL_SHIP  1
#define CELL_HIT   2
#define CELL_MISS  3

// Ship types
#define SHIP_CARRIER    5
#define SHIP_BATTLESHIP 4
#define SHIP_CRUISER    3
#define SHIP_SUBMARINE  3
#define SHIP_DESTROYER  2

// Game states
#define STATE_PLACING  0
#define STATE_PLAYING  1
#define STATE_WIN      2
#define STATE_LOSE     3

// Internal types
typedef struct {
    int row;
    int col;
} Pos;

typedef struct {
    int size;
    int row;
    int col;
    int horizontal;
    int hits;
    int sunk;
} Ship;

typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE];
    int real_board[BOARD_SIZE][BOARD_SIZE];
    int ship_count;
    int hit_count;
    int miss_count;
    int ships_sunk;
    int state;
} Player;

void battleship_init_impl(void);
void battleship_reset_impl(void);
int  battleship_get_cell_impl(int board_type, int row, int col);
int  battleship_get_game_state_impl(void);
int  battleship_get_ships_sunk_impl(void);
int  battleship_get_hit_count_impl(void);
int  battleship_get_miss_count_impl(void);
int  battleship_get_ship_count_impl(void);
int  battleship_get_computer_ships_sunk_impl(void);
int  battleship_place_ship_impl(int ship_idx, int row, int col, int horizontal);
int  battleship_shoot_impl(int row, int col);
int  battleship_computer_turn_impl(void);

#endif // GAME_H
