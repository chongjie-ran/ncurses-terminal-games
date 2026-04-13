/*
 * wasm_main.c - Emscripten wrapper for Power Broker
 */
#include <emscripten/emscripten.h>
#include "game.h"

static char _str_buf[256];
static const char* _copy_str(const char* s) {
    int i=0;
    while(s[i] && i<255) { _str_buf[i]=s[i]; i++; }
    _str_buf[i]=0;
    return _str_buf;
}

EMSCRIPTEN_KEEPALIVE void _wasm_init(void) { wasm_init(); }
EMSCRIPTEN_KEEPALIVE void _wasm_new_game(int d) { wasm_new_game(d); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_state(void) { return wasm_get_state(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_day(void) { return wasm_get_day(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_cash(void) { return wasm_get_cash(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_power(void) { return wasm_get_power(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_price(void) { return wasm_get_price(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_profit(void) { return wasm_get_profit(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_total_assets(void) { return wasm_get_total_assets(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_win_streak(void) { return wasm_get_win_streak(); }
EMSCRIPTEN_KEEPALIVE int _wasm_is_boss_day(void) { return wasm_is_boss_day(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_ach_count(void) { return wasm_get_ach_count(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_ach_unlocked(int i) { return wasm_get_ach_unlocked(i); }
EMSCRIPTEN_KEEPALIVE const char* _wasm_get_event_name(void) { return _copy_str(wasm_get_event_name()); }
EMSCRIPTEN_KEEPALIVE const char* _wasm_get_event_desc(void) { return _copy_str(wasm_get_event_desc()); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_event_impact(void) { return wasm_get_event_impact(); }
EMSCRIPTEN_KEEPALIVE void _wasm_player_action(int t, int a) { wasm_player_action(t, a); }
EMSCRIPTEN_KEEPALIVE void _wasm_end_day(void) { wasm_end_day(); }
EMSCRIPTEN_KEEPALIVE void _wasm_return_to_menu(void) { wasm_return_to_menu(); }
