#include "game.h"
#include <stdlib.h>
#include <string.h>

static int dirs[8][2] = {
    {-1,-1}, {-1,0}, {-1,1},
    {0,-1},          {0,1},
    {1,-1},  {1,0},  {1,1}
};

void game_init(GameState* gs) {
    memset(gs->cells, 0, sizeof(gs->cells));
    memset(gs->revealed, 0, sizeof(gs->revealed));
    gs->game_state = 0;
    gs->first_click = 1;
    gs->cells_left = GRID_ROWS * GRID_COLS - NUM_MINES;
}

static int is_valid(int r, int c) {
    return r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS;
}

void game_place_mines(GameState* gs, int click_row, int click_col) {
    // Place mines randomly, avoiding first click
    int placed = 0;
    while (placed < NUM_MINES) {
        int r = rand() % GRID_ROWS;
        int c = rand() % GRID_COLS;
        // Avoid first click and its neighbors
        if (abs(r - click_row) <= 1 && abs(c - click_col) <= 1) continue;
        if (gs->cells[r][c] == CELL_MINE) continue;
        gs->cells[r][c] = CELL_MINE;
        placed++;
    }
    // Compute adjacent counts
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (gs->cells[r][c] == CELL_MINE) continue;
            int cnt = 0;
            for (int d = 0; d < 8; d++) {
                int nr = r + dirs[d][0];
                int nc = c + dirs[d][1];
                if (is_valid(nr, nc) && gs->cells[nr][nc] == CELL_MINE) cnt++;
            }
            gs->cells[r][c] = cnt;
        }
    }
}

// Returns: 0=ok, 1=won, 2=lost
int game_reveal(GameState* gs, int row, int col) {
    if (!is_valid(row, col) || gs->revealed[row][col]) return 0;
    if (gs->cells[row][col] == CELL_FLAG) return 0;
    if (gs->game_state != 0) return 0;

    if (gs->first_click) {
        gs->first_click = 0;
        game_place_mines(gs, row, col);
    }

    if (gs->cells[row][col] == CELL_MINE) {
        gs->revealed[row][col] = 1;
        gs->game_state = 2;
        return 2;
    }

    // BFS flood fill for 0-cells
    int stack_r[GRID_ROWS * GRID_COLS];
    int stack_c[GRID_ROWS * GRID_COLS];
    int sp = 0;
    stack_r[sp] = row; stack_c[sp] = col; sp++;

    while (sp > 0) {
        sp--;
        int r = stack_r[sp];
        int c = stack_c[sp];
        if (!is_valid(r, c) || gs->revealed[r][c]) continue;
        if (gs->cells[r][c] == CELL_MINE || gs->cells[r][c] == CELL_FLAG) continue;

        gs->revealed[r][c] = 1;
        gs->cells_left--;

        if (gs->cells[r][c] == 0) {
            for (int d = 0; d < 8; d++) {
                int nr = r + dirs[d][0];
                int nc = c + dirs[d][1];
                if (is_valid(nr, nc) && !gs->revealed[nr][nc]) {
                    stack_r[sp] = nr; stack_c[sp] = nc; sp++;
                }
            }
        }
    }

    if (gs->cells_left == 0) gs->game_state = 1;
    return gs->game_state == 1 ? 1 : 0;
}

int game_toggle_flag(GameState* gs, int row, int col) {
    if (!is_valid(row, col) || gs->revealed[row][col]) return 0;
    if (gs->game_state != 0) return 0;
    if (gs->cells[row][col] == CELL_FLAG) {
        gs->cells[row][col] = CELL_HIDDEN;
    } else if (gs->cells[row][col] != CELL_MINE) {
        gs->cells[row][col] = CELL_FLAG;
    }
    return 0;
}

// Returns display value: 0-8, 9=mine, 10=flag(hidden), 20=flag(revealed)
int game_get_cell(GameState* gs, int row, int col) {
    if (!is_valid(row, col)) return -1;
    if (gs->revealed[row][col]) {
        return gs->cells[row][col];  // 0-8 or 9(mine)
    }
    if (gs->cells[row][col] == CELL_FLAG) return 10;
    return 0;  // hidden but not flagged
}

int game_is_revealed(GameState* gs, int row, int col) {
    if (!is_valid(row, col)) return 0;
    return gs->revealed[row][col] ? 1 : 0;
}
