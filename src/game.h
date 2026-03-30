#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <raylib.h>

#define ROWS 9
#define COLS 9
#define MINES 10
#define CELL_SIZE 44
#define GRID_PAD_X 22
#define GRID_PAD_Y 80
#define WINDOW_W (COLS * CELL_SIZE + GRID_PAD_X * 2)
#define WINDOW_H (ROWS * CELL_SIZE + GRID_PAD_Y + 50)

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAMEOVER,
    STATE_WIN
} GameState;

typedef struct {
    bool is_mine;
    bool is_revealed;
    bool is_flagged;
    int adjacent_mines;
} Cell;

typedef struct {
    Cell grid[ROWS][COLS];
    GameState state;
    bool first_click;
    int revealed_count;
    int flagged_count;
    int best_time;       // in seconds, -1 = not set
    int elapsed_seconds;
    float timer_accum;
} MinesweeperGame;

void game_init(MinesweeperGame *g);
void game_reset(MinesweeperGame *g);
void game_place_mines(MinesweeperGame *g, int safe_r, int safe_c);
void game_calc_adjacent(MinesweeperGame *g);
void game_reveal(MinesweeperGame *g, int r, int c);
void game_flood_fill(MinesweeperGame *g, int r, int c);
void game_toggle_flag(MinesweeperGame *g, int r, int c);
void game_update(MinesweeperGame *g, float dt);
int  game_get_mouse_cell(Vector2 mouse, int *r, int *c);
bool game_check_win(MinesweeperGame *g);
void game_load_best(MinesweeperGame *g);
void game_save_best(MinesweeperGame *g);

// Drawing (defined in draw.c)
void draw_game(MinesweeperGame *g);

#endif // GAME_H
