#include "game.h"
#include <stdlib.h>
#include <string.h>

// Global game state
SnakeGame g_game;

// Simple random number generator (LCG)
static unsigned int g_seed = 12345;
static int g_rand(void) {
    g_seed = g_seed * 1103515245 + 12345;
    return (g_seed >> 16) & 0x7FFF;
}

static int rand_int(int min, int max) {
    return min + g_rand() % (max - min + 1);
}

// Check if position is occupied by snake
static bool is_snake_at(int x, int y) {
    for (int i = 0; i < g_game.snake_len; i++) {
        if (g_game.snake[i].x == x && g_game.snake[i].y == y)
            return true;
    }
    return false;
}

// Spawn food at random empty location
static void spawn_food(void) {
    int attempts = 0;
    int x, y;
    do {
        x = rand_int(0, GRID_W - 1);
        y = rand_int(0, GRID_H - 1);
        attempts++;
    } while (is_snake_at(x, y) && attempts < 1000);
    g_game.food_x = x;
    g_game.food_y = y;
}

// Move snake one step
static void move_snake(void) {
    g_game.dir = g_game.next_dir;
    
    Segment head = g_game.snake[0];
    switch (g_game.dir) {
        case DIR_UP:    head.y--; break;
        case DIR_DOWN:  head.y++; break;
        case DIR_LEFT:  head.x--; break;
        case DIR_RIGHT: head.x++; break;
    }
    
    // Wall collision
    if (head.x < 0 || head.x >= GRID_W || head.y < 0 || head.y >= GRID_H) {
        g_game.state = STATE_GAME_OVER;
        return;
    }
    
    // Self collision (check all except tail which will move)
    for (int i = 0; i < g_game.snake_len - 1; i++) {
        if (g_game.snake[i].x == head.x && g_game.snake[i].y == head.y) {
            g_game.state = STATE_GAME_OVER;
            return;
        }
    }
    
    // Check food
    bool ate = (head.x == g_game.food_x && head.y == g_game.food_y);
    
    // Move snake: shift body
    for (int i = g_game.snake_len - 1; i > 0; i--) {
        g_game.snake[i] = g_game.snake[i - 1];
    }
    g_game.snake[0] = head;
    
    if (ate) {
        g_game.snake_len++;
        g_game.score += 10;
        
        // Speed up every 50 points
        if (g_game.score % 50 == 0 && g_game.speed < 5) {
            g_game.speed++;
            g_game.move_interval = 12 - (g_game.speed - 1) * 2;
            if (g_game.move_interval < 4) g_game.move_interval = 4;
        }
        
        spawn_food();
    }
}

// --- Public API ---

void wasm_init_game(void) {
    g_seed = 12345;
    memset(&g_game, 0, sizeof(g_game));
    
    int start_x = GRID_W / 2;
    int start_y = GRID_H / 2;
    g_game.snake[0].x = start_x;
    g_game.snake[0].y = start_y;
    g_game.snake[1].x = start_x - 1;
    g_game.snake[1].y = start_y;
    g_game.snake[2].x = start_x - 2;
    g_game.snake[2].y = start_y;
    g_game.snake_len = 3;
    
    g_game.dir = DIR_RIGHT;
    g_game.next_dir = DIR_RIGHT;
    g_game.score = 0;
    g_game.state = STATE_MENU;
    g_game.speed = 1;
    g_game.move_timer = 0;
    g_game.move_interval = 12;
    g_game.dir_timer = 0;
    
    spawn_food();
}

void wasm_start_game(void) {
    wasm_init_game();
    g_game.state = STATE_PLAYING;
}

void wasm_set_dir_game(int dir) {
    if (g_game.dir_timer > 0) return;
    if (dir == DIR_UP    && g_game.dir != DIR_DOWN)  g_game.next_dir = dir;
    if (dir == DIR_DOWN  && g_game.dir != DIR_UP)    g_game.next_dir = dir;
    if (dir == DIR_LEFT  && g_game.dir != DIR_RIGHT) g_game.next_dir = dir;
    if (dir == DIR_RIGHT && g_game.dir != DIR_LEFT)  g_game.next_dir = dir;
    g_game.dir_timer = 3;
}

void wasm_toggle_pause_game(void) {
    if (g_game.state == STATE_PLAYING) {
        g_game.state = STATE_PAUSED;
    } else if (g_game.state == STATE_PAUSED) {
        g_game.state = STATE_PLAYING;
    }
}

void wasm_restart_game(void) {
    wasm_init_game();
    g_game.state = STATE_PLAYING;
}

void wasm_tick_game(void) {
    if (g_game.state != STATE_PLAYING) return;
    if (g_game.dir_timer > 0) g_game.dir_timer--;
    g_game.move_timer++;
    if (g_game.move_timer >= g_game.move_interval) {
        g_game.move_timer = 0;
        move_snake();
    }
}

// Getters
int wasm_get_state_game(void) { return g_game.state; }
int wasm_get_score_game(void) { return g_game.score; }
int wasm_get_speed_game(void) { return g_game.speed; }
int wasm_get_snake_len_game(void) { return g_game.snake_len; }
int wasm_get_snake_x_game(int i) { return (i < g_game.snake_len) ? g_game.snake[i].x : 0; }
int wasm_get_snake_y_game(int i) { return (i < g_game.snake_len) ? g_game.snake[i].y : 0; }
int wasm_get_food_x_game(void) { return g_game.food_x; }
int wasm_get_food_y_game(void) { return g_game.food_y; }
