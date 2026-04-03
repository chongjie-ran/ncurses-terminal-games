#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define WORD_LEN 5
#define MAX_GUESSES 6

typedef enum {
    STATE_PLAYING,
    STATE_WIN,
    STATE_LOSE
} GameState;

// Letter state: 0=empty, 1=wrong, 2=present, 3=correct
typedef enum {
    LS_EMPTY = 0,
    LS_WRONG = 1,
    LS_PRESENT = 2,
    LS_CORRECT = 3
} LetterState;

typedef struct {
    char target[WORD_LEN + 1];
    char guesses[MAX_GUESSES][WORD_LEN + 1];
    uint8_t eval[MAX_GUESSES][WORD_LEN]; // evaluation per cell
    uint8_t keyboard[26]; // letter state per keyboard key
    uint8_t current_guess;
    uint8_t cursor_col;
    GameState state;
} WordleGame;

void wordle_init(WordleGame* g, const char* target);
int wordle_type_letter(WordleGame* g, char letter);
int wordle_backspace(WordleGame* g);
int wordle_enter(WordleGame* g);

#endif // GAME_H
