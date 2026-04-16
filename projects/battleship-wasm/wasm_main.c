#include <emscripten.h>
#include "game.h"

EMSCRIPTEN_KEEPALIVE
void battleship_init(void) {
    battleship_init_impl();
}

EMSCRIPTEN_KEEPALIVE
void battleship_reset(void) {
    battleship_reset_impl();
}

EMSCRIPTEN_KEEPALIVE
int battleship_get_cell(int board_type, int row, int col) {
    return battleship_get_cell_impl(board_type, row, col);
}

EMSCRIPTEN_KEEPALIVE
int battleship_get_game_state(void) {
    return battleship_get_game_state_impl();
}

EMSCRIPTEN_KEEPALIVE
int battleship_get_ships_sunk(void) {
    return battleship_get_ships_sunk_impl();
}

EMSCRIPTEN_KEEPALIVE
int battleship_get_computer_ships_sunk(void) {
    return battleship_get_computer_ships_sunk_impl();
}

EMSCRIPTEN_KEEPALIVE
int battleship_get_hit_count(void) {
    return battleship_get_hit_count_impl();
}

EMSCRIPTEN_KEEPALIVE
int battleship_get_miss_count(void) {
    return battleship_get_miss_count_impl();
}

EMSCRIPTEN_KEEPALIVE
int battleship_get_ship_count(void) {
    return battleship_get_ship_count_impl();
}

EMSCRIPTEN_KEEPALIVE
int battleship_place_ship(int ship_idx, int row, int col, int horizontal) {
    return battleship_place_ship_impl(ship_idx, row, col, horizontal);
}

EMSCRIPTEN_KEEPALIVE
int battleship_shoot(int row, int col) {
    return battleship_shoot_impl(row, col);
}

EMSCRIPTEN_KEEPALIVE
int battleship_computer_turn(void) {
    return battleship_computer_turn_impl();
}
