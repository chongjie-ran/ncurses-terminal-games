#include "othello.h"
#include <string.h>

static const int DIRS[8][2] = {
    {-1,-1}, {-1,0}, {-1,1},
    {0,-1},          {0,1},
    {1,-1},  {1,0},  {1,1}
};

void othello_init(OthelloGame* g) {
    memset(g, 0, sizeof(OthelloGame));
    for (int r = 0; r < BOARD_SIZE; r++)
        for (int c = 0; c < BOARD_SIZE; c++)
            g->board[r][c] = EMPTY;
    g->board[3][3] = WHITE;
    g->board[3][4] = BLACK;
    g->board[4][3] = BLACK;
    g->board[4][4] = WHITE;
    g->current_player = BLACK;
    g->black_count = 2;
    g->white_count = 2;
    g->game_over = 0;
    g->winner = 0;
    othello_get_valid_moves(g);
}

int othello_get_winner(OthelloGame* g) {
    if (!g->game_over) return 0;
    if (g->black_count > g->white_count) return BLACK;
    if (g->white_count > g->black_count) return WHITE;
    return 0;
}

static int in_bounds(int r, int c) {
    return r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE;
}

static int get_flippable_in_dir(OthelloGame* g, int r, int c, int dr, int dc, int player) {
    int opp = (player == BLACK) ? WHITE : BLACK;
    int nr = r + dr, nc = c + dc;
    int count = 0;
    while (in_bounds(nr, nc) && g->board[nr][nc] == opp) {
        nr += dr; nc += dc; count++;
    }
    if (!in_bounds(nr, nc) || g->board[nr][nc] != player) return 0;
    return count;
}

int othello_is_valid_move(OthelloGame* g, int row, int col) {
    if (!in_bounds(row, col) || g->board[row][col] != EMPTY) return 0;
    int player = g->current_player;
    for (int d = 0; d < 8; d++) {
        if (get_flippable_in_dir(g, row, col, DIRS[d][0], DIRS[d][1], player) > 0)
            return 1;
    }
    return 0;
}

void othello_get_valid_moves(OthelloGame* g) {
    g->valid_count = 0;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (othello_is_valid_move(g, r, c))
                g->valid_moves[g->valid_count++] = r * BOARD_SIZE + c;
        }
    }
}

int othello_make_move(OthelloGame* g, int row, int col) {
    if (!othello_is_valid_move(g, row, col)) return 0;
    int player = g->current_player;
    int opp = (player == BLACK) ? WHITE : BLACK;
    g->board[row][col] = player;
    int total_flipped = 1;
    
    for (int d = 0; d < 8; d++) {
        int count = get_flippable_in_dir(g, row, col, DIRS[d][0], DIRS[d][1], player);
        if (count > 0) {
            int nr = row + DIRS[d][0], nc = col + DIRS[d][1];
            for (int i = 0; i < count; i++) {
                g->board[nr][nc] = player;
                nr += DIRS[d][0]; nc += DIRS[d][1];
                total_flipped++;
            }
        }
    }
    
    if (player == BLACK) {
        g->black_count += total_flipped;
        g->white_count -= (total_flipped - 1);
    } else {
        g->white_count += total_flipped;
        g->black_count -= (total_flipped - 1);
    }
    
    // 换手
    g->current_player = opp;
    othello_get_valid_moves(g);
    
    if (othello_is_game_over(g)) {
        g->game_over = 1;
        g->winner = othello_get_winner(g);
    } else if (g->valid_count == 0) {
        // 对手没棋，换回原玩家继续
        g->current_player = player;
        othello_get_valid_moves(g);
        if (g->valid_count == 0) {
            g->game_over = 1;
            g->winner = othello_get_winner(g);
        }
    }
    return 1;
}

int othello_is_game_over(OthelloGame* g) {
    return (g->black_count + g->white_count == BOARD_SIZE * BOARD_SIZE)
        || (g->black_count == 0 || g->white_count == 0);
}
