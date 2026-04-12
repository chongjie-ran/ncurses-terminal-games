/*
 * wasm_main.c - Emscripten wrapper for Power Broker
 */
#include <emscripten/emscripten.h>
#include "game.h"

EMSCRIPTEN_KEEPALIVE
void init(void) { wasm_init(); }

EMSCRIPTEN_KEEPALIVE
void new_game(int diff) { wasm_new_game(diff); }

EMSCRIPTEN_KEEPALIVE
int get_state(void) { return wasm_get_state(); }
EMSCRIPTEN_KEEPALIVE
int get_day(void) { return wasm_get_day(); }
EMSCRIPTEN_KEEPALIVE
int get_cash(void) { return wasm_get_cash(); }
EMSCRIPTEN_KEEPALIVE
int get_power(void) { return wasm_get_power(); }
EMSCRIPTEN_KEEPALIVE
int get_price(void) { return wasm_get_price(); }
EMSCRIPTEN_KEEPALIVE
int get_profit(void) { return wasm_get_profit(); }
EMSCRIPTEN_KEEPALIVE
int get_total_assets(void) { return wasm_get_total_assets(); }
EMSCRIPTEN_KEEPALIVE
int get_win_streak(void) { return wasm_get_win_streak(); }
EMSCRIPTEN_KEEPALIVE
int get_boss(void) { return wasm_is_boss_day(); }
EMSCRIPTEN_KEEPALIVE
int get_ach_count(void) { return wasm_get_ach_count(); }
EMSCRIPTEN_KEEPALIVE
int ach_unlocked(int idx) { return wasm_get_ach_unlocked(idx); }

EMSCRIPTEN_KEEPALIVE
void player_action(int type, int amt) { wasm_player_action(type, amt); }
EMSCRIPTEN_KEEPALIVE
void end_day(void) { wasm_end_day(); }
EMSCRIPTEN_KEEPALIVE
void back_to_menu(void) { wasm_return_to_menu(); }
