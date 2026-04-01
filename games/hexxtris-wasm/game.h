// game.h - Hextris WASM
// Hexagonal falling-piece puzzle game

#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define GRID_W 7
#define GRID_H 13
#define MAX_PIECE_BLOCKS 4
#define PIECE_TYPES 3

// Direction indices (0-5 clockwise from RIGHT)
#define DIR_RIGHT    0
#define DIR_UPRIGHT  1
#define DIR_UPLEFT   2
#define DIR_LEFT     3
#define DIR_DOWNLEFT 4
#define DIR_DOWNRIGHT 5

typedef struct {
    int col;
    int row;
} HexPos;

typedef struct {
    int num_blocks;
    HexPos blocks[MAX_PIECE_BLOCKS];
    int color;
} Piece;

typedef struct {
    int grid[GRID_W][GRID_H];  // 0=empty, 1-3=color
    int score;
    int lines_cleared;
    int game_over;
    Piece current_piece;
    Piece next_piece;
    int piece_x, piece_y;  // anchor position of current piece (col, row of piece center)
    int piece_dir;         // current direction 0-5
    int tick_counter;
    int tick_interval;     // frames between auto-drops
    bool lock_input;
} HextrisGame;

// g_game defined in wasm_main.c

// Direction deltas for odd-q offset hex grid
// Each entry: {dcol_even, drow_even, dcol_odd, drow_odd}
void get_dir_delta(int dir, int anchor_col, int* dcol, int* drow);

bool is_valid_pos(HextrisGame* g, int col, int row);
bool check_collision(HextrisGame* g, int anchor_col, int anchor_row, int dir);
void try_spawn_piece(HextrisGame* g);

void init_game(HextrisGame* g);
void start_game(HextrisGame* g);
void tick_game(HextrisGame* g);
bool set_direction(HextrisGame* g, int dir);
bool move_piece(HextrisGame* g, int dcol, int drow);
bool rotate_piece_fn(HextrisGame* g);
bool hard_drop_fn(HextrisGame* g);
void lock_piece(HextrisGame* g);
int check_lines(HextrisGame* g);
void remove_line(HextrisGame* g, int row);
void compact_grid(HextrisGame* g);

int get_grid_cell(HextrisGame* g, int col, int row);
int get_piece_block_col(Piece* p, int idx);
int get_piece_block_row(Piece* p, int idx);
int get_piece_color(Piece* p);

#endif
