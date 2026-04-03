#include "game.h"
#include <string.h>

static int li(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= 'A' && c <= 'Z') return c - 'A';
    return -1;
}

void wordle_init(WordleGame* g, const char* target) {
    memset(g, 0, sizeof(WordleGame));
    strncpy(g->target, target, WORD_LEN);
    g->target[WORD_LEN] = '\0';
    g->state = STATE_PLAYING;
    g->cursor_col = 0;
    for (int i = 0; i < 26; i++) g->keyboard[i] = LS_EMPTY;
}

int wordle_type_letter(WordleGame* g, char letter) {
    if (g->state != STATE_PLAYING) return 0;
    if (g->current_guess >= MAX_GUESSES) return 0;
    int col = g->cursor_col;
    if (col < 0 || col >= WORD_LEN) return 0;
    g->guesses[g->current_guess][col] = letter;
    if (col < WORD_LEN - 1) g->cursor_col++;
    return 1;
}

int wordle_backspace(WordleGame* g) {
    if (g->state != STATE_PLAYING) return 0;
    if (g->cursor_col > 0) {
        g->cursor_col--;
        g->guesses[g->current_guess][g->cursor_col] = '\0';
    } else if (g->cursor_col == 0 && g->guesses[g->current_guess][0] != '\0') {
        g->guesses[g->current_guess][0] = '\0';
    }
    return 1;
}

int wordle_enter(WordleGame* g) {
    if (g->state != STATE_PLAYING) return 0;
    if (g->current_guess >= MAX_GUESSES) return 0;
    
    char* guess = g->guesses[g->current_guess];
    if (strlen(guess) != WORD_LEN) return 0;

    // Evaluate: first pass mark correct, second pass mark present
    char target_copy[WORD_LEN + 1];
    strncpy(target_copy, g->target, WORD_LEN);
    target_copy[WORD_LEN] = '\0';

    // Pass 1: correct
    for (int i = 0; i < WORD_LEN; i++) {
        if (guess[i] == g->target[i]) {
            g->eval[g->current_guess][i] = LS_CORRECT;
            target_copy[i] = '#';
        }
    }

    // Pass 2: present or wrong
    for (int i = 0; i < WORD_LEN; i++) {
        if (g->eval[g->current_guess][i] == LS_CORRECT) continue;
        int found = 0;
        for (int j = 0; j < WORD_LEN; j++) {
            if (target_copy[j] == guess[i]) {
                g->eval[g->current_guess][i] = LS_PRESENT;
                target_copy[j] = '#';
                found = 1;
                break;
            }
        }
        if (!found) g->eval[g->current_guess][i] = LS_WRONG;
    }

    // Update keyboard state
    for (int i = 0; i < WORD_LEN; i++) {
        int ki = li(guess[i]);
        if (ki < 0) continue;
        uint8_t s = g->eval[g->current_guess][i];
        if (s > g->keyboard[ki]) g->keyboard[ki] = s;
    }

    // Check win
    int won = 1;
    for (int i = 0; i < WORD_LEN; i++) {
        if (g->eval[g->current_guess][i] != LS_CORRECT) { won = 0; break; }
    }
    if (won) { g->state = STATE_WIN; g->current_guess++; return 1; }

    g->current_guess++;
    g->cursor_col = 0;

    if (g->current_guess >= MAX_GUESSES) {
        g->state = STATE_LOSE;
        return 0;
    }
    return 1;
}
