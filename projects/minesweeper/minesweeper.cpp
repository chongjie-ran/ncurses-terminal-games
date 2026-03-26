/**
 * 扫雷游戏 - Terminal Version
 * 
 * 编译: g++ -std=c++17 -Wall -Wextra -O2 -o minesweeper minesweeper.cpp -lncurses
 * 运行: ./minesweeper
 * 
 * 操作:
 *   方向键 / WASD  移动光标
 *   Enter / 空格    翻开格子
 *   F              标记/取消标记地雷
 *   R              重新开始
 *   Q              退出
 */

#include "minesweeper.hpp"
#include <ncurses.h>
#include <algorithm>
#include <vector>
#include <random>

Minesweeper::Minesweeper() : rng_(std::random_device{}()) {
    reset();
}

void Minesweeper::reset() {
    for (auto& row : grid_) {
        for (auto& cell : row) {
            cell = Cell{};
        }
    }
    game_over_ = false;
    game_win_ = false;
    revealed_count_ = 0;
    flagged_count_ = 0;
}

void Minesweeper::place_mines(int exclude_r, int exclude_c) {
    std::vector<std::pair<int,int>> candidates;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            // 第一次点击及其周围不放雷，保障玩家体验
            if (std::abs(r - exclude_r) <= 1 && std::abs(c - exclude_c) <= 1) continue;
            candidates.emplace_back(r, c);
        }
    }
    std::shuffle(candidates.begin(), candidates.end(), rng_);
    int to_place = std::min(MINES, static_cast<int>(candidates.size()));
    for (int i = 0; i < to_place; ++i) {
        auto [r, c] = candidates[i];
        grid_[r][c].is_mine = true;
        grid_[r][c].value = CellValue::MINE;
    }
}

void Minesweeper::compute_numbers() {
    const int dr[8] = {-1,-1,-1,0,0,1,1,1};
    const int dc[8] = {-1,0,1,-1,1,-1,0,1};
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (grid_[r][c].is_mine) continue;
            int cnt = 0;
            for (int k = 0; k < 8; ++k) {
                int nr = r + dr[k], nc = c + dc[k];
                if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && grid_[nr][nc].is_mine) {
                    ++cnt;
                }
            }
            grid_[r][c].value = static_cast<CellValue>(cnt);
        }
    }
}

void Minesweeper::flood_fill(int r, int c) {
    // BFS flood fill: 展开空白格子及相邻数字格子
    std::vector<std::pair<int,int>> q;
    q.emplace_back(r, c);
    std::vector<std::vector<bool>> visited(ROWS, std::vector<bool>(COLS, false));
    visited[r][c] = true;

    const int dr[8] = {-1,-1,-1,0,0,1,1,1};
    const int dc[8] = {-1,0,1,-1,1,-1,0,1};

    size_t head = 0;
    while (head < q.size()) {
        auto [cr, cc] = q[head++];
        if (grid_[cr][cc].state == CellState::REVEALED) continue;
        grid_[cr][cc].state = CellState::REVEALED;
        ++revealed_count_;

        // 如果是数字格（>0）或地雷，不再继续扩展
        if (grid_[cr][cc].value != CellValue::EMPTY) continue;

        // 空白格，向8个方向扩展
        for (int k = 0; k < 8; ++k) {
            int nr = cr + dr[k], nc = cc + dc[k];
            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && !visited[nr][nc]) {
                visited[nr][nc] = true;
                if (!grid_[nr][nc].is_mine && grid_[nr][nc].state != CellState::REVEALED) {
                    q.emplace_back(nr, nc);
                }
            }
        }
    }
}

bool Minesweeper::reveal(int r, int c) {
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return false;
    if (grid_[r][c].state == CellState::REVEALED) return false;
    if (grid_[r][c].state == CellState::FLAGGED) return false;

    // 第一次点击：放置地雷
    if (revealed_count_ == 0) {
        place_mines(r, c);
        compute_numbers();
    }

    if (grid_[r][c].is_mine) {
        grid_[r][c].state = CellState::REVEALED;
        game_over_ = true;
        return false;
    }

    flood_fill(r, c);

    // 判断胜利：所有非地雷格子都已翻开
    if (revealed_count_ == ROWS * COLS - MINES) {
        game_over_ = true;
        game_win_ = true;
    }
    return true;
}

void Minesweeper::toggle_flag(int r, int c) {
    if (r < 0 || r >= ROWS || c >= COLS) return;
    if (grid_[r][c].state == CellState::REVEALED) return;
    if (grid_[r][c].state == CellState::FLAGGED) {
        grid_[r][c].state = CellState::HIDDEN;
        --flagged_count_;
    } else {
        grid_[r][c].state = CellState::FLAGGED;
        ++flagged_count_;
    }
}

bool Minesweeper::is_win() const {
    return game_win_;
}

// ============================ UI 绘制 ============================

// ncurses 颜色对
#define CLR_BG        1
#define CLR_HIDDEN    2
#define CLR_REVEALED  3
#define CLR_MINE      4
#define CLR_FLAG      5
#define CLR_CURSOR    6
#define CLR_N1        7
#define CLR_N2        8
#define CLR_N3        9
#define CLR_N4        10
#define CLR_N5        11
#define CLR_N6        12
#define CLR_N7        13
#define CLR_N8        14

void init_colors() {
    start_color();
    use_default_colors();
    init_pair(CLR_BG, COLOR_WHITE, COLOR_CYAN);
    init_pair(CLR_HIDDEN, COLOR_WHITE, COLOR_CYAN);
    init_pair(CLR_REVEALED, COLOR_BLACK, COLOR_WHITE);
    init_pair(CLR_MINE, COLOR_WHITE, COLOR_RED);
    init_pair(CLR_FLAG, COLOR_RED, COLOR_CYAN);
    init_pair(CLR_CURSOR, COLOR_BLACK, COLOR_YELLOW);
    init_pair(CLR_N1, COLOR_BLUE, COLOR_WHITE);
    init_pair(CLR_N2, COLOR_GREEN, COLOR_WHITE);
    init_pair(CLR_N3, COLOR_RED, COLOR_WHITE);
    init_pair(CLR_N4, COLOR_BLUE, COLOR_WHITE);
    init_pair(CLR_N5, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(CLR_N6, COLOR_CYAN, COLOR_WHITE);
    init_pair(CLR_N7, COLOR_BLACK, COLOR_WHITE);
    init_pair(CLR_N8, COLOR_WHITE, COLOR_WHITE);
}

char cell_digit_char(CellValue v) {
    switch (v) {
        case CellValue::EMPTY:  return ' ';
        case CellValue::MINE:   return '*';
        case CellValue::N1:     return '1';
        case CellValue::N2:     return '2';
        case CellValue::N3:     return '3';
        case CellValue::N4:     return '4';
        case CellValue::N5:     return '5';
        case CellValue::N6:     return '6';
        case CellValue::N7:     return '7';
        case CellValue::N8:     return '8';
        default:                return '?';
    }
}

int cell_color_pair(CellValue v) {
    switch (v) {
        case CellValue::N1: return CLR_N1;
        case CellValue::N2: return CLR_N2;
        case CellValue::N3: return CLR_N3;
        case CellValue::N4: return CLR_N4;
        case CellValue::N5: return CLR_N5;
        case CellValue::N6: return CLR_N6;
        case CellValue::N7: return CLR_N7;
        case CellValue::N8: return CLR_N8;
        default: return CLR_REVEALED;
    }
}

int calc_mine_color(const Cell& cell) {
    if (!cell.is_mine) return CLR_MINE;
    if (cell.state == CellState::FLAGGED) return CLR_FLAG;
    return CLR_MINE;
}

void draw_grid(WINDOW* win, const Minesweeper& game, int cursor_r, int cursor_c, int /*offset_y*/, int /*offset_x*/) {
    werase(win);
    box(win, 0, 0);

    for (int r = 0; r < Minesweeper::ROWS; ++r) {
        for (int c = 0; c < Minesweeper::COLS; ++c) {
            const Cell& cell = game.cell(r, c);
            bool is_cursor = (r == cursor_r && c == cursor_c);
            int y = r + 1;
            int x = c * 2 + 1;

            if (is_cursor) {
                wattron(win, COLOR_PAIR(CLR_CURSOR));
                mvwprintw(win, y, x, "  ");
                wattroff(win, COLOR_PAIR(CLR_CURSOR));
                continue;
            }

            if (game.is_game_over()) {
                // 游戏结束，显示所有格子
                if (cell.is_mine) {
                    wattron(win, COLOR_PAIR(CLR_MINE) | A_BOLD);
                    mvwprintw(win, y, x, " *");
                    wattroff(win, COLOR_PAIR(CLR_MINE) | A_BOLD);
                } else if (cell.value != CellValue::EMPTY) {
                    wattron(win, COLOR_PAIR(cell_color_pair(cell.value)));
                    mvwprintw(win, y, x, " %c", cell_digit_char(cell.value));
                    wattroff(win, COLOR_PAIR(cell_color_pair(cell.value)));
                } else {
                    mvwprintw(win, y, x, "  ");
                }
            } else {
                if (cell.state == CellState::FLAGGED) {
                    wattron(win, COLOR_PAIR(CLR_FLAG) | A_BOLD);
                    mvwprintw(win, y, x, " F");
                    wattroff(win, COLOR_PAIR(CLR_FLAG) | A_BOLD);
                } else if (cell.state == CellState::REVEALED) {
                    if (cell.value == CellValue::EMPTY) {
                        wattron(win, COLOR_PAIR(CLR_REVEALED));
                        mvwprintw(win, y, x, "  ");
                        wattroff(win, COLOR_PAIR(CLR_REVEALED));
                    } else if (cell.value == CellValue::MINE) {
                        wattron(win, COLOR_PAIR(CLR_MINE) | A_BOLD);
                        mvwprintw(win, y, x, " *");
                        wattroff(win, COLOR_PAIR(CLR_MINE) | A_BOLD);
                    } else {
                        wattron(win, COLOR_PAIR(cell_color_pair(cell.value)));
                        mvwprintw(win, y, x, " %c", cell_digit_char(cell.value));
                        wattroff(win, COLOR_PAIR(cell_color_pair(cell.value)));
                    }
                } else {
                    wattron(win, COLOR_PAIR(CLR_HIDDEN));
                    mvwprintw(win, y, x, " .");
                    wattroff(win, COLOR_PAIR(CLR_HIDDEN));
                }
            }
        }
    }
    wrefresh(win);
}

void draw_status(WINDOW* win, const Minesweeper& game, int cursor_r, int cursor_c) {
    werase(win);
    int mine_remaining = Minesweeper::MINES;
    for (int r = 0; r < Minesweeper::ROWS; ++r) {
        for (int c = 0; c < Minesweeper::COLS; ++c) {
            if (game.cell(r, c).state == CellState::FLAGGED) --mine_remaining;
        }
    }
    mvwprintw(win, 0, 1, "Mines: %3d/%d  |  Cursor: (%2d,%2d)  |  WASD/Arrows: Move  F: Flag  Enter: Reveal  R: Restart  Q: Quit",
              mine_remaining, Minesweeper::MINES, cursor_r, cursor_c);
    wrefresh(win);
}

void draw_msg(WINDOW* win, const char* msg) {
    werase(win);
    wattron(win, A_BOLD);
    mvwprintw(win, 0, 2, "%s", msg);
    wattroff(win, A_BOLD);
    wrefresh(win);
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    init_colors();
    refresh();

    int grid_h = Minesweeper::ROWS + 2;
    int grid_w = Minesweeper::COLS * 2 + 2;
    int status_h = 1;
    int msg_h = 1;

    int total_h = grid_h + status_h + msg_h;
    int total_w = std::max(grid_w, 80);
    int start_y = (LINES - total_h) / 2;
    int start_x = (COLS - total_w) / 2;

    WINDOW* msg_win = newwin(msg_h, COLS, start_y, 0);
    WINDOW* grid_win = newwin(grid_h, grid_w, start_y + msg_h, start_x);
    WINDOW* status_win = newwin(status_h, COLS, start_y + msg_h + grid_h, 0);

    scrollok(msg_win, TRUE);

    Minesweeper game;
    int cursor_r = Minesweeper::ROWS / 2;
    int cursor_c = Minesweeper::COLS / 2;

    bool quit = false;
    while (!quit) {
        draw_msg(msg_win, "扫雷 | Minesweeper  [16x30, 99 mines]");
        draw_grid(grid_win, game, cursor_r, cursor_c, start_y, start_x);
        draw_status(status_win, game, cursor_r, cursor_c);

        int ch = getch();
        switch (ch) {
            case KEY_UP:    case 'w': case 'W': cursor_r = std::max(0, cursor_r - 1); break;
            case KEY_DOWN:  case 's': case 'S': cursor_r = std::min(Minesweeper::ROWS - 1, cursor_r + 1); break;
            case KEY_LEFT:  case 'a': case 'A': cursor_c = std::max(0, cursor_c - 1); break;
            case KEY_RIGHT: case 'd': case 'D': cursor_c = std::min(Minesweeper::COLS - 1, cursor_c + 1); break;
            case 'f': case 'F':
                if (!game.is_game_over()) game.toggle_flag(cursor_r, cursor_c);
                break;
            case '\n': case KEY_ENTER: case ' ':
                if (!game.is_game_over()) {
                    game.reveal(cursor_r, cursor_c);
                }
                break;
            case 'r': case 'R':
                game.reset();
                cursor_r = Minesweeper::ROWS / 2;
                cursor_c = Minesweeper::COLS / 2;
                break;
            case 'q': case 'Q':
                quit = true;
                break;
        }

        if (game.is_game_over()) {
            if (game.is_win()) {
                mvwprintw(msg_win, 0, 2, "🎉 胜利！所有地雷已标记！按 R 重新开始 或 Q 退出");
            } else {
                mvwprintw(msg_win, 0, 2, "💥 游戏结束！你踩到了地雷！按 R 重新开始 或 Q 退出");
            }
            wrefresh(msg_win);
            // 绘制最终状态
            draw_grid(grid_win, game, -1, -1, start_y, start_x);
        }
    }

    delwin(msg_win);
    delwin(grid_win);
    delwin(status_win);
    endwin();
    return 0;
}
