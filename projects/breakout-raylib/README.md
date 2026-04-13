# Breakout - Raylib

Classic Breakout arcade game built with Raylib.

## Controls
- **Mouse / Arrow Keys / A,D**: Move paddle
- **SPACE / Click**: Launch ball
- **P**: Pause

## Build
```bash
g++ src/main.c src/game.c src/draw.c -o breakout_raylib -Wall -Wextra -O2 -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -lm
./breakout_raylib
```

## Features
- 5 rows × 10 cols bricks (color coded by row)
- 3 lives, score tracking
- Level progression
- Paddle-angle ball physics
- Raylib 3D-effect paddle rendering
