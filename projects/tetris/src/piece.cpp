#include "piece.h"

// Each 16-bit shape: row-major 4x4, bit=1 means filled cell
// Format: bits [15:12]=row0, [11:8]=row1, [7:4]=row2, [3:0]=row3
// Hex digit per row: e.g. 0x0F00 = row0:0000, row1:1111, row2:0000, row3:0000
static const PieceDef PIECE_DEFS[] = {
    // I: horizontal → vertical
    {'I', 1, {0x0F00, 0x2222, 0x00F0, 0x8888}},
    // O: doesn't rotate (same all states)
    {'O', 2, {0x0660, 0x0660, 0x0660, 0x0660}},
    // T
    {'T', 3, {0x0E20, 0x4C40, 0x4E00, 0x4640}},
    // S
    {'S', 4, {0x06C0, 0x8C40, 0x06C0, 0x8C40}},
    // Z
    {'Z', 5, {0x0C60, 0x4C80, 0x0C60, 0x4C80}},
    // J
    {'J', 6, {0x0E80, 0xC440, 0x2E00, 0x44C0}},
    // L
    {'L', 7, {0x0E08, 0xC440, 0x2E00, 0x44C0}},
};

const PieceDef& Piece::getDef(char type) {
    for (const auto& def : PIECE_DEFS) {
        if (def.type == type) return def;
    }
    return PIECE_DEFS[0];
}

Piece::Piece(char type, int rotation_state)
    : type_(type), rotation_(rotation_state & 3) {}

uint16_t Piece::getShape() const {
    return getDef(type_).shape[rotation_];
}


void Piece::rotate(int direction) {
    rotation_ = (rotation_ + 4 + direction) & 3;
}

std::vector<std::pair<int,int>> Piece::getCells(int ox, int oy) const {
    uint16_t shape = getShape();
    std::vector<std::pair<int,int>> cells;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            int bit = (shape >> (15 - (r*4 + c))) & 1;
            if (bit) {
                cells.emplace_back(oy + r, ox + c);
            }
        }
    }
    return cells;
}

char Piece::randomType() {
    static const char types[] = {'I','O','T','S','Z','J','L'};
    return types[rand() % 7];
}
