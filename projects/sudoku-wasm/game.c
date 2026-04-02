#include "game.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

SudokuGame g_game;

// Difficulty: how many cells to remove
static int g_remove_count[4] = {35, 45, 52, 57};

static bool is_valid_placement(int row, int col, int digit) {
    // Check row
    for (int c = 0; c < BOARD_SIZE; c++) {
        if (c != col && g_game.board[row][c] == digit) return false;
    }
    // Check column
    for (int r = 0; r < BOARD_SIZE; r++) {
        if (r != row && g_game.board[r][col] == digit) return false;
    }
    // Check 3x3 box
    int box_r = (row / BOX_SIZE) * BOX_SIZE;
    int box_c = (col / BOX_SIZE) * BOX_SIZE;
    for (int r = box_r; r < box_r + BOX_SIZE; r++) {
        for (int c = box_c; c < box_c + BOX_SIZE; c++) {
            if ((r != row || c != col) && g_game.board[r][c] == digit) return false;
        }
    }
    return true;
}

static bool solve_board(uint8_t board[BOARD_SIZE][BOARD_SIZE]) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board[row][col] == 0) {
                // Shuffle digits for random puzzle generation
                uint8_t digits[9] = {1,2,3,4,5,6,7,8,9};
                for (int i = 8; i > 0; i--) {
                    int j = rand() % (i + 1);
                    uint8_t tmp = digits[i]; digits[i] = digits[j]; digits[j] = tmp;
                }
                for (int di = 0; di < 9; di++) {
                    int d = digits[di];
                    bool valid = true;
                    // Check row
                    for (int c = 0; c < BOARD_SIZE; c++) if (board[row][c] == d) { valid = false; break; }
                    if (valid) {
                        // Check column
                        for (int r = 0; r < BOARD_SIZE; r++) if (board[r][col] == d) { valid = false; break; }
                    }
                    if (valid) {
                        // Check box
                        int box_r = (row / BOX_SIZE) * BOX_SIZE;
                        int box_c = (col / BOX_SIZE) * BOX_SIZE;
                        for (int r = box_r; valid && r < box_r + BOX_SIZE; r++) {
                            for (int c = box_c; c < box_c + BOX_SIZE; c++) {
                                if (board[r][c] == d) { valid = false; break; }
                            }
                        }
                    }
                    if (valid) {
                        board[row][col] = d;
                        if (solve_board(board)) return true;
                        board[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

static bool solve_from_current(void) {
    // Copy current board to a temp and solve it
    uint8_t temp[BOARD_SIZE][BOARD_SIZE];
    memcpy(temp, g_game.board, sizeof(temp));
    if (solve_board(temp)) {
        memcpy(g_game.solution, temp, sizeof(temp));
        return true;
    }
    return false;
}

static void update_candidates(void) {
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (g_game.board[r][c] != 0) {
                g_game.candidates[r][c] = 0;
            } else {
                uint16_t mask = 0;
                for (int d = 1; d <= 9; d++) {
                    bool valid = true;
                    for (int cc = 0; valid && cc < BOARD_SIZE; cc++) if (cc != c && g_game.board[r][cc] == d) valid = false;
                    for (int rr = 0; valid && rr < BOARD_SIZE; rr++) if (rr != r && g_game.board[rr][c] == d) valid = false;
                    int box_r = (r / BOX_SIZE) * BOX_SIZE;
                    int box_c = (c / BOX_SIZE) * BOX_SIZE;
                    for (int rr = box_r; valid && rr < box_r + BOX_SIZE; rr++) {
                        for (int cc = box_c; cc < box_c + BOX_SIZE; cc++) {
                            if ((rr != r || cc != c) && g_game.board[rr][cc] == d) valid = false;
                        }
                    }
                    if (valid) mask |= (1 << (d - 1));
                }
                g_game.candidates[r][c] = (uint8_t)mask;
            }
        }
    }
}

static void generate_puzzle(Difficulty diff) {
    srand((unsigned int)time(NULL));
    // First generate a complete solved board
    memset(g_game.board, 0, sizeof(g_game.board));
    solve_board(g_game.board);
    memcpy(g_game.solution, g_game.board, sizeof(g_game.solution));

    // Then remove cells based on difficulty
    int remove = g_remove_count[diff];
    int positions[81];
    for (int i = 0; i < 81; i++) positions[i] = i;
    // Shuffle
    for (int i = 80; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = positions[i]; positions[i] = positions[j]; positions[j] = tmp;
    }

    int removed = 0;
    for (int i = 0; i < 81 && removed < remove; i++) {
        int idx = positions[i];
        int row = idx / 9;
        int col = idx % 9;
        uint8_t saved = g_game.board[row][col];
        g_game.board[row][col] = 0;

        // Count solutions - for simplicity, just make sure it's still solvable
        uint8_t temp[BOARD_SIZE][BOARD_SIZE];
        memcpy(temp, g_game.board, sizeof(temp));
        if (solve_board(temp)) {
            removed++;
        } else {
            g_game.board[row][col] = saved; // restore
        }
    }

    // Mark pre-filled cells as fixed
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            g_game.fixed[r][c] = (g_game.board[r][c] != 0) ? 1 : 0;
        }
    }
}

void sudoku_start_game_impl(Difficulty diff) {
    memset(&g_game, 0, sizeof(g_game));
    g_game.difficulty = diff;
    g_game.state = STATE_PLAYING;
    g_game.selected_row = 4;
    g_game.selected_col = 4;
    g_game.selected_num = 0;
    g_game.show_candidates = false;
    generate_puzzle(diff);
    update_candidates();
}

void sudoku_select_cell_impl(int row, int col) {
    if (g_game.state != STATE_PLAYING && g_game.state != STATE_EDIT) return;
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return;
    g_game.selected_row = (uint8_t)row;
    g_game.selected_col = (uint8_t)col;
}

void sudoku_input_digit_impl(int digit) {
    if (g_game.state != STATE_PLAYING) return;
    if (digit < 1 || digit > 9) return;
    int r = g_game.selected_row, c = g_game.selected_col;
    if (g_game.fixed[r][c]) return; // can't edit fixed cells

    if (g_game.board[r][c] == digit) return; // same digit, do nothing

    // Check if valid
    uint8_t old = g_game.board[r][c];
    g_game.board[r][c] = (uint8_t)digit;
    if (!is_valid_placement(r, c, digit)) {
        g_game.error_count++;
        g_game.board[r][c] = old; // revert on invalid
    }
    update_candidates();

    // Check if solved
    if (sudoku_is_solved_impl()) {
        g_game.state = STATE_SOLVED;
    }
}

void sudoku_erase_impl(void) {
    if (g_game.state != STATE_PLAYING) return;
    int r = g_game.selected_row, c = g_game.selected_col;
    if (g_game.fixed[r][c]) return;
    g_game.board[r][c] = 0;
    update_candidates();
}

void sudoku_toggle_candidates_impl(void) {
    g_game.show_candidates = (g_game.show_candidates == 0);
}

void sudoku_hint_impl(void) {
    if (g_game.state != STATE_PLAYING) return;
    int r = g_game.selected_row, c = g_game.selected_col;
    if (g_game.fixed[r][c]) return;
    // Solve if not already solved, then fill correct digit
    if (g_game.solution[r][c] != 0) {
        g_game.board[r][c] = g_game.solution[r][c];
        g_game.hints_used++;
        update_candidates();
    } else {
        solve_from_current();
        if (g_game.solution[r][c] != 0) {
            g_game.board[r][c] = g_game.solution[r][c];
            g_game.hints_used++;
            update_candidates();
        }
    }
    if (sudoku_is_solved_impl()) {
        g_game.state = STATE_SOLVED;
    }
}

void sudoku_solve_impl(void) {
    if (g_game.state != STATE_PLAYING) return;
    if (solve_from_current()) {
        memcpy(g_game.board, g_game.solution, sizeof(g_game.board));
        update_candidates();
        g_game.state = STATE_SOLVED;
    }
}

void sudoku_reset_impl(void) {
    if (g_game.state != STATE_PLAYING) return;
    // Reset to puzzle start
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (!g_game.fixed[r][c]) {
                g_game.board[r][c] = 0;
            }
        }
    }
    g_game.error_count = 0;
    update_candidates();
}

// Getters
int sudoku_get_cell_impl(int row, int col) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return -1;
    return g_game.board[row][col];
}

int sudoku_get_fixed_impl(int row, int col) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return -1;
    return g_game.fixed[row][col];
}

int sudoku_get_candidates_impl(int row, int col) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return -1;
    return g_game.candidates[row][col];
}

int sudoku_get_state_impl(void) { return g_game.state; }
int sudoku_get_selected_row_impl(void) { return g_game.selected_row; }
int sudoku_get_selected_col_impl(void) { return g_game.selected_col; }
int sudoku_get_selected_num_impl(void) { return g_game.selected_num; }
int sudoku_get_error_count_impl(void) { return g_game.error_count; }
int sudoku_get_hints_used_impl(void) { return g_game.hints_used; }
int sudoku_get_difficulty_impl(void) { return g_game.difficulty; }
int sudoku_get_timer_impl(void) { return g_game.timer_seconds; }

void sudoku_set_timer_impl(int seconds) { g_game.timer_seconds = (uint32_t)seconds; }
int sudoku_get_show_candidates_impl(void) { return g_game.show_candidates ? 1 : 0; }

bool sudoku_check_valid_impl(int row, int col, int digit) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return false;
    return is_valid_placement(row, col, digit);
}

bool sudoku_is_solved_impl(void) {
    // Check all cells filled and valid
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (g_game.board[r][c] == 0) return false;
        }
    }
    // Verify all rows, cols, boxes
    // Rows
    for (int r = 0; r < BOARD_SIZE; r++) {
        uint8_t seen = 0;
        for (int c = 0; c < BOARD_SIZE; c++) {
            uint8_t d = g_game.board[r][c];
            if (d == 0 || (seen & (1 << (d-1)))) return false;
            seen |= (1 << (d-1));
        }
    }
    // Columns
    for (int c = 0; c < BOARD_SIZE; c++) {
        uint8_t seen = 0;
        for (int r = 0; r < BOARD_SIZE; r++) {
            uint8_t d = g_game.board[r][c];
            if (d == 0 || (seen & (1 << (d-1)))) return false;
            seen |= (1 << (d-1));
        }
    }
    // Boxes
    for (int br = 0; br < BOX_SIZE; br++) {
        for (int bc = 0; bc < BOX_SIZE; bc++) {
            uint8_t seen = 0;
            for (int r = br * BOX_SIZE; r < br * BOX_SIZE + BOX_SIZE; r++) {
                for (int c = bc * BOX_SIZE; c < bc * BOX_SIZE + BOX_SIZE; c++) {
                    uint8_t d = g_game.board[r][c];
                    if (d == 0 || (seen & (1 << (d-1)))) return false;
                    seen |= (1 << (d-1));
                }
            }
        }
    }
    return true;
}
