#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

static NonogramGame g_game;

// Puzzle: 10x10 smiley face
// Row clues (from top to bottom)
static int PUZZLE_ROWS = 10;
static int PUZZLE_COLS = 10;
static int PUZZLE_SOLUTION[10][10] = {
    {0,1,1,0,0,0,0,1,1,0},  // clue: 2 2
    {1,1,1,1,0,0,1,1,1,1},  // clue: 4 4
    {1,1,1,1,1,1,1,1,1,1},  // clue: 10
    {1,1,1,1,1,1,1,1,1,1},  // clue: 10
    {1,1,1,1,1,1,1,1,1,1},  // clue: 10
    {0,0,1,1,1,1,1,1,0,0},  // clue: 6
    {0,1,1,1,1,1,1,1,1,0},  // clue: 8
    {0,0,1,1,1,1,1,1,0,0},  // clue: 6
    {0,0,0,1,1,1,1,0,0,0},  // clue: 4
    {0,0,0,0,1,1,0,0,0,0},  // clue: 2
};

static int PUZZLE_ROW_CLUES[10][5] = {
    {2, 2, 0, 0, 0},
    {4, 4, 0, 0, 0},
    {10, 0, 0, 0, 0},
    {10, 0, 0, 0, 0},
    {10, 0, 0, 0, 0},
    {6, 0, 0, 0, 0},
    {8, 0, 0, 0, 0},
    {6, 0, 0, 0, 0},
    {4, 0, 0, 0, 0},
    {2, 0, 0, 0, 0},
};

static int PUZZLE_COL_CLUES[10][5] = {
    {2, 0, 0, 0, 0},
    {4, 4, 0, 0, 0},
    {6, 2, 2, 0, 0},
    {6, 4, 0, 0, 0},
    {7, 3, 0, 0, 0},
    {7, 3, 0, 0, 0},
    {6, 4, 0, 0, 0},
    {6, 2, 2, 0, 0},
    {4, 4, 0, 0, 0},
    {2, 0, 0, 0, 0},
};

static int PUZZLE_ROW_CLUE_COUNTS[10] = {2, 2, 1, 1, 1, 1, 1, 1, 1, 1};
static int PUZZLE_COL_CLUE_COUNTS[10] = {1, 2, 3, 2, 2, 2, 2, 3, 2, 1};

void nono_init_game(void) {
    g_game.rows = PUZZLE_ROWS;
    g_game.cols = PUZZLE_COLS;
    g_game.won = 0;
    g_game.selected_row = -1;
    g_game.selected_col = -1;
    
    // Copy solution
    for (int r = 0; r < g_game.rows; r++) {
        for (int c = 0; c < g_game.cols; c++) {
            g_game.solution[r][c] = PUZZLE_SOLUTION[r][c];
            g_game.user_grid[r][c] = CELL_EMPTY;
        }
    }
    
    // Copy row clues
    for (int r = 0; r < g_game.rows; r++) {
        g_game.row_clue_counts[r] = PUZZLE_ROW_CLUE_COUNTS[r];
        for (int i = 0; i < MAX_CLUES; i++) {
            g_game.row_clues[r][i] = PUZZLE_ROW_CLUES[r][i];
        }
    }
    
    // Copy col clues
    for (int c = 0; c < g_game.cols; c++) {
        g_game.col_clue_counts[c] = PUZZLE_COL_CLUE_COUNTS[c];
        for (int i = 0; i < MAX_CLUES; i++) {
            g_game.col_clues[c][i] = PUZZLE_COL_CLUES[c][i];
        }
    }
}

// Toggle cell: empty -> filled -> marked -> empty
void nono_click(int row, int col) {
    if (g_game.won) return;
    if (row < 0 || row >= g_game.rows || col < 0 || col >= g_game.cols) return;
    
    if (g_game.user_grid[row][col] == CELL_EMPTY) {
        g_game.user_grid[row][col] = CELL_FILLED;
    } else if (g_game.user_grid[row][col] == CELL_FILLED) {
        g_game.user_grid[row][col] = CELL_MARKED;
    } else {
        g_game.user_grid[row][col] = CELL_EMPTY;
    }
    
    g_game.selected_row = row;
    g_game.selected_col = col;
    
    // Check win condition
    int won = 1;
    for (int r = 0; r < g_game.rows && won; r++) {
        for (int c = 0; c < g_game.cols && won; c++) {
            if (g_game.solution[r][c] == 1 && g_game.user_grid[r][c] != CELL_FILLED) {
                won = 0;
            }
            if (g_game.solution[r][c] == 0 && g_game.user_grid[r][c] == CELL_FILLED) {
                won = 0;
            }
        }
    }
    g_game.won = won;
}

void nono_tick(void) {
    // No animation needed for this game
}

int nono_get_cell(int row, int col) {
    if (row < 0 || row >= g_game.rows || col < 0 || col >= g_game.cols) return -1;
    return g_game.user_grid[row][col];
}

int nono_get_row_clue(int row, int idx) {
    if (row < 0 || row >= g_game.rows || idx < 0 || idx >= MAX_CLUES) return -1;
    return g_game.row_clues[row][idx];
}

int nono_get_row_clue_count(int row) {
    if (row < 0 || row >= g_game.rows) return -1;
    return g_game.row_clue_counts[row];
}

int nono_get_col_clue(int col, int idx) {
    if (col < 0 || col >= g_game.cols || idx < 0 || idx >= MAX_CLUES) return -1;
    return g_game.col_clues[col][idx];
}

int nono_get_col_clue_count(int col) {
    if (col < 0 || col >= g_game.cols) return -1;
    return g_game.col_clue_counts[col];
}

int nono_get_rows(void) { return g_game.rows; }
int nono_get_cols(void) { return g_game.cols; }
int nono_get_won(void) { return g_game.won; }
