#!/bin/bash
cd "$(dirname "$0")"
echo "=== Building Pinball WASM ==="
emcc -O3 \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS="[_game_reset,_game_tick,_set_key,_set_left_flipper,_set_right_flipper,_get_game_state,_get_score,_get_lives,_get_ball_x,_get_ball_y,_get_ball_active,_get_paddle_x,_get_ball_radius,_get_paddle_width,_get_paddle_height,_get_canvas_width,_get_canvas_height,_get_bumper_count,_get_bumper_x,_get_bumper_y,_get_bumper_radius_i,_get_bumper_hit,_get_left_flipper_x,_get_left_flipper_angle]" \
    -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="createModule" \
    src/game.c wasm_main.c \
    -o pinball.js
echo "Done: pinball.js + pinball.wasm"
