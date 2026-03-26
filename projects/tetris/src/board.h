#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <cstdint>

class Board {
public:
    static constexpr int WIDTH = 10;
    static constexpr int HEIGHT = 20;

    Board();

    // Check if a piece at (row, col) with given shape would collide
    bool collides(const std::vector<std::pair<int,int>>& cells) const;

    // Lock a piece into the board
    void lock(const std::vector<std::pair<int,int>>& cells, uint32_t color_idx);

    // Remove completed rows, return count of cleared rows
    int clearLines();

    // Check if any cell in top 2 rows is occupied (game over)
    bool isGameOver() const;

    // Access board cell: returns color index (0 = empty)
    uint32_t cell(int row, int col) const { return grid_[row][col]; }

    void reset();

private:
    uint32_t grid_[HEIGHT][WIDTH];  // 0=empty, >0=color index
};

#endif // BOARD_H
