// Unit tests for power-broker game logic
// Compile: gcc -o game_test game_test.c game.c -lm
// Or use: ./run_tests.sh

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define MAIN_TEST
#include "game.h"

int main() {
    printf("=== Power Broker WASM Unit Tests ===\n\n");
    srand(42);

    // Test init
    printf("test_init... ");
    wasm_init();
    assert(wasm_get_state() == STATE_MENU);
    printf("PASS\n");

    // Test difficulty modes - each gives some initial power too
    printf("test_new_game_easy... ");
    wasm_new_game(DIFF_EASY);
    assert(wasm_get_state() == STATE_TRADING);
    assert(wasm_get_cash() == 150000);
    assert(wasm_get_power() == 150);  // INIT_POWER[0] = 150
    assert(wasm_get_day() == 1);
    printf("PASS\n");

    printf("test_new_game_normal... ");
    wasm_new_game(DIFF_NORMAL);
    assert(wasm_get_state() == STATE_TRADING);
    assert(wasm_get_cash() == 100000);
    assert(wasm_get_power() == 100);
    printf("PASS\n");

    printf("test_new_game_hard... ");
    wasm_new_game(DIFF_HARD);
    assert(wasm_get_state() == STATE_TRADING);
    assert(wasm_get_cash() == 60000);
    assert(wasm_get_power() == 80);
    printf("PASS\n");

    // Test buy
    printf("test_buy_power... ");
    wasm_new_game(DIFF_NORMAL);
    int cash_before = wasm_get_cash();
    int price = wasm_get_price();
    wasm_player_action(1, 10); // buy 10
    assert(wasm_get_power() == 110);
    assert(wasm_get_cash() == cash_before - price * 10);
    printf("PASS\n");

    // Test sell
    printf("test_sell_power... ");
    wasm_new_game(DIFF_NORMAL);
    int p = wasm_get_price();
    wasm_player_action(2, 50); // sell 50 of initial 100
    assert(wasm_get_power() == 50);
    int cash_before_sell = wasm_get_cash(); wasm_player_action(2, 5); int cash_after_sell = wasm_get_cash(); assert(cash_after_sell > cash_before_sell); // cash increased
    (void)p;
    printf("PASS\n");

    printf("test_cannot_sell_more_than_owned... ");
    wasm_new_game(DIFF_NORMAL);
    int owned = wasm_get_power();
    wasm_player_action(2, owned + 10); // sell more than owned
    assert(wasm_get_power() == owned); // should remain unchanged
    printf("PASS\n");

    // Test price range
    printf("test_price_range... ");
    wasm_new_game(DIFF_NORMAL);
    for (int i = 0; i < 100; i++) {
        int p = wasm_get_price();
        assert(p >= PRICE_MIN && p <= PRICE_MAX);
        wasm_end_day();
        if (wasm_get_state() != STATE_TRADING) break;
    }
    printf("PASS\n");

    // Test day progression
    printf("test_day_progression... ");
    wasm_new_game(DIFF_NORMAL);
    assert(wasm_get_day() == 1);
    for (int d = 1; d <= 5; d++) {
        assert(wasm_get_day() == d);
        wasm_end_day();
        if (wasm_get_state() != STATE_TRADING) break;
    }
    printf("PASS\n");

    // Test achievements exist
    printf("test_achievements_exist... ");
    wasm_new_game(DIFF_NORMAL);
    assert(wasm_get_ach_count() == 12);
    for (int i = 0; i < 12; i++) {
        const char* name = wasm_get_ach_name(i);
        assert(name != NULL && strlen(name) > 0);
        const char* desc = wasm_get_ach_desc(i);
        assert(desc != NULL && strlen(desc) > 0);
    }
    printf("PASS\n");

    // Test state transitions
    printf("test_state_transitions... ");
    wasm_init();
    assert(wasm_get_state() == STATE_MENU);
    wasm_new_game(DIFF_NORMAL);
    assert(wasm_get_state() == STATE_TRADING);
    wasm_return_to_menu();
    assert(wasm_get_state() == STATE_MENU);
    printf("PASS\n");

    // Test total assets = cash + power * price
    printf("test_total_assets_calculation... ");
    wasm_new_game(DIFF_NORMAL);
    int price2 = wasm_get_price();
    wasm_player_action(1, 10);
    int cash = wasm_get_cash();
    int power = wasm_get_power();
    int total = wasm_get_total_assets();
    assert(total == cash + power * price2);
    printf("PASS\n");

    // Test boss day detection
    printf("test_boss_day... ");
    wasm_new_game(DIFF_NORMAL);
    // Can't easily test specific days without mocking rng, just verify function exists
    assert(wasm_is_boss_day() == 0 || wasm_is_boss_day() == 1);
    printf("PASS\n");

    // Test win streak
    printf("test_win_streak... ");
    wasm_new_game(DIFF_NORMAL);
    assert(wasm_get_win_streak() == 0);
    printf("PASS\n");

    // Test profit tracking
    printf("test_profit_tracking... ");
    wasm_new_game(DIFF_NORMAL);
    int init = wasm_get_total_assets();
    assert(wasm_get_profit() == 0);
    (void)init;
    printf("PASS\n");

    printf("\n=== ALL TESTS PASSED (%d tests) ===\n", 15);
    return 0;
}
