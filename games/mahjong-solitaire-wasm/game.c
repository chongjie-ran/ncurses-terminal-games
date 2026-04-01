// Mahjong Solitaire - Pure C game logic
#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Board: [layer][row][col]
static int board[MAX_LAYERS][GRID_H][GRID_W];
static int tiles_count = 0;
int selected_layer = -1, selected_row = -1, selected_col = -1;

int game_state = STATE_MENU;
int selected_tile_idx = -1;
int score = 0;
int tiles_remaining = 0;

static int total_tiles[MAX_LAYERS][GRID_H][GRID_W] = {
    // Layer 0: 12x8 = 96 tiles
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    },
    // Layer 1: 10x6 = 60 tiles (offset by 1 in each direction)
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    // Layer 2: 8x4 = 32 tiles (offset by 2)
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    }
};

// Generate a shuffled tile set (144 tiles total, 4 of each type)
static void shuffle_tiles(int* tiles, int n) {
    srand(time(NULL));
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = tiles[i];
        tiles[i] = tiles[j];
        tiles[j] = tmp;
    }
}

static int get_tile_type(int idx) {
    // 14 tile types, 4 of each = 56 tiles per "set"
    // We need 144 tiles for 3 layers
    int type = (idx % TOTAL_TILE_TYPES) + 1;
    // Assign tile types cycling through
    // For variety, we create 3 complete sets (14 types × 4 each = 56 × 3 = 168)
    // We use 144
    return ((idx % 14) + 1);
}

void init_game(void) {
    memset(board, 0, sizeof(board));
    
    // Count total tiles needed
    int total = 0;
    for (int l = 0; l < MAX_LAYERS; l++) {
        for (int r = 0; r < GRID_H; r++) {
            for (int c = 0; c < GRID_W; c++) {
                if (total_tiles[l][r][c]) total++;
            }
        }
    }
    tiles_remaining = total;
    score = 0;
    selected_tile_idx = -1;
    selected_layer = selected_row = selected_col = -1;
    
    // Generate and shuffle tile types
    int* tile_pool = (int*)malloc(total * sizeof(int));
    for (int i = 0; i < total; i++) {
        tile_pool[i] = get_tile_type(i);
    }
    shuffle_tiles(tile_pool, total);
    
    // Place tiles on board
    int idx = 0;
    for (int l = 0; l < MAX_LAYERS; l++) {
        for (int r = 0; r < GRID_H; r++) {
            for (int c = 0; c < GRID_W; c++) {
                if (total_tiles[l][r][c]) {
                    board[l][r][c] = tile_pool[idx++];
                }
            }
        }
    }
    
    free(tile_pool);
    game_state = STATE_PLAYING;
}

int get_tile(int layer, int row, int col) {
    if (layer < 0 || layer >= MAX_LAYERS) return TILE_EMPTY;
    if (row < 0 || row >= GRID_H) return TILE_EMPTY;
    if (col < 0 || col >= GRID_W) return TILE_EMPTY;
    return board[layer][row][col];
}

int is_tile_free(int layer, int row, int col) {
    if (get_tile(layer, row, col) == TILE_EMPTY) return 0;
    if (layer >= MAX_LAYERS - 1) return 1; // Top layer always free on top
    
    // Check if covered from above (any tile on higher layer overlapping this position)
    // Tiles on higher layers cover a 2x2 area of lower layer
    for (int l = layer + 1; l < MAX_LAYERS; l++) {
        int r_offset = l - layer;
        int c_offset = l - layer;
        // A tile at (l, r, c) covers positions on layer at (r-r_offset, c-c_offset) to (r, c)
        // Simplified: check if any higher layer tile overlaps
        for (int dr = 0; dr <= r_offset; dr++) {
            for (int dc = 0; dc <= c_offset; dc++) {
                int nr = row - dr;
                int nc = col - dc;
                if (nr >= 0 && nr < GRID_H && nc >= 0 && nc < GRID_W) {
                    if (board[l][nr][nc] != TILE_EMPTY) return 0;
                }
            }
        }
    }
    
    // Check left OR right side is free (not blocked by same-layer tile)
    // Left side free: col == 0 OR (col > 0 AND no tile at same layer on left)
    int left_free = (col == 0) || (board[layer][row][col-1] == TILE_EMPTY);
    int right_free = (col == GRID_W - 1) || (board[layer][row][col+1] == TILE_EMPTY);
    
    return left_free || right_free;
}

int select_tile(int layer, int row, int col) {
    if (game_state != STATE_PLAYING) return -1;
    
    int tile = get_tile(layer, row, col);
    if (tile == TILE_EMPTY) return -1;
    if (!is_tile_free(layer, row, col)) return -1;
    
    if (selected_layer < 0) {
        // First selection
        selected_layer = layer;
        selected_row = row;
        selected_col = col;
        selected_tile_idx = tile;
        return 0;
    } else {
        // Second selection
        if (layer == selected_layer && row == selected_row && col == selected_col) {
            // Deselect
            selected_layer = selected_row = selected_col = -1;
            selected_tile_idx = -1;
            return 0;
        }
        
        if (tile == selected_tile_idx) {
            // Match! Remove both tiles
            board[selected_layer][selected_row][selected_col] = TILE_EMPTY;
            board[layer][row][col] = TILE_EMPTY;
            tiles_remaining -= 2;
            score += 10;
            
            selected_layer = selected_row = selected_col = -1;
            selected_tile_idx = -1;
            
            if (check_win()) {
                game_state = STATE_WIN;
                score += tiles_remaining == 0 ? 100 : 0;
            } else if (!has_valid_moves()) {
                game_state = STATE_NO_MOVES;
            }
            return 1; // Match found
        } else {
            // No match, select new tile
            selected_layer = layer;
            selected_row = row;
            selected_col = col;
            selected_tile_idx = tile;
            return 0;
        }
    }
}

int has_valid_moves(void) {
    // Find all free tiles and check if any pair exists
    int free_tiles[MAX_LAYERS * GRID_H * GRID_W][3];
    int count = 0;
    
    for (int l = 0; l < MAX_LAYERS; l++) {
        for (int r = 0; r < GRID_H; r++) {
            for (int c = 0; c < GRID_W; c++) {
                if (board[l][r][c] != TILE_EMPTY && is_tile_free(l, r, c)) {
                    free_tiles[count][0] = l;
                    free_tiles[count][1] = r;
                    free_tiles[count][2] = c;
                    count++;
                }
            }
        }
    }
    
    // Check for any matching pairs
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (board[free_tiles[i][0]][free_tiles[i][1]][free_tiles[i][2]] ==
                board[free_tiles[j][0]][free_tiles[j][1]][free_tiles[j][2]]) {
                return 1;
            }
        }
    }
    return 0;
}

int check_win(void) {
    return tiles_remaining == 0;
}
