/*
 * Model League WASM - AI Card Battle Game
 * Pure C + Emscripten
 */

#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// === Constants ===
#define MAX_CARDS 18
#define MAX_OWNED 18
#define MAX_DECK 5
#define MAX_MSG 256

// === Game State ===
static struct {
    int state;
    int gold;
    int exp;
    int level;
    int cycle;
    int faction;
    int boss_defeated;
} g;

// === Message Buffer ===
static char g_msg[MAX_MSG];

// === Cards ===
typedef struct {
    int id;
    const char* name;
    int type;
    int faction;
    int atk;
    int def;
    int spd;
    int cost;
    int rarity;
    int owned;
    int level;
} Card;

static Card ALL_CARDS[MAX_CARDS] = {
    {1,"Claude",0,1,85,70,60,3,3,0,0},
    {2,"GPT-5",0,1,90,65,55,3,3,0,0},
    {3,"Gemini",2,0,75,75,80,3,2,0,0},
    {4,"DeepSeek",1,0,70,60,95,2,2,0,0},
    {5,"Llama",2,2,72,72,72,2,1,0,0},
    {6,"Mistral",1,0,68,58,90,2,2,0,0},
    {7,"Qwen",0,1,88,68,58,3,3,0,0},
    {8,"Grok-2",1,2,80,50,88,3,3,0,0},
    {9,"Grok-3",0,2,95,75,70,4,4,0,0},
    {10,"Perplexity",1,0,78,55,99,3,3,0,0},
    {11,"Copilot",2,1,88,88,70,4,4,0,0},
    {12,"StableLM",2,0,65,65,65,2,1,0,0},
    {13,"CharacterAI",0,2,92,60,75,3,3,0,0},
    {14,"Inflection",0,1,82,72,62,3,2,0,0},
    {15,"MetaAI",2,0,76,76,76,3,2,0,0},
    {16,"AmazonQ",1,1,74,84,74,3,3,0,0},
    {17,"Groq",1,0,88,48,100,4,4,0,0},
    {18,"xAI-Omega",0,2,100,60,80,5,5,0,0},
};

// === Deck ===
static int DECK[MAX_DECK];
static int DECK_SIZE = 0;

// === RNG ===
static unsigned int rng_seed;
static int rng_next(void) {
    rng_seed = rng_seed * 1103515245 + 12345;
    return (int)((rng_seed >> 16) & 0x7FFF);
}

// === Faction Bonus ===
static int faction_bonus(void) {
    if (g.faction == FAC_SHANXIAN) return 10; // ATK+10%
    if (g.faction == FAC_JUDNAO) return 10;   // DEF+10%
    if (g.faction == FAC_HUNDUN) return 15;   // SPD+15%
    return 0;
}

// === Card Power ===
static int card_power(const Card* c) {
    int p = c->atk + c->def + c->spd;
    int up = c->level * 5;
    p += up;
    int bonus = faction_bonus();
    if (g.faction == FAC_SHANXIAN) p += (p * 10) / 100;
    if (g.faction == FAC_JUDNAO) p += (p * 10) / 100;
    if (g.faction == FAC_HUNDUN) p += (p * 15) / 100;
    return p;
}

// === String helpers ===
static const char* fac_names[] = {"SHANXIAN", "JUDNAO", "HUNDUN"};
static const char* skill_names[] = {"无","全力一击","护盾","加速","治疗","削弱"};

static void msg_set(const char* s) {
    int i = 0;
    while (s[i] && i < MAX_MSG - 1) { g_msg[i] = s[i]; i++; }
    g_msg[i] = 0;
}

// === WASM Exports ===
void wasm_init(void) {
    g.state = STATE_MENU;
    g.gold = 500;
    g.exp = 0;
    g.level = 1;
    g.cycle = 0;
    g.faction = FAC_SHANXIAN;
    g.boss_defeated = 0;
    DECK_SIZE = 0;
    for (int i = 0; i < MAX_CARDS; i++) {
        ALL_CARDS[i].owned = 0;
        ALL_CARDS[i].level = 0;
    }
    // Start with Llama (id=5, index=4)
    ALL_CARDS[4].owned = 1;
    DECK[0] = 5;
    DECK_SIZE = 1;
    rng_seed = (unsigned int)time(NULL);
    msg_set("欢迎来到模型联盟大作战！获得初始卡牌 [Llama]");
}

void wasm_new_game(int faction) {
    if (faction < 0 || faction > 2) faction = FAC_SHANXIAN;
    wasm_init();
    g.faction = faction;
    g.state = STATE_MENU;
}

int wasm_get_state(void) { return g.state; }
int wasm_get_gold(void) { return g.gold; }
int wasm_get_exp(void) { return g.exp; }
int wasm_get_level(void) { return g.level; }
int wasm_get_cycle(void) { return g.cycle; }
int wasm_get_faction(void) { return g.faction; }
int wasm_get_card_count(void) {
    int n = 0;
    for (int i = 0; i < MAX_CARDS; i++) if (ALL_CARDS[i].owned) n++;
    return n;
}

int wasm_card_owned(int card_id) {
    if (card_id < 1 || card_id > MAX_CARDS) return 0;
    return ALL_CARDS[card_id - 1].owned;
}
int wasm_card_atk(int card_id) {
    if (card_id < 1 || card_id > MAX_CARDS) return 0;
    return ALL_CARDS[card_id - 1].atk + ALL_CARDS[card_id - 1].level * 5;
}
int wasm_card_def(int card_id) {
    if (card_id < 1 || card_id > MAX_CARDS) return 0;
    return ALL_CARDS[card_id - 1].def + ALL_CARDS[card_id - 1].level * 5;
}
int wasm_card_spd(int card_id) {
    if (card_id < 1 || card_id > MAX_CARDS) return 0;
    return ALL_CARDS[card_id - 1].spd + ALL_CARDS[card_id - 1].level * 5;
}
int wasm_card_cost(int card_id) {
    if (card_id < 1 || card_id > MAX_CARDS) return 0;
    return ALL_CARDS[card_id - 1].cost * 50;
}
int wasm_card_rarity(int card_id) {
    if (card_id < 1 || card_id > MAX_CARDS) return 0;
    return ALL_CARDS[card_id - 1].rarity;
}
int wasm_card_skill(int card_id) {
    if (card_id < 1 || card_id > MAX_CARDS) return 0;
    return ALL_CARDS[card_id - 1].type; // Using type as skill placeholder
}
int wasm_card_lv(int card_id) {
    if (card_id < 1 || card_id > MAX_CARDS) return 0;
    return ALL_CARDS[card_id - 1].level;
}

const char* wasm_card_name(int card_id) {
    if (card_id < 1 || card_id > MAX_CARDS) return "";
    return ALL_CARDS[card_id - 1].name;
}

int wasm_deck_size(void) { return DECK_SIZE; }
int wasm_deck_card(int slot) {
    if (slot < 0 || slot >= MAX_DECK) return 0;
    return DECK[slot];
}

void wasm_shop_buy(int card_id) {
    if (g.state != STATE_SHOP) { msg_set("请在商店中使用"); return; }
    if (card_id < 1 || card_id > MAX_CARDS) { msg_set("无效卡牌"); return; }
    Card* c = &ALL_CARDS[card_id - 1];
    if (c->owned) { msg_set("已拥有此卡牌"); return; }
    int price = c->cost * 50;
    if (g.gold < price) { msg_set("金币不足"); return; }
    g.gold -= price;
    c->owned = 1;
    char buf[128];
    snprintf(buf, sizeof(buf), "获得 [%s]！", c->name);
    msg_set(buf);
}

void wasm_deck_toggle(int card_id) {
    if (card_id < 1 || card_id > MAX_CARDS) return;
    if (!ALL_CARDS[card_id - 1].owned) return;
    
    // Check if already in deck
    int idx = -1;
    for (int i = 0; i < DECK_SIZE; i++) {
        if (DECK[i] == card_id) { idx = i; break; }
    }
    
    if (idx >= 0) {
        // Remove from deck
        for (int i = idx; i < DECK_SIZE - 1; i++) DECK[i] = DECK[i + 1];
        DECK_SIZE--;
        msg_set("已从卡组移除");
    } else if (DECK_SIZE < MAX_DECK) {
        // Add to deck
        DECK[DECK_SIZE++] = card_id;
        msg_set("已添加到卡组");
    } else {
        msg_set("卡组已满(5张)");
    }
}

void wasm_upgrade_card(int card_id) {
    if (g.state != STATE_UPGRADE) { msg_set("请在升级界面使用"); return; }
    if (card_id < 1 || card_id > MAX_CARDS) { msg_set("无效卡牌"); return; }
    Card* c = &ALL_CARDS[card_id - 1];
    if (!c->owned) { msg_set("未拥有此卡牌"); return; }
    if (c->level >= 3) { msg_set("已满级(3级)"); return; }
    int price = (c->rarity + 1) * 50;
    if (g.gold < price) { msg_set("金币不足"); return; }
    g.gold -= price;
    c->level++;
    char buf[128];
    snprintf(buf, sizeof(buf), "升级成功！[%s] Lv.%d ATK:%d DEF:%d SPD:%d",
        c->name, c->level, wasm_card_atk(card_id), wasm_card_def(card_id), wasm_card_spd(card_id));
    msg_set(buf);
}

void wasm_set_faction(int faction) {
    if (faction < 0 || faction > 2) return;
    g.faction = faction;
    char buf[64];
    snprintf(buf, sizeof(buf), "加入阵营: %s", fac_names[faction]);
    msg_set(buf);
}

void wasm_battle_fight(void) {
    if (g.state != STATE_BATTLE) return;
    if (DECK_SIZE == 0) { msg_set("请先构建卡组！"); return; }
    
    // Calculate player power
    int pp = 0;
    for (int i = 0; i < DECK_SIZE; i++) {
        Card* c = &ALL_CARDS[DECK[i] - 1];
        pp += card_power(c);
    }
    pp = pp / MAX_DECK;
    
    // Enemy power (based on cycle)
    int enemy_lvl = (g.cycle / 5) + 1;
    int ep = 50 + enemy_lvl * 20 + rng_next() % 30;
    
    int isBoss = (g.cycle > 0 && g.cycle % 5 == 0 && !g.boss_defeated);
    if (isBoss) {
        ep += 50;
        g.boss_defeated = 1;
    }
    
    // Battle simulation
    for (int r = 1; r <= 5; r++) {
        pp += (rng_next() % 20) - 10;
        ep += (rng_next() % 20) - 10;
    }
    
    int victory = pp >= ep;
    if (victory) {
        g.gold += isBoss ? 300 : 100;
        g.exp += isBoss ? 50 : 20;
        g.boss_defeated = isBoss ? 1 : g.boss_defeated;
        msg_set("胜利！");
    } else {
        g.exp += 5;
        msg_set("败北... +5经验");
    }
    
    // Level up check
    int need = g.level * 100;
    if (g.exp >= need) {
        g.exp -= need;
        g.level++;
        msg_set(victory ? "胜利！升级！" : "败北...升级！");
    }
}

void wasm_mine_gold(void) {
    if (g.state != STATE_MINE) return;
    int m = 50 + rng_next() % 100;
    g.gold += m;
    char buf[64];
    snprintf(buf, sizeof(buf), "挖矿获得 %d 金币！总计: %d", m, g.gold);
    msg_set(buf);
}

void wasm_next_cycle(void) {
    g.cycle++;
    g.boss_defeated = 0;
    int reward = g.cycle * 50;
    g.gold += reward;
    char buf[64];
    snprintf(buf, sizeof(buf), "进入第%d周期！奖励+%d金币", g.cycle, reward);
    msg_set(buf);
}

void wasm_navigate(int state) { g.state = state; }
void wasm_back(void) { g.state = STATE_MENU; }
const char* wasm_get_msg(void) { return g_msg; }
