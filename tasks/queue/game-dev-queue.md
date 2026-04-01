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

### In Progress
| Game | Priority | Notes |
|------|----------|-------|
| - | - | - |

### Queue (Next Up)
| Game | Priority | Notes |
|------|----------|-------|
| (All planned games complete) | - | WASM games count: 13 |

### Architecture Note
- Pure C game logic (game.c/game.h) - no raylib dependency
- wasm_main.c - Emscripten wrapper exports C functions
- index.html - Canvas 2D rendering + WASM loader
- Key fix: ALLOW_MEMORY_GROWTH=1 required for dynamic memory
- Key fix: GameModule loaded AFTER game.js (script order critical!)
- Key fix: MemoryMatchModule Promise `.then()` after init
- Key fix: MODULARIZE=1 needs `window.Module = mod` in `.then()` callback
- Key fix: Don't mix C syntax (int, typedef) in JS code
- DAS input: DELAY=150ms, ARR=50ms for smooth keyboard repeat
- Wall Kick: SRS-style 4-position kick table for rotation
- Frogger: water zone needs player to ride with log (player.x += direction * speed)
- Sokoban: duplicate symbol issue - only game.c defines exported functions with EMSCRIPTEN_KEEPALIVE
- Breakout: paddle angle bounce, rect-circle collision for ball-paddle and ball-brick
- Space Invaders: bottom-alien shooting, alien movement step-down on wall hit, wave progression
- Flappy Bird: gravity applied per-frame, pipe spawns at PIPE_INTERVAL spacing, localStorage best score
- Pong: AI moves toward ball.y, paddle collision changes ball angle based on hit position
- Snake: grid-based movement, food spawn, wall/self-collision, speed increases with score

## Raylib Games (C++)

### Completed ✅
| Game | Status | Notes |
|------|--------|-------|
| Snake Raylib | ✅ Deployed | 5 deployed locations |

### Deferred (WASM first)
- 2048 Raylib → Replaced by 2048 WASM (lighter, no raylib dependency)
- Breakout Raylib → Replaced by Breakout WASM (2026-04-01)
- Pong Raylib → Replaced by Pong WASM (2026-04-01)
- Flappy Bird Raylib → Replaced by Flappy Bird WASM (2026-04-01)
- Tetris Raylib → Replaced by Tetris WASM (2026-04-01)
- Frogger Raylib → Replaced by Frogger WASM (2026-04-01)
- Sokoban Raylib → Replaced by Sokoban WASM (2026-04-01)
- Space Invaders Raylib → Replaced by Space Invaders WASM (2026-04-01)

*Last updated: 2026-04-01*
