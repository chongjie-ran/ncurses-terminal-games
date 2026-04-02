#ifndef SUDOKU_GAME_H
#define SUDOKU_GAME_H

#include <stdbool.h>
#include <stdint.h>

#define BOARD_SIZE 9
#define BOX_SIZE 3
#define NUM_CELLS 81
#define NUM_DIGITS 9

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_SOLVING,
    STATE_SOLVED,
    STATE_EDIT
} GameState;

typedef enum {
    DIFF_EASY,
    DIFF_MEDIUM,
    DIFF_HARD,
    DIFF_EXPERT
} Difficulty;

typedef struct {
    uint8_t board[BOARD_SIZE][BOARD_SIZE];       // 0 = empty, 1-9 = digit
    uint8_t solution[BOARD_SIZE][BOARD_SIZE];     // solved board
    uint8_t fixed[BOARD_SIZE][BOARD_SIZE];       // 1 = pre-filled (can't edit)
    uint8_t candidates[BOARD_SIZE][BOARD_SIZE];  // bitmask of possible digits (bit 0 = digit 1)
    uint8_t selected_row;
    uint8_t selected_col;
    uint8_t selected_num;                        // 0 = none selected, 1-9
    uint8_t error_count;
    uint8_t hints_used;
    Difficulty difficulty;
    GameState state;
    uint32_t timer_seconds;
    uint8_t solving_step;                       // for animated solving
    bool show_candidates;
} SudokuGame;

extern SudokuGame g_game;

// Core API
void sudoku_start_game_impl(Difficulty diff);
void sudoku_select_cell_impl(int row, int col);
void sudoku_input_digit_impl(int digit);
void sudoku_erase_impl(void);
void sudoku_toggle_candidates_impl(void);
void sudoku_hint_impl(void);
void sudoku_solve_impl(void);
void sudoku_reset_impl(void);

// Getters
int sudoku_get_cell_impl(int row, int col);
int sudoku_get_fixed_impl(int row, int col);
int sudoku_get_candidates_impl(int row, int col);
int sudoku_get_state_impl(void);
int sudoku_get_selected_row_impl(void);
int sudoku_get_selected_col_impl(void);
int sudoku_get_selected_num_impl(void);
int sudoku_get_error_count_impl(void);
int sudoku_get_hints_used_impl(void);
int sudoku_get_difficulty_impl(void);
int sudoku_get_timer_impl(void);
void sudoku_set_timer_impl(int seconds);
int sudoku_get_show_candidates_impl(void);

// Board helpers
bool sudoku_check_valid_impl(int row, int col, int digit);
bool sudoku_is_solved_impl(void);

#endif
