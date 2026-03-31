#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

#define MAX_GRID_W 20
#define MAX_GRID_H 15

// Level 0: floor, 1: wall, 2: box, 3: target, 4: player, 5: box on target, 6: player on target
// Encoding for C: 0=floor, 1=wall, 2=box, 3=target, 4=player
// We'll separate boxes/targets in parallel arrays

static int grid[MAX_GRID_H][MAX_GRID_W];
static int boxes[MAX_GRID_H][MAX_GRID_W];  // 1 = box here
static int targets[MAX_GRID_H][MAX_GRID_W]; // 1 = target here
static int num_boxes;
static int num_targets_filled;

int grid_width = 0;
int grid_height = 0;
int player_x = 0;
int player_y = 0;
int moves = 0;
int pushes = 0;
int game_state = GAME_STATE_PLAYING;
int current_level = 0;

// Sokoban levels - compact string format
// '.'=floor, '#'=wall, '$'=box, '.'=target(separate), '@'=player, '*'=box+target, '+'=player+target
static const char* LEVELS[] = {
    // Level 1 - Simple introduction
    "#######\n"
    "#.@ # #\n"
    "# $   #\n"
    "#   # #\n"
    "#  #$ #\n"
    "#     #\n"
    "#######",

    // Level 2 - Two boxes
    "########\n"
    "#  .   #\n"
    "# $    #\n"
    "#  @ $ #\n"
    "#   .  #\n"
    "########",

    // Level 3 - Classic layout
    "  #####\n"
    "###   #\n"
    "# $ # ##\n"
    "# #  . #\n"
    "#   #. #\n"
    "##$    #\n"
    " #@ .  #\n"
    " ######",

    // Level 4 - Corridors
    "########\n"
    "#      #\n"
    "# .$$# #\n"
    "#.#@  ##\n"
    "#      #\n"
    "########",

    // Level 5 - Tight squeeze
    "#######\n"
    "#.    #\n"
    "#.#$# #\n"
    "# $  $ #\n"
    "##@ ###\n"
    " #$ #\n"
    " #. #\n"
    " ###",

    // Level 6 - Medium
    " ######\n"
    "##    #\n"
    "# $ $ #\n"
    "# ## ##\n"
    "# #@.  #\n"
    "#   .  #\n"
    "#### ###",

    // Level 7 - Multi-target
    "########\n"
    "#      #\n"
    "# $#$# #\n"
    "#  @   #\n"
    "# ##.##\n"
    "# .#.  #\n"
    "########",

    // Level 8 - Harder
    " #######\n"
    "##  #  #\n"
    "# $ #  #\n"
    "#   $  #\n"
    "## @#  #\n"
    " #. .# #\n"
    " # .  ###\n"
    " #####",
};

#define NUM_LEVELS (int)(sizeof(LEVELS) / sizeof(LEVELS[0]))

void parse_level(const char* level_str) {
    memset(grid, 0, sizeof(grid));
    memset(boxes, 0, sizeof(boxes));
    memset(targets, 0, sizeof(targets));
    num_boxes = 0;
    num_targets_filled = 0;
    
    int h = 0;
    int w = 0;
    const char* p = level_str;
    int row = 0;
    
    while (*p) {
        if (*p == '\n') {
            if (w > grid_width) grid_width = w;
            row++;
            w = 0;
            p++;
            continue;
        }
        char c = *p;
        if (c == '#') {
            grid[row][w] = 1;
        } else if (c == '$') {
            grid[row][w] = 0;
            boxes[row][w] = 1;
            num_boxes++;
        } else if (c == '.') {
            grid[row][w] = 0;
            targets[row][w] = 1;
        } else if (c == '@') {
            grid[row][w] = 0;
            player_x = w;
            player_y = row;
        } else if (c == '*') {
            grid[row][w] = 0;
            boxes[row][w] = 1;
            targets[row][w] = 1;
            num_boxes++;
        } else if (c == '+') {
            grid[row][w] = 0;
            targets[row][w] = 1;
            player_x = w;
            player_y = row;
        } else {
            grid[row][w] = 0;
        }
        w++;
        p++;
    }
    grid_height = row;
}

EMSCRIPTEN_KEEPALIVE
void init_level(int level) {
    if (level < 0 || level >= NUM_LEVELS) level = 0;
    current_level = level;
    parse_level(LEVELS[level]);
    moves = 0;
    pushes = 0;
    game_state = GAME_STATE_PLAYING;
}

int is_wall(int x, int y) {
    if (x < 0 || x >= grid_width || y < 0 || y >= grid_height) return 1;
    return grid[y][x] == 1;
}

int is_box(int x, int y) {
    if (x < 0 || x >= grid_width || y < 0 || y >= grid_height) return 0;
    return boxes[y][x];
}

int is_target(int x, int y) {
    if (x < 0 || x >= grid_width || y < 0 || y >= grid_height) return 0;
    return targets[y][x];
}

EMSCRIPTEN_KEEPALIVE
int wasm_click_move(int dx, int dy) {
    if (game_state != GAME_STATE_PLAYING) return 0;
    
    int nx = player_x + dx;
    int ny = player_y + dy;
    
    if (is_wall(nx, ny)) return 0;
    
    if (is_box(nx, ny)) {
        int bx = nx + dx;
        int by = ny + dy;
        if (is_wall(bx, by) || is_box(bx, by)) return 0;
        // Push box
        boxes[ny][nx] = 0;
        boxes[by][bx] = 1;
        pushes++;
    }
    
    player_x = nx;
    player_y = ny;
    moves++;
    
    // Check win condition
    int filled = 0;
    for (int y = 0; y < grid_height; y++) {
        for (int x = 0; x < grid_width; x++) {
            if (boxes[y][x] && targets[y][x]) filled++;
        }
    }
    num_targets_filled = filled;
    
    int total_targets = 0;
    for (int y = 0; y < grid_height; y++) {
        for (int x = 0; x < grid_width; x++) {
            if (targets[y][x]) total_targets++;
        }
    }
    
    if (filled == total_targets) {
        game_state = GAME_STATE_WON;
    }
    
    return 1;
}

EMSCRIPTEN_KEEPALIVE
int wasm_tick(void) {
    return game_state;
}

EMSCRIPTEN_KEEPALIVE
void wasm_restart(void) {
    init_level(current_level);
}

EMSCRIPTEN_KEEPALIVE
int wasm_next_level(void) {
    int next = (current_level + 1) % NUM_LEVELS;
    init_level(next);
    return current_level;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_cell(int x, int y) {
    if (x < 0 || x >= grid_width || y < 0 || y >= grid_height) return CELL_WALL;
    if (x == player_x && y == player_y) {
        return is_target(x, y) ? CELL_PLAYER_ON_TARGET : CELL_PLAYER;
    }
    if (boxes[y][x]) {
        return is_target(x, y) ? CELL_BOX_ON_TARGET : CELL_BOX;
    }
    if (is_target(x, y)) return CELL_TARGET;
    if (grid[y][x] == 1) return CELL_WALL;
    return CELL_FLOOR;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_grid_width(void)  { return grid_width; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_grid_height(void) { return grid_height; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_player_x(void)    { return player_x; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_player_y(void)    { return player_y; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_moves(void)       { return moves; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_pushes(void)      { return pushes; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_state(void)       { return game_state; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_current_level(void){ return current_level; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_total_levels(void) { return (int)NUM_LEVELS; }
