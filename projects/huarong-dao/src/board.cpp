#include "board.h"
#include <iostream>

Board::Board() {
    blocks.reserve(8);
}

std::vector<int> Board::cells() const {
    std::vector<int> c(ROWS * COLS, EMPTY);
    for (const auto& b : blocks)
        for (int r = b.r; r < b.r + b.h; ++r)
            for (int col = b.c; col < b.c + b.w; ++col)
                if (r >= 0 && r < ROWS && col >= 0 && col < COLS)
                    c[r * COLS + col] = b.type;
    return c;
}

std::string Board::stateKey() const {
    std::string s(ROWS * COLS, '0');
    for (const auto& b : blocks)
        for (int r = b.r; r < b.r + b.h; ++r)
            for (int col = b.c; col < b.c + b.w; ++col)
                if (r >= 0 && r < ROWS && col >= 0 && col < COLS)
                    s[r * COLS + col] = char('0' + b.type);
    return s;
}

bool Board::isGoal() const {
    for (const auto& b : blocks)
        if (b.type == CAO) return b.r == 2 && b.c == 1;
    return false;
}

std::vector<std::tuple<int,int,int>> Board::validMoves() const {
    std::vector<std::tuple<int,int,int>> moves;
    auto c = cells();
    for (size_t i = 0; i < blocks.size(); ++i) {
        const auto& b = blocks[i];
        const int D[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
        for (auto& d : D) {
            int nr = b.r + d[0], nc = b.c + d[1];
            bool ok = true;
            for (int r = nr; r < nr + b.h && ok; ++r) {
                for (int col = nc; col < nc + b.w && ok; ++col) {
                    if (r < 0 || r >= ROWS || col < 0 || col >= COLS) { ok = false; break; }
                    // Destination cell must be empty OR be one of our own current cells
                    if (c[r * COLS + col] != EMPTY) {
                        // Allow if this cell is currently occupied by THIS piece
                        bool is_own = (r >= b.r && r < b.r + b.h && col >= b.c && col < b.c + b.w);
                        if (!is_own) { ok = false; break; }
                    }
                }
            }
            if (ok) moves.emplace_back((int)i, d[0], d[1]);
        }
    }
    return moves;
}

Board Board::afterMove(int pieceIdx, int dr, int dc) const {
    Board nb = *this;
    nb.blocks[pieceIdx].r += dr;
    nb.blocks[pieceIdx].c += dc;
    return nb;
}

void Board::print() const {
    auto c = cells();
    const char NAMES[] = " ?CZYHM12";
    for (int r = 0; r < ROWS; ++r) {
        std::cout << "  +";
        for (int col = 0; col < COLS; ++col) std::cout << "---+";
        std::cout << "\n  |";
        for (int col = 0; col < COLS; ++col) {
            int v = c[r * COLS + col];
            std::cout << ' ' << (v ? NAMES[v] : ' ') << " |";
        }
        std::cout << "\n";
    }
    std::cout << "  +";
    for (int col = 0; col < COLS; ++col) std::cout << "---+";
    std::cout << "\n";
}

// SOLVABLE LAYOUT (goal = CAO top-left at row 2, col 1)
// Solution: BING1.down → WEI.up → CAO.down → GOAL
//
//   0 1 2 3 4
// 0 Z Z . . H   ZHAO(1×2), HUANG(1×2)
// 1 B . C C .   B=BING1(1×1), CAO(2×2) at (1,2)
// 2 W . . . .   WEI(2×1) at (2,0)
// 3 W . . . .   goal: CAO(2×2) top-left at (2,1)
Board Board::initial() {
    Board b;
    b.blocks = {
        {ZHAO,  0, 0, 1, 2},   // Zhao Yun 1×2 at (0,0),(0,1)
        {HUANG, 0, 3, 1, 2},   // Huang Zhong 1×2 at (0,3),(0,4)
        {BING1, 1, 0, 1, 1},   // Soldier 1×1 at (1,0)
        {CAO,   1, 2, 2, 2},   // Cao Cao 2×2 at (1,2),(1,3),(2,2),(2,3)
        {WEI,   2, 0, 2, 1},   // Wei Yan 2×1 at (2,0),(3,0)
    };
    return b;
}
