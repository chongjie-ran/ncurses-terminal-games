#include "game.h"
#include <string.h>

ConnectGame g_game;

static bool can_drop(int col) {
    return col >= 0 && col < COLS && g_game.board[0][col] == EMPTY;
}

static int get_drop_row(int col) {
    for (int r = ROWS - 1; r >= 0; r--) {
        if (g_game.board[r][col] == EMPTY) return r;
    }
    return -1;
}

static int check_win_at(int row, int col, int player) {
    int DX[4] = {1, 0, 1, 1};
    int DY[4] = {0, 1, 1, -1};
    for (int d = 0; d < 4; d++) {
        int dx = DX[d], dy = DY[d];
        int count = 1;
        int x1 = col, y1 = row;
        for (int i = 1; i <= 3; i++) {
            int nx = col + dx * i, ny = row + dy * i;
            if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS && g_game.board[ny][nx] == player) {
                count++; x1 = nx; y1 = ny;
            } else break;
        }
        int x2 = col, y2 = row;
        for (int i = 1; i <= 3; i++) {
            int nx = col - dx * i, ny = row - dy * i;
            if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS && g_game.board[ny][nx] == player) {
                count++; x2 = nx; y2 = ny;
            } else break;
        }
        if (count >= 4) {
            g_game.win_x1 = x2; g_game.win_y1 = y2;
            g_game.win_x2 = x1; g_game.win_y2 = y1;
            g_game.win_type = d;
            return 1;
        }
    }
    return 0;
}

void cf_init_game_impl(void) {
    memset(&g_game, 0, sizeof(g_game));
    g_game.state = STATE_MENU;
    g_game.current_player = PLAYER1;
}

void cf_start_game_impl(void) {
    memset(&g_game, 0, sizeof(g_game));
    g_game.state = STATE_PLAYING;
    g_game.current_player = PLAYER1;
}

int cf_drop_disc_impl(int col) {
    if (g_game.state != STATE_PLAYING) return -1;
    if (!can_drop(col)) return -1;
    int row = get_drop_row(col);
    if (row < 0) return -1;
    g_game.board[row][col] = g_game.current_player;
    g_game.move_count++;
    if (check_win_at(row, col, g_game.current_player)) {
        g_game.state = STATE_GAME_OVER;
        g_game.winner = g_game.current_player;
        return row;
    }
    if (g_game.move_count >= COLS * ROWS) {
        g_game.state = STATE_GAME_OVER;
        g_game.winner = DRAW;
        return row;
    }
    g_game.current_player = (g_game.current_player == PLAYER1) ? PLAYER2 : PLAYER1;
    return row;
}

int cf_get_cell_impl(int row, int col) {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return -1;
    return g_game.board[row][col];
}

int cf_can_drop_impl(int col) {
    return can_drop(col) ? 1 : 0;
}

int cf_get_state_impl(void) { return g_game.state; }
int cf_get_player_impl(void) { return g_game.current_player; }
int cf_get_winner_impl(void) { return g_game.winner; }
int cf_get_move_count_impl(void) { return g_game.move_count; }
int cf_get_win_x1_impl(void) { return g_game.win_x1; }
int cf_get_win_y1_impl(void) { return g_game.win_y1; }
int cf_get_win_x2_impl(void) { return g_game.win_x2; }
int cf_get_win_y2_impl(void) { return g_game.win_y2; }
int cf_get_win_type_impl(void) { return g_game.win_type; }
