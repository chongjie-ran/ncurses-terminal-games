# Game Development Queue

## WASM Games (Emscripten + Pure C)

### Completed ✅
| Game | Status | Notes |
|------|--------|-------|
| Snake WASM | ✅ Deployed | Pure C + Emscripten, Canvas 2D |
| **2048 WASM** | ✅ Built & Tested | Pure C + Emscripten, Canvas 2D |
| **Minesweeper WASM** | ✅ Built & Tested | Pure C + Emscripten, 16x16, flood-fill reveal, flag, safe first-click |
| **Memory Match WASM** | ✅ Built & Tested (2026-03-31) | Pure C + Emscripten, 4x4 card flip, Playwright PASS |

### In Progress
| Game | Priority | Notes |
|------|----------|-------|
| 2048 WASM Deployment | P2 | Needs web hosting |

### Queue (Next Up)
| Game | Priority | Notes |
|------|----------|-------|
| Tetris WASM | P3 | Rotation, line clear |
| Sokoban WASM | P3 | Push box puzzles |
| Frogger WASM | P3 | Road crossing, river jumping |

### Architecture Note
- Pure C game logic (game.c/game.h) - no raylib dependency
- wasm_main.c - Emscripten wrapper exports C functions
- index.html - Canvas 2D rendering + WASM loader
- Key fix: ALLOW_MEMORY_GROWTH=1 required for dynamic memory
- Key fix: Game2048Module loaded AFTER game2048.js (script order critical!)
- Key fix: MemoryMatchModule Promise `.then()` after init

## Raylib Games (C++)

### Completed ✅
| Game | Status | Notes |
|------|--------|-------|
| Snake Raylib | ✅ Deployed | 5 deployed locations |

### Deferred (WASM first)
- 2048 Raylib → Replaced by 2048 WASM (lighter, no raylib dependency)
- Breakout Raylib → Consider WASM version
- Pong Raylib → Consider WASM version
- Flappy Bird Raylib → Consider WASM version
- Tetris Raylib → Queue WASM version instead
