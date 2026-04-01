// Emscripten wrapper for Mahjong Solitaire
#include "game.h"
#include <emscripten/emscripten.h>

EMSCRIPTEN_KEEPALIVE
void start_game(void) {
    init_game();
}

EMSCRIPTEN_KEEPALIVE
int get_game_state(void) {
    return game_state;
}

EMSCRIPTEN_KEEPALIVE
int get_score(void) {
    return score;
}

EMSCRIPTEN_KEEPALIVE
int get_tiles_remaining(void) {
    return tiles_remaining;
}

EMSCRIPTEN_KEEPALIVE
int get_selected_tile_idx(void) {
    return selected_tile_idx;
}

EMSCRIPTEN_KEEPALIVE
int get_selected_layer(void) {
    return selected_layer;
}

EMSCRIPTEN_KEEPALIVE
int get_selected_row(void) {
    return selected_row;
}

EMSCRIPTEN_KEEPALIVE
int get_selected_col(void) {
    return selected_col;
}

EMSCRIPTEN_KEEPALIVE
int get_tile_at(int layer, int row, int col) {
    return get_tile(layer, row, col);
}

EMSCRIPTEN_KEEPALIVE
int tile_is_free(int layer, int row, int col) {
    return is_tile_free(layer, row, col);
}

EMSCRIPTEN_KEEPALIVE
int click_tile(int layer, int row, int col) {
    return select_tile(layer, row, col);
}

EMSCRIPTEN_KEEPALIVE
int check_has_valid_moves(void) {
    return has_valid_moves();
}

EMSCRIPTEN_KEEPALIVE
int get_grid_w(void) {
    return GRID_W;
}

EMSCRIPTEN_KEEPALIVE
int get_grid_h(void) {
    return GRID_H;
}

EMSCRIPTEN_KEEPALIVE
int get_max_layers(void) {
    return MAX_LAYERS;
}
