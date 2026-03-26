/**
 * Snake Game - Terminal Edition
 * 使用 ncurses 编写的贪吃蛇游戏
 *
 * 编译: g++ -std=c++17 -o snake snake.cpp -lncurses
 * 运行: ./snake
 *
 * 控制: WASD 或 方向键
 */

#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <ncurses.h>
#include <deque>
#include <vector>
#include <random>
#include <chrono>
#include <cstdlib>

// 游戏配置
constexpr int GRID_WIDTH  = 30;
constexpr int GRID_HEIGHT = 15;
constexpr int CELL_WIDTH  = 2;
constexpr int GAME_SPEED_MS = 120; // 蛇移动间隔（毫秒）

// 方向枚举
enum class Direction { UP, DOWN, LEFT, RIGHT };

// 位置
struct Position {
    int x, y;
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// 游戏状态
enum class GameState { START, PLAYING, PAUSED, GAME_OVER };

class SnakeGame {
public:
    SnakeGame();
    ~SnakeGame();

    // 禁用拷贝
    SnakeGame(const SnakeGame&) = delete;
    SnakeGame& operator=(const SnakeGame&) = delete;

    void run();

private:
    // 初始化
    void init_ncurses();
    void init_game();

    // 游戏循环
    void update();
    void render();

    // 游戏逻辑
    void move_snake();
    void spawn_food();
    bool check_collision(const Position& head) const;
    void handle_input();

    // 渲染辅助
    void draw_cell(int x, int y, chtype ch);
    void draw_border();
    void draw_score();
    void draw_game_over();
    void draw_start_screen();

    // 状态
    std::deque<Position> snake_;
    Position food_;
    Direction dir_;
    Direction next_dir_;
    int score_;
    GameState state_;
    bool food_spawned_;
    int move_counter_;    // 控制移动速度
    int speed_counter_;   // 速度等级

    // 随机数生成
    std::mt19937 rng_;
    std::uniform_int_distribution<int> dist_x_;
    std::uniform_int_distribution<int> dist_y_;

    // ncurses 窗口
    WINDOW* win_;
};

#endif // SNAKE_HPP
