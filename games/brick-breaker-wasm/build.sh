#!/bin/bash
set -e
cd "$(dirname "$0")"
source ~/emsdk/emsdk_env.sh 2>/dev/null || true

FUNCS="[_init_game,_reset_ball,_restart,_set_key,_set_paddle_x,_get_ball_x,_get_ball_y,_get_paddle_x,_get_paddle_w,_get_paddle_h,_get_ball_r,_get_brick_w,_get_brick_h,_get_brick_gap,_get_brick_top,_is_brick_alive,_get_brick_color,_get_score,_get_lives,_is_game_over,_is_game_won,_update_game]"

emcc -O3 \
  -s WASM=1 \
  -s MODULARIZE=0 \
  -s EXPORTED_FUNCTIONS="$FUNCS" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=32MB \
  -o brick_breaker.js game.c

echo "Build complete: brick_breaker.js + brick_breaker.wasm"
