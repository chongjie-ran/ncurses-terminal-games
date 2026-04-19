// Dots and Boxes - Core Game Logic
#include "game.h"
#include <stdlib.h>
#include <string.h>

// h_edges[r][c]: horizontal edge between dot (r,c) and dot (r+1,c)
//   r: [0, H_EDGE_ROWS-1] = [0, BOX_ROWS]
//   c: [0, H_EDGE_COLS-1] = [0, BOX_COLS-1]
// v_edges[r][c]: vertical edge between dot (r,c) and dot (r,c+1)
//   r: [0, V_EDGE_ROWS-1] = [0, BOX_ROWS-1]
//   c: [0, V_EDGE_COLS-1] = [0, BOX_COLS]
static unsigned char h_edges[H_EDGE_ROWS][H_EDGE_COLS];
static unsigned char v_edges[V_EDGE_ROWS][V_EDGE_COLS];
// box_owner[r][c]: 255 = unclaimed, 0/1/2 = player owner
static unsigned char box_owner[BOX_ROWS][BOX_COLS];

int current_player = 0;
int scores[NUM_PLAYERS] = {0, 0, 0};
int phase = PHASE_PLACING;
int last_edge_r = -1, last_edge_c = -1, last_edge_type = -1;
int last_box_r = -1, last_box_c = -1;
int boxes_completed_last_turn = 0;

void db_init(void) {
    memset(h_edges, 0, sizeof(h_edges));
    memset(v_edges, 0, sizeof(v_edges));
    memset(box_owner, 255, sizeof(box_owner));
    current_player = 0;
    scores[0] = scores[1] = scores[2] = 0;
    phase = PHASE_PLACING;
    last_edge_r = last_edge_c = last_edge_type = -1;
    last_box_r = last_box_c = -1;
    boxes_completed_last_turn = 0;
}

// Count total completed boxes
static int total_boxes(void) {
    int total = 0;
    for (int r = 0; r < BOX_ROWS; r++)
        for (int c = 0; c < BOX_COLS; c++)
            if ((int)box_owner[r][c] < 255) total++;
    return total;
}

// Check if drawing horizontal edge at (r, c) completes adjacent boxes
// h_edge (r, c) is the TOP edge of box (r, c) and BOTTOM edge of box (r-1, c)
static int check_box_completions_h(int r, int c) {
    int completed = 0;
    // Top box at (r-1, c)
    if (r > 0) {
        if (h_edges[r-1][c] && v_edges[r-1][c] && v_edges[r-1][c+1] && h_edges[r][c]) {
            if ((int)box_owner[r-1][c] == 255) {
                box_owner[r-1][c] = (unsigned char)current_player;
                completed++;
            }
        }
    }
    // Bottom box at (r, c)
    if (r < BOX_ROWS) {
        if (h_edges[r][c] && v_edges[r][c] && v_edges[r][c+1] && h_edges[r+1][c]) {
            if ((int)box_owner[r][c] == 255) {
                box_owner[r][c] = (unsigned char)current_player;
                completed++;
            }
        }
    }
    return completed;
}

// Check if drawing vertical edge at (r, c) completes adjacent boxes
// v_edge (r, c) is the LEFT edge of box (r, c) and RIGHT edge of box (r, c-1)
static int check_box_completions_v(int r, int c) {
    int completed = 0;
    // Left box at (r, c-1)
    if (c > 0) {
        if (v_edges[r][c-1] && h_edges[r][c-1] && h_edges[r+1][c-1] && v_edges[r][c]) {
            if ((int)box_owner[r][c-1] == 255) {
                box_owner[r][c-1] = (unsigned char)current_player;
                completed++;
            }
        }
    }
    // Right box at (r, c)
    if (c < BOX_COLS) {
        if (v_edges[r][c] && h_edges[r][c] && h_edges[r+1][c] && v_edges[r][c+1]) {
            if ((int)box_owner[r][c] == 255) {
                box_owner[r][c] = (unsigned char)current_player;
                completed++;
            }
        }
    }
    return completed;
}

int db_draw_h_edge(int r, int c) {
    if (phase == PHASE_GAME_OVER) return -1;
    if (r < 0 || r >= H_EDGE_ROWS || c < 0 || c >= H_EDGE_COLS) return -1;
    if (h_edges[r][c]) return 1;

    h_edges[r][c] = 1;
    last_edge_r = r;
    last_edge_c = c;
    last_edge_type = EDGE_H;
    last_box_r = last_box_c = -1;

    int boxes = check_box_completions_h(r, c);
    boxes_completed_last_turn = boxes;

    if (boxes > 0) {
        scores[current_player] += boxes;
        last_box_r = r;
        last_box_c = c;
    }

    if (total_boxes() >= BOX_ROWS * BOX_COLS) {
        phase = PHASE_GAME_OVER;
    }

    return 0;
}

int db_draw_v_edge(int r, int c) {
    if (phase == PHASE_GAME_OVER) return -1;
    if (r < 0 || r >= V_EDGE_ROWS || c < 0 || c >= V_EDGE_COLS) return -1;
    if (v_edges[r][c]) return 1;

    v_edges[r][c] = 1;
    last_edge_r = r;
    last_edge_c = c;
    last_edge_type = EDGE_V;
    last_box_r = last_box_c = -1;

    int boxes = check_box_completions_v(r, c);
    boxes_completed_last_turn = boxes;

    if (boxes > 0) {
        scores[current_player] += boxes;
        last_box_r = r;
        last_box_c = c;
    }

    if (total_boxes() >= BOX_ROWS * BOX_COLS) {
        phase = PHASE_GAME_OVER;
    }

    return 0;
}

// Grid coordinate system (for JS renderer):
// Grid size: (2*BOX_ROWS+1) x (2*BOX_COLS+1) = 9x9
// (r,c) even/even = dot
// (r,c) even/odd  = horizontal edge
// (r,c) odd/even  = vertical edge
// (r,c) odd/odd   = box cell
int db_get_cell(int r, int c) {
    if (r < 0 || r > 2*BOX_ROWS || c < 0 || c > 2*BOX_COLS) return -1;

    if (r % 2 == 0 && c % 2 == 0) {
        return CELL_DOT;
    }

    if (r % 2 == 0 && c % 2 == 1) {
        int er = r / 2;
        int ec = (c - 1) / 2;
        if (er >= 0 && er < H_EDGE_ROWS && ec >= 0 && ec < H_EDGE_COLS) {
            if (h_edges[er][ec]) return CELL_H_LINE_0;
        }
        return CELL_EMPTY;
    }

    if (r % 2 == 1 && c % 2 == 0) {
        int er = (r - 1) / 2;
        int ec = c / 2;
        if (er >= 0 && er < V_EDGE_ROWS && ec >= 0 && ec < V_EDGE_COLS) {
            if (v_edges[er][ec]) return CELL_V_LINE_0;
        }
        return CELL_EMPTY;
    }

    if (r % 2 == 1 && c % 2 == 1) {
        int br = (r - 1) / 2;
        int bc = (c - 1) / 2;
        if (br >= 0 && br < BOX_ROWS && bc >= 0 && bc < BOX_COLS) {
            if ((int)box_owner[br][bc] < 255) {
                return CELL_BOX_0 + box_owner[br][bc];
            }
        }
        return CELL_EMPTY;
    }

    return CELL_EMPTY;
}

int db_get_score(int player) {
    if (player < 0 || player >= NUM_PLAYERS) return -1;
    return scores[player];
}

int db_get_current_player(void) {
    return current_player;
}

int db_get_phase(void) {
    return phase;
}

int db_get_last_edge(int *r, int *c, int *type) {
    if (r) *r = last_edge_r;
    if (c) *c = last_edge_c;
    if (type) *type = last_edge_type;
    return (last_edge_r >= 0) ? 0 : -1;
}

int db_get_last_box(int *r, int *c) {
    if (r) *r = last_box_r;
    if (c) *c = last_box_c;
    return (last_box_r >= 0) ? 0 : -1;
}
