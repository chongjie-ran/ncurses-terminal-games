// Tetris WASM - Pure C Game Logic
// 2026-04-01

#ifndef GAME_H
#define GAME_H

// 画布尺寸
#define CANVAS_W 320
#define CANVAS_H 640

// 游戏区 (10x20)
#define GRID_W 10
#define GRID_H 20
#define CELL_SIZE 32

// 方块形状 (4个旋转状态 × 4行 × 4列)
// I, O, T, S, Z, J, L
#define NUM_SHAPES 7
#define SHAPE_SIZE 4

// 游戏状态
#define GAME_STATE_PLAYING 0
#define GAME_STATE_PAUSED  1
#define GAME_STATE_GAMEOVER 2

// 方块形状定义 (每个形状4个旋转状态)
extern const int SHAPES[NUM_SHAPES][4][SHAPE_SIZE][SHAPE_SIZE];

// 俄罗斯方块7种形状的初始朝向索引
#define SHAPE_I 0
#define SHAPE_O 1
#define SHAPE_T 2
#define SHAPE_S 3
#define SHAPE_Z 4
#define SHAPE_J 5
#define SHAPE_L 6

// 游戏结构
typedef struct {
    int grid[GRID_H][GRID_W];  // 游戏区域，0=空，非0=颜色ID
    int current_shape;          // 当前形状类型
    int current_rot;           // 当前旋转状态
    int current_x;             // 当前x位置(列)
    int current_y;             // 当前y位置(行)
    int next_shape;            // 下一个形状
    int score;                 // 得分
    int lines;                 // 消除行数
    int level;                 // 等级(速度)
    int game_state;            // 游戏状态
    int drop_timer;            // 下落计时器
    int drop_interval;         // 下落间隔(帧数)
    int lock_delay;            // 锁定延迟
} GameContext;

// 初始化游戏
void game_init(GameContext* ctx);

// 随机形状
int random_shape(void);

// 重置随机种子(供wasm调用)
void reset_rng_seed(unsigned int seed);

// 获取形状某旋转状态的4x4网格
void get_shape_grid(int shape, int rotation, int grid_out[SHAPE_SIZE][SHAPE_SIZE]);

// 检查碰撞
int check_collision(GameContext* ctx, int shape, int rot, int px, int py);

// 固定方块到游戏区
void lock_piece(GameContext* ctx);

// 消除满行
int clear_lines(GameContext* ctx);

// 产生新方块
int spawn_piece(GameContext* ctx);

// 旋转
int rotate_piece(GameContext* ctx, int dir);

// 移动
int move_piece(GameContext* ctx, int dx, int dy);

// 硬降(直接落到底)
int hard_drop(GameContext* ctx);

// 游戏tick(每帧调用)
void game_tick(GameContext* ctx);

// 重启
void game_restart(GameContext* ctx);

#endif // GAME_H
