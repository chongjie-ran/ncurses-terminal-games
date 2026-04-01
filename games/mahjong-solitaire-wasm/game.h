// Mahjong Solitaire - Pure C + Emscripten
// 麻将配对游戏：选择两个相同的"自由"麻将牌消除

#ifndef GAME_H
#define GAME_H

// Tile types
#define TILE_EMPTY 0
#define TILE_BAMBOO_1 1
#define TILE_BAMBOO_2 2
#define TILE_BAMBOO_3 3
#define TILE_CIRCLE_1 11
#define TILE_CIRCLE_2 12
#define TILE_CIRCLE_3 13
#define TILE_CHAR_1 21
#define TILE_CHAR_2 22
#define TILE_CHAR_3 23
#define TILE_WIND_E 31
#define TILE_WIND_S 32
#define TILE_DRAGON_RED 41
#define TILE_DRAGON_GRN 42
#define TOTAL_TILE_TYPES 14

// Game state
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_WIN 2
#define STATE_NO_MOVES 3

#define MAX_LAYERS 3
#define GRID_W 12
#define GRID_H 8

extern int game_state;
extern int selected_tile_idx;
extern int selected_layer;
extern int selected_row;
extern int selected_col;
extern int score;
extern int tiles_remaining;

// Core functions
void init_game(void);
int is_tile_free(int layer, int row, int col);
int get_tile(int layer, int row, int col);
int select_tile(int layer, int row, int col);
int has_valid_moves(void);
int check_win(void);

#endif
