#!/bin/bash
EMCC=~/emsdk/upstream/emscripten/emcc
$EMCC -o brick_breaker.js game.c wasm_main.c \
  -s EXPORTED_FUNCTIONS="['_init_game','_tick_game','_get_game_state','_get_score','_get_lives','_get_paused','_get_ball_x','_get_ball_y','_get_ball_dx','_get_ball_dy','_get_paddle_x','_get_paddle_y','_get_paddle_w','_get_paddle_h','_get_brick_color','_get_ball_r','_get_frame_count','_game_set_paddle_dir']" \
  -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
  -s MODULARIZE=0 \
  -s ALLOW_MEMORY_GROWTH=1 \
  -O2 \
  --no-entry \
  && echo "Build OK" && ls -la brick_breaker.wasm
