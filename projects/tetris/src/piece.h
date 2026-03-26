#ifndef PIECE_H
#define PIECE_H

#include <vector>
#include <cstdint>

// 7 Tetromino types: I, O, T, S, Z, J, L
// Each defined as 4x4 matrix with rotation states (0-3)
// Convention: origin at top-left of the 4x4 bounding box
struct PieceDef {
    char type;                  // I, O, T, S, Z, J, L
    uint32_t color;            // ncurses color pair index
    // 4 rotation states, each state = 16 bits (4x4 grid, row-major)
    // bit=1 means occupied cell
    uint16_t shape[4];
};

class Piece {
public:
    Piece(char type, int rotation_state = 0);

    // Get current shape (4x4 grid flattened to 16 bits)
    uint16_t getShape() const;
    // Get bounding box occupied cells
    std::vector<std::pair<int,int>> getCells(int origin_x, int origin_y) const;
    // Rotation: +1 = clockwise, -1 = counter-clockwise
    void rotate(int direction);
    char getType() const { return type_; }
    int getRotation() const { return rotation_; }
    uint32_t getColor() const { return getDef(type_).color; }

    static const PieceDef& getDef(char type);
    static char randomType();

private:
    char type_;
    int rotation_;  // 0-3
};

#endif // PIECE_H
