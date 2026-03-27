#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <tuple>

class Board {
public:
    static constexpr int ROWS = 4;
    static constexpr int COLS = 5;
    // Piece type IDs (used in state encoding)
    static constexpr int EMPTY = 0;
    static constexpr int CAO  = 1;  // 2×2 — main block to escape
    static constexpr int ZHANG = 2;  // 2×1 vertical
    static constexpr int ZHAO  = 3;  // 1×2 horizontal
    static constexpr int HUANG = 4;  // 1×2 horizontal
    static constexpr int WEI   = 5;  // 2×1 vertical
    static constexpr int MA    = 6;  // 2×1 vertical
    static constexpr int BING1 = 7;  // 1×1
    static constexpr int BING2 = 8;  // 1×1

    struct Block {
        int type;  // unique type ID
        int r, c;  // top-left position
        int h, w;  // dimensions
    };

    std::vector<Block> blocks;

    Board();

    // Current board as flat array for collision checking
    std::vector<int> cells() const;

    // State string for hashing (20 chars, '0'-'8')
    std::string stateKey() const;

    // Goal: CAO top-left reaches row 3 col 1 (bottom exits)
    bool isGoal() const;

    // All valid moves: (blockIndex, deltaRow, deltaCol)
    std::vector<std::tuple<int,int,int>> validMoves() const;

    // Apply move to a COPY of this board
    Board afterMove(int pieceIdx, int dr, int dc) const;

    void print() const;  // ASCII debug print

    static Board initial();
};

#endif
