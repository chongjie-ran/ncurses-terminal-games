#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define COLS 7
#define ROWS 6
#define EMPTY 0
#define PLAYER1 1
#define PLAYER2 2
#define DRAW 3

// Game states
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_GAME_OVER 2

// Win types
#define WIN_HORIZ 0
#define WIN_VERT 1
#define WIN_DIAG1 2
#define WIN_DIAG2 3

typedef struct {
    int board[ROWS][COLS];
    int current_player;
    int state;
    int winner;
    int move_count;
    int win_x1, win_y1, win_x2, win_y2;
    int win_type;
    bool animating;
    int anim_col;
    int anim_row;
} ConnectGame;

#endif
