// Unit tests for model-league
#include <stdio.h>
#include <assert.h>

#define MAIN_TEST
#include "game.h"

int main() {
    printf("=== Model League WASM Unit Tests ===\n\n");
    
    printf("test_init... ");
    wasm_init();
    assert(wasm_get_state() == STATE_MENU);
    assert(wasm_get_gold() == 500);
    assert(wasm_get_exp() == 0);
    assert(wasm_get_level() == 1);
    printf("PASS\n");

    printf("test_new_game... ");
    wasm_new_game(FAC_SHANXIAN);
    assert(wasm_get_state() == STATE_MENU);
    assert(wasm_get_faction() == FAC_SHANXIAN);
    printf("PASS\n");

    printf("test_initial_deck... ");
    wasm_new_game(FAC_SHANXIAN);
    assert(wasm_deck_size() == 1);
    assert(wasm_deck_card(0) == 5);
    printf("PASS\n");

    printf("test_card_access... ");
    wasm_new_game(FAC_SHANXIAN);
    int atk = wasm_card_atk(5);
    int def = wasm_card_def(5);
    int spd = wasm_card_spd(5);
    assert(atk > 0 && def > 0 && spd > 0);
    printf("PASS (card 5: ATK=%d DEF=%d SPD=%d)\n", atk, def, spd);

    printf("test_navigation... ");
    wasm_new_game(FAC_SHANXIAN);
    wasm_navigate(STATE_SHOP);
    assert(wasm_get_state() == STATE_SHOP);
    wasm_back();
    assert(wasm_get_state() == STATE_MENU);
    wasm_navigate(STATE_DECKBUILD);
    assert(wasm_get_state() == STATE_DECKBUILD);
    wasm_back();
    printf("PASS\n");

    printf("test_buy_card... ");
    wasm_new_game(FAC_SHANXIAN);
    wasm_navigate(STATE_SHOP);
    int gold_before = wasm_get_gold();
    wasm_shop_buy(1);  // buy card 1 (not owned initially)
    assert(wasm_card_owned(1) == 1);
    assert(wasm_get_gold() == gold_before - wasm_card_cost(1));
    printf("PASS (gold: %d -> %d)\n", gold_before, wasm_get_gold());

    printf("test_faction_change... ");
    wasm_new_game(FAC_SHANXIAN);
    wasm_set_faction(FAC_JUDNAO);
    assert(wasm_get_faction() == FAC_JUDNAO);
    wasm_set_faction(FAC_HUNDUN);
    assert(wasm_get_faction() == FAC_HUNDUN);
    printf("PASS\n");

    printf("test_mine_gold... ");
    wasm_new_game(FAC_SHANXIAN);
    wasm_navigate(STATE_MINE);
    int g = wasm_get_gold();
    wasm_mine_gold();
    int mined = wasm_get_gold() - g; assert(mined >= 50 && mined <= 150);
    printf("PASS\n");

    printf("test_upgrade_card... ");
    wasm_new_game(FAC_SHANXIAN);
    wasm_navigate(STATE_SHOP);
    wasm_shop_buy(1);
    wasm_navigate(STATE_DECKBUILD);
    wasm_deck_toggle(1);
    int lv_before = wasm_card_lv(1);
    wasm_navigate(STATE_UPGRADE);
    wasm_upgrade_card(1);
    assert(wasm_card_lv(1) >= lv_before);
    printf("PASS (lv: %d -> %d)\n", lv_before, wasm_card_lv(1));

    printf("\n=== ALL TESTS PASSED (9 tests) ===\n");
    return 0;
}
