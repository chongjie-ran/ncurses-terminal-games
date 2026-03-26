/**
 * Snake Game - Terminal Edition
 * 使用 ncurses 编写的贪吃蛇游戏
 */

#include "snake.hpp"
#include <unistd.h>
#include <algorithm>

SnakeGame::SnakeGame()
    : snake_()
    , food_()
    , dir_(Direction::RIGHT)
    , next_dir_(Direction::RIGHT)
    , score_(0)
    , state_(GameState::START)
    , food_spawned_(false)
    , move_counter_(0)
    , speed_counter_(1)
    , rng_(std::chrono::steady_clock::now().time_since_epoch().count())
    , dist_x_(1, GRID_WIDTH)
    , dist_y_(1, GRID_HEIGHT)
    , win_(nullptr)
{
    init_ncurses();
}

SnakeGame::~SnakeGame() {
    if (win_) delwin(win_);
    endwin();
}

void SnakeGame::init_ncurses() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    timeout(GAME_SPEED_MS);
    refresh();
}

void SnakeGame::init_game() {
    // 初始化蛇在中间位置
    snake_.clear();
    int start_x = GRID_WIDTH / 2;
    int start_y = GRID_HEIGHT / 2;
    snake_.push_back({start_x, start_y});       // 头
    snake_.push_back({start_x - 1, start_y});   // 身体
    snake_.push_back({start_x - 2, start_y});   // 尾巴

    dir_ = Direction::RIGHT;
    next_dir_ = Direction::RIGHT;
    score_ = 0;
    move_counter_ = 0;
    speed_counter_ = 1;
    food_spawned_ = false;
    spawn_food();
}

void SnakeGame::run() {
    while (state_ != GameState::GAME_OVER) {
        handle_input();

        if (state_ == GameState::PLAYING) {
            update();
        }

        render();
    }

    // 显示最终画面
    draw_game_over();
    refresh();
    napms(1500);
}

// ---------------------------------------------------------------------------
// 键盘输入处理
// ---------------------------------------------------------------------------
void SnakeGame::handle_input() {
    int ch = getch();
    if (ch == ERR) return;

    switch (ch) {
        case 'q':
        case 'Q':
        case 27: // ESC
            state_ = GameState::GAME_OVER;
            break;

        case ' ':
        case 'p':
        case 'P':
            if (state_ == GameState::PLAYING) {
                state_ = GameState::PAUSED;
            } else if (state_ == GameState::PAUSED) {
                state_ = GameState::PLAYING;
            }
            break;

        case 'r':
        case 'R':
            if (state_ == GameState::GAME_OVER) {
                init_game();
                state_ = GameState::PLAYING;
            }
            break;

        case KEY_UP:
        case 'w':
        case 'W':
            if (dir_ != Direction::DOWN)  next_dir_ = Direction::UP;
            break;
        case KEY_DOWN:
        case 's':
        case 'S':
            if (dir_ != Direction::UP)    next_dir_ = Direction::DOWN;
            break;
        case KEY_LEFT:
        case 'a':
        case 'A':
            if (dir_ != Direction::RIGHT) next_dir_ = Direction::LEFT;
            break;
        case KEY_RIGHT:
        case 'd':
        case 'D':
            if (dir_ != Direction::LEFT)  next_dir_ = Direction::RIGHT;
            break;

        // 开始/重新开始
        case '\n':
        case KEY_ENTER:
            if (state_ == GameState::START) {
                init_game();
                state_ = GameState::PLAYING;
            }
            break;
    }
}

// ---------------------------------------------------------------------------
// 游戏逻辑更新
// ---------------------------------------------------------------------------
void SnakeGame::update() {
    dir_ = next_dir_;
    move_snake();
}

// 蛇移动一步
void SnakeGame::move_snake() {
    const Position& head = snake_.front();
    Position new_head = head;

    switch (dir_) {
        case Direction::UP:    --new_head.y; break;
        case Direction::DOWN:  ++new_head.y; break;
        case Direction::LEFT:  --new_head.x; break;
        case Direction::RIGHT: ++new_head.x; break;
    }

    // 撞墙检测
    if (new_head.x < 1 || new_head.x > GRID_WIDTH ||
        new_head.y < 1 || new_head.y > GRID_HEIGHT) {
        state_ = GameState::GAME_OVER;
        return;
    }

    // 撞自己检测（排除尾巴，因为尾巴会移动走）
    for (size_t i = 0; i + 1 < snake_.size(); ++i) {
        if (snake_[i] == new_head) {
            state_ = GameState::GAME_OVER;
            return;
        }
    }

    // 插入新头部
    snake_.push_front(new_head);

    // 检查是否吃到食物
    if (new_head == food_) {
        score_ += 10;
        food_spawned_ = false;
        spawn_food();

        // 每50分加速
        if (score_ % 50 == 0 && speed_counter_ < 5) {
            ++speed_counter_;
        }
    } else {
        // 没吃到食物，移除尾巴（蛇移动）
        snake_.pop_back();
    }
}

// 随机生成食物
void SnakeGame::spawn_food() {
    Position pos;
    int attempts = 0;
    do {
        pos = {dist_x_(rng_), dist_y_(rng_)};
        ++attempts;
    } while (
        (std::find(snake_.begin(), snake_.end(), pos) != snake_.end())
        && attempts < 1000
    );
    food_ = pos;
    food_spawned_ = true;
}

// 碰撞检测（扩展用）
bool SnakeGame::check_collision(const Position& pos) const {
    if (pos.x < 1 || pos.x > GRID_WIDTH ||
        pos.y < 1 || pos.y > GRID_HEIGHT) {
        return true;
    }
    return std::find(snake_.begin(), snake_.end(), pos) != snake_.end();
}

// ---------------------------------------------------------------------------
// 渲染
// ---------------------------------------------------------------------------
void SnakeGame::render() {
    erase();

    if (state_ == GameState::START) {
        draw_start_screen();
        return;
    }

    draw_border();
    draw_score();

    if (state_ == GameState::PAUSED) {
        mvprintw(GRID_HEIGHT / 2, GRID_WIDTH - 5, "PAUSED");
        return;
    }

    // 绘制蛇
    for (size_t i = 0; i < snake_.size(); ++i) {
        const auto& p = snake_[i];
        chtype ch = (i == 0) ? '*' : 'o'; // 头用 *，身体用 o
        draw_cell(p.x, p.y, ch);
    }

    // 绘制食物
    if (food_spawned_) {
        draw_cell(food_.x, food_.y, '@'); // 食物用 @
    }
}

void SnakeGame::draw_cell(int x, int y, chtype ch) {
    int sx = x * CELL_WIDTH;
    int sy = y;
    for (int w = 0; w < CELL_WIDTH; ++w) {
        mvaddch(sy, sx + w, ch);
    }
}

void SnakeGame::draw_border() {
    // 顶部和底部边框
    for (int x = 0; x <= (GRID_WIDTH + 1) * CELL_WIDTH; ++x) {
        mvaddch(0, x, '#');
        mvaddch(GRID_HEIGHT + 1, x, '#');
    }
    // 左右边框
    for (int y = 0; y <= GRID_HEIGHT + 1; ++y) {
        mvaddch(y, 0, '#');
        mvaddch(y, (GRID_WIDTH + 1) * CELL_WIDTH - 1, '#');
    }
}

void SnakeGame::draw_score() {
    mvprintw(0, 2, "SCORE: %d", score_);
}

void SnakeGame::draw_start_screen() {
    int cols = (GRID_WIDTH + 1) * CELL_WIDTH;
    int rows = GRID_HEIGHT + 2;

    int mid_x = cols / 2;
    int mid_y = rows / 2;

    mvprintw(mid_y - 4, mid_x - 8, "===== SNAKE =====");
    mvprintw(mid_y - 2, mid_x - 15, "Use WASD or Arrow Keys to move");
    mvprintw(mid_y,     mid_x - 12, "SPACE/P to pause");
    mvprintw(mid_y + 2, mid_x - 13, "Press ENTER to start");
    mvprintw(mid_y + 4, mid_x - 7,  "Q/ESC to quit");
    refresh();
}

void SnakeGame::draw_game_over() {
    int cols = (GRID_WIDTH + 1) * CELL_WIDTH;
    int mid_x = cols / 2;
    int mid_y = (GRID_HEIGHT + 2) / 2;

    mvprintw(mid_y - 2, mid_x - 7, "GAME OVER!");
    mvprintw(mid_y,     mid_x - 9, "Score: %d", score_);
    mvprintw(mid_y + 2, mid_x - 12, "Press R to restart");
    mvprintw(mid_y + 4, mid_x - 7, "Q to quit");
    refresh();
}

// ---------------------------------------------------------------------------
// 入口
// ---------------------------------------------------------------------------
int main() {
    SnakeGame game;
    game.run();
    return 0;
}
