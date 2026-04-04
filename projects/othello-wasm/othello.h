#ifndef OTHELLO_H
#define OTHELLO_H

#define BOARD_SIZE 8
#define EMPTY 0
#define BLACK 1    // 先手
#define WHITE 2    // 后手
#define MAX_MOVES 64

typedef struct {
    int row, col;
} Move;

typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE];
    int current_player;  // BLACK or WHITE
    int black_count;
    int white_count;
    int game_over;
    int winner;
    int valid_moves[MAX_MOVES];
    int valid_count;
} OthelloGame;

void othello_init(OthelloGame* g);
int  othello_get_winner(OthelloGame* g);
void othello_get_valid_moves(OthelloGame* g);
int  othello_is_valid_move(OthelloGame* g, int row, int col);
int  othello_make_move(OthelloGame* g, int row, int col);
void othello_skip_turn(OthelloGame* g);
int  othello_is_game_over(OthelloGame* g);
void othello_play(OthelloGame* g, int row, int col);

#endif
