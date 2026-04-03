#include <emscripten.h>
#include <string.h>
#include "game.h"

WordleGame g_wasm_game;

EMSCRIPTEN_KEEPALIVE
void wasm_init_wordle(const char* target) {
    wordle_init(&g_wasm_game, target);
}

EMSCRIPTEN_KEEPALIVE
int wasm_type_letter(char letter) {
    return wordle_type_letter(&g_wasm_game, letter);
}

EMSCRIPTEN_KEEPALIVE
int wasm_backspace(void) {
    return wordle_backspace(&g_wasm_game);
}

EMSCRIPTEN_KEEPALIVE
int wasm_enter(void) {
    return wordle_enter(&g_wasm_game);
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_state(void) {
    return g_wasm_game.state;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_current_guess(void) {
    return g_wasm_game.current_guess;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_cursor_col(void) {
    return g_wasm_game.cursor_col;
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_guess_char(int row, int col) {
    if (row < 0 || row >= MAX_GUESSES || col < 0 || col >= WORD_LEN) return 0;
    return g_wasm_game.guesses[row][col];
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_eval(int row, int col) {
    if (row < 0 || row >= MAX_GUESSES || col < 0 || col >= WORD_LEN) return 0;
    return g_wasm_game.eval[row][col];
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_keyboard(int letter) {
    if (letter < 0 || letter >= 26) return 0;
    return g_wasm_game.keyboard[letter];
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_current_guess_len(void) {
    if (g_wasm_game.current_guess >= MAX_GUESSES) return 5;
    return (int)strlen(g_wasm_game.guesses[g_wasm_game.current_guess]);
}

EMSCRIPTEN_KEEPALIVE
int wasm_get_target_char(int col) {
    if (col < 0 || col >= WORD_LEN) return 0;
    return g_wasm_game.target[col];
}
