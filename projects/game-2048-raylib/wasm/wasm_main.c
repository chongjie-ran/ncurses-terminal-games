#include "game.h"
#include <emscripten.h>

static struct Game2048 game;

// Called once on load
EMSCRIPTEN_KEEPALIVE
void wasm_init() {
    Game2048_Init(&game);
}

// Start new game
EMSCRIPTEN_KEEPALIVE
void wasm_start_game() {
    Game2048_Reset(&game);
}

// dir: 0=up, 1=down, 2=left, 3=right
// Returns 1 if moved, 0 if not
EMSCRIPTEN_KEEPALIVE
int wasm_move(int dir) {
    return Game2048_Move(&game, dir);
}

// Expose state to JS renderer
EMSCRIPTEN_KEEPALIVE
int wasm_get_state() { return Game2048_GetState(&game); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_score() { return Game2048_GetScore(&game); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_best_score() { return Game2048_GetBestScore(&game); }

EMSCRIPTEN_KEEPALIVE
void wasm_set_best_score(int s) { Game2048_SetBestScore(&game, s); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_won() { return Game2048_GetWon(&game); }

EMSCRIPTEN_KEEPALIVE
int wasm_get_cell(int row, int col) { return Game2048_GetCell(&game, row, col); }

// Canvas dimensions (exported as globals for JS to read)
EMSCRIPTEN_KEEPALIVE
int canvasWidth = 480;

EMSCRIPTEN_KEEPALIVE
int canvasHeight = 600;
