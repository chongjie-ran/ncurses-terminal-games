// Dots and Boxes - Emscripten WASM exports
#include <emscripten.h>
#include "game.h"

EMSCRIPTEN_KEEPALIVE
void db_reset(void) {
    db_init();
}

EMSCRIPTEN_KEEPALIVE
int db_click_h(int r, int c) {
    return db_draw_h_edge(r, c);
}

EMSCRIPTEN_KEEPALIVE
int db_click_v(int r, int c) {
    return db_draw_v_edge(r, c);
}

EMSCRIPTEN_KEEPALIVE
int db_cell(int r, int c) {
    return db_get_cell(r, c);
}

EMSCRIPTEN_KEEPALIVE
int db_score(int player) {
    return db_get_score(player);
}

EMSCRIPTEN_KEEPALIVE
int db_player(void) {
    return db_get_current_player();
}

EMSCRIPTEN_KEEPALIVE
int db_phase(void) {
    return db_get_phase();
}

EMSCRIPTEN_KEEPALIVE
int db_last_edge(int *r, int *c, int *type) {
    return db_get_last_edge(r, c, type);
}

EMSCRIPTEN_KEEPALIVE
int db_last_box(int *r, int *c) {
    return db_get_last_box(r, c);
}

EMSCRIPTEN_KEEPALIVE
int db_boxes_done(void) {
    int total = 0;
    for (int i = 0; i < NUM_PLAYERS; i++) total += db_get_score(i);
    return total;
}
