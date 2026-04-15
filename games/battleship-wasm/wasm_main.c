#include <emscripten.h>
#include "game.h"

EMSCRIPTEN_KEEPALIVE
void init_game(Game* g) {
    initGame(g);
}

EMSCRIPTEN_KEEPALIVE
int place_ship(Game* g, int shipIndex, int row, int col, int horizontal) {
    return placeShip(g, shipIndex, row, col, horizontal);
}

EMSCRIPTEN_KEEPALIVE
int fire_at(Game* g, int row, int col) {
    return fireAt(g, row, col);
}

EMSCRIPTEN_KEEPALIVE
int check_win(Game* g) {
    return checkWin(g);
}

EMSCRIPTEN_KEEPALIVE
int get_grid(Game* g, int row, int col) {
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) return -1;
    return g->grid[row][col];
}

EMSCRIPTEN_KEEPALIVE
int get_enemy_grid(Game* g, int row, int col) {
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) return -1;
    return g->enemyGrid[row][col];
}

EMSCRIPTEN_KEEPALIVE
int get_game_state(Game* g) {
    return g->gameState;
}

EMSCRIPTEN_KEEPALIVE
int get_current_ship(Game* g) {
    return g->currentShip;
}

EMSCRIPTEN_KEEPALIVE
int get_ship_length(int index) {
    if (index < 0 || index >= NUM_SHIPS) return -1;
    return SHIP_LENGTHS[index];
}

EMSCRIPTEN_KEEPALIVE
int get_player_score(Game* g) {
    return g->playerScore;
}

EMSCRIPTEN_KEEPALIVE
int get_enemy_score(Game* g) {
    return g->enemyScore;
}

EMSCRIPTEN_KEEPALIVE
int get_ships_sunk(Game* g) {
    return g->shipsSunk;
}

EMSCRIPTEN_KEEPALIVE
int get_enemy_ships_sunk(Game* g) {
    return g->enemyShipsSunk;
}

EMSCRIPTEN_KEEPALIVE
void reset_game(Game* g) {
    initGame(g);
}
