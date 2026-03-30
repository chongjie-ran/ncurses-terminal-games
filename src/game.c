#include "game.h"
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static bool in_bounds(int r, int c) {
    return r >= 0 && r < ROWS && c >= 0 && c < COLS;
}

void game_load_best(MinesweeperGame *g) {
    FILE *f = fopen("minesweeper_best.txt", "r");
    if (f) {
        fscanf(f, "%d", &g->best_time);
        fclose(f);
    } else {
        g->best_time = -1;
    }
}

void game_save_best(MinesweeperGame *g) {
    if (g->best_time < 0) return;
    FILE *f = fopen("minesweeper_best.txt", "w");
    if (f) {
        fprintf(f, "%d\n", g->best_time);
        fclose(f);
    }
}

void game_init(MinesweeperGame *g) {
    game_load_best(g);
    game_reset(g);
}

void game_reset(MinesweeperGame *g) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            g->grid[r][c] = (Cell){0};

    g->state = STATE_MENU;
    g->first_click = true;
    g->revealed_count = 0;
    g->flagged_count = 0;
    g->elapsed_seconds = 0;
    g->timer_accum = 0.0f;
}

void game_place_mines(MinesweeperGame *g, int safe_r, int safe_c) {
    // Build candidates: exclude safe cell + 3x3 neighborhood
    int cand_r[ROWS * COLS];
    int cand_c[ROWS * COLS];
    int ncand = 0;

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (abs(r - safe_r) <= 1 && abs(c - safe_c) <= 1) continue;
            cand_r[ncand] = r;
            cand_c[ncand] = c;
            ncand++;
        }
    }

    // Fisher-Yates shuffle (simple inline)
    srand((unsigned int)time(NULL));
    for (int i = ncand - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp_r = cand_r[i]; cand_r[i] = cand_r[j]; cand_r[j] = tmp_r;
        int tmp_c = cand_c[i]; cand_c[i] = cand_c[j]; cand_c[j] = tmp_c;
    }

    int to_place = (MINES < ncand) ? MINES : ncand;
    for (int i = 0; i < to_place; i++) {
        g->grid[cand_r[i]][cand_c[i]].is_mine = true;
    }

    game_calc_adjacent(g);
}

void game_calc_adjacent(MinesweeperGame *g) {
    const int dr[8] = {-1,-1,-1,0,0,1,1,1};
    const int dc[8] = {-1,0,1,-1,1,-1,0,1};

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (g->grid[r][c].is_mine) {
                g->grid[r][c].adjacent_mines = -1;
                continue;
            }
            int cnt = 0;
            for (int k = 0; k < 8; k++) {
                int nr = r + dr[k], nc = c + dc[k];
                if (in_bounds(nr, nc) && g->grid[nr][nc].is_mine) cnt++;
            }
            g->grid[r][c].adjacent_mines = cnt;
        }
    }
}

void game_reveal(MinesweeperGame *g, int r, int c) {
    if (!in_bounds(r, c)) return;
    Cell *cell = &g->grid[r][c];
    if (cell->is_revealed || cell->is_flagged) return;

    cell->is_revealed = true;
    g->revealed_count++;

    if (cell->is_mine) {
        g->state = STATE_GAMEOVER;
        return;
    }

    if (cell->adjacent_mines == 0) {
        game_flood_fill(g, r, c);
    }
}

void game_flood_fill(MinesweeperGame *g, int r, int c) {
    const int dr[8] = {-1,-1,-1,0,0,1,1,1};
    const int dc[8] = {-1,0,1,-1,1,-1,0,1};

    for (int k = 0; k < 8; k++) {
        int nr = r + dr[k], nc = c + dc[k];
        if (!in_bounds(nr, nc)) continue;
        Cell *nb = &g->grid[nr][nc];
        if (nb->is_revealed || nb->is_flagged || nb->is_mine) continue;

        nb->is_revealed = true;
        g->revealed_count++;

        if (nb->adjacent_mines == 0) {
            game_flood_fill(g, nr, nc);
        }
    }
}

void game_toggle_flag(MinesweeperGame *g, int r, int c) {
    if (!in_bounds(r, c)) return;
    Cell *cell = &g->grid[r][c];
    if (cell->is_revealed) return;

    cell->is_flagged = !cell->is_flagged;
    g->flagged_count += cell->is_flagged ? 1 : -1;
}

void game_update(MinesweeperGame *g, float dt) {
    if (g->state == STATE_PLAYING) {
        g->timer_accum += dt;
        if (g->timer_accum >= 1.0f) {
            g->timer_accum -= 1.0f;
            g->elapsed_seconds++;
        }
        if (game_check_win(g)) {
            g->state = STATE_WIN;
            if (g->best_time < 0 || g->elapsed_seconds < g->best_time) {
                g->best_time = g->elapsed_seconds;
                game_save_best(g);
            }
        }
    }
}

int game_get_mouse_cell(Vector2 mouse, int *r, int *c) {
    int col = (int)((mouse.x - GRID_PAD_X) / CELL_SIZE);
    int row = (int)((mouse.y - GRID_PAD_Y) / CELL_SIZE);
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return -1;
    *r = row; *c = col;
    return 0;
}

bool game_check_win(MinesweeperGame *g) {
    return g->revealed_count == (ROWS * COLS - MINES);
}
