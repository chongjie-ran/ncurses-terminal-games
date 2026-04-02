#include "game.h"
#include <stdlib.h>
#include <string.h>

static Player player;
static Ship ships[NUM_SHIPS] = {
    {SHIP_CARRIER,    -1, -1, 0, 0, 0},
    {SHIP_BATTLESHIP, -1, -1, 0, 0, 0},
    {SHIP_CRUISER,    -1, -1, 0, 0, 0},
    {SHIP_SUBMARINE,  -1, -1, 0, 0, 0},
    {SHIP_DESTROYER,  -1, -1, 0, 0, 0},
};

// Simple deterministic RNG (LCG)
static uint32_t rng_state = 12345;
static uint32_t rng_next(void) {
    rng_state = rng_state * 1103515245 + 12345;
    return (rng_state >> 16) & 0x7FFF;
}

int battleship_check_win_impl(void) {
    int r, c;
    for (r = 0; r < BOARD_SIZE; r++) {
        for (c = 0; c < BOARD_SIZE; c++) {
            if (player.real_board[r][c] == CELL_SHIP) {
                if (player.board[r][c] != CELL_HIT) return 0;
            }
        }
    }
    return 1;
}

void battleship_init_impl(void) {
    battleship_reset_impl();
}

void battleship_reset_impl(void) {
    int i, r, c;
    for (r = 0; r < BOARD_SIZE; r++) {
        for (c = 0; c < BOARD_SIZE; c++) {
            player.board[r][c] = CELL_EMPTY;
            player.real_board[r][c] = CELL_EMPTY;
        }
    }
    player.hit_count = 0;
    player.miss_count = 0;
    player.ships_sunk = 0;
    player.state = STATE_PLACING;
    player.ship_count = 0;
    rng_state = 12345;

    for (i = 0; i < NUM_SHIPS; i++) {
        ships[i].hits = 0;
        ships[i].sunk = 0;
        ships[i].row = -1;
        ships[i].col = -1;
    }
}

int battleship_get_cell_impl(int board_type, int row, int col) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return -1;
    if (board_type == 0) {
        return player.board[row][col];
    } else {
        return player.real_board[row][col];
    }
}

int battleship_get_game_state_impl(void) {
    return player.state;
}

int battleship_get_ships_sunk_impl(void) {
    return player.ships_sunk;
}

int battleship_get_hit_count_impl(void) {
    return player.hit_count;
}

int battleship_get_miss_count_impl(void) {
    return player.miss_count;
}

int battleship_get_ship_count_impl(void) {
    return player.ship_count;
}

int battleship_place_ship_impl(int ship_idx, int row, int col, int horizontal) {
    if (ship_idx < 0 || ship_idx >= NUM_SHIPS) return 0;
    if (player.ship_count >= NUM_SHIPS) return 0;

    int size = ships[ship_idx].size;
    int r, c, ok = 1;

    // Check bounds
    if (horizontal) {
        if (col + size > BOARD_SIZE) return 0;
        for (c = col; c < col + size; c++) {
            if (player.real_board[row][c] == CELL_SHIP) { ok = 0; break; }
        }
    } else {
        if (row + size > BOARD_SIZE) return 0;
        for (r = row; r < row + size; r++) {
            if (player.real_board[r][col] == CELL_SHIP) { ok = 0; break; }
        }
    }
    if (!ok) return 0;

    // Place ship
    if (horizontal) {
        for (c = col; c < col + size; c++) {
            player.real_board[row][c] = CELL_SHIP;
        }
    } else {
        for (r = row; r < row + size; r++) {
            player.real_board[r][col] = CELL_SHIP;
        }
    }

    ships[ship_idx].row = row;
    ships[ship_idx].col = col;
    ships[ship_idx].horizontal = horizontal;
    ships[ship_idx].hits = 0;
    ships[ship_idx].sunk = 0;
    player.ship_count++;

    if (player.ship_count >= NUM_SHIPS) {
        player.state = STATE_PLAYING;
    }
    return 1;
}

int battleship_shoot_impl(int row, int col) {
    if (player.state != STATE_PLAYING) return -1;
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return -1;
    if (player.board[row][col] == CELL_HIT || player.board[row][col] == CELL_MISS) return -1;

    int cell = player.real_board[row][col];

    if (cell == CELL_SHIP) {
        player.board[row][col] = CELL_HIT;
        player.hit_count++;

        // Find which ship was hit
        int i;
        for (i = 0; i < NUM_SHIPS; i++) {
            int r, c, hit_count = 0;
            if (ships[i].row < 0) continue;
            if (ships[i].horizontal) {
                for (c = ships[i].col; c < ships[i].col + ships[i].size; c++) {
                    if (player.board[ships[i].row][c] == CELL_HIT) hit_count++;
                }
            } else {
                for (r = ships[i].row; r < ships[i].row + ships[i].size; r++) {
                    if (player.board[r][ships[i].col] == CELL_HIT) hit_count++;
                }
            }
            ships[i].hits = hit_count;
            if (hit_count >= ships[i].size) {
                ships[i].sunk = 1;
                player.ships_sunk++;
            }
        }

        if (battleship_check_win_impl()) {
            player.state = STATE_WIN;
        }
        return 1; // HIT
    } else {
        player.board[row][col] = CELL_MISS;
        player.miss_count++;
        return 0; // MISS
    }
}
