// Memory Match WASM - Pure C game logic
// 4x4 grid = 16 cards = 8 pairs

#define GRID_W 4
#define GRID_H 4
#define TOTAL_CARDS (GRID_W * GRID_H)
#define NUM_PAIRS 8
#define CARD_VALUES {0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7}

// Card states
#define STATE_HIDDEN 0
#define STATE_REVEALED 1
#define STATE_MATCHED 2

// Game states
#define GAME_STATE_PLAYING 0
#define GAME_STATE_WAITING 1   // waiting for second card flip back
#define GAME_STATE_WON 2

static int cards[TOTAL_CARDS];       // card values (0-7)
static int card_states[TOTAL_CARDS]; // STATE_HIDDEN/REVEALED/MATCHED
static int game_state;                // GAME_STATE_*
static int selected[TOTAL_CARDS];     // indices of selected cards (max 2)
static int num_selected;              // 0, 1, or 2
static int matched_pairs;             // 0 to NUM_PAIRS
static int moves;                     // move counter
static int wait_timer;                // frames to wait before flipping back
static int lock_input;                // prevent clicks during WAITING

void shuffle(int* arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = (i * 7 + 3) % (i + 1); // simple deterministic shuffle
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void game_init(void) {
    // Initialize card values (8 pairs: 0,0,1,1,...,7,7)
    int vals[] = CARD_VALUES;
    for (int i = 0; i < TOTAL_CARDS; i++) cards[i] = vals[i];
    shuffle(cards, TOTAL_CARDS);
    
    for (int i = 0; i < TOTAL_CARDS; i++) card_states[i] = STATE_HIDDEN;
    
    selected[0] = selected[1] = -1;
    num_selected = 0;
    matched_pairs = 0;
    moves = 0;
    wait_timer = 0;
    lock_input = 0;
    game_state = GAME_STATE_PLAYING;
}

void game_tick(void) {
    if (game_state == GAME_STATE_WAITING) {
        if (wait_timer > 0) {
            wait_timer--;
        } else {
            // Flip both cards back
            for (int i = 0; i < TOTAL_CARDS; i++) {
                if (card_states[i] == STATE_REVEALED) {
                    card_states[i] = STATE_HIDDEN;
                }
            }
            num_selected = 0;
            lock_input = 0;
            game_state = GAME_STATE_PLAYING;
        }
    }
}

int game_click(int card_index) {
    if (card_index < 0 || card_index >= TOTAL_CARDS) return 0;
    if (lock_input) return 0;
    if (card_states[card_index] != STATE_HIDDEN) return 0;
    
    // Reveal card
    card_states[card_index] = STATE_REVEALED;
    selected[num_selected++] = card_index;
    
    if (num_selected == 2) {
        moves++;
        int a = selected[0];
        int b = selected[1];
        
        if (cards[a] == cards[b]) {
            // Match found!
            card_states[a] = STATE_MATCHED;
            card_states[b] = STATE_MATCHED;
            matched_pairs++;
            num_selected = 0;
            
            if (matched_pairs == NUM_PAIRS) {
                game_state = GAME_STATE_WON;
            }
        } else {
            // No match - wait then flip back
            game_state = GAME_STATE_WAITING;
            lock_input = 1;
            wait_timer = 60; // ~1 second at 60fps
        }
    }
    return 1;
}

void game_restart(void) {
    game_init();
}

// Getters
int wasm_get_state(void) { return game_state; }
int wasm_get_moves(void) { return moves; }
int wasm_get_matched(void) { return matched_pairs; }
int wasm_get_card_state(int idx) { return (idx >= 0 && idx < TOTAL_CARDS) ? card_states[idx] : -1; }
int wasm_get_card_value(int idx) { return (idx >= 0 && idx < TOTAL_CARDS) ? cards[idx] : -1; }
int wasm_get_grid_w(void) { return GRID_W; }
int wasm_get_grid_h(void) { return GRID_H; }
