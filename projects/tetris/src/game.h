#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "piece.h"
#include <cstdint>
#include <chrono>

enum class GameState { PLAYING, PAUSED, GAME_OVER };

struct WallKick {
    int dx, dy;  // delta x, delta y to try
};

class Game {
public:
    Game();

    void start();
    void handleInput(int ch);
    void tick();              // Advance one game step (gravity)
    bool isGameOver() const { return state_ == GameState::GAME_OVER; }
    const Board& getBoardRef() const { return board_; }
    int  getScore() const { return score_; }
    int  getLevel() const { return level_; }
    int  getLines() const { return lines_cleared_; }
    GameState getState() const { return state_; }
    const Piece& getCurrentPiece() const { return current_; }
    const Piece& getNextPiece() const { return next_; }
    int  getCurrentX() const { return cur_x_; }
    int  getCurrentY() const { return cur_y_; }

private:
    void spawnPiece();
    bool movePiece(int dx, int dy);
    void rotatePiece(int dir);   // +1=CW, -1=CCW
    void lockPiece();
    void computeLevel();

    static constexpr int DROP_INTERVAL_BASE_MS = 800;
    static constexpr int LOCK_DELAY_MS = 500;
    static constexpr int WALL_KICK_COUNT = 5;

    Board board_;
    Piece current_;
    Piece next_;
    int cur_x_;          // top-left of 4x4 bounding box, col
    int cur_y_;          // top-left of 4x4 bounding box, row
    int score_;
    int level_;
    int lines_cleared_;
    GameState state_;

    std::chrono::steady_clock::time_point last_tick_;
    std::chrono::steady_clock::time_point last_lock_attempt_;
    bool lock_pending_;
};

#endif // GAME_H
