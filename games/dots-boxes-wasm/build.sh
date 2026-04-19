#!/bin/bash
# Build Dots and Boxes WASM
emcc -O2 \
  game.c wasm_main.c \
  -s WASM=1 \
  -s EXPORTED_FUNCTIONS='["_db_reset","_db_click_h","_db_click_v","_db_cell","_db_score","_db_player","_db_phase","_db_last_edge","_db_last_box","_db_boxes_done"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
  -o dots_boxes.js \
  --no-entry
echo "Build complete: dots_boxes.js, dots_boxes.wasm"
