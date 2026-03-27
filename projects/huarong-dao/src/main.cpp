#include "board.h"
#include "solver.h"
#include <ncurses.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Display names for each piece type (unique → no ambiguity)
static char PIECE_CHAR(int type) {
    switch(type) {
        case Board::CAO:   return 'C';
        case Board::ZHANG: return 'Z';
        case Board::ZHAO:  return 'Y';
        case Board::HUANG: return 'H';
        case Board::WEI:   return 'W';
        case Board::MA:    return 'M';
        case Board::BING1: return '1';
        case Board::BING2: return '2';
        default:          return ' ';
    }
}

class Game {
public:
    Board board;
    int selected;
    int move_count;
    bool solved;

    Game(): selected(-1), move_count(0), solved(false) {
        board = Board::initial();
    }

    bool tryMove(int dr, int dc) {
        if (selected < 0) return false;
        const auto& b = board.blocks[selected];
        int nr = b.r + dr, nc = b.c + dc;
        auto cells = board.cells();
        for (int r = nr; r < nr + b.h; ++r)
            for (int c = nc; c < nc + b.w; ++c)
                if (r < 0 || r >= Board::ROWS || c < 0 || c >= Board::COLS
                    || cells[r * Board::COLS + c] != Board::EMPTY)
                    return false;
        board.blocks[selected].r = nr;
        board.blocks[selected].c = nc;
        move_count++;
        if (board.isGoal()) solved = true;
        return true;
    }

    void reset() {
        board = Board::initial();
        selected = -1;
        move_count = 0;
        solved = false;
    }
};

void drawBoard(WINDOW* win, const Board& board, int selIdx) {
    werase(win);
    box(win, 0, 0);

    auto cells = board.cells();
    int by = 2, bx = 2;  // board origin inside window

    for (int r = 0; r < Board::ROWS; ++r) {
        for (int c = 0; c < Board::COLS; ++c) {
            int val = cells[r * Board::COLS + c];
            bool sel = false;
            if (selIdx >= 0) {
                const auto& b = board.blocks[selIdx];
                sel = (r >= b.r && r < b.r + b.h && c >= b.c && c < b.c + b.w);
            }
            int pair = (val > 0) ? (val % 7 + 1) : 0;
            if (sel) wattron(win, A_REVERSE);
            if (pair > 0) wattron(win, COLOR_PAIR(pair));
            mvwprintw(win, by + r, bx + c * 4, " %c ", PIECE_CHAR(val));
            if (pair > 0) wattroff(win, COLOR_PAIR(pair));
            if (sel) wattroff(win, A_REVERSE);
        }
    }

    // Exit marker at bottom center
    wattron(win, COLOR_PAIR(8));
    mvwprintw(win, by + 4, bx + 1 * 4, " V ");
    mvwprintw(win, by + 4, bx + 2 * 4, " V ");
    wattroff(win, COLOR_PAIR(8));

    wrefresh(win);
}

void drawStatus(WINDOW* win, const Game& g) {
    werase(win);
    if (g.solved)
        mvwprintw(win, 0, 1, "[华容道] 步数: %d 🎉 成功!", g.move_count);
    else
        mvwprintw(win, 0, 1, "[华容道] 步数: %d", g.move_count);
    if (g.selected >= 0) {
        const auto& b = g.board.blocks[g.selected];
        mvwprintw(win, 1, 1, "选中: %c (%d×%d) at (%d,%d)",
                  PIECE_CHAR(b.type), b.h, b.w, b.r, b.c);
    }
    wrefresh(win);
}

void drawHelp(WINDOW* win) {
    werase(win);
    mvwprintw(win, 0, 1, "方向键: 移动棋子 | 空格: 选择棋子 | R: 重置");
    mvwprintw(win, 1, 1, "S: 自动求解 | Q: 退出");
    wrefresh(win);
}

int main() {
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE);
    curs_set(0);
    start_color(); use_default_colors();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(8, COLOR_GREEN, COLOR_BLACK);

    Game game;

    int maxY, maxX; getmaxyx(stdscr, maxY, maxX);
    int bh = Board::ROWS * 1 + 4;
    int bw = Board::COLS * 4 + 4;
    int by = (maxY - bh - 6) / 2;
    int bx = (maxX - bw) / 2;

    WINDOW* boardWin = newwin(bh, bw, by, bx);
    WINDOW* statusWin = newwin(3, bw, by + bh, bx);
    WINDOW* helpWin = newwin(3, bw, by + bh + 3, bx);

    drawBoard(boardWin, game.board, game.selected);
    drawStatus(statusWin, game);
    drawHelp(helpWin);

    while (true) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') break;
        if (ch == 'r' || ch == 'R') {
            game.reset();
            drawBoard(boardWin, game.board, game.selected);
            drawStatus(statusWin, game);
            continue;
        }
        if (ch == 's' || ch == 'S') {
            Solver solver;
            auto res = solver.solve(game.board);
            if (res.moves > 0) {
                mvprintw(0, 0, "求解完成: %d 步! 按任意键继续...", res.moves);
                refresh();
                getch();
            } else if (res.moves == 0) {
                mvprintw(0, 0, "已达目标状态!");
                refresh(); getch();
            } else {
                mvprintw(0, 0, "当前布局无解 (或布局异常)");
                refresh(); getch();
            }
            drawBoard(boardWin, game.board, game.selected);
            drawStatus(statusWin, game);
            continue;
        }
        if (ch == ' ') {
            // Find piece at center of board and select it
            if (game.selected >= 0) { game.selected = -1; }
            else {
                // Auto-select CAO for ease
                for (size_t i = 0; i < game.board.blocks.size(); ++i)
                    if (game.board.blocks[i].type == Board::CAO)
                        { game.selected = (int)i; break; }
            }
            drawBoard(boardWin, game.board, game.selected);
            drawStatus(statusWin, game);
            continue;
        }

        int dr = 0, dc = 0;
        if (ch == KEY_UP) dr = -1;
        else if (ch == KEY_DOWN) dr = 1;
        else if (ch == KEY_LEFT) dc = -1;
        else if (ch == KEY_RIGHT) dc = 1;

        if ((dr || dc) && game.selected >= 0) {
            if (game.tryMove(dr, dc)) {
                drawBoard(boardWin, game.board, game.selected);
                drawStatus(statusWin, game);
                if (game.solved) {
                    mvprintw(0, 0, "🎉 恭喜! 用 %d 步完成华容道!", game.move_count);
                    refresh(); getch();
                    mvprintw(0, 0, "                    ");
                    refresh();
                }
            }
        }
    }

    delwin(boardWin); delwin(statusWin); delwin(helpWin);
    endwin();
    return 0;
}
