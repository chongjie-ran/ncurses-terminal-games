#include "game.h"
#include <string.h>

// Global game state
GomokuGame g_game;

// Direction vectors: horizontal, vertical, diag1 (\), diag2 (/)
static const int DIR_X[DIR_COUNT] = {1, 0, 1, 1};
static const int DIR_Y[DIR_COUNT] = {0, 1, 1, -1};

// Count consecutive stones in a direction from (x,y)
static int count_stones(int x, int y, int dx, int dy, int player) {
    int count = 0;
    int cx = x + dx;
    int cy = y + dy;
    while (cx >= 0 && cx < BOARD_SIZE && cy >= 0 && cy < BOARD_SIZE) {
        if (g_game.board[cy][cx] == player) {
            count++;
            cx += dx;
            cy += dy;
        } else {
            break;
        }
    }
    return count;
}

// Check if player wins starting from (x,y)
static int check_win_from(int x, int y, int player) {
    for (int d = 0; d < DIR_COUNT; d++) {
        int dx = DIR_X[d];
        int dy = DIR_Y[d];

        // Count in positive direction
        int pos = 1 + count_stones(x, y, dx, dy, player);
        // Count in negative direction
        int neg = count_stones(x, y, -dx, -dy, player);
        int total = pos + neg;

        if (total >= WIN_COUNT) {
            // Found winning line - record it
            // Positive direction endpoint
            int end_x = x;
            int end_y = y;
            for (int i = 0; i < WIN_COUNT - 1; i++) {
                end_x += dx;
                end_y += dy;
            }
            // Make sure we don't go out of bounds for the end point
            // Actually find the actual end of the line
            int cx = x + dx * (WIN_COUNT - 1);
            int cy = y + dy * (WIN_COUNT - 1);
            // Clamp to board
            if (cx < 0) cx = 0;
            if (cx >= BOARD_SIZE) cx = BOARD_SIZE - 1;
            if (cy < 0) cy = 0;
            if (cy >= BOARD_SIZE) cy = BOARD_SIZE - 1;

            g_game.win_start_x = x;
            g_game.win_start_y = y;
            g_game.win_end_x = cx;
            g_game.win_end_y = cy;

            // Make sure start < end for consistent rendering
            if (g_game.win_start_x > g_game.win_end_x ||
                (g_game.win_start_x == g_game.win_end_x && g_game.win_start_y > g_game.win_end_y)) {
                int tmp;
                tmp = g_game.win_start_x; g_game.win_start_x = g_game.win_end_x; g_game.win_end_x = tmp;
                tmp = g_game.win_start_y; g_game.win_start_y = g_game.win_end_y; g_game.win_end_y = tmp;
            }
            return 1;
        }
    }
    return 0;
}

// Check if board is completely full
static int is_board_full(void) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (g_game.board[y][x] == CELL_EMPTY) {
                return 0;
            }
        }
    }
    return 1;
}

// --- Public API ---

void init_game(void) {
    memset(&g_game, 0, sizeof(g_game));
    g_game.current_player = CELL_BLACK;  // Black goes first
    g_game.game_over = 0;
    g_game.winner = WIN_NONE;
    g_game.last_x = -1;
    g_game.last_y = -1;
    g_game.win_start_x = -1;
    g_game.win_start_y = -1;
    g_game.win_end_x = -1;
    g_game.win_end_y = -1;
}

int place_stone(int x, int y) {
    if (g_game.game_over) {
        return PLACE_GAME_OVER;
    }

    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return PLACE_OCCUPIED;  // Invalid position
    }

    if (g_game.board[y][x] != CELL_EMPTY) {
        return PLACE_OCCUPIED;
    }

    // Place the stone
    g_game.board[y][x] = g_game.current_player;
    g_game.last_x = x;
    g_game.last_y = y;

    // Check for win
    if (check_win_from(x, y, g_game.current_player)) {
        g_game.game_over = 1;
        g_game.winner = g_game.current_player;
        return PLACE_OK;
    }

    // Check for draw (board full)
    if (is_board_full()) {
        g_game.game_over = 1;
        g_game.winner = WIN_DRAW;
        return PLACE_OK;
    }

    // Switch player
    g_game.current_player = (g_game.current_player == CELL_BLACK) ? CELL_WHITE : CELL_BLACK;

    return PLACE_OK;
}

int check_winner(void) {
    if (g_game.game_over) {
        return g_game.winner;
    }
    return WIN_NONE;
}

int get_cell(int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return CELL_EMPTY;
    }
    return g_game.board[y][x];
}

int get_current_player(void) {
    return g_game.current_player;
}

int get_game_over(void) {
    return g_game.game_over;
}

int get_win_start_x(void) { return g_game.win_start_x; }
int get_win_start_y(void) { return g_game.win_start_y; }
int get_win_end_x(void) { return g_game.win_end_x; }
int get_win_end_y(void) { return g_game.win_end_y; }

int get_last_x(void) { return g_game.last_x; }
int get_last_y(void) { return g_game.last_y; }
