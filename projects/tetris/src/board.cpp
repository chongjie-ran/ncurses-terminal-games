#include "board.h"
#include <algorithm>
#include <cstring>

Board::Board() {
    reset();
}

void Board::reset() {
    for (int r = 0; r < HEIGHT; ++r)
        for (int c = 0; c < WIDTH; ++c)
            grid_[r][c] = 0;
}

bool Board::collides(const std::vector<std::pair<int,int>>& cells) const {
    for (const auto& [r, c] : cells) {
        if (r < 0 || r >= HEIGHT || c < 0 || c >= WIDTH) return true;
        if (grid_[r][c] != 0) return true;
    }
    return false;
}

void Board::lock(const std::vector<std::pair<int,int>>& cells, uint32_t color_idx) {
    for (const auto& [r, c] : cells) {
        if (r >= 0 && r < HEIGHT && c >= 0 && c < WIDTH)
            grid_[r][c] = color_idx;
    }
}

int Board::clearLines() {
    int cleared = 0;
    for (int r = HEIGHT - 1; r >= 0; ) {
        bool full = true;
        for (int c = 0; c < WIDTH; ++c) {
            if (grid_[r][c] == 0) { full = false; break; }
        }
        if (full) {
            ++cleared;
            // Move all rows above down by 1
            for (int row = r; row > 0; --row)
                for (int c = 0; c < WIDTH; ++c)
                    grid_[row][c] = grid_[row-1][c];
            for (int c = 0; c < WIDTH; ++c)
                grid_[0][c] = 0;
            // Don't decrement r; check same row again
        } else {
            --r;
        }
    }
    return cleared;
}

bool Board::isGameOver() const {
    for (int r = 0; r < 2; ++r)
        for (int c = 0; c < WIDTH; ++c)
            if (grid_[r][c] != 0) return true;
    return false;
}
