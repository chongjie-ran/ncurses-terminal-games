/*
 * wasm_main.c - Emscripten wrapper for Model League
 */
#include <emscripten/emscripten.h>
#include "game.h"

static char _str_buf[512];
static const char* _copy_str(const char* s) {
    int i=0;
    while(s[i] && i<511) { _str_buf[i]=s[i]; i++; }
    _str_buf[i]=0;
    return _str_buf;
}

EMSCRIPTEN_KEEPALIVE void _wasm_init(void) { wasm_init(); }
EMSCRIPTEN_KEEPALIVE void _wasm_new_game(int f) { wasm_new_game(f); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_state(void) { return wasm_get_state(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_gold(void) { return wasm_get_gold(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_exp(void) { return wasm_get_exp(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_level(void) { return wasm_get_level(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_cycle(void) { return wasm_get_cycle(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_faction(void) { return wasm_get_faction(); }
EMSCRIPTEN_KEEPALIVE int _wasm_get_card_count(void) { return wasm_get_card_count(); }
EMSCRIPTEN_KEEPALIVE int _wasm_card_owned(int id) { return wasm_card_owned(id); }
EMSCRIPTEN_KEEPALIVE int _wasm_card_atk(int id) { return wasm_card_atk(id); }
EMSCRIPTEN_KEEPALIVE int _wasm_card_def(int id) { return wasm_card_def(id); }
EMSCRIPTEN_KEEPALIVE int _wasm_card_spd(int id) { return wasm_card_spd(id); }
EMSCRIPTEN_KEEPALIVE int _wasm_card_cost(int id) { return wasm_card_cost(id); }
EMSCRIPTEN_KEEPALIVE int _wasm_card_rarity(int id) { return wasm_card_rarity(id); }
EMSCRIPTEN_KEEPALIVE int _wasm_card_skill(int id) { return wasm_card_skill(id); }
EMSCRIPTEN_KEEPALIVE int _wasm_card_lv(int id) { return wasm_card_lv(id); }
EMSCRIPTEN_KEEPALIVE const char* _wasm_card_name(int id) { return _copy_str(wasm_card_name(id)); }
EMSCRIPTEN_KEEPALIVE int _wasm_deck_size(void) { return wasm_deck_size(); }
EMSCRIPTEN_KEEPALIVE int _wasm_deck_card(int slot) { return wasm_deck_card(slot); }
EMSCRIPTEN_KEEPALIVE void _wasm_shop_buy(int id) { wasm_shop_buy(id); }
EMSCRIPTEN_KEEPALIVE void _wasm_deck_toggle(int id) { wasm_deck_toggle(id); }
EMSCRIPTEN_KEEPALIVE void _wasm_upgrade_card(int id) { wasm_upgrade_card(id); }
EMSCRIPTEN_KEEPALIVE void _wasm_set_faction(int f) { wasm_set_faction(f); }
EMSCRIPTEN_KEEPALIVE void _wasm_battle_fight(void) { wasm_battle_fight(); }
EMSCRIPTEN_KEEPALIVE void _wasm_mine_gold(void) { wasm_mine_gold(); }
EMSCRIPTEN_KEEPALIVE void _wasm_next_cycle(void) { wasm_next_cycle(); }
EMSCRIPTEN_KEEPALIVE void _wasm_navigate(int s) { wasm_navigate(s); }
EMSCRIPTEN_KEEPALIVE void _wasm_back(void) { wasm_back(); }
EMSCRIPTEN_KEEPALIVE const char* _wasm_get_msg(void) { return _copy_str(wasm_get_msg()); }
