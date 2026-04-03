# Game Development Queue

## WASM Games (Emscripten + Pure C)

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
| **Pong WASM** | ✅ Built & Tested (2026-04-01) | Pure C + Emscripten, AI opponent, angle bounce, Canvas 2D, Playwright PASS |
| **Wordle WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, DOM渲染, 猜词评估, 键盘颜色, Playwright PASS |
| **Mahjong Solitaire WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, 3层立体麻将, 自由牌判定, Playwright PASS |
| **Hextris WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, odd-q offset hex grid, 3-piece types, Playwright PASS |
| **Gomoku WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, 15×15 board, 5-in-a-row win, star points, last-stone marker, Playwright PASS |
| **Connect Four WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, 7x6 grid, gravity drop, 4-in-a-row win, 2-player, Playwright PASS |
| **Sudoku WASM** | ✅ Built & Tested (2026-04-02) | Pure C + Emscripten, 9x9 grid, backtracking solver, candidates, hints, 4 difficulty levels, Playwright PASS |
| **Battleship WASM** | ✅ Built & Tested (2026-04-03) | Pure C + Emscripten, 10x10 grid, ship placement, hit/miss tracking, Playwright PASS |
| **Connect Four AI WASM** | ✅ Built & Tested (2026-04-03) | Pure C + Emscripten, 3级AI(Minimax+Alpha-Beta), 防御优先, Playwright PASS |
| **WASM Game Builder Script** | ✅ Built & Tested (2026-04-02) | 批量编译+测试所有WASM游戏, 支持--games过滤, HTTP server测试, Playwright集成 |

### In Progress
| Game | Priority | Notes |
|------|----------|-------|

### Queue (Next Up)
| Game | Priority | Notes |
|------|----------|-------|
| **Raylib Games** | P1 | Space Invaders, Breakout using raylib instead of Canvas |

### Architecture Note
- Pure C game logic (game.c/game.h) - no raylib dependency
- wasm_main.c - Emscripten wrapper exports C functions
- index.html - Canvas 2D or DOM rendering + WASM loader
- Key fix: ALLOW_MEMORY_GROWTH=1 required for dynamic memory
- Key fix: GameModule loaded AFTER game.js (script order critical!)
- Key fix: MODULARIZE=1 exports functions as Module._wasm_xxx()
- DAS input: DELAY=150ms, ARR=50ms for smooth keyboard repeat
- Wall Kick: SRS-style 4-position kick table for rotation
- Frogger: water zone needs player to ride with log (player.x += direction * speed)
- Sokoban: duplicate symbol issue - only game.c defines exported functions with EMSCRIPTEN_KEEPALIVE
- Breakout: paddle angle bounce, rect-circle collision for ball-paddle and ball-brick
- Space Invaders: bottom-alien shooting, alien movement step-down on wall hit, wave progression
- Flappy Bird: gravity applied per-frame, pipe spawns at PIPE_INTERVAL spacing, localStorage best score
- Pong: AI moves toward ball.y, paddle collision changes ball angle based on hit position
- Snake: grid-based movement, food spawn, wall/self-collision, speed increases with score
- Wordle: DOM rendering (no Canvas), keyboard color upgrade, 2-pass evaluation (correct→present→wrong)
- WASM Builder: grep+sed提取EMSCRIPTEN_KEEPALIVE函数, JSON格式EXPORTED_FUNCTIONS, HTTP server解决CORS
- Hextris: odd-q offset hex coordinates, 6-direction movement, flat-topped hex rendering
- Gomoku: 15×15 board, 4-direction win check (horizontal/vertical/diag), 5-in-a-row win detection
- Connect Four: 7x6 grid, gravity drop, 4-direction win detection, win line rendering
- Sudoku: 9×9 grid, backtracking solver, candidate bitmask, puzzle generation with solvability check, 4 difficulty levels
- Connect Four AI: Minimax + Alpha-Beta pruning, window scoring heuristic, immediate win/block detection, 3 difficulty depths

*Last updated: 2026-04-03 (Connect Four AI WASM added)*
