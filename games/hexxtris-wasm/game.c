// game.c - Hextris WASM pure C game logic
// Hexagonal falling-piece puzzle game using odd-q offset coordinates

#include "game.h"
#include <stdlib.h>
#include <string.h>

// Piece templates (relative positions, anchor at index 0)
// Type 0: horizontal bar (3 hexes in a row)
static const HexPos TEMPLATE_HBAR[3] = {
    {0,0}, {1,0}, {2,0}
};
// Type 1: diagonal bar pointing down-right (3 hexes)
static const HexPos TEMPLATE_DIAG_DR[3] = {
    {0,0}, {0,1}, {1,1}
};
// Type 2: diagonal bar pointing up-right (3 hexes)
static const HexPos TEMPLATE_DIAG_UR[3] = {
    {0,0}, {0,-1}, {1,-1}
};

static const int TEMPLATE_SIZES[PIECE_TYPES] = {3, 3, 3};
static const HexPos* TEMPLATES[PIECE_TYPES] = {
    TEMPLATE_HBAR, TEMPLATE_DIAG_DR, TEMPLATE_DIAG_UR
};

// Direction deltas for odd-q offset hex grid (flat-topped)
// Even columns (col%2==0): row_offset_even = 0
// Odd columns (col%2==1): row_offset_odd = 1
void get_dir_delta(int dir, int anchor_col, int* dcol, int* drow) {
    int even = (anchor_col % 2 == 0) ? 1 : 0;
    switch(dir) {
        case DIR_RIGHT:    *dcol=1; *drow=0; break;
        case DIR_UPRIGHT:  *dcol=even?0:1; *drow=even?-1:-1; break;
        case DIR_UPLEFT:   *dcol=even?-1:0; *drow=even?-1:-1; break;
        case DIR_LEFT:     *dcol=-1; *drow=0; break;
        case DIR_DOWNLEFT:  *dcol=even?-1:0; *drow=even?+1:+1; break;
        case DIR_DOWNRIGHT: *dcol=even?0:1; *drow=even?+1:+1; break;
        default: *dcol=0; *drow=0; break;
    }
}

bool is_in_bounds(int col, int row) {
    if (col < 0 || col >= GRID_W) return false;
    if (row < 0 || row >= GRID_H) return false;
    return true;
}

bool is_valid_pos(HextrisGame* g, int col, int row) {
    if (!is_in_bounds(col, row)) return false;
    if (g->grid[col][row] != 0) return false;
    return true;
}

bool move_piece(HextrisGame* g, int dcol, int drow) {
    if (g->game_over) return false;
    if (g->lock_input) return false;

    Piece* p = &g->current_piece;
    for (int i = 0; i < p->num_blocks; i++) {
        int target_col = g->piece_x + p->blocks[i].col + dcol;
        int target_row = g->piece_y + p->blocks[i].row + drow;
        if (!is_in_bounds(target_col, target_row)) return false;
        if (g->grid[target_col][target_row] != 0) return false;
    }
    g->piece_x += dcol;
    g->piece_y += drow;
    return true;
}

bool set_direction(HextrisGame* g, int dir) {
    if (g->game_over) return false;
    if (g->lock_input) return false;
    if (dir < 0 || dir > 5) return false;

    // Rotate piece by 'dir' steps clockwise
    Piece* p = &g->current_piece;
    int new_dir = (g->piece_dir + dir) % 6;

    // Compute new positions after rotation
    HexPos new_blocks[MAX_PIECE_BLOCKS];
    for (int i = 0; i < p->num_blocks; i++) {
        int bc = p->blocks[i].col;
        int br = p->blocks[i].row;
        for (int d = 0; d < new_dir; d++) {
            int dc, dr;
            get_dir_delta(d, g->piece_x + bc, &dc, &dr);
            bc += dc;
            br += dr;
        }
        new_blocks[i].col = bc;
        new_blocks[i].row = br;
    }

    // Check if new positions are valid
    for (int i = 0; i < p->num_blocks; i++) {
        int col = g->piece_x + new_blocks[i].col;
        int row = g->piece_y + new_blocks[i].row;
        if (!is_in_bounds(col, row) || g->grid[col][row] != 0) {
            return false;
        }
    }

    // Apply rotation
    for (int i = 0; i < p->num_blocks; i++) {
        p->blocks[i] = new_blocks[i];
    }
    g->piece_dir = new_dir;
    return true;
}

bool rotate_piece_fn(HextrisGame* g) {
    return set_direction(g, 1);
}

void make_piece(Piece* p, int type, int color) {
    p->num_blocks = TEMPLATE_SIZES[type];
    p->color = color;
    for (int i = 0; i < p->num_blocks; i++) {
        p->blocks[i] = TEMPLATES[type][i];
    }
}

int random_type(void) {
    return rand() % PIECE_TYPES;
}

int random_color(void) {
    return 1 + (rand() % 3);
}

void init_game(HextrisGame* g) {
    memset(g, 0, sizeof(HextrisGame));
    g->tick_interval = 40;
}

void start_game(HextrisGame* g) {
    memset(g->grid, 0, sizeof(g->grid));
    g->score = 0;
    g->lines_cleared = 0;
    g->game_over = 0;
    g->lock_input = false;
    g->tick_counter = 0;
    g->piece_dir = 0;
    make_piece(&g->current_piece, random_type(), random_color());
    make_piece(&g->next_piece, random_type(), random_color());
    g->piece_x = GRID_W / 2;
    g->piece_y = 1;
}

void lock_piece(HextrisGame* g) {
    if (g->game_over) return;
    Piece* p = &g->current_piece;
    for (int i = 0; i < p->num_blocks; i++) {
        int col = g->piece_x + p->blocks[i].col;
        int row = g->piece_y + p->blocks[i].row;
        if (col >= 0 && col < GRID_W && row >= 0 && row < GRID_H) {
            g->grid[col][row] = p->color;
        }
    }

    int lines = check_lines(g);
    if (lines > 0) {
        g->lines_cleared += lines;
        g->score += lines * lines * 10;
        if (g->tick_interval > 5) {
            g->tick_interval -= lines * 2;
        }
    }

    g->current_piece = g->next_piece;
    make_piece(&g->next_piece, random_type(), random_color());
    g->piece_dir = 0;
    g->piece_x = GRID_W / 2;
    g->piece_y = 1;

    for (int i = 0; i < g->current_piece.num_blocks; i++) {
        int col = g->piece_x + g->current_piece.blocks[i].col;
        int row = g->piece_y + g->current_piece.blocks[i].row;
        if (is_in_bounds(col, row) && g->grid[col][row] != 0) {
            g->game_over = 1;
            return;
        }
    }
}

bool hard_drop_fn(HextrisGame* g) {
    if (g->game_over) return false;
    int drops = 0;
    while (move_piece(g, 0, 1)) {
        drops++;
    }
    g->score += drops * 2;
    lock_piece(g);
    return true;
}

int check_lines(HextrisGame* g) {
    int lines = 0;
    for (int row = 0; row < GRID_H; row++) {
        bool full = true;
        for (int col = 0; col < GRID_W; col++) {
            if (g->grid[col][row] == 0) {
                full = false;
                break;
            }
        }
        if (full) {
            lines++;
            remove_line(g, row);
            row--;
        }
    }
    return lines;
}

void remove_line(HextrisGame* g, int row) {
    for (int r = row; r > 0; r--) {
        for (int col = 0; col < GRID_W; col++) {
            g->grid[col][r] = g->grid[col][r-1];
        }
    }
    for (int col = 0; col < GRID_W; col++) {
        g->grid[col][0] = 0;
    }
}

void tick_game(HextrisGame* g) {
    if (g->game_over) return;
    g->tick_counter++;
    if (g->tick_counter >= g->tick_interval) {
        g->tick_counter = 0;
        if (!move_piece(g, 0, 1)) {
            lock_piece(g);
        }
    }
}
