#pragma once
#include <array>
#include <vector>
#include <random>

enum class CellState { HIDDEN, REVEALED, FLAGGED };
enum class CellValue { MINE = -1, EMPTY = 0, N1, N2, N3, N4, N5, N6, N7, N8 };

struct Cell {
    CellState state = CellState::HIDDEN;
    CellValue value = CellValue::EMPTY;
    bool is_mine = false;
};

class Minesweeper {
public:
    static constexpr int ROWS = 16;
    static constexpr int COLS = 30;
    static constexpr int MINES = 99;

    Minesweeper();
    void reset();
    bool reveal(int r, int c);
    void toggle_flag(int r, int c);
    bool is_win() const;
    bool is_game_over() const { return game_over_; }
    const Cell& cell(int r, int c) const { return grid_[r][c]; }

private:
    void place_mines(int exclude_r, int exclude_c);
    void compute_numbers();
    void flood_fill(int r, int c);

    std::array<std::array<Cell, COLS>, ROWS> grid_;
    bool game_over_ = false;
    bool game_win_ = false;
    int revealed_count_ = 0;
    int flagged_count_ = 0;
    std::mt19937 rng_;
};
