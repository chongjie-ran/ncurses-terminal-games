// Tetris WASM - Emscripten Main Entry
// 2026-04-01
// 导出函数供JS调用

#include <emscripten.h>
#include "game.h"
#include <stdlib.h>
#include <string.h>

// 全局游戏上下文
static GameContext g_ctx;

// 导出: 初始化游戏
EMSCRIPTEN_KEEPALIVE
void wasm_init(void) {
    reset_rng_seed(12345); // 重置随机种子
    game_init(&g_ctx);
}

// 导出: 每帧tick
EMSCRIPTEN_KEEPALIVE
void wasm_tick(void) {
    game_tick(&g_ctx);
}

// 导出: 重启
EMSCRIPTEN_KEEPALIVE
void wasm_restart(void) {
    game_restart(&g_ctx);
}

// 导出: 移动 (-1=左,1=右,0=下)
EMSCRIPTEN_KEEPALIVE
int wasm_move(int dx) {
    return move_piece(&g_ctx, dx, 0);
}

// 导出: 下落一步
EMSCRIPTEN_KEEPALIVE
int wasm_down(void) {
    int r = move_piece(&g_ctx, 0, 1);
    if (!r) {
        lock_piece(&g_ctx);
        clear_lines(&g_ctx);
        spawn_piece(&g_ctx);
    }
    return r;
}

// 导出: 硬降
EMSCRIPTEN_KEEPALIVE
int wasm_hard_drop(void) {
    return hard_drop(&g_ctx);
}

// 导出: 旋转 (-1=逆时针,1=顺时针)
EMSCRIPTEN_KEEPALIVE
int wasm_rotate(int dir) {
    return rotate_piece(&g_ctx, dir);
}

// 导出: 获取游戏区格子
// 返回: 0=空, 非0=颜色ID
EMSCRIPTEN_KEEPALIVE
int wasm_get_grid(int row, int col) {
    if (row < 0 || row >= GRID_H || col < 0 || col >= GRID_W) return 0;
    return g_ctx.grid[row][col];
}

// 导出: 获取当前活动方块的格子
// 返回: 0=空, 非0=颜色ID
EMSCRIPTEN_KEEPALIVE
int wasm_get_active(int row, int col) {
    int shape_grid[SHAPE_SIZE][SHAPE_SIZE];
    get_shape_grid(g_ctx.current_shape, g_ctx.current_rot, shape_grid);
    int gy = row - g_ctx.current_y;
    int gx = col - g_ctx.current_x;
    if (gy >= 0 && gy < SHAPE_SIZE && gx >= 0 && gx < SHAPE_SIZE) {
        return shape_grid[gy][gx] ? g_ctx.current_shape + 1 : 0;
    }
    return 0;
}

// 导出: 获取游戏状态
EMSCRIPTEN_KEEPALIVE
int wasm_get_state(void) {
    return g_ctx.game_state;
}

// 导出: 获取分数
EMSCRIPTEN_KEEPALIVE
int wasm_get_score(void) {
    return g_ctx.score;
}

// 导出: 获取消除行数
EMSCRIPTEN_KEEPALIVE
int wasm_get_lines(void) {
    return g_ctx.lines;
}

// 导出: 获取等级
EMSCRIPTEN_KEEPALIVE
int wasm_get_level(void) {
    return g_ctx.level;
}

// 导出: 获取下一个形状ID
EMSCRIPTEN_KEEPALIVE
int wasm_get_next_shape(void) {
    return g_ctx.next_shape;
}

// 导出: 获取当前形状ID
EMSCRIPTEN_KEEPALIVE
int wasm_get_current_shape(void) {
    return g_ctx.current_shape;
}

// 导出: 获取当前旋转状态
EMSCRIPTEN_KEEPALIVE
int wasm_get_current_rot(void) {
    return g_ctx.current_rot;
}

// 导出: 获取当前X位置
EMSCRIPTEN_KEEPALIVE
int wasm_get_current_x(void) {
    return g_ctx.current_x;
}

// 导出: 获取当前Y位置
EMSCRIPTEN_KEEPALIVE
int wasm_get_current_y(void) {
    return g_ctx.current_y;
}

// 导出: 获取网格尺寸
EMSCRIPTEN_KEEPALIVE
int wasm_get_grid_w(void) { return GRID_W; }
EMSCRIPTEN_KEEPALIVE
int wasm_get_grid_h(void) { return GRID_H; }
