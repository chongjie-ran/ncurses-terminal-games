#ifndef GAME_H
#define GAME_H

#define ROWS 6
#define COLS 7
#define EMPTY 0
#define PLAYER1 1
#define PLAYER2 2
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_GAME_OVER 2
#define MODE_2P 0
#define MODE_VS_AI 1
#define AI_EASY 1
#define AI_MEDIUM 2
#define AI_HARD 3
#define DRAW 3

typedef struct {
    int board[ROWS][COLS];
    int current_player;
    int state;
    int winner;
    int move_count;
    int last_col;
    int last_row;
    int win_x1, win_y1, win_x2, win_y2;
    int win_type;
    int ai_mode;
    int ai_difficulty;
    int ai_thinking;
} ConnectGame;

extern ConnectGame g_game;

void cf_init_game_impl(void);
void cf_start_game_impl(void);
void cf_start_game_ex_impl(int ai_mode, int ai_difficulty);
int cf_drop_disc_impl(int col);
int cf_get_cell_impl(int row, int col);
int cf_can_drop_impl(int col);
int cf_get_state_impl(void);
int cf_get_player_impl(void);
int cf_get_winner_impl(void);
int cf_get_move_count_impl(void);
int cf_get_win_x1_impl(void);
int cf_get_win_y1_impl(void);
int cf_get_win_x2_impl(void);
int cf_get_win_y2_impl(void);
int cf_get_win_type_impl(void);
int cf_get_last_col_impl(void);
int cf_get_last_row_impl(void);
int cf_get_ai_mode_impl(void);
int cf_get_ai_difficulty_impl(void);
int cf_is_ai_thinking_impl(void);
int cf_ai_move_impl(void);

#endif
