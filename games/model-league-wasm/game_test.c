// Minimal unit tests for model-league
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define MAIN_TEST
#include "game.h"

int main() {
    printf("=== Model League WASM Unit Tests ===\n\n");
    
    printf("test_init... ");
    wasm_init();
    assert(wasm_get_state() == STATE_MENU);
    printf("PASS\n");

    printf("test_start_game... ");
    wasm_new_game();
    assert(wasm_get_state() == STATE_SHOP);
    assert(wasm_get_gold() == 500);
    assert(wasm_get_exp() == 0);
    assert(wasm_get_level() == 1);
    printf("PASS\n");

    printf("test_card_access... ");
    wasm_new_game();
    int count = wasm_get_card_count();
    assert(count == 18);
    int atk = wasm_card_atk(5);
    int def = wasm_card_def(5);
    int spd = wasm_card_spd(5);
    assert(atk > 0 && def > 0 && spd > 0);
    printf("PASS (card 5: ATK=%d DEF=%d SPD=%d)\n", atk, def, spd);

    printf("test_buy_card... ");
    wasm_new_game();
    int gold_before = wasm_get_gold();
    int owned_before = wasm_card_owned(5);
    wasm_shop_buy(5);
    assert(wasm_card_owned(5) == owned_before + 1);
    printf("PASS (gold: %d -> %d)\n", gold_before, wasm_get_gold());

    printf("test_deck_add... ");
    wasm_new_game();
    wasm_shop_buy(5);
    wasm_deck_toggle(5);
    assert(wasm_deck_size() == 1);
    assert(wasm_deck_card(0) == 5);
    printf("PASS\n");

    printf("test_faction_change... ");
    wasm_new_game();
    wasm_set_faction(1);
    assert(wasm_get_faction() == 1);
    wasm_set_faction(2);
    assert(wasm_get_faction() == 2);
    printf("PASS\n");

    printf("test_mine_gold... ");
    wasm_new_game();
    int g = wasm_get_gold();
    wasm_mine_gold();
    assert(wasm_get_gold() == g + 20);
    printf("PASS\n");

    printf("test_navigation... ");
    wasm_new_game();
    wasm_navigate(2); // deck
    assert(wasm_get_state() == STATE_DECK);
    wasm_back();
    assert(wasm_get_state() == STATE_SHOP);
    wasm_navigate(3); // battle
    assert(wasm_get_state() == STATE_BATTLE);
    wasm_back();
    printf("PASS\n");

    printf("test_upgrade_card... ");
    wasm_new_game();
    wasm_shop_buy(5);
    wasm_deck_toggle(5);
    int lv_before = wasm_card_lv(5);
    wasm_upgrade_card(5);
    assert(wasm_card_lv(5) == lv_before + 1);
    printf("PASS (lv: %d -> %d)\n", lv_before, wasm_card_lv(5));

    printf("\n=== ALL TESTS PASSED (9 tests) ===\n");
    return 0;
}
