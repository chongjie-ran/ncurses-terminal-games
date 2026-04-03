// Tetris WASM - Pure C Game Logic Implementation
// 2026-04-01

#include "game.h"
#include <stdlib.h>
#include <string.h>

// 方块形状定义: 7种形状 × 4个旋转状态 × 4×4网格
// 0=空, 1=I(青色), 2=O(黄色), 3=T(紫色), 4=S(绿色), 5=Z(红色), 6=J(蓝), 7=L(橙色)
const int SHAPES[NUM_SHAPES][4][SHAPE_SIZE][SHAPE_SIZE] = {
    // I (4个旋转)
    {
        {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
        {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
        {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},
    },
    // O
    {
        {{0,0,0,0},{0,2,2,0},{0,2,2,0},{0,0,0,0}},
        {{0,0,0,0},{0,2,2,0},{0,2,2,0},{0,0,0,0}},
        {{0,0,0,0},{0,2,2,0},{0,2,2,0},{0,0,0,0}},
        {{0,0,0,0},{0,2,2,0},{0,2,2,0},{0,0,0,0}},
    },
    // T
    {
        {{0,0,0,0},{0,3,0,0},{3,3,3,0},{0,0,0,0}},
        {{0,0,0,0},{0,3,0,0},{0,3,3,0},{0,3,0,0}},
        {{0,0,0,0},{0,0,0,0},{3,3,3,0},{0,3,0,0}},
        {{0,0,0,0},{0,3,0,0},{3,3,0,0},{0,3,0,0}},
    },
    // S
    {
        {{0,0,0,0},{0,4,4,0},{4,4,0,0},{0,0,0,0}},
        {{0,0,0,0},{0,4,0,0},{0,4,4,0},{0,0,4,0}},
        {{0,0,0,0},{0,0,0,0},{0,4,4,0},{4,4,0,0}},
        {{0,0,0,0},{4,0,0,0},{4,4,0,0},{0,4,0,0}},
    },
    // Z
    {
        {{0,0,0,0},{5,5,0,0},{0,5,5,0},{0,0,0,0}},
        {{0,0,0,0},{0,0,5,0},{0,5,5,0},{0,5,0,0}},
        {{0,0,0,0},{0,0,0,0},{5,5,0,0},{0,5,5,0}},
        {{0,0,0,0},{0,5,0,0},{5,5,0,0},{5,0,0,0}},
    },
    // J
    {
        {{0,0,0,0},{6,0,0,0},{6,6,6,0},{0,0,0,0}},
        {{0,0,0,0},{0,6,6,0},{0,6,0,0},{0,6,0,0}},
        {{0,0,0,0},{0,0,0,0},{6,6,6,0},{0,0,6,0}},
        {{0,0,0,0},{0,6,0,0},{0,6,0,0},{6,6,0,0}},
    },
    // L
    {
        {{0,0,0,0},{0,0,7,0},{7,7,7,0},{0,0,0,0}},
        {{0,0,0,0},{0,7,0,0},{0,7,0,0},{0,7,7,0}},
        {{0,0,0,0},{0,0,0,0},{7,7,7,0},{7,0,0,0}},
        {{0,0,0,0},{7,7,0,0},{0,7,0,0},{0,7,0,0}},
    },
};

// 随机形状 (简单线性同余生成)
static unsigned int rng_seed = 12345;
static int rng(void) {
    rng_seed = rng_seed * 1103515245 + 12345;
    return (rng_seed >> 16) & 0x7FFF;
}

void reset_rng_seed(unsigned int seed) {
    rng_seed = seed;
}

void game_init(GameContext* ctx) {
    memset(ctx, 0, sizeof(GameContext));
    ctx->game_state = GAME_STATE_PLAYING;
    ctx->drop_interval = 60; // 60帧 = 1秒@60fps
    ctx->drop_timer = 0;
    ctx->next_shape = rng() % NUM_SHAPES;
    spawn_piece(ctx);
}

int random_shape(void) {
    return rng() % NUM_SHAPES;
}

void get_shape_grid(int shape, int rotation, int grid_out[SHAPE_SIZE][SHAPE_SIZE]) {
    for (int r = 0; r < SHAPE_SIZE; r++) {
        for (int c = 0; c < SHAPE_SIZE; c++) {
            grid_out[r][c] = SHAPES[shape][rotation % 4][r][c];
        }
    }
}

int check_collision(GameContext* ctx, int shape, int rot, int px, int py) {
    int shape_grid[SHAPE_SIZE][SHAPE_SIZE];
    get_shape_grid(shape, rot, shape_grid);
    
    for (int r = 0; r < SHAPE_SIZE; r++) {
        for (int c = 0; c < SHAPE_SIZE; c++) {
            if (shape_grid[r][c]) {
                int gx = px + c;
                int gy = py + r;
                // 边界检查
                if (gx < 0 || gx >= GRID_W || gy >= GRID_H) return 1;
                // 忽略游戏区上方的
                if (gy < 0) continue;
                // 格子已有方块
                if (ctx->grid[gy][gx]) return 1;
            }
        }
    }
    return 0;
}

void lock_piece(GameContext* ctx) {
    int shape_grid[SHAPE_SIZE][SHAPE_SIZE];
    get_shape_grid(ctx->current_shape, ctx->current_rot, shape_grid);
    
    for (int r = 0; r < SHAPE_SIZE; r++) {
        for (int c = 0; c < SHAPE_SIZE; c++) {
            if (shape_grid[r][c]) {
                int gy = ctx->current_y + r;
                int gx = ctx->current_x + c;
                if (gy >= 0 && gy < GRID_H && gx >= 0 && gx < GRID_W) {
                    ctx->grid[gy][gx] = ctx->current_shape + 1; // 颜色ID = shape+1
                }
            }
        }
    }
}

int clear_lines(GameContext* ctx) {
    int cleared = 0;
    for (int r = GRID_H - 1; r >= 0; ) {
        int full = 1;
        for (int c = 0; c < GRID_W; c++) {
            if (ctx->grid[r][c] == 0) {
                full = 0;
                break;
            }
        }
        if (full) {
            // 消除该行，上移所有上面的行
            for (int rr = r; rr > 0; rr--) {
                for (int c = 0; c < GRID_W; c++) {
                    ctx->grid[rr][c] = ctx->grid[rr-1][c];
                }
            }
            for (int c = 0; c < GRID_W; c++) {
                ctx->grid[0][c] = 0;
            }
            cleared++;
        } else {
            r--;
        }
    }
    return cleared;
}

int spawn_piece(GameContext* ctx) {
    ctx->current_shape = ctx->next_shape;
    ctx->next_shape = random_shape();
    ctx->current_rot = 0;
    // 居中放置
    ctx->current_x = (GRID_W - SHAPE_SIZE) / 2;
    ctx->current_y = -2; // 部分在屏幕上方
    
    // 检查初始位置是否有碰撞(游戏结束)
    if (check_collision(ctx, ctx->current_shape, ctx->current_rot, ctx->current_x, ctx->current_y)) {
        ctx->game_state = GAME_STATE_GAMEOVER;
        return 0;
    }
    return 1;
}

int move_piece(GameContext* ctx, int dx, int dy) {
    if (ctx->game_state != GAME_STATE_PLAYING) return 0;
    int nx = ctx->current_x + dx;
    int ny = ctx->current_y + dy;
    if (!check_collision(ctx, ctx->current_shape, ctx->current_rot, nx, ny)) {
        ctx->current_x = nx;
        ctx->current_y = ny;
        return 1;
    }
    return 0;
}

int rotate_piece(GameContext* ctx, int dir) {
    if (ctx->game_state != GAME_STATE_PLAYING) return 0;
    int new_rot = (ctx->current_rot + dir + 4) % 4;
    // 尝试旋转
    if (!check_collision(ctx, ctx->current_shape, new_rot, ctx->current_x, ctx->current_y)) {
        ctx->current_rot = new_rot;
        return 1;
    }
    // 墙踢(Wall Kick): 尝试左右偏移
    int kicks[] = {-1, 1, -2, 2};
    for (int i = 0; i < 4; i++) {
        if (!check_collision(ctx, ctx->current_shape, new_rot, ctx->current_x + kicks[i], ctx->current_y)) {
            ctx->current_x += kicks[i];
            ctx->current_rot = new_rot;
            return 1;
        }
    }
    return 0;
}

int hard_drop(GameContext* ctx) {
    if (ctx->game_state != GAME_STATE_PLAYING) return 0;
    int dropped = 0;
    while (!check_collision(ctx, ctx->current_shape, ctx->current_rot, ctx->current_x, ctx->current_y + 1)) {
        ctx->current_y++;
        dropped++;
    }
    // 锁定
    lock_piece(ctx);
    int cleared = clear_lines(ctx);
    ctx->score += dropped * 2 + cleared * cleared * 100;
    ctx->lines += cleared;
    // 速度升级
    if (ctx->lines >= (ctx->level + 1) * 10) {
        ctx->level++;
        if (ctx->drop_interval > 5) ctx->drop_interval -= 5;
    }
    spawn_piece(ctx);
    return 1;
}

void game_tick(GameContext* ctx) {
    if (ctx->game_state != GAME_STATE_PLAYING) return;
    
    ctx->drop_timer++;
    if (ctx->drop_timer >= ctx->drop_interval) {
        ctx->drop_timer = 0;
        if (!move_piece(ctx, 0, 1)) {
            // 触底，锁定
            lock_piece(ctx);
            int cleared = clear_lines(ctx);
            ctx->score += cleared * cleared * 100;
            ctx->lines += cleared;
            if (ctx->lines >= (ctx->level + 1) * 10) {
                ctx->level++;
                if (ctx->drop_interval > 5) ctx->drop_interval -= 5;
            }
            spawn_piece(ctx);
        }
    }
}

void game_restart(GameContext* ctx) {
    memset(ctx, 0, sizeof(GameContext));
    ctx->game_state = GAME_STATE_PLAYING;
    ctx->drop_interval = 60;
    ctx->next_shape = random_shape();
    spawn_piece(ctx);
}
