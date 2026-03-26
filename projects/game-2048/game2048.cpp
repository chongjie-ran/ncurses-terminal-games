/**
 * 2048 Game - Terminal Version
 * 
 * Build: g++ -std=c++17 -Wall -Wextra -O2 -o game2048 game2048.cpp -lncurses
 * Run:   ./game2048
 * 
 * Controls: Arrow keys or WASD to move tiles
 *           'q' to quit, 'r' to restart
 */

#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <ncurses.h>

constexpr int GRID_SIZE = 4;
constexpr int WIN_VALUE = 2048;

// Tile colors (pair index, intensity based on value)
enum {
    COLOR_DEFAULT = 1,
    COLOR_2 = 2, COLOR_4 = 3, COLOR_8 = 4, COLOR_16 = 5,
    COLOR_32 = 6, COLOR_64 = 7, COLOR_128 = 8, COLOR_256 = 9,
    COLOR_512 = 10, COLOR_1024 = 11, COLOR_2048 = 12
};

struct Pos { int r, c; };

class Game2048 {
private:
    std::array<std::array<int, GRID_SIZE>, GRID_SIZE> grid_;
    std::mt19937 rng_;
    int score_ = 0;
    int best_score_ = 0;
    bool won_ = false;
    bool game_over_ = false;

public:
    Game2048() {
        rng_.seed(std::random_device{}());
        best_score_ = load_best_score();
        init_colors();
        reset();
    }

    void init_colors() {
        start_color();
        use_default_colors();
        
        init_pair(COLOR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_2, COLOR_BLACK, COLOR_WHITE);          // 2: white bg
        init_pair(COLOR_4, COLOR_BLACK, COLOR_YELLOW);         // 4: yellow
        init_pair(COLOR_8, COLOR_BLACK, COLOR_CYAN);          // 8: cyan
        init_pair(COLOR_16, COLOR_WHITE, COLOR_MAGENTA);       // 16: magenta
        init_pair(COLOR_32, COLOR_WHITE, COLOR_RED);           // 32: red
        init_pair(COLOR_64, COLOR_WHITE, COLOR_GREEN);        // 64: green
        init_pair(COLOR_128, COLOR_BLACK, COLOR_CYAN);        // 128: bright cyan
        init_pair(COLOR_256, COLOR_BLACK, COLOR_YELLOW);       // 256: bright yellow
        init_pair(COLOR_512, COLOR_BLACK, COLOR_MAGENTA);     // 512: bright magenta
        init_pair(COLOR_1024, COLOR_WHITE, COLOR_GREEN);      // 1024: bright green
        init_pair(COLOR_2048, COLOR_WHITE, COLOR_RED);        // 2048: gold/red
    }

    void reset() {
        grid_.fill({});
        score_ = 0;
        won_ = false;
        game_over_ = false;
        add_random_tile();
        add_random_tile();
    }

    void add_random_tile() {
        std::vector<Pos> empty;
        for (int r = 0; r < GRID_SIZE; ++r)
            for (int c = 0; c < GRID_SIZE; ++c)
                if (grid_[r][c] == 0)
                    empty.push_back({r, c});
        
        if (empty.empty()) return;
        
        std::uniform_int_distribution<> dist(0, empty.size() - 1);
        auto p = empty[dist(rng_)];
        // 90% chance for 2, 10% chance for 4
        grid_[p.r][p.c] = (dist(rng_) < empty.size() * 0.1) ? 4 : 2;
    }

    // Slide and merge a single row to the left
    // Returns {moved, score_gained}
    std::pair<bool, int> slide_row_left(std::array<int, GRID_SIZE>& row) {
        std::array<int, GRID_SIZE> new_row = {};
        int write_idx = 0;
        int gained = 0;
        bool moved = false;

        for (int i = 0; i < GRID_SIZE; ++i) {
            if (row[i] == 0) continue;
            
            if (write_idx > 0 && new_row[write_idx - 1] == row[i] && row[i] > 0) {
                // Merge
                new_row[write_idx - 1] *= 2;
                gained += new_row[write_idx - 1];
                write_idx--;
                moved = true;
            } else {
                new_row[write_idx++] = row[i];
                if (write_idx - 1 != i) moved = true;
            }
        }

        row = new_row;
        return {moved, gained};
    }

    bool move_left() {
        bool any_moved = false;
        int gained = 0;
        
        for (int r = 0; r < GRID_SIZE; ++r) {
            auto [moved, score] = slide_row_left(grid_[r]);
            if (moved) any_moved = true;
            gained += score;
        }
        
        if (gained > 0) {
            score_ += gained;
            best_score_ = std::max(best_score_, score_);
        }
        return any_moved;
    }

    bool move_right() {
        bool any_moved = false;
        int gained = 0;
        
        for (int r = 0; r < GRID_SIZE; ++r) {
            std::array<int, GRID_SIZE> rev = grid_[r];
            std::reverse(rev.begin(), rev.end());
            auto [moved, score] = slide_row_left(rev);
            std::reverse(rev.begin(), rev.end());
            if (moved) any_moved = true;
            gained += score;
            grid_[r] = rev;
        }
        
        if (gained > 0) {
            score_ += gained;
            best_score_ = std::max(best_score_, score_);
        }
        return any_moved;
    }

    bool move_up() {
        // Transpose, move_left, transpose back
        bool any_moved = false;
        int gained = 0;
        
        for (int c = 0; c < GRID_SIZE; ++c) {
            std::array<int, GRID_SIZE> col;
            for (int r = 0; r < GRID_SIZE; ++r) col[r] = grid_[r][c];
            auto [moved, score] = slide_row_left(col);
            if (moved) any_moved = true;
            gained += score;
            for (int r = 0; r < GRID_SIZE; ++r) grid_[r][c] = col[r];
        }
        
        if (gained > 0) {
            score_ += gained;
            best_score_ = std::max(best_score_, score_);
        }
        return any_moved;
    }

    bool move_down() {
        // Transpose, move_right, transpose back
        bool any_moved = false;
        int gained = 0;
        
        for (int c = 0; c < GRID_SIZE; ++c) {
            std::array<int, GRID_SIZE> col;
            for (int r = 0; r < GRID_SIZE; ++r) col[r] = grid_[r][c];
            std::reverse(col.begin(), col.end());
            auto [moved, score] = slide_row_left(col);
            std::reverse(col.begin(), col.end());
            if (moved) any_moved = true;
            gained += score;
            for (int r = 0; r < GRID_SIZE; ++r) grid_[r][c] = col[r];
        }
        
        if (gained > 0) {
            score_ += gained;
            best_score_ = std::max(best_score_, score_);
        }
        return any_moved;
    }

    bool can_move() const {
        // Check for empty cells
        for (int r = 0; r < GRID_SIZE; ++r)
            for (int c = 0; c < GRID_SIZE; ++c)
                if (grid_[r][c] == 0) return true;
        
        // Check for possible merges
        for (int r = 0; r < GRID_SIZE; ++r)
            for (int c = 0; c < GRID_SIZE; ++c) {
                int val = grid_[r][c];
                if (c < GRID_SIZE - 1 && grid_[r][c + 1] == val) return true;
                if (r < GRID_SIZE - 1 && grid_[r + 1][c] == val) return true;
            }
        return false;
    }

    bool check_win() const {
        for (int r = 0; r < GRID_SIZE; ++r)
            for (int c = 0; c < GRID_SIZE; ++c)
                if (grid_[r][c] >= WIN_VALUE) return true;
        return false;
    }

    void draw(WINDOW* win) const {
        int rows, cols;
        getmaxyx(win, rows, cols);
        
        int grid_start_y = 3;
        int cell_width = 9;
        int cell_height = 3;
        int padding = 1;

        // Draw title
        mvwprintw(win, 0, (cols - 14) / 2, "=== 2048 GAME ===");

        // Draw score
        mvwprintw(win, 1, 2, "SCORE: %d", score_);
        mvwprintw(win, 1, cols - 25, "BEST: %d", best_score_);

        // Draw grid
        for (int r = 0; r < GRID_SIZE; ++r) {
            for (int c = 0; c < GRID_SIZE; ++c) {
                int y = grid_start_y + r * (cell_height + padding);
                int x = 2 + c * (cell_width + padding);
                
                int val = grid_[r][c];
                
                // Choose color based on value
                int color_pair = COLOR_DEFAULT;
                if (val == 2) color_pair = COLOR_2;
                else if (val == 4) color_pair = COLOR_4;
                else if (val == 8) color_pair = COLOR_8;
                else if (val == 16) color_pair = COLOR_16;
                else if (val == 32) color_pair = COLOR_32;
                else if (val == 64) color_pair = COLOR_64;
                else if (val == 128) color_pair = COLOR_128;
                else if (val == 256) color_pair = COLOR_256;
                else if (val == 512) color_pair = COLOR_512;
                else if (val == 1024) color_pair = COLOR_1024;
                else if (val >= 2048) color_pair = COLOR_2048;

                wattron(win, COLOR_PAIR(color_pair));
                for (int dy = 0; dy < cell_height; ++dy) {
                    mvwprintw(win, y + dy, x, "%*s", cell_width, "");
                }
                
                if (val > 0) {
                    mvwprintw(win, y + cell_height / 2, x + (cell_width - std::to_string(val).size()) / 2, "%d", val);
                }
                wattroff(win, COLOR_PAIR(color_pair));
                
                // Draw border
                wattron(win, COLOR_PAIR(COLOR_DEFAULT));
                box(win, 0, 0);
            }
        }

        // Draw instructions
        mvwprintw(win, rows - 3, 2, "Controls: Arrow keys/WASD move, 'r' restart, 'q' quit");
        if (won_ && !game_over_) {
            mvwprintw(win, rows - 2, 2, "YOU WIN! Press 'c' to continue or 'q' to quit");
        } else if (game_over_) {
            mvwprintw(win, rows - 2, 2, "GAME OVER! Press 'r' to restart or 'q' to quit");
        }

        wrefresh(win);
    }

    bool is_won() const { return won_; }
    bool is_game_over() const { return game_over_; }
    void set_game_over() { game_over_ = true; }
    void set_won() { won_ = true; }

    int load_best_score() {
        // Try to load from file
        std::ifstream f("/tmp/2048_best.txt");
        int best = 0;
        if (f) f >> best;
        return best;
    }

    void save_best_score() {
        std::ofstream f("/tmp/2048_best.txt");
        f << best_score_;
    }
};

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    Game2048 game;
    
    int ch;
    while ((ch = getch()) != 'q') {
        if (ch == 'r') {
            game.reset();
        } else if (ch == KEY_UP || ch == 'w' || ch == 'W') {
            if (game.move_up()) {
                game.add_random_tile();
            }
        } else if (ch == KEY_DOWN || ch == 's' || ch == 'S') {
            if (game.move_down()) {
                game.add_random_tile();
            }
        } else if (ch == KEY_LEFT || ch == 'a' || ch == 'A') {
            if (game.move_left()) {
                game.add_random_tile();
            }
        } else if (ch == KEY_RIGHT || ch == 'd' || ch == 'D') {
            if (game.move_right()) {
                game.add_random_tile();
            }
        } else if ((ch == 'c' || ch == 'C') && game.is_won() && !game.is_game_over()) {
            // Continue playing after win
        }

        // Check win condition
        if (!game.is_won() && game.check_win()) {
            game.set_won();
        }

        // Check game over
        if (!game.can_move() && !game.is_won()) {
            game.set_game_over();
        }

        game.draw(stdscr);
    }

    game.save_best_score();
    endwin();
    return 0;
}
