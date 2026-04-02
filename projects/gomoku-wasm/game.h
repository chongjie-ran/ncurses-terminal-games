#ifndef GOMOKU_GAME_H
#define GOMOKU_GAME_H

// Board dimensions
#define BOARD_SIZE 15
#define WIN_COUNT 5

// Cell values
#define CELL_EMPTY 0
#define CELL_BLACK 1
#define CELL_WHITE 2

// Game state
#define GAME_NOT_OVER 0
#define GAME_OVER 1

// Result codes for place_stone
#define PLACE_OK        0
#define PLACE_OCCUPIED  1
#define PLACE_GAME_OVER 2

// Winner codes for check_winner
#define WIN_NONE  0
#define WIN_BLACK 1
#define WIN_WHITE 2
#define WIN_DRAW  3

// Direction vectors for win checking
#define DIR_COUNT 4

// Game state struct
typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE];
    int current_player;  // 1 = black, 2 = white
    int game_over;
    int winner;
    int last_x;
    int last_y;
    int win_start_x;
    int win_start_y;
    int win_end_x;
    int win_end_y;
} GomokuGame;

// External state
extern GomokuGame g_game;

// Core game functions
void init_game(void);
int place_stone(int x, int y);
int check_winner(void);
int get_cell(int x, int y);
int get_current_player(void);
int get_game_over(void);

// Win line getters
int get_win_start_x(void);
int get_win_start_y(void);
int get_win_end_x(void);
int get_win_end_y(void);

// Last stone position
int get_last_x(void);
int get_last_y(void);

#endif // GOMOKU_GAME_H
