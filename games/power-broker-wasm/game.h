#ifndef POWER_BROKER_GAME_H
#define POWER_BROKER_GAME_H

#include <stdbool.h>

// === State ===
#define STATE_MENU 0
#define STATE_DIFFSEL 1
#define STATE_TRADING 2
#define STATE_ACHIEVEMENTS 3
#define STATE_GAMEOVER 4

#define DAY_LIMIT 30
#define PRICE_MIN 20
#define PRICE_MAX 500

// === Difficulty ===
#define DIFF_EASY 0
#define DIFF_NORMAL 1
#define DIFF_HARD 2

// === WASM Exports ===
void wasm_init(void);
void wasm_new_game(int difficulty);
void wasm_load_save(void);
void wasm_save_game(void);
int wasm_get_state(void);
int wasm_get_day(void);
int wasm_get_cash(void);
int wasm_get_power(void);
int wasm_get_price(void);
int wasm_get_profit(void);
int wasm_get_init_assets(void);
int wasm_get_total_assets(void);
int wasm_get_win_streak(void);
const char* wasm_get_event_name(void);
const char* wasm_get_event_desc(void);
int wasm_get_event_impact(void);
int wasm_is_boss_day(void);
int wasm_get_ach_count(void);
int wasm_get_ach_unlocked(int idx);
const char* wasm_get_ach_name(int idx);
const char* wasm_get_ach_desc(int idx);
void wasm_player_action(int action_type, int amount);
void wasm_end_day(void);
void wasm_return_to_menu(void);

#endif
