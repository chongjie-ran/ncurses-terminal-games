// Dots and Boxes - Pure C + Emscripten
// 点格棋：连接相邻点形成方格，完成方格得分

#ifndef GAME_H
#define GAME_H

// 4x4 grid of boxes = 5x5 grid of dots
#define BOX_ROWS 4
#define BOX_COLS 4
#define NUM_PLAYERS 3

// Edge dimensions (one more than boxes)
#define H_EDGE_ROWS (BOX_ROWS + 1)  // 5
#define H_EDGE_COLS BOX_COLS        // 4
#define V_EDGE_ROWS BOX_ROWS        // 4
#define V_EDGE_COLS (BOX_COLS + 1)  // 5

// Edge types
#define EDGE_H 0  // horizontal
#define EDGE_V 1  // vertical

// Cell values (for JS side display)
#define CELL_EMPTY 0
#define CELL_DOT 1
#define CELL_H_LINE_0 2
#define CELL_H_LINE_1 3
#define CELL_H_LINE_2 4
#define CELL_V_LINE_0 5
#define CELL_V_LINE_1 6
#define CELL_V_LINE_2 7
#define CELL_BOX_0 8
#define CELL_BOX_1 9
#define CELL_BOX_2 10

// Game phases
#define PHASE_PLACING 0
#define PHASE_GAME_OVER 1

// Public state
extern int current_player;
extern int scores[NUM_PLAYERS];
extern int phase;
extern int last_edge_r, last_edge_c, last_edge_type;
extern int last_box_r, last_box_c;
extern int boxes_completed_last_turn;

// Core API
void db_init(void);
int db_draw_h_edge(int r, int c);
int db_draw_v_edge(int r, int c);
int db_get_cell(int r, int c);
int db_get_score(int player);
int db_get_current_player(void);
int db_get_phase(void);
int db_get_last_edge(int *r, int *c, int *type);
int db_get_last_box(int *r, int *c);

#endif
