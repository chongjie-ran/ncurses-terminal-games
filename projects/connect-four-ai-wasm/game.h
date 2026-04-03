#ifndef GAME_H
#define GAME_H

#define ROWS 6
#define COLS 7
#define EMPTY 0
#define PLAYER1 1
#define PLAYER2 2

typedef struct {
    int board[ROWS][COLS];
    int current_player;
    int state; // 0=menu, 1=playing, 2=p1_wins, 3=p2_wins, 4=draw
    int win_x1, win_y1, win_x2, win_y2;
    int ai_depth;
} GameState;

void cf_init_game(int ai_depth);
int cf_drop_disc(int col);
int cf_get_cell(int row, int col);
int cf_get_state(void);
int cf_get_current_player(void);
int cf_can_drop(int col);
int cf_get_winner(void);
int cf_get_win_x1(void);
int cf_get_win_y1(void);
int cf_get_win_x2(void);
int cf_get_win_y2(void);
int cf_ai_move(void);

// Internal AI
int ai_evaluate_position(void);
int ai_minimax(int depth, int alpha, int beta, int maximizing);
int ai_get_best_column(void);
int check_winner(void);

#endif
