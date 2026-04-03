#include "game.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

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
    g_game.ai_mode = MODE_2P;
    g_game.ai_difficulty = AI_MEDIUM;
}

void cf_start_game_impl(void) {
    memset(&g_game, 0, sizeof(g_game));
    g_game.state = STATE_PLAYING;
    g_game.current_player = PLAYER1;
    g_game.ai_mode = MODE_2P;
    g_game.ai_difficulty = AI_MEDIUM;
}

void cf_start_game_ex_impl(int ai_mode, int ai_difficulty) {
    memset(&g_game, 0, sizeof(g_game));
    g_game.state = STATE_PLAYING;
    g_game.current_player = PLAYER1;
    g_game.ai_mode = ai_mode;
    g_game.ai_difficulty = (ai_difficulty >= AI_EASY && ai_difficulty <= AI_HARD) ? ai_difficulty : AI_MEDIUM;
}

int cf_drop_disc_impl(int col) {
    if (g_game.state != STATE_PLAYING) return -1;
    if (!can_drop(col)) return -1;
    int row = get_drop_row(col);
    if (row < 0) return -1;
    g_game.board[row][col] = g_game.current_player;
    g_game.last_col = col;
    g_game.last_row = row;
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

int cf_can_drop_impl(int col) { return can_drop(col) ? 1 : 0; }
int cf_get_state_impl(void) { return g_game.state; }
int cf_get_player_impl(void) { return g_game.current_player; }
int cf_get_winner_impl(void) { return g_game.winner; }
int cf_get_move_count_impl(void) { return g_game.move_count; }
int cf_get_win_x1_impl(void) { return g_game.win_x1; }
int cf_get_win_y1_impl(void) { return g_game.win_y1; }
int cf_get_win_x2_impl(void) { return g_game.win_x2; }
int cf_get_win_y2_impl(void) { return g_game.win_y2; }
int cf_get_win_type_impl(void) { return g_game.win_type; }
int cf_get_last_col_impl(void) { return g_game.last_col; }
int cf_get_last_row_impl(void) { return g_game.last_row; }
int cf_get_ai_mode_impl(void) { return g_game.ai_mode; }
int cf_get_ai_difficulty_impl(void) { return g_game.ai_difficulty; }
int cf_is_ai_thinking_impl(void) { return g_game.ai_thinking; }

// Score a window of 4 cells
static int score_window(int b[ROWS][COLS], int row, int col, int dr, int dc, int player) {
    int opp = (player == PLAYER1) ? PLAYER2 : PLAYER1;
    int pc = 0, oc = 0, ec = 0;
    for (int i = 0; i < 4; i++) {
        int r = row + i * dr, c = col + i * dc;
        if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return 0;
        if (b[r][c] == player) pc++;
        else if (b[r][c] == opp) oc++;
        else ec++;
    }
    if (pc == 4) return 10000;
    if (pc == 3 && ec == 1) return 50;
    if (pc == 2 && ec == 2) return 10;
    if (oc == 3 && ec == 1) return -80;
    return 0;
}

static int evaluate_board(int b[ROWS][COLS], int player) {
    int score = 0;
    for (int c = 0; c < COLS; c++)
        for (int r = 0; r <= ROWS - 4; r++)
            score += score_window(b, r, c, 1, 0, player);
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c <= COLS - 4; c++)
            score += score_window(b, r, c, 0, 1, player);
    for (int r = 0; r <= ROWS - 4; r++)
        for (int c = 0; c <= COLS - 4; c++)
            score += score_window(b, r, c, 1, 1, player);
    for (int r = 3; r < ROWS; r++)
        for (int c = 0; c <= COLS - 4; c++)
            score += score_window(b, r, c, -1, 1, player);
    for (int r = 0; r < ROWS; r++)
        if (b[r][COLS/2] == player) score += 3;
    return score;
}

static int check_winner_static(int b[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (b[r][c] == EMPTY) continue;
            int DX[4] = {1, 0, 1, 1};
            int DY[4] = {0, 1, 1, -1};
            for (int d = 0; d < 4; d++) {
                int dx = DX[d], dy = DY[d];
                int count = 1;
                for (int i = 1; i <= 3; i++) {
                    int nx = c + dx * i, ny = r + dy * i;
                    if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS && b[ny][nx] == b[r][c]) count++;
                    else break;
                }
                if (count >= 4) return b[r][c];
            }
        }
    }
    return 0;
}

static int minimax(int b[ROWS][COLS], int depth, int alpha, int beta, int maximizing) {
    int winner = check_winner_static(b);
    if (winner == PLAYER2) return 100000 + depth;
    if (winner == PLAYER1) return -100000 - depth;
    if (depth == 0) return evaluate_board(b, PLAYER2) - evaluate_board(b, PLAYER1);
    int valid_cols[COLS], valid_count = 0;
    for (int c = 0; c < COLS; c++) if (b[0][c] == EMPTY) valid_cols[valid_count++] = c;
    if (valid_count == 0) return 0;
    for (int i = 0; i < valid_count - 1; i++)
        for (int j = 0; j < valid_count - i - 1; j++)
            if (abs(valid_cols[j] - COLS/2) > abs(valid_cols[j+1] - COLS/2)) {
                int tmp = valid_cols[j]; valid_cols[j] = valid_cols[j+1]; valid_cols[j+1] = tmp;
            }
    if (maximizing) {
        int value = INT_MIN;
        for (int i = 0; i < valid_count; i++) {
            int c = valid_cols[i], drop_row = -1;
            for (int r = ROWS - 1; r >= 0; r--) if (b[r][c] == EMPTY) { drop_row = r; break; }
            if (drop_row < 0) continue;
            b[drop_row][c] = PLAYER2;
            int eval = minimax(b, depth - 1, alpha, beta, 0);
            b[drop_row][c] = EMPTY;
            if (eval > value) value = eval;
            if (value > beta) { beta = value; if (beta >= alpha) break; }
        }
        return value == INT_MIN ? 0 : value;
    } else {
        int value = INT_MAX;
        for (int i = 0; i < valid_count; i++) {
            int c = valid_cols[i], drop_row = -1;
            for (int r = ROWS - 1; r >= 0; r--) if (b[r][c] == EMPTY) { drop_row = r; break; }
            if (drop_row < 0) continue;
            b[drop_row][c] = PLAYER1;
            int eval = minimax(b, depth - 1, alpha, beta, 1);
            b[drop_row][c] = EMPTY;
            if (eval < value) value = eval;
            if (value < alpha) { alpha = value; if (beta <= alpha) break; }
        }
        return value == INT_MAX ? 0 : value;
    }
}

int cf_ai_move_impl(void) {
    if (g_game.state != STATE_PLAYING) return -1;
    if (g_game.current_player != PLAYER2 || g_game.ai_mode != MODE_VS_AI) return -1;
    g_game.ai_thinking = 1;
    int board_copy[ROWS][COLS];
    memcpy(board_copy, g_game.board, sizeof(g_game.board));
    int depth = g_game.ai_difficulty;
    int pieces = 0;
    for (int r = 0; r < ROWS; r++) for (int c = 0; c < COLS; c++) if (board_copy[r][c] != EMPTY) pieces++;
    if (pieces <= 6) depth++;
    int best_col = -1, best_score = INT_MIN;
    int valid_cols[COLS], valid_count = 0;
    for (int c = 0; c < COLS; c++) if (board_copy[0][c] == EMPTY) valid_cols[valid_count++] = c;
    for (int i = 0; i < valid_count - 1; i++)
        for (int j = 0; j < valid_count - i - 1; j++)
            if (abs(valid_cols[j] - COLS/2) > abs(valid_cols[j+1] - COLS/2)) {
                int tmp = valid_cols[j]; valid_cols[j] = valid_cols[j+1]; valid_cols[j+1] = tmp;
            }
    for (int i = 0; i < valid_count; i++) {
        int c = valid_cols[i], drop_row = -1;
        for (int r = ROWS - 1; r >= 0; r--) if (board_copy[r][c] == EMPTY) { drop_row = r; break; }
        if (drop_row < 0) continue;
        board_copy[drop_row][c] = PLAYER2;
        if (check_winner_static(board_copy) == PLAYER2) { board_copy[drop_row][c] = EMPTY; g_game.ai_thinking = 0; cf_drop_disc_impl(c); return c; }
        board_copy[drop_row][c] = EMPTY;
        board_copy[drop_row][c] = PLAYER1;
        if (check_winner_static(board_copy) == PLAYER1) { board_copy[drop_row][c] = EMPTY; best_col = c; break; }
        board_copy[drop_row][c] = EMPTY;
        board_copy[drop_row][c] = PLAYER2;
        int score = minimax(board_copy, depth - 1, INT_MIN, INT_MAX, 0);
        board_copy[drop_row][c] = EMPTY;
        if (score > best_score) { best_score = score; best_col = c; }
    }
    g_game.ai_thinking = 0;
    if (best_col >= 0) { cf_drop_disc_impl(best_col); return best_col; }
    return -1;
}
