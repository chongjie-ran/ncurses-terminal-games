# Game Development Queue

## WASM Games (Emscripten + Pure C + Canvas 2D)

### Completed ✅
| Game | Status | Notes |
|------|--------|-------|
| Snake WASM | ✅ Built & Tested (2026-04-01) | Pure C + Emscripten, Canvas 2D, Playwright PASS |
| **2048 WASM** | ✅ Built & Tested | Pure C + Emscripten, Canvas 2D |
| **Minesweeper WASM** | ✅ Built & Tested | Pure C + Emscripten, 16x16, flood-fill reveal, flag, safe first-click |
| **Memory Match WASM** | ✅ Built & Tested (2026-03-31) | Pure C + Emscripten, 4x4 card flip, Playwright PASS |
| **Tetris WASM** | ✅ Built & Tested (2026-04-01) | Pure C + Emscripten, 10x20, SRS wall kick, DAS input, score/level system |
| **Frogger WASM** | ✅ Built & Tested (2026-04-01) | Pure C + Emscripten, 14x15 grid, 11-lane crossing, water/road/grass zones, Playwright PASS |
| **Sokoban WASM** | ✅ Built & Tested (2026-04-01) | Pure C + Emscripten, 8 levels, push box puzzle, Playwright PASS |
| **Space Invaders WASM** | ✅ Built & Tested (2026-04-01) | Pure C + Emscripten, 5×8 alien grid, wave system, alien bullets, Playwright PASS |
| **Breakout WASM** | ✅ Built & Tested (2026-04-01) | Pure C + Emscripten, 5×10 bricks, ball/paddle physics, angle bounce, Playwright PASS |
| **Pac-Man WASM** | ✅ Verified (2026-04-01) | 28×31 maze, 4 ghost types, frightened mode, power pellets, Playwright PASS |
| **Flappy Bird WASM** | ✅ Built & Tested (2026-04-01) | Pure C + Emscripten, gravity physics, pipe gap, score/best tracking, Playwright PASS |
| **Pong WASM** | ✅ Built & Tested (2026-04-04) | Pure C + Emscripten, AI opponent, angle bounce, Canvas 2D, Playwright PASS |
| **Othello WASM** | ✅ Built & Tested (2026-04-04) | Pure C + Emscripten, 8x8 board, 8-dir flip, Canvas 2D, Playwright PASS |
| **Wordle WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, DOM渲染, 猜词评估, 键盘颜色, Playwright PASS |
| **Mahjong Solitaire WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, 3层立体麻将, 自由牌判定, Playwright PASS |
| **Hextris WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, odd-q offset hex grid, 3-piece types, Playwright PASS |
| **Gomoku WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, 15×15 board, 5-in-a-row win, star points, last-stone marker, Playwright PASS |
| **Connect Four WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, 7x6 grid, gravity drop, 4-in-a-row win, 2-player, Playwright PASS |
| **Sudoku WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, 9x9 grid, backtracking solver, candidates, hints, 4 difficulty levels, Playwright PASS |
| **Battleship WASM** | ✅ Built & Tested (2026-04-03) | Pure C + Emscripten, 10x10 grid, ship placement, hit/miss tracking, Playwright PASS |
| **Connect Four AI WASM** | ✅ Built & Tested (2026-04-03) | Pure C + Emscripten, 3级AI(Minimax+Alpha-Beta), 防御优先, Playwright PASS |
| **Simon Says WASM** | ✅ Built & Tested (2026-04-05) | Pure C + Emscripten, 4-color memory game, sequence playback, Canvas 2D, Node.js验证PASS |
| **Dominoes WASM** | ✅ Built & Tested (2026-04-05) | Pure C + Emscripten, 多米诺骨牌, Canvas 2D, Node.js验证PASS |
| **Nonogram/Picross WASM** | ✅ Built & Tested (2026-04-05) | Pure C + Emscripten, 像素逻辑谜题, Canvas 2D, 5种尺寸 |
| **Pinball WASM** | ✅ Built & Tested (2026-04-05) | Pure C + Emscripten, 翻转球+弹球柱, 重力物理, Canvas 2D, Node.js验证PASS |
| **Brick Breaker WASM** | ✅ Built (2026-04-06) | Pure C + Emscripten, 6×8砖块, 角度反弹, 分数系统 |

### In Progress
| Game | Priority | Notes |
|------|----------|-------|

### Queue (Next Up)
| Game | Priority | Notes |
|------|----------|-------|
| **New Games Ideas** | P1 | 待开发新游戏或移植 |
| **Balloon Pop WASM** | P2 | 气球射击/消除游戏 |

---

## Raylib Games (Native C + Raylib)

### Completed ✅
| Game | Status | WASM Status |
|------|--------|-------------|
| **Space Invaders** | ✅ Complete | ✅ Has wasm/ |
| **Breakout** | ✅ Complete | ✅ Has wasm/ |
| **Flappy Bird** | ✅ Complete | ❌ Not yet |
| **Frogger** | ✅ Complete | ❌ Not yet |
| **2048** | ✅ Complete | ❌ Not yet |
| **Memory Match** | ✅ Complete | ❌ Not yet |
| **Minesweeper** | ✅ Complete | ❌ Not yet |
| **Pac-Man** | ✅ Complete | ❌ Not yet |
| **Snake** | ✅ Complete | ❌ Not yet |
| **Sokoban** | ✅ Complete | ❌ Not yet |
| **Tetris** | ✅ Complete | ❌ Not yet |

### Note on Raylib → WASM
- Raylib games use native C compilation for desktop
- WASM versions use Pure C + Emscripten + Canvas 2D (different approach)
- Raylib → WASM requires emscripten ports of raylib (complex)
- Emscripten available at: ~/emsdk/upstream/emscripten/emcc

---

## Architecture Summary

### Pure C + Emscripten + Canvas 2D (WASM)
- 26 games completed

### Pure C + Raylib (Native)
- 11 games completed

### Key Build Commands

#### WASM (Emscripten)
- Emscripten: ~/emsdk/upstream/emscripten/emcc

#### Raylib (Native)


---

## Game Patterns Reference

### DAS Input (Delayed Auto Shift)
- DELAY=150ms, ARR=50ms
- For smooth keyboard repeat in action games

### Wall Kick (SRS-style)
- 4-position kick table for rotation
- Used in Tetris

### Odd-Q Offset Hex Coordinates
- Used in Hextris
- 6-direction movement

### Minimax + Alpha-Beta Pruning
- Used in Connect Four AI
- Depth: 3-5 levels

### Flood Fill (BFS/DFS)
- Used in Minesweeper (reveal), Wordle (evaluation)
- Grid traversal with boundary marking

### Pinball Flipper Physics
- Line segment to circle collision (quadratic formula)
- Reflection: v - 2*(v·n)*n
- Up-flip boost: dy -= 5, dx += direction*3
- Gravity: dy += 0.15 per frame
- Speed cap: clamp to BALL_SPEED_MAX=15

### Brick Breaker Physics
- Paddle angle bounce: hit_pos determines reflection angle
- Speed increase: speed *= 1.005 per paddle hit
- Max speed cap: 8 units

---

*Last updated: 2026-04-06 (Brick Breaker WASM added, 26 games total)*
