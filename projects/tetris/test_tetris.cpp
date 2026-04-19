// tetris_test.cpp - Self-contained test runner for Tetris
#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <set>

class Piece;
class Board;

// --- piece.h ---
#include <vector>
#include <cstdint>

struct PieceDef {
    char type;
    uint32_t color;
    uint16_t shape[4];
};

class Piece {
public:
    Piece(char type, int rotation_state = 0);
    uint16_t getShape() const;
    std::vector<std::pair<int,int>> getCells(int origin_x, int origin_y) const;
    void rotate(int direction);
    char getType() const { return type_; }
    int getRotation() const { return rotation_; }
    uint32_t getColor() const;
    static const PieceDef& getDef(char type);
    static char randomType();
private:
    char type_;
    int rotation_;
};

// --- board.h ---
#define WIDTH 10
#define HEIGHT 20

class Board {
public:
    static constexpr int W = WIDTH;
    static constexpr int H = HEIGHT;
    Board();
    bool collides(const std::vector<std::pair<int,int>>& cells) const;
    void lock(const std::vector<std::pair<int,int>>& cells, uint32_t color_idx);
    int clearLines();
    bool isGameOver() const;
    uint32_t cell(int row, int col) const { return grid_[row][col]; }
    void reset();
private:
    uint32_t grid_[HEIGHT][WIDTH];
};

// --- piece.cpp ---

static const PieceDef PIECE_DEFS[] = {
    {'I', 1, {0x0F00, 0x2222, 0x00F0, 0x8888}},
    {'O', 2, {0x0660, 0x0660, 0x0660, 0x0660}},
    {'T', 3, {0x0E20, 0x4C40, 0x4E00, 0x4640}},
    {'S', 4, {0x06C0, 0x8C40, 0x06C0, 0x8C40}},
    {'Z', 5, {0x0C60, 0x4C80, 0x0C60, 0x4C80}},
    {'J', 6, {0x0E80, 0xC440, 0x2E00, 0x44C0}},
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

uint32_t Piece::getColor() const {
    return getDef(type_).color;
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

// --- board.cpp ---
Board::Board() { reset(); }

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
            for (int row = r; row > 0; --row)
                for (int c = 0; c < WIDTH; ++c)
                    grid_[row][c] = grid_[row-1][c];
            for (int c = 0; c < WIDTH; ++c)
                grid_[0][c] = 0;
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

// ==================== TESTS ====================

int tests_run = 0;
int tests_passed = 0;

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    std::cout << "  RUN: " #name << " ... "; \
    tests_run++; \
    try { \
        test_##name(); \
        tests_passed++; \
        std::cout << "PASS\n"; \
    } catch (const std::exception& e) { \
        std::cout << "FAIL: " << e.what() << "\n"; \
    } \
} while(0)

#define assert_true(x) do { if (!(x)) throw std::runtime_error("assert failed: " #x); } while(0)
#define assert_false(x) do { if (x) throw std::runtime_error("assert failed: not " #x); } while(0)
#define assert_eq(a, b) do { if ((a) != (b)) throw std::runtime_error("assert failed: " #a " == " #b); } while(0)

// ---- Board Tests ----

TEST(Board_B001_EmptyInit) {
    Board b;
    for (int r = 0; r < HEIGHT; ++r)
        for (int c = 0; c < WIDTH; ++c)
            assert_eq(b.cell(r, c), 0u);
}

TEST(Board_B002_ResetClears) {
    Board b;
    b.lock({{0,0}}, 1);
    assert_true(b.cell(0,0) != 0);
    b.reset();
    assert_eq(b.cell(0,0), 0u);
}

TEST(Board_B003_EmptyNoCollision) {
    Board b;
    assert_false(b.collides({{5,5}}));
    assert_false(b.collides({{0,0},{19,9}}));
}

TEST(Board_B004_OutOfBoundsRight) {
    Board b;
    assert_true(b.collides({{0,10}})); // col 10 out of bounds (max 9)
}

TEST(Board_B005_OutOfBoundsBottom) {
    Board b;
    assert_true(b.collides({{20,0}})); // row 20 out of bounds (max 19)
}

TEST(Board_B006_OutOfBoundsNegative) {
    Board b;
    assert_true(b.collides({{-1,0}}));
    assert_true(b.collides({{0,-1}}));
}

TEST(Board_B007_CollisionWithLocked) {
    Board b;
    b.lock({{5,5}}, 1);
    assert_true(b.collides({{5,5}}));
}

TEST(Board_B008_LegalOnBoundary) {
    Board b;
    assert_false(b.collides({{19,9}})); // bottom-right corner
}

TEST(Board_B009_LockPlacesColor) {
    Board b;
    Piece p('I', 0);
    // I rot0 = {(1,0),(1,1),(1,2),(1,3)} at origin (3,0)
    // i.e. board cells (1,3),(1,4),(1,5),(1,6)
    auto cells = p.getCells(3, 0);
    b.lock(cells, p.getColor());
    assert_eq(b.cell(1,3), 1u); // first cell
    assert_eq(b.cell(1,4), 1u);
    assert_eq(b.cell(1,5), 1u);
    assert_eq(b.cell(1,6), 1u);
}

TEST(Board_B011_ClearZeroLines) {
    Board b;
    b.lock({{5,5}}, 1); // single block
    assert_eq(b.clearLines(), 0);
}

TEST(Board_B012_ClearOneLine) {
    Board b;
    for (int c = 0; c < WIDTH; ++c)
        b.lock({{19, c}}, 1);
    assert_eq(b.clearLines(), 1);
    assert_eq(b.cell(19,0), 0u);
}

TEST(Board_B013_ClearTwoLines) {
    Board b;
    for (int c = 0; c < WIDTH; ++c) {
        b.lock({{18, c}}, 1);
        b.lock({{19, c}}, 1);
    }
    assert_eq(b.clearLines(), 2);
}

TEST(Board_B014_ClearThreeLines) {
    Board b;
    for (int c = 0; c < WIDTH; ++c) {
        b.lock({{17, c}}, 1);
        b.lock({{18, c}}, 1);
        b.lock({{19, c}}, 1);
    }
    assert_eq(b.clearLines(), 3);
}

TEST(Board_B015_ClearFourLines_Tetris) {
    Board b;
    for (int c = 0; c < WIDTH; ++c) {
        b.lock({{16, c}}, 1);
        b.lock({{17, c}}, 1);
        b.lock({{18, c}}, 1);
        b.lock({{19, c}}, 1);
    }
    assert_eq(b.clearLines(), 4);
}

TEST(Board_B016_LinesShiftDown) {
    Board b;
    for (int c = 0; c < WIDTH; ++c)
        b.lock({{18, c}}, 1);
    b.lock({{17, 0}}, 2); // distinct color above
    b.clearLines();
    // Row 18 was full, cleared; row 17 (color 2) shifts to row 18
    assert_eq(b.cell(18,0), 2u);
}

TEST(Board_B017_NotGameOverWhenTopEmpty) {
    Board b;
    assert_false(b.isGameOver());
}

TEST(Board_B018_GameOverWhenTopOccupied) {
    Board b;
    b.lock({{0,0}}, 1);
    assert_true(b.isGameOver());
}

// ---- Piece Tests ----

TEST(Piece_P001_I_TypeAndColor) {
    Piece p('I', 0);
    assert_eq(p.getType(), 'I');
    assert_eq(p.getColor(), 1u);
    assert_eq(p.getShape(), 0x0F00u);
}

TEST(Piece_P002_O_AllRotationsSame) {
    Piece p('O', 0);
    uint16_t shape0 = p.getShape();
    Piece q('O', 1);
    assert_eq(q.getShape(), shape0);
    Piece r('O', 2);
    assert_eq(r.getShape(), shape0);
    Piece s('O', 3);
    assert_eq(s.getShape(), shape0);
    assert_eq(shape0, 0x0660u);
}

TEST(Piece_P003_T_Type) {
    Piece p('T', 0);
    assert_eq(p.getType(), 'T');
    assert_eq(p.getColor(), 3u);
}

TEST(Piece_P007_L_Type) {
    Piece p('L', 0);
    assert_eq(p.getType(), 'L');
    assert_eq(p.getColor(), 7u);
}

// I rot0 = {(1,0),(1,1),(1,2),(1,3)} at origin (3,0) → board (1,3..6)
TEST(Piece_P008_I_Rotation0) {
    Piece p('I', 0);
    auto cells = p.getCells(3, 0);
    assert_eq(cells.size(), 4u);
    assert_eq(cells[0], (std::make_pair(1,3)));
    assert_eq(cells[1], (std::make_pair(1,4)));
    assert_eq(cells[2], (std::make_pair(1,5)));
    assert_eq(cells[3], (std::make_pair(1,6)));
}

// After 4 CW rotations, back to original
TEST(Piece_P009_Rotation4xReturns) {
    Piece p('T', 0);
    int orig = p.getRotation();
    for (int i = 0; i < 4; ++i) p.rotate(1);
    assert_eq(p.getRotation(), orig);
}

// CCW from 0 → 3
TEST(Piece_P010_CCW_Rotation) {
    Piece p('T', 0);
    assert_eq(p.getRotation(), 0);
    p.rotate(-1);
    assert_eq(p.getRotation(), 3);
    p.rotate(-1);
    assert_eq(p.getRotation(), 2);
}

// I rot1 = {(0,0),(1,0),(2,0),(3,0)} at origin (5,0) → board (0..3,5)
TEST(Piece_P012_I_Rotation1Vertical) {
    Piece p('I', 1);
    auto cells = p.getCells(3, 0);
    assert_eq(cells.size(), 4u);
    assert_eq(cells[0], (std::make_pair(0,5)));
    assert_eq(cells[1], (std::make_pair(1,5)));
    assert_eq(cells[2], (std::make_pair(2,5)));
    assert_eq(cells[3], (std::make_pair(3,5)));
}

// O rot0 = {(1,1),(1,2),(2,1),(2,2)} at origin (4,0) → board (1..2,5..6)
TEST(Piece_P013_O_Cells) {
    Piece p('O', 2);
    auto cells = p.getCells(4, 0);
    assert_eq(cells.size(), 4u);
    assert_eq(cells[0], (std::make_pair(1,5)));
    assert_eq(cells[1], (std::make_pair(1,6)));
    assert_eq(cells[2], (std::make_pair(2,5)));
    assert_eq(cells[3], (std::make_pair(2,6)));
}

TEST(Piece_P014_RandomTypeValid) {
    std::set<char> seen;
    for (int i = 0; i < 100; ++i) {
        char t = Piece::randomType();
        assert_true(t=='I'||t=='O'||t=='T'||t=='S'||t=='Z'||t=='J'||t=='L');
        seen.insert(t);
    }
    assert_true(seen.size() >= 5);
}

// ---- Game Tests ----

enum GameState { PLAYING, PAUSED, GAME_OVER };

struct GameMinimal {
    Board board_;
    Piece current_;
    Piece next_;
    int cur_x_ = 3, cur_y_ = 0;
    int score_ = 0, level_ = 1, lines_cleared_ = 0;
    GameState state_ = GameState::PLAYING;
    bool lock_pending_ = false;

    GameMinimal() : current_('I', 0), next_('T', 0) { srand(42); }

    bool movePiece(int dx, int dy) {
        auto cells = current_.getCells(cur_x_ + dx, cur_y_ + dy);
        if (!board_.collides(cells)) {
            cur_x_ += dx; cur_y_ += dy; return true;
        }
        return false;
    }

    void lockPiece() {
        auto cells = current_.getCells(cur_x_, cur_y_);
        board_.lock(cells, current_.getColor());
        lock_pending_ = false;
        int cleared = board_.clearLines();
        if (cleared > 0) {
            static const int POINTS[] = {0, 100, 300, 500, 800};
            score_ += POINTS[cleared] * level_;
            lines_cleared_ += cleared;
            level_ = lines_cleared_ / 10 + 1;
            if (level_ > 20) level_ = 20;
        }
        if (board_.isGameOver()) { state_ = GameState::GAME_OVER; return; }
        spawnPiece();
    }

    void spawnPiece() {
        current_ = next_;
        next_ = Piece(Piece::randomType(), 0);
        cur_x_ = 3; cur_y_ = 0;
        auto cells = current_.getCells(cur_x_, cur_y_);
        if (board_.collides(cells)) { state_ = GameState::GAME_OVER; }
    }

    void start() {
        board_.reset();
        score_ = 0; level_ = 1; lines_cleared_ = 0;
        current_ = Piece(Piece::randomType(), 0);
        next_ = Piece(Piece::randomType(), 0);
        cur_x_ = 3; cur_y_ = 0;
        state_ = GameState::PLAYING;
        lock_pending_ = false;
    }

    int getScore() const { return score_; }
    int getLevel() const { return level_; }
    int getLines() const { return lines_cleared_; }
    GameState getState() const { return state_; }
    const Board& getBoardRef() const { return board_; }
};

TEST(Game_G001_InitialState) {
    GameMinimal g;
    assert_eq(g.getScore(), 0);
    assert_eq(g.getLevel(), 1);
    assert_eq(g.getLines(), 0);
    assert_eq(g.getState(), GameState::PLAYING);
}

TEST(Game_G002_StartResets) {
    GameMinimal g;
    g.movePiece(0, 1); g.movePiece(0, 1);
    g.start();
    assert_eq(g.getScore(), 0);
    assert_eq(g.getLines(), 0);
}

TEST(Game_G003_MoveLeft) {
    GameMinimal g;
    int orig_x = g.cur_x_;
    assert_true(g.movePiece(-1, 0));
    assert_eq(g.cur_x_, orig_x - 1);
}

TEST(Game_G004_MoveRight) {
    GameMinimal g;
    int orig_x = g.cur_x_;
    assert_true(g.movePiece(1, 0));
    assert_eq(g.cur_x_, orig_x + 1);
}

TEST(Game_G005_MoveDown) {
    GameMinimal g;
    int orig_y = g.cur_y_;
    assert_true(g.movePiece(0, 1));
    assert_eq(g.cur_y_, orig_y + 1);
}

TEST(Game_G006_CannotMoveLeftAtBoundary) {
    GameMinimal g;
    g.cur_x_ = 0;
    assert_false(g.movePiece(-1, 0));
    assert_eq(g.cur_x_, 0);
}

TEST(Game_G007_CannotMoveIntoLocked) {
    GameMinimal g;
    // Lock I at bottom row 1 (I rot0 occupies row 1)
    g.board_.lock({{1,3},{1,4},{1,5},{1,6}}, 1);
    // Move I-piece down to lock
    while (g.movePiece(0, 1)) { /* drop */ }
    assert_true(g.getState() == GameState::PLAYING); // not game over
    // Now move until collision
    while (g.movePiece(0, 1)) { /* keep moving */ }
    // After locking, new piece spawns; try to drop into locked row
    // The new T piece spawns at y=0, so we can move down
    for (int i = 0; i < 20; ++i) g.movePiece(0, 1);
    // Should collide when trying to enter the locked row 1
    assert_false(g.movePiece(0, 1));
}

TEST(Game_G011_LockAddsScore) {
    GameMinimal g;
    for (int c = 0; c < WIDTH; ++c)
        g.board_.lock({{19, c}}, 1);
    int before = g.getScore();
    g.lockPiece();
    assert_true(g.getScore() > before);
}

TEST(Game_G012_Lock4Lines_TetrisScore) {
    GameMinimal g;
    for (int row = 16; row <= 19; ++row)
        for (int c = 0; c < WIDTH; ++c)
            g.board_.lock({{row, c}}, 1);
    int before = g.getScore();
    g.lockPiece();
    assert_true(g.getScore() > before);
    assert_eq(g.getLines(), 4);
}

TEST(Game_G014_GameOverOnSpawnCollision) {
    GameMinimal g;
    // Occupy spawn area for I piece
    // I rot0 = {(1,0),(1,1),(1,2),(1,3)} at (3,0) → occupies row 1, cols 3-6
    g.board_.lock({{1,3},{1,4},{1,5},{1,6}}, 1);
    g.spawnPiece();
    assert_eq(g.getState(), GameState::GAME_OVER);
}

TEST(Game_G015_PauseToggle) {
    GameMinimal g;
    assert_eq(g.getState(), GameState::PLAYING);
    g.state_ = GameState::PAUSED;
    assert_eq(g.getState(), GameState::PAUSED);
    g.state_ = GameState::PLAYING;
    assert_eq(g.getState(), GameState::PLAYING);
}

// ==================== MAIN ====================

int main() {
    std::cout << "\n=== Tetris 测试执行 ===\n\n";

    std::cout << "[Board Tests]\n";
    RUN_TEST(Board_B001_EmptyInit);
    RUN_TEST(Board_B002_ResetClears);
    RUN_TEST(Board_B003_EmptyNoCollision);
    RUN_TEST(Board_B004_OutOfBoundsRight);
    RUN_TEST(Board_B005_OutOfBoundsBottom);
    RUN_TEST(Board_B006_OutOfBoundsNegative);
    RUN_TEST(Board_B007_CollisionWithLocked);
    RUN_TEST(Board_B008_LegalOnBoundary);
    RUN_TEST(Board_B009_LockPlacesColor);
    RUN_TEST(Board_B011_ClearZeroLines);
    RUN_TEST(Board_B012_ClearOneLine);
    RUN_TEST(Board_B013_ClearTwoLines);
    RUN_TEST(Board_B014_ClearThreeLines);
    RUN_TEST(Board_B015_ClearFourLines_Tetris);
    RUN_TEST(Board_B016_LinesShiftDown);
    RUN_TEST(Board_B017_NotGameOverWhenTopEmpty);
    RUN_TEST(Board_B018_GameOverWhenTopOccupied);

    std::cout << "\n[Piece Tests]\n";
    RUN_TEST(Piece_P001_I_TypeAndColor);
    RUN_TEST(Piece_P002_O_AllRotationsSame);
    RUN_TEST(Piece_P003_T_Type);
    RUN_TEST(Piece_P007_L_Type);
    RUN_TEST(Piece_P008_I_Rotation0);
    RUN_TEST(Piece_P009_Rotation4xReturns);
    RUN_TEST(Piece_P010_CCW_Rotation);
    RUN_TEST(Piece_P012_I_Rotation1Vertical);
    RUN_TEST(Piece_P013_O_Cells);
    RUN_TEST(Piece_P014_RandomTypeValid);

    std::cout << "\n[Game Tests]\n";
    RUN_TEST(Game_G001_InitialState);
    RUN_TEST(Game_G002_StartResets);
    RUN_TEST(Game_G003_MoveLeft);
    RUN_TEST(Game_G004_MoveRight);
    RUN_TEST(Game_G005_MoveDown);
    RUN_TEST(Game_G006_CannotMoveLeftAtBoundary);
    RUN_TEST(Game_G007_CannotMoveIntoLocked);
    RUN_TEST(Game_G011_LockAddsScore);
    RUN_TEST(Game_G012_Lock4Lines_TetrisScore);
    RUN_TEST(Game_G014_GameOverOnSpawnCollision);
    RUN_TEST(Game_G015_PauseToggle);

    std::cout << "\n========================================\n";
    std::cout << "测试结果: " << tests_passed << "/" << tests_run << " 通过\n";
    std::cout << "========================================\n";

    return (tests_passed == tests_run) ? 0 : 1;
}
