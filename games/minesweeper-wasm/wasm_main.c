#include <emscripten.h>
#include "game.h"

static GameState gs;

// JS calls this to init a new game
EMSCRIPTEN_KEEPALIVE
void ms_init() {
    game_init(&gs);
}

// Returns cell value at (row, col)
EMSCRIPTEN_KEEPALIVE
int ms_get_cell(int row, int col) {
    return game_get_cell(&gs, row, col);
}

// Reveal cell: returns game state after
EMSCRIPTEN_KEEPALIVE
int ms_reveal(int row, int col) {
    return game_reveal(&gs, row, col);
}

// Toggle flag: returns 0
EMSCRIPTEN_KEEPALIVE
int ms_toggle_flag(int row, int col) {
    return game_toggle_flag(&gs, row, col);
}

// Get game state: 0=playing, 1=won, 2=lost
EMSCRIPTEN_KEEPALIVE
int ms_get_state() {
    return gs.game_state;
}

// Get cells left to reveal (for progress)
EMSCRIPTEN_KEEPALIVE
int ms_get_cells_left() {
    return gs.cells_left;
}

EMSCRIPTEN_KEEPALIVE
int ms_is_revealed(int row, int col) {
    return game_is_revealed(&gs, row, col);
}
