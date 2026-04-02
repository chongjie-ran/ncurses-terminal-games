#include <emscripten.h>

// These are the exported WASM functions - they call the impl functions from game.c
EMSCRIPTEN_KEEPALIVE
void cf_start_game(void) {
    extern void cf_start_game_impl(void);
    cf_start_game_impl();
}

EMSCRIPTEN_KEEPALIVE
int cf_drop_disc(int col) {
    extern int cf_drop_disc_impl(int);
    return cf_drop_disc_impl(col);
}

EMSCRIPTEN_KEEPALIVE
int cf_get_cell(int row, int col) {
    extern int cf_get_cell_impl(int, int);
    return cf_get_cell_impl(row, col);
}

EMSCRIPTEN_KEEPALIVE
int cf_can_drop(int col) {
    extern int cf_can_drop_impl(int);
    return cf_can_drop_impl(col);
}

EMSCRIPTEN_KEEPALIVE
int cf_get_state(void) {
    extern int cf_get_state_impl(void);
    return cf_get_state_impl();
}

EMSCRIPTEN_KEEPALIVE
int cf_get_player(void) {
    extern int cf_get_player_impl(void);
    return cf_get_player_impl();
}

EMSCRIPTEN_KEEPALIVE
int cf_get_winner(void) {
    extern int cf_get_winner_impl(void);
    return cf_get_winner_impl();
}

EMSCRIPTEN_KEEPALIVE
int cf_get_move_count(void) {
    extern int cf_get_move_count_impl(void);
    return cf_get_move_count_impl();
}

EMSCRIPTEN_KEEPALIVE
int cf_get_win_x1(void) {
    extern int cf_get_win_x1_impl(void);
    return cf_get_win_x1_impl();
}

EMSCRIPTEN_KEEPALIVE
int cf_get_win_y1(void) {
    extern int cf_get_win_y1_impl(void);
    return cf_get_win_y1_impl();
}

EMSCRIPTEN_KEEPALIVE
int cf_get_win_x2(void) {
    extern int cf_get_win_x2_impl(void);
    return cf_get_win_x2_impl();
}

EMSCRIPTEN_KEEPALIVE
int cf_get_win_y2(void) {
    extern int cf_get_win_y2_impl(void);
    return cf_get_win_y2_impl();
}

EMSCRIPTEN_KEEPALIVE
int cf_get_win_type(void) {
    extern int cf_get_win_type_impl(void);
    return cf_get_win_type_impl();
}
