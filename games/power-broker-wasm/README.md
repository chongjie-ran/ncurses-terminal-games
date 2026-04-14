# 算力经纪人 (Power Broker) - WASM Edition

> 在AI芯片短缺时代，扮演算力经纪人调配全球算力资源。

## Play Online

**GitHub Pages**: https://chongjie-ran.github.io/ncurses-terminal-games/

## Features
- 3 difficulty modes (Easy / Normal / Hard)
- 10 normal events + 6 Boss events
- 12 achievements
- AI competitors (2-3 AI traders)
- Save/Load system
- Win streak tracking

## Build from Source

### Prerequisites
- Emscripten SDK 3.1+

### Build
```bash
git clone https://github.com/chongjie-ran/ncurses-terminal-games.git
cd ncurses-terminal-games/games/power-broker-wasm
./build.sh
```

### Local Play
```bash
# Serve with any static server
npx serve .
# or
python3 -m http.server 8080
```

Then open `http://localhost:8080` in your browser.

## Tech Stack
- Game logic: Pure C (compiled to WASM)
- Frontend: Vanilla JS + HTML
- Build: Emscripten (emcc)
- CI/CD: GitHub Actions

## Project Structure
```
games/power-broker-wasm/
├── game.c         # Core game logic (C)
├── game.h         # Header definitions
├── wasm_main.c    # WASM entry point & JS bindings
├── index.html     # Game UI
├── build.sh       # Build script
└── .github/workflows/ci-cd.yml
```

## Version History
- v1.4 (2026-04-12): Rich feature release - difficulty/Boss/achievements/AI/save-load
- v1.3 (2026-04-09): MVP baseline
