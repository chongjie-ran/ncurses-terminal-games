#ifndef MODEL_LEAGUE_GAME_H
#define MODEL_LEAGUE_GAME_H

#include <stdbool.h>

// === Game States ===
#define STATE_MENU       0
#define STATE_SHOP       1
#define STATE_COLLECTION 2
#define STATE_DECKBUILD  3
#define STATE_UPGRADE     4
#define STATE_BATTLE      5
#define STATE_FACTION     6
#define STATE_MINE        7
#define STATE_GAMEOVER    8

// === Card Types ===
#define TYPE_THINK     0
#define TYPE_EFF       1
#define TYPE_BALANCE   2

// === Factions ===
#define FAC_SHANXIAN  0
#define FAC_JUDNAO    1
#define FAC_HUNDUN    2

// === Skills ===
#define SKILL_NONE         0
#define SKILL_POWER_STRIKE 1
#define SKILL_SHIELD        2
#define SKILL_SPEED_BOOST   3
#define SKILL_HEAL          4
#define SKILL_DEBUFF        5

// === WASM Exports ===
void wasm_init(void);
void wasm_new_game(int faction);
int wasm_get_state(void);
int wasm_get_gold(void);
int wasm_get_exp(void);
int wasm_get_level(void);
int wasm_get_cycle(void);
int wasm_get_faction(void);
int wasm_get_card_count(void);
int wasm_card_owned(int card_id);
int wasm_card_atk(int card_id);
int wasm_card_def(int card_id);
int wasm_card_spd(int card_id);
int wasm_card_cost(int card_id);
int wasm_card_rarity(int card_id);
int wasm_card_skill(int card_id);
int wasm_card_lv(int card_id);
const char* wasm_card_name(int card_id);
int wasm_deck_size(void);
int wasm_deck_card(int slot);
void wasm_shop_buy(int card_id);
void wasm_deck_toggle(int card_id);
void wasm_upgrade_card(int card_id);
void wasm_set_faction(int faction);
void wasm_battle_fight(void);
void wasm_mine_gold(void);
void wasm_next_cycle(void);
void wasm_navigate(int state);
void wasm_back(void);
const char* wasm_get_msg(void);

#endif
