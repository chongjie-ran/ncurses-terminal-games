#ifndef DOMINOES_H
#define DOMINOES_H

#define MAX_PLAYERS 2
#define HAND_SIZE 7
#define TOTAL_DOMINOES 28
#define MAX_BONEYARD 24
#define MAX_CHAIN 60

typedef struct {
    int a, b;  // pip values, a <= b
    int index; // original domino index
} Domino;

typedef struct {
    int left;
    int right;
} ChainEnds;

typedef struct {
    Domino hand[HAND_SIZE];
    int count;
} Player;

typedef struct {
    Player players[MAX_PLAYERS];
    Domino boneyard[MAX_BONEYARD];
    int boneyardCount;
    ChainEnds chainEnds;
    int chainLength;
    int chain[MAX_CHAIN]; // domino indices
    int currentPlayer;
    int gameOver;
    int winner; // -1 = ongoing, 0 = human, 1 = AI
    int lastPlayedBy; // who played last
    int humanPassed;  // did human pass last turn
    int aiPassed;     // did AI pass last turn
} GameState;

void gameInit(GameState* g);
void shuffleDominoes(int* indices, int n);
int dealDominoes(GameState* g);
int findDouble(int* hand, int count);
void placeFirst(GameState* g, int dominoIdx);
int canPlay(GameState* g, int player);
int canPlayLeft(GameState* g, int dominoIdx);
int canPlayRight(GameState* g, int dominoIdx);
int playLeft(GameState* g, int player, int dominoIdx);
int playRight(GameState* g, int player, int dominoIdx);
int drawFromBoneyard(GameState* g, int player);
int aiChooseMove(GameState* g);
int canPlayerPlay(GameState* g, int player);
void checkGameOver(GameState* g);
int dominoCompare(const void* a, const void* b);

#endif
