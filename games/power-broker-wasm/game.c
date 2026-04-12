/*
 * Power Broker WASM - AI算力市场交易游戏
 * Pure C + Emscripten
 */

#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// === Game State ===
static struct {
    int state;
    int difficulty;
    int day;
    int cash;
    int power;
    int price;
    int profit;
    int init_assets;
    int total_assets;
    int win_streak;
    int day_profit;
    int volume;
    int boss_today;
} g;

// === Difficulty Config ===
static const int INIT_CASH[]  = {150000, 100000, 60000};
static const int INIT_POWER[] = {150,     100,    80};
static const int DAY_LIMITS[] = {30,      30,     25};
static const int VOLATILITY[] = {8,       12,     18};

// === Events ===
#define N_EVENTS 12
static const char* EVT_NAME[] = {
    "AI芯片禁令", "数据中心故障", "新能源突破", "新矿难",
    "大厂抛售", "政策利好", "黑客攻击", "5nm量产",
    "算力泡沫", "战争风险", "矿场事故", "关税下调"
};
static const char* EVT_DESC[] = {
    "美国扩大芯片出口管制", "某大厂数据中心宕机", "电价暴跌，算力成本降低",
    "内蒙古矿难，GPU供应紧张", "某巨头出售过剩算力", "国家出台AI算力扶持政策",
    "交易所遭攻击", "新一代制程量产，成本下降", "市场过热，监管介入",
    "地缘冲突升级", "西部矿场安全事故", "进口GPU关税降低"
};
static const int EVT_IMPACT[] = {
    +40, +25, -20, +30, -35, +15, -15, -25, -20, +35, +20, -18
};

// === Achievements ===
#define N_ACHS 12
static const char* ACH_NAME[] = {
    "初入市场","大额交易","小有盈利","百万富翁",
    "触底反弹","三连胜","五连胜","算力大亨",
    "孤注一掷","Boss克星","交易王","硬核玩家"
};
static const char* ACH_DESC[] = {
    "完成首次交易","单次交易≥50单位","累计盈利≥$10000","累计盈利≥$100000",
    "资产缩水50%后恢复","连续3天赚钱","连续5天赚钱","持有≥1000单位算力",
    "全仓买入","成功应对Boss级事件","总交易量≥1000单位","通关Hard难度"
};
static int ach_unlocked[N_ACHS];

// === Current Event ===
static int cur_event_idx = 0;
static int cur_event_impact = 0;
static int loss_flag = 0;
static int first_done = 0;
static int allin_done = 0;

// === Random ===
static unsigned int rng_seed;
static int rng_next(void) {
    rng_seed = rng_seed * 1103515245 + 12345;
    return (int)((rng_seed >> 16) & 0x7FFF);
}

// === Core Logic ===
static int price_clamp(int p) {
    if (p < PRICE_MIN) return PRICE_MIN;
    if (p > PRICE_MAX) return PRICE_MAX;
    return p;
}

static int price_fluctuate(void) {
    int vol = VOLATILITY[g.difficulty];
    return (rng_next() % (vol * 2 + 1)) - vol;
}

static int assets(void) {
    return g.cash + g.power * g.price;
}

static int daily_profit(void) {
    return assets() - g.init_assets;
}

static void update_event(void) {
    cur_event_idx = rng_next() % N_EVENTS;
    cur_event_impact = EVT_IMPACT[cur_event_idx];
    
    // Boss days: 10, 20
    g.boss_today = (g.day == 10 || g.day == 20) ? 1 : 0;
    
    int delta = price_fluctuate();
    if (g.boss_today) {
        delta += (cur_event_impact > 0) ? 60 : 50;
        delta = (cur_event_impact > 0) ? (delta > 0 ? 60 : -40) : (delta < 0 ? -40 : 10);
    } else {
        delta += (g.price * cur_event_impact) / 100;
    }
    
    g.price = price_clamp(g.price + delta);
}

static void check_achieves(void) {
    int a = assets();
    int p = daily_profit();
    g.profit = p;
    
    // First trade
    if (!first_done && g.volume > 0) {
        first_done = 1;
        ach_unlocked[0] = 1;
    }
    // Profit milestones
    if (p >= 10000) ach_unlocked[2] = 1;
    if (p >= 100000) ach_unlocked[3] = 1;
    // Streak
    if (g.win_streak >= 3) ach_unlocked[5] = 1;
    if (g.win_streak >= 5) ach_unlocked[6] = 1;
    // Power
    if (g.power >= 1000) ach_unlocked[7] = 1;
    // Boss
    if (g.boss_today) ach_unlocked[9] = 1;
    // Volume
    if (g.volume >= 1000) ach_unlocked[10] = 1;
    // Hard
    if (g.difficulty == DIFF_HARD) ach_unlocked[11] = 1;
    // Loss recovery
    if (!loss_flag && a <= g.init_assets / 2) loss_flag = 1;
    if (loss_flag && a >= g.init_assets) {
        loss_flag = 0;
        ach_unlocked[4] = 1;
    }
}

static int count_achs(void) {
    int n = 0;
    for (int i = 0; i < N_ACHS; i++) if (ach_unlocked[i]) n++;
    return n;
}

// === WASM Exports ===
void wasm_init(void) {
    g.state = STATE_MENU;
    g.day = 1;
    g.cash = 100000;
    g.power = 100;
    g.price = 100;
    g.profit = 0;
    g.init_assets = 110000;
    g.total_assets = 110000;
    g.win_streak = 0;
    g.volume = 0;
    g.boss_today = 0;
    rng_seed = (unsigned int)time(NULL);
    for (int i = 0; i < N_ACHS; i++) ach_unlocked[i] = 0;
    first_done = allin_done = loss_flag = 0;
}

void wasm_new_game(int difficulty) {
    if (difficulty < 0 || difficulty > 2) difficulty = 1;
    g.difficulty = difficulty;
    g.cash = INIT_CASH[difficulty];
    g.power = INIT_POWER[difficulty];
    g.price = 100;
    g.day = 1;
    g.profit = 0;
    g.init_assets = g.cash + g.power * g.price;
    g.total_assets = g.init_assets;
    g.win_streak = 0;
    g.volume = 0;
    g.boss_today = 0;
    for (int i = 0; i < N_ACHS; i++) ach_unlocked[i] = 0;
    first_done = allin_done = loss_flag = 0;
    rng_seed = (unsigned int)(time(NULL) + difficulty * 17);
    
    update_event();
    g.state = STATE_TRADING;
}

int wasm_get_state(void) { return g.state; }
int wasm_get_day(void) { return g.day; }
int wasm_get_cash(void) { return g.cash; }
int wasm_get_power(void) { return g.power; }
int wasm_get_price(void) { return g.price; }
int wasm_get_profit(void) { return g.profit; }
int wasm_get_init_assets(void) { return g.init_assets; }
int wasm_get_total_assets(void) { return assets(); }
int wasm_get_win_streak(void) { return g.win_streak; }
int wasm_is_boss_day(void) { return g.boss_today; }

const char* wasm_get_event_name(void) {
    return EVT_NAME[cur_event_idx];
}
const char* wasm_get_event_desc(void) {
    return EVT_DESC[cur_event_idx];
}
int wasm_get_event_impact(void) {
    return cur_event_impact;
}

int wasm_get_ach_count(void) { return N_ACHS; }
int wasm_get_ach_unlocked(int idx) {
    if (idx < 0 || idx >= N_ACHS) return 0;
    return ach_unlocked[idx];
}
const char* wasm_get_ach_name(int idx) {
    if (idx < 0 || idx >= N_ACHS) return "";
    return ACH_NAME[idx];
}
const char* wasm_get_ach_desc(int idx) {
    if (idx < 0 || idx >= N_ACHS) return "";
    return ACH_DESC[idx];
}

void wasm_player_action(int action_type, int amount) {
    if (g.state != STATE_TRADING) return;
    // action_type: 1=buy, 2=sell
    int cost = amount * g.price;
    
    if (action_type == 1) { // Buy
        if (cost <= g.cash && amount > 0) {
            g.cash -= cost;
            g.power += amount;
            g.volume += amount;
            if (!allin_done && g.cash < g.price) { allin_done = 1; ach_unlocked[8] = 1; }
            if (amount >= 50) ach_unlocked[1] = 1;
        }
    } else if (action_type == 2) { // Sell
        if (amount <= g.power && amount > 0) {
            g.cash += cost;
            g.power -= amount;
            g.volume += amount;
        }
    }
}

void wasm_end_day(void) {
    if (g.state != STATE_TRADING) return;
    
    int a = assets();
    g.profit = a - g.init_assets;
    if (g.profit > 0) g.win_streak++; else g.win_streak = 0;
    
    check_achieves();
    
    g.day++;
    g.total_assets = assets();
    
    if (g.day > DAY_LIMITS[g.difficulty]) {
        g.state = STATE_GAMEOVER;
    } else {
        update_event();
    }
}

void wasm_return_to_menu(void) {
    g.state = STATE_MENU;
}
