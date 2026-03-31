#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

#define NUM_OBSTACLES 20
#define SAFE_ZONE_ROWS 3
#define ROAD_ROWS 5
#define WATER_ROWS 5

static Player player;
static Obstacle obstacles[NUM_OBSTACLES];
static int obstacle_count;
static int death_timer;
static int forward_progress[5]; // track which home slots are filled
static int hop_animation;
static int prev_grid_y;

// Lane config: type 0=road, 1=water, 2=grass(safe)
static const int lane_types[NUM_LANES] = {
    2, // grass - home
    1, // water (lily pads)
    1, // water (turtles)
    1, // water (logs)
    1, // water (logs)
    2, // grass median
    0, // road (trucks)
    0, // road (cars)
    0, // road (cars)
    0, // road (trucks)
    2, // grass - start
};

// Simple deterministic random
static int seed = 42;
static int my_rand(void) {
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}

void game_init(void) {
    memset(obstacles, 0, sizeof(obstacles));
    memset(forward_progress, 0, sizeof(forward_progress));
    
    player.grid_x = PLAYER_START_X;
    player.grid_y = PLAYER_START_Y;
    player.x = PLAYER_START_X * CELL_SIZE;
    player.y = PLAYER_START_Y * CELL_SIZE;
    player.lives = 3;
    player.score = 0;
    player.level = 1;
    player.state = GAME_STATE_PLAYING;
    
    prev_grid_y = player.grid_y;
    hop_animation = 0;
    death_timer = 0;
    obstacle_count = 0;
    seed = 42;
    
    // Create obstacles per lane
    int idx = 0;
    for (int lane = 0; lane < NUM_LANES && idx < NUM_OBSTACLES; lane++) {
        int lane_y = lane + 1; // offset by 1 for home row
        int count = 3;
        float speed = 0.5f + (my_rand() % 5) * 0.3f;
        int dir = (lane % 2 == 0) ? -1 : 1;
        int type = 0; // car/truck
        int w = 2;
        
        if (lane_types[lane] == 1) {
            type = (my_rand() % 3 == 0) ? 1 : 0; // turtle or log
            speed = 0.8f + (my_rand() % 4) * 0.4f;
            count = 3;
            w = (type == 1) ? 2 : 3;
        } else if (lane_types[lane] == 0) {
            type = 0;
            speed = 1.0f + (my_rand() % 3) * 0.5f;
            count = 4;
            w = (my_rand() % 2 == 0) ? 2 : 3;
        }
        
        for (int j = 0; j < count && idx < NUM_OBSTACLES; j++) {
            obstacles[idx].x = (j * (GRID_W * CELL_SIZE / count)) - CELL_SIZE;
            obstacles[idx].y = lane_y * CELL_SIZE;
            obstacles[idx].grid_x = 0;
            obstacles[idx].grid_y = lane_y;
            obstacles[idx].type = type;
            obstacles[idx].direction = dir;
            obstacles[idx].speed = speed;
            obstacles[idx].width = w;
            idx++;
        }
    }
    obstacle_count = idx;
}

static int is_on_log(int idx) {
    Obstacle *o = &obstacles[idx];
    float px = player.x + CELL_SIZE / 2;
    float py = player.y + CELL_SIZE / 2;
    
    float ox = o->x + CELL_SIZE / 2;
    float ow = o->width * CELL_SIZE;
    
    if (py >= o->y && py <= o->y + CELL_SIZE) {
        if (px >= o->x && px <= o->x + ow) {
            return 1;
        }
    }
    return 0;
}

static int is_safe_zone(int gy) {
    // Top 3 rows (home) and bottom row (start) are safe
    return (gy == 0 || gy == NUM_LANES + 1);
}

static int is_water_zone(int gy) {
    int lane = gy - 1;
    if (lane < 0 || lane >= NUM_LANES) return 0;
    return lane_types[lane] == 1;
}

static int is_road_zone(int gy) {
    int lane = gy - 1;
    if (lane < 0 || lane >= NUM_LANES) return 0;
    return lane_types[lane] == 0;
}

int game_tick(void) {
    if (player.state == GAME_STATE_DEAD) {
        death_timer--;
        if (death_timer <= 0) {
            if (player.lives <= 0) {
                player.state = GAME_STATE_GAMEOVER;
            } else {
                player.grid_x = PLAYER_START_X;
                player.grid_y = PLAYER_START_Y;
                player.x = PLAYER_START_X * CELL_SIZE;
                player.y = PLAYER_START_Y * CELL_SIZE;
                player.state = GAME_STATE_PLAYING;
            }
        }
        return 0;
    }
    
    if (player.state == GAME_STATE_WIN || player.state == GAME_STATE_GAMEOVER) {
        return 0;
    }
    
    // Move obstacles
    for (int i = 0; i < obstacle_count; i++) {
        obstacles[i].x += obstacles[i].direction * obstacles[i].speed;
        
        // Wrap around
        if (obstacles[i].direction > 0 && obstacles[i].x > GRID_W * CELL_SIZE) {
            obstacles[i].x = -obstacles[i].width * CELL_SIZE;
        } else if (obstacles[i].direction < 0 && obstacles[i].x + obstacles[i].width * CELL_SIZE < 0) {
            obstacles[i].x = GRID_W * CELL_SIZE;
        }
    }
    
    // If on water and not on log, die
    if (is_water_zone(player.grid_y)) {
        int on_log = 0;
        for (int i = 0; i < obstacle_count; i++) {
            if (is_on_log(i)) {
                on_log = 1;
                // Move with log
                player.x += obstacles[i].direction * obstacles[i].speed;
                break;
            }
        }
        if (!on_log) {
            // Fell in water
            player.state = GAME_STATE_DEAD;
            death_timer = 30;
            player.lives--;
            return 0;
        }
    }
    
    // Check road collision
    if (is_road_zone(player.grid_y)) {
        for (int i = 0; i < obstacle_count; i++) {
            Obstacle *o = &obstacles[i];
            float px = player.x + CELL_SIZE / 2;
            float py = player.y + CELL_SIZE / 2;
            float ox = o->x + CELL_SIZE / 2;
            float ow = o->width * CELL_SIZE;
            
            if (py >= o->y && py <= o->y + CELL_SIZE) {
                if (px >= o->x && px <= o->x + ow) {
                    player.state = GAME_STATE_DEAD;
                    death_timer = 30;
                    player.lives--;
                    return 0;
                }
            }
        }
    }
    
    // Check out of bounds
    if (player.x < -CELL_SIZE || player.x >= GRID_W * CELL_SIZE) {
        player.state = GAME_STATE_DEAD;
        death_timer = 30;
        player.lives--;
        return 0;
    }
    
    // Check home (top row)
    if (player.grid_y == 0) {
        int slot = player.grid_x / 3; // 5 slots
        if (slot >= 0 && slot < 5 && !forward_progress[slot]) {
            forward_progress[slot] = 1;
            player.score += 100 * player.level;
        }
        // Reset to start
        player.grid_x = PLAYER_START_X;
        player.grid_y = PLAYER_START_Y;
        player.x = PLAYER_START_X * CELL_SIZE;
        player.y = PLAYER_START_Y * CELL_SIZE;
        player.score += 10;
        player.state = GAME_STATE_WIN;
        death_timer = 45;
    }
    
    return 0;
}

int game_click(int grid_x, int grid_y) {
    if (player.state != GAME_STATE_PLAYING) return 0;
    
    int dx = grid_x - player.grid_x;
    int dy = grid_y - player.grid_y;
    
    // Only allow move of 1 in any direction
    if (dx == 0 && dy == -1) { // up
        if (player.grid_y > 0) {
            player.grid_y--;
            player.y = player.grid_y * CELL_SIZE;
            player.score += 10;
            prev_grid_y = player.grid_y;
            return 1;
        }
    } else if (dx == 0 && dy == 1) { // down
        if (player.grid_y < GRID_H - 1) {
            player.grid_y++;
            player.y = player.grid_y * CELL_SIZE;
            return 1;
        }
    } else if (dy == 0 && dx == -1) { // left
        if (player.grid_x > 0) {
            player.grid_x--;
            player.x = player.grid_x * CELL_SIZE;
            return 1;
        }
    } else if (dy == 0 && dx == 1) { // right
        if (player.grid_x < GRID_W - 1) {
            player.grid_x++;
            player.x = player.grid_x * CELL_SIZE;
            return 1;
        }
    }
    return 0;
}

void game_restart(void) {
    game_init();
}

int get_player_grid_x(void) { return player.grid_x; }
int get_player_grid_y(void) { return player.grid_y; }
int get_player_lives(void) { return player.lives; }
int get_player_score(void) { return player.score; }
int get_player_level(void) { return player.level; }
int get_player_state(void) { return player.state; }

int get_obstacle_count(void) { return obstacle_count; }
float get_obstacle_x(int idx) { return idx < obstacle_count ? obstacles[idx].x : 0; }
float get_obstacle_y(int idx) { return idx < obstacle_count ? obstacles[idx].y : 0; }
int get_obstacle_type(int idx) { return idx < obstacle_count ? obstacles[idx].type : 0; }
int get_obstacle_width(int idx) { return idx < obstacle_count ? obstacles[idx].width : 0; }
int get_obstacle_dir(int idx) { return idx < obstacle_count ? obstacles[idx].direction : 0; }
float get_obstacle_speed(int idx) { return idx < obstacle_count ? obstacles[idx].speed : 0; }
