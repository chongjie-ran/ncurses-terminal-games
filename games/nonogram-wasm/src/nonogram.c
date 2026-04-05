#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

#define MAX_GRID 15
#define MAX_HINTS 10

// Game state
typedef struct {
    int grid[MAX_GRID][MAX_GRID];     // 0=empty, 1=filled, 2=marked(x)
    int solution[MAX_GRID][MAX_GRID];  // 0=empty, 1=filled
    int row_hints[MAX_GRID][MAX_HINTS];
    int col_hints[MAX_GRID][MAX_HINTS];
    int row_hint_counts[MAX_GRID];
    int col_hint_counts[MAX_GRID];
    int grid_rows;
    int grid_cols;
    int cell_size;
    int padding;
    int is_complete;
    int current_puzzle;
} GameState;

static GameState state;

// Calculate hints from solution
void calculate_hints(int row, int is_row) {
    int* hints = is_row ? state.row_hints[row] : state.col_hints[row];
    int* hint_count = is_row ? &state.row_hint_counts[row] : &state.col_hint_counts[row];
    int* sol = is_row ? state.solution[row] : (int*)state.solution;
    
    *hint_count = 0;
    int count = 0;
    
    for (int i = 0; i < (is_row ? state.grid_cols : state.grid_rows); i++) {
        int val = is_row ? sol[i] : ((int*)state.solution)[i * state.grid_rows + row];
        if (val == 1) {
            count++;
        } else if (count > 0) {
            hints[(*hint_count)++] = count;
            count = 0;
        }
    }
    if (count > 0) {
        hints[(*hint_count)++] = count;
    }
}

// Generate a random puzzle
void generate_puzzle(int rows, int cols, int puzzle_id) {
    state.grid_rows = rows;
    state.grid_cols = cols;
    state.current_puzzle = puzzle_id;
    state.is_complete = 0;
    
    // Cell size based on grid
    if (rows <= 10 && cols <= 10) {
        state.cell_size = 40;
    } else {
        state.cell_size = 30;
    }
    state.padding = 100;
    
    // Generate solution with some patterns
    srand(puzzle_id * 12345);
    
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            // Create interesting patterns
            int rand_val = rand() % 3;
            if (rand_val == 0 || (r == 0 && c == 0)) {
                state.solution[r][c] = 1;
            } else {
                state.solution[r][c] = 0;
            }
            state.grid[r][c] = 0; // Clear player grid
        }
    }
    
    // Add some row patterns
    for (int r = 0; r < rows; r++) {
        int start = rand() % 2;
        int len = cols / 2 + rand() % (cols / 2);
        for (int c = start; c < start + len && c < cols; c++) {
            state.solution[r][c] = 1;
        }
    }
    
    // Calculate hints
    for (int i = 0; i < rows; i++) {
        calculate_hints(i, 1);
    }
    for (int i = 0; i < cols; i++) {
        calculate_hints(i, 0);
    }
}

// Check completion
int check_complete() {
    for (int r = 0; r < state.grid_rows; r++) {
        for (int c = 0; c < state.grid_cols; c++) {
            if (state.grid[r][c] != state.solution[r][c]) {
                return 0;
            }
        }
    }
    state.is_complete = 1;
    return 1;
}

// Get pixel data for rendering
EMSCRIPTEN_KEEPALIVE
const char* get_render_data() {
    static char data[8192];
    char* p = data;
    
    // Game state
    p += sprintf(p, "ROWS:%d\n", state.grid_rows);
    p += sprintf(p, "COLS:%d\n", state.grid_cols);
    p += sprintf(p, "CELL:%d\n", state.cell_size);
    p += sprintf(p, "PAD:%d\n", state.padding);
    p += sprintf(p, "DONE:%d\n", state.is_complete);
    
    // Grid state
    for (int r = 0; r < state.grid_rows; r++) {
        for (int c = 0; c < state.grid_cols; c++) {
            p += sprintf(p, "G%d:%d,%d:%d\n", r, c, state.grid[r][c], state.solution[r][c]);
        }
    }
    
    // Row hints
    for (int r = 0; r < state.grid_rows; r++) {
        p += sprintf(p, "RH%d:", r);
        for (int h = 0; h < state.row_hint_counts[r]; h++) {
            p += sprintf(p, "%d ", state.row_hints[r][h]);
        }
        p += sprintf(p, "\n");
    }
    
    // Col hints
    for (int c = 0; c < state.grid_cols; c++) {
        p += sprintf(p, "CH%d:", c);
        for (int h = 0; h < state.col_hint_counts[c]; h++) {
            p += sprintf(p, "%d ", state.col_hints[c][h]);
        }
        p += sprintf(p, "\n");
    }
    
    return data;
}

// Toggle cell at grid position
EMSCRIPTEN_KEEPALIVE
void toggle_cell(int row, int col) {
    if (row >= 0 && row < state.grid_rows && col >= 0 && col < state.grid_cols) {
        state.grid[row][col] = (state.grid[row][col] + 1) % 3;
    }
}

// Mark cell (X)
EMSCRIPTEN_KEEPALIVE
void mark_cell(int row, int col) {
    if (row >= 0 && row < state.grid_rows && col >= 0 && col < state.grid_cols) {
        state.grid[row][col] = (state.grid[row][col] == 2) ? 0 : 2;
    }
}

// New game
EMSCRIPTEN_KEEPALIVE
void new_game(int puzzle_id) {
    int sizes[5][2] = {{5, 5}, {7, 7}, {10, 10}, {10, 15}, {15, 15}};
    int idx = puzzle_id % 5;
    generate_puzzle(sizes[idx][0], sizes[idx][1], puzzle_id);
}

// Get completion status
EMSCRIPTEN_KEEPALIVE
int is_complete() {
    if (state.is_complete) return 1;
    return check_complete();
}

int main() {
    new_game(0);
    return 0;
}
