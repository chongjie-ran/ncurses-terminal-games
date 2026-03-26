#include "game.h"
#include <cstdlib>
#include <algorithm>

// SRS Wall Kick data: [rotation_from][rotation_to] = array of (dx,dy) to try
// Simplified 5-attempt wall kick
static const WallKick JLSTZ_WALL_KICKS[8][5] = {
    // 0→1
    {{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}},
    // 1→2
    {{0,0},{1,0},{1,-1},{0,2},{1,2}},
    // 2→3
    {{0,0},{1,0},{1,1},{0,-2},{1,-2}},
    // 3→0
    {{0,0},{-1,0},{-1,-1},{0,2},{-1,2}},
    // 0→3 (CCW)
    {{0,0},{1,0},{1,-1},{0,2},{1,2}},
    // 3→2
    {{0,0},{-1,0},{-1,1},{0,-2},{-1,-2}},
    // 2→1
    {{0,0},{-1,0},{-1,-1},{0,2},{-1,2}},
    // 1→0
    {{0,0},{1,0},{1,1},{0,-2},{1,2}},
};
static const WallKick I_WALL_KICKS[8][5] = {
    {{0,0},{-2,0},{1,0},{-2,-1},{1,2}},
    {{0,0},{-1,0},{2,0},{-1,2},{2,-1}},
    {{0,0},{2,0},{-1,0},{2,1},{-1,-2}},
    {{0,0},{1,0},{-2,0},{1,-2},{-2,1}},
    {{0,0},{2,0},{-1,0},{2,-1},{-1,2}},
    {{0,0},{1,0},{-2,0},{1,2},{-2,-1}},
    {{0,0},{-2,0},{1,0},{-2,1},{1,-2}},
    {{0,0},{-1,0},{2,0},{-1,-2},{2,1}},
};

Game::Game()
    : current_(Piece::randomType(), 0)
    , next_(Piece::randomType(), 0)
    , cur_x_(3), cur_y_(0)
    , score_(0), level_(1), lines_cleared_(0)
    , state_(GameState::PLAYING)
    , lock_pending_(false)
{
    srand(static_cast<unsigned>(time(nullptr)));
    // Compute spawn position: center piece in top rows
    cur_x_ = 3;
    cur_y_ = 0;
    last_tick_ = std::chrono::steady_clock::now();
}

void Game::start() {
    state_ = GameState::PLAYING;
    board_.reset();
    score_ = 0; level_ = 1; lines_cleared_ = 0;
    current_ = Piece(Piece::randomType(), 0);
    next_    = Piece(Piece::randomType(), 0);
    cur_x_ = 3; cur_y_ = 0;
    lock_pending_ = false;
    last_tick_ = std::chrono::steady_clock::now();
}

bool Game::movePiece(int dx, int dy) {
    auto cells = current_.getCells(cur_x_ + dx, cur_y_ + dy);
    if (!board_.collides(cells)) {
        cur_x_ += dx;
        cur_y_ += dy;
        if (lock_pending_ && dy == 0) {
            // moved horizontally while lock pending → reset lock timer
            last_lock_attempt_ = std::chrono::steady_clock::now();
        }
        return true;
    }
    return false;
}

void Game::rotatePiece(int dir) {
    Piece rotated = current_;
    rotated.rotate(dir);
    int from_rot = current_.getRotation();
    bool is_i = (current_.getType() == 'I');
    const WallKick (*kicks)[5] = is_i ? I_WALL_KICKS : JLSTZ_WALL_KICKS;
    int idx = (from_rot * 2 + (dir > 0 ? 1 : 0)) & 7;

    for (int i = 0; i < WALL_KICK_COUNT; ++i) {
        int nx = cur_x_ + kicks[idx][i].dx;
        int ny = cur_y_ + kicks[idx][i].dy;
        auto cells = rotated.getCells(nx, ny);
        if (!board_.collides(cells)) {
            current_ = rotated;
            cur_x_ = nx;
            cur_y_ = ny;
            if (lock_pending_) {
                last_lock_attempt_ = std::chrono::steady_clock::now();
            }
            return;
        }
    }
    // No valid wall kick found: don't rotate
}

void Game::lockPiece() {
    auto cells = current_.getCells(cur_x_, cur_y_);
    board_.lock(cells, current_.getColor());
    lock_pending_ = false;
    int cleared = board_.clearLines();
    if (cleared > 0) {
        static const int POINTS[] = {0, 100, 300, 500, 800};
        score_ += POINTS[cleared] * level_;
        lines_cleared_ += cleared;
        computeLevel();
    }
    if (board_.isGameOver()) {
        state_ = GameState::GAME_OVER;
        return;
    }
    spawnPiece();
}

void Game::spawnPiece() {
    current_ = next_;
    next_ = Piece(Piece::randomType(), 0);
    cur_x_ = 3;
    cur_y_ = 0;
    // Check immediate collision at spawn → game over
    auto cells = current_.getCells(cur_x_, cur_y_);
    if (board_.collides(cells)) {
        state_ = GameState::GAME_OVER;
    }
    last_tick_ = std::chrono::steady_clock::now();
}

void Game::tick() {
    using namespace std::chrono;
    auto now = steady_clock::now();

    if (state_ != GameState::PLAYING) return;

    if (!movePiece(0, 1)) {
        // Can't move down: lock delay
        if (!lock_pending_) {
            lock_pending_ = true;
            last_lock_attempt_ = now;
        } else {
            auto elapsed = duration_cast<milliseconds>(now - last_lock_attempt_).count();
            if (elapsed >= LOCK_DELAY_MS) {
                lockPiece();
            }
        }
    } else {
        lock_pending_ = false;
    }
    last_tick_ = now;
}

void Game::handleInput(int ch) {
    if (state_ == GameState::GAME_OVER) {
        if (ch == 'r' || ch == 'R' || ch == '\n')
            start();
        return;
    }
    if (ch == 'p' || ch == 'P' || ch == 27) {
        state_ = (state_ == GameState::PAUSED) ? GameState::PLAYING : GameState::PAUSED;
        last_tick_ = std::chrono::steady_clock::now();
        return;
    }
    if (state_ == GameState::PAUSED) return;

    switch (ch) {
        case 'a': case 260:  movePiece(-1, 0); break;   // KEY_LEFT
        case 'd': case 261:  movePiece(1, 0);  break;  // KEY_RIGHT
        case 's': case 258:  movePiece(0, 1);  break;  // KEY_DOWN
        case 'w': case 259:  rotatePiece(1);    break;  // KEY_UP
        case 'z': case 26:   rotatePiece(-1);   break; // Ctrl+Z
        case ' ':                  // hard drop
            while (movePiece(0, 1)) {}
            lockPiece();
            break;
        case 'q': case 'Q':
            state_ = GameState::GAME_OVER;
            break;
    }
}

void Game::computeLevel() {
    level_ = lines_cleared_ / 10 + 1;
    if (level_ > 20) level_ = 20;
}
