#include "game.h"
#include <ncurses.h>
#include <string>
#include <unistd.h>
#include <sys/time.h>
#include <cstring>

// Color indices: 0=black(empty), 1=cyan(I), 2=yellow(O), 3=purple(T),
// 4=green(S), 5=red(Z), 6=blue(J), 7=orange(L)
static const short COLOR_MAP[8] = {
    COLOR_BLACK, COLOR_CYAN, COLOR_YELLOW, COLOR_MAGENTA,
    COLOR_GREEN, COLOR_RED, COLOR_BLUE, COLOR_YELLOW
};

// Characters for filled cells
static const char* BLOCK_CHARS = "  ";

static int tetris_color_pair(int idx) {
    return idx + 1;
}

static void init_colors() {
    if (has_colors()) {
        start_color();
        use_default_colors();
        for (int i = 1; i <= 7; ++i) {
            init_pair(i, COLOR_MAP[i], COLOR_BLACK);
        }
    }
}

static void draw_block(WINDOW* win, int row, int col, uint32_t color_idx) {
    if (color_idx == 0) {
        wattron(win, COLOR_PAIR(0));
        mvwaddstr(win, row, col, "  ");
        wattroff(win, COLOR_PAIR(0));
    } else {
        wattron(win, COLOR_PAIR(tetris_color_pair(color_idx)));
        mvwaddstr(win, row, col, "██");
        wattroff(win, COLOR_PAIR(tetris_color_pair(color_idx)));
    }
}

static void draw_board(WINDOW* win, const Board& board, int start_row, int start_col) {
    for (int r = 0; r < Board::HEIGHT; ++r) {
        for (int c = 0; c < Board::WIDTH; ++c) {
            draw_block(win, start_row + r, start_col + c*2, board.cell(r, c));
        }
    }
    // Border
    wattron(win, A_BOLD);
    mvwhline(win, start_row - 1, start_col, 0, Board::WIDTH * 2);
    mvwhline(win, start_row + Board::HEIGHT, start_col, 0, Board::WIDTH * 2);
    mvwvline(win, start_row, start_col - 2, 0, Board::HEIGHT);
    mvwvline(win, start_row, start_col + Board::WIDTH * 2, 0, Board::HEIGHT);
    wattroff(win, A_BOLD);
}

static void draw_ghost(WINDOW* win, const Piece& piece, int px, int py,
                        const Board& board, int start_row, int start_col) {
    // Find ghost position (drop to lowest valid row)
    int ghost_y = py;
    while (true) {
        auto cells = piece.getCells(px, ghost_y + 1);
        if (board.collides(cells)) break;
        ++ghost_y;
    }
    if (ghost_y == py) return;  // no ghost needed

    auto cells = piece.getCells(px, ghost_y);
    for (const auto& [r, c] : cells) {
        if (r >= 0 && r < Board::HEIGHT && c >= 0 && c < Board::WIDTH) {
            int yr = start_row + r;
            int xc = start_col + c * 2;
            wattron(win, A_DIM | COLOR_PAIR(tetris_color_pair(piece.getColor())));
            mvwaddstr(win, yr, xc, "░░");
            wattroff(win, A_DIM | COLOR_PAIR(tetris_color_pair(piece.getColor())));
        }
    }
}

static void draw_piece_on_board(WINDOW* win, const Piece& piece, int px, int py,
                                 const Board& board, int start_row, int start_col) {
    auto cells = piece.getCells(px, py);
    for (const auto& [r, c] : cells) {
        if (r >= 0 && r < Board::HEIGHT && c >= 0 && c < Board::WIDTH) {
            draw_block(win, start_row + r, start_col + c*2, piece.getColor());
        }
    }
}

static void draw_next_piece(WINDOW* win, const Piece& next, int start_row, int start_col) {
    mvwaddstr(win, start_row - 1, start_col, "NEXT:");
    uint16_t shape = next.getShape();
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            int bit = (shape >> (15 - (r*4 + c))) & 1;
            if (bit) {
                wattron(win, COLOR_PAIR(tetris_color_pair(next.getColor())));
                mvwaddstr(win, start_row + r, start_col + c*2, "██");
                wattroff(win, COLOR_PAIR(tetris_color_pair(next.getColor())));
            }
        }
    }
}

static void draw_info(WINDOW* win, const Game& game, int start_row) {
    mvwaddstr(win, start_row + 0,  1, "SCORE:");
    mvwprintw(win, start_row + 0, 8, "%06d", game.getScore());
    mvwaddstr(win, start_row + 2,  1, "LEVEL:");
    mvwprintw(win, start_row + 2, 8, "%d", game.getLevel());
    mvwaddstr(win, start_row + 4,  1, "LINES:");
    mvwprintw(win, start_row + 4, 8, "%d", game.getLines());
    mvwaddstr(win, start_row + 6, 1, "[P]ause");
    mvwaddstr(win, start_row + 7, 1, "[R]estart");
    mvwaddstr(win, start_row + 8, 1, "[Q]uit");
}

static void draw_controls(WINDOW* win, int row, int col) {
    mvwaddstr(win, row, col, "←→ Move  ↑ Rotate  ↓ Soft  Space HardDrop  Z CCW");
}

static void draw_game_over(WINDOW* win, int rows, int cols, const Game& game) {
    int msg_rows = 7;
    int msg_cols = 30;
    int y = (rows - msg_rows) / 2 - 1;
    int x = (cols - msg_cols) / 2;
    WINDOW* popup = newwin(msg_rows, msg_cols, y, x);
    box(popup, 0, 0);
    mvwaddstr(popup, 1, 4, "  === GAME OVER ===  ");
    mvwprintw(popup, 3, 4, "Score: %d", game.getScore());
    mvwprintw(popup, 4, 4, "Lines: %d", game.getLines());
    mvwaddstr(popup, 5, 4, "Press [R] to restart");
    wrefresh(popup);
    delwin(popup);
}

static void draw_paused(WINDOW* win, int rows, int cols) {
    int msg_rows = 4;
    int msg_cols = 20;
    int y = (rows - msg_rows) / 2 - 1;
    int x = (cols - msg_cols) / 2;
    WINDOW* popup = newwin(msg_rows, msg_cols, y, x);
    box(popup, 0, 0);
    mvwaddstr(popup, 1, 3, "  === PAUSED ===  ");
    mvwaddstr(popup, 2, 3, "Press [P] to resume");
    wrefresh(popup);
    delwin(popup);
}

static long get_elapsed_ms() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
    initscr();
    curs_set(0);
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    init_colors();

    Game game;
    game.start();

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // Layout: board left-center, info right
    int board_h = Board::HEIGHT;
    int board_w = Board::WIDTH * 2 + 4;
    int board_start_row = (rows - board_h) / 2;
    int board_start_col = (cols - board_w) / 2 - 10;

    if (board_start_col < 2) board_start_col = 2;
    if (board_start_row < 1) board_start_row = 1;

    int info_col = board_start_col + board_w + 2;
    int info_row = board_start_row;
    int next_row = info_row + 9;

    int ch;
    long last_update = get_elapsed_ms();
    const int TICK_MS = 50;  // 20 fps render

    while (true) {
        long now = get_elapsed_ms();
        ch = getch();
        if (ch != ERR) {
            if (ch == 'q' || ch == 'Q') break;
            game.handleInput(ch);
        }

        // Tick at regular interval (gravity)
        if (game.getState() == GameState::PLAYING) {
            long elapsed = now - last_update;
            int drop_interval = std::max(50, 800 - (game.getLevel() - 1) * 40);
            if (elapsed >= drop_interval) {
                game.tick();
                last_update = now;
            }
        }

        if (now - last_update >= TICK_MS) {
            // Render
            erase();
            mvaddstr(0, (cols - 20) / 2, "TETRIS - ncurses terminal");
            draw_controls(stdscr, rows - 1, 2);

            draw_board(stdscr, game.getBoardRef(), board_start_row, board_start_col);

            if (game.getState() == GameState::PLAYING || game.getState() == GameState::PAUSED) {
                const Piece& cur = game.getCurrentPiece();
                int px = game.getCurrentX();
                int py = game.getCurrentY();
                draw_ghost(stdscr, cur, px, py, game.getBoardRef(), board_start_row, board_start_col);
                draw_piece_on_board(stdscr, cur, px, py, game.getBoardRef(), board_start_row, board_start_col);
            }

            draw_next_piece(stdscr, game.getNextPiece(), next_row, info_col);
            draw_info(stdscr, game, info_row);

            if (game.getState() == GameState::GAME_OVER)
                draw_game_over(stdscr, rows, cols, game);
            else if (game.getState() == GameState::PAUSED)
                draw_paused(stdscr, rows, cols);

            refresh();
            last_update = now;
        } else {
            usleep(5000);
        }
    }

    endwin();
    return 0;
}
