#ifndef NONOGRAM_GAME_H
#define NONOGRAM_GAME_H

#define MAX_ROWS 15
#define MAX_COLS 15
#define MAX_CLUES 10

typedef struct {
    int row_clues[MAX_ROWS][MAX_CLUES];
    int row_clue_counts[MAX_ROWS];
    int col_clues[MAX_COLS][MAX_CLUES];
    int col_clue_counts[MAX_COLS];
    int solution[MAX_ROWS][MAX_COLS];  // 0=empty, 1=filled
    int user_grid[MAX_ROWS][MAX_COLS]; // 0=empty, 1=filled, 2=marked(X)
    int rows, cols;
    int selected_row;
    int selected_col;
    int won;
} NonogramGame;

// Cell states
#define CELL_EMPTY  0
#define CELL_FILLED 1
#define CELL_MARKED 2  // X mark

void nono_init_game(void);
void nono_click(int row, int col);
void nono_tick(void);
int nono_get_cell(int row, int col);
int nono_get_row_clue(int row, int idx);
int nono_get_row_clue_count(int row);
int nono_get_col_clue(int col, int idx);
int nono_get_col_clue_count(int col);
int nono_get_rows(void);
int nono_get_cols(void);
int nono_get_won(void);

#endif
