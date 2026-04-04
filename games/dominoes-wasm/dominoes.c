#include "dominoes.h"
#include <stdlib.h>
#include <string.h>

// Fisher-Yates shuffle
void shuffleDominoes(int* indices, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }
}

void gameInit(GameState* g) {
    memset(g, 0, sizeof(GameState));
    g->gameOver = 0;
    g->winner = -1;
    g->currentPlayer = -1;
    g->humanPassed = 0;
    g->aiPassed = 0;
    g->chainLength = 0;
    g->boneyardCount = 0;
    
    int idx = 0;
    for (int a = 0; a <= 6; a++) {
        for (int b = a; b <= 6; b++) {
            g->boneyard[g->boneyardCount].a = a;
            g->boneyard[g->boneyardCount].b = b;
            g->boneyard[g->boneyardCount].index = idx;
            g->boneyardCount++;
            idx++;
        }
    }
}

int dealDominoes(GameState* g) {
    int all[TOTAL_DOMINOES];
    for (int i = 0; i < TOTAL_DOMINOES; i++) all[i] = i;
    shuffleDominoes(all, TOTAL_DOMINOES);
    
    for (int p = 0; p < MAX_PLAYERS; p++) {
        g->players[p].count = HAND_SIZE;
        for (int i = 0; i < HAND_SIZE; i++) {
            int dIdx = all[p * HAND_SIZE + i];
            int a = 0, b = 0, cnt = 0;
            for (int x = 0; x <= 6; x++) {
                for (int y = x; y <= 6; y++) {
                    if (cnt == dIdx) { a = x; b = y; }
                    cnt++;
                }
            }
            g->players[p].hand[i].a = a;
            g->players[p].hand[i].b = b;
            g->players[p].hand[i].index = dIdx;
        }
    }
    
    g->boneyardCount = TOTAL_DOMINOES - MAX_PLAYERS * HAND_SIZE;
    return 0;
}

int isDouble(int a, int b) {
    return a == b;
}

int findHighestDouble(int* hand, int count) {
    int best = -1;
    for (int i = 0; i < count; i++) {
        int a = hand[i] >> 16 & 0xFF;
        int b = hand[i] & 0xFF;
        if (a == b && a > best) best = a;
    }
    return best;
}

int encodeDomino(int a, int b) {
    return (a << 16) | b;
}

int decodeA(int code) {
    return (code >> 16) & 0xFF;
}

int decodeB(int code) {
    return code & 0xFF;
}

int canPlayLeft(GameState* g, int dominoIdx) {
    if (g->chainLength == 0) return 1;
    Domino* d = &g->players[0].hand[dominoIdx];
    int left = g->chainEnds.left;
    return (d->a == left || d->b == left);
}

int canPlayRight(GameState* g, int dominoIdx) {
    if (g->chainLength == 0) return 1;
    Domino* d = &g->players[0].hand[dominoIdx];
    int right = g->chainEnds.right;
    return (d->a == right || d->b == right);
}

int playLeft(GameState* g, int player, int dominoIdx) {
    Domino* d = &g->players[player].hand[dominoIdx];
    
    if (g->chainLength == 0) {
        g->chainEnds.left = d->a;
        g->chainEnds.right = d->b;
    } else {
        if (d->a == g->chainEnds.left) {
            g->chainEnds.left = d->b;
        } else if (d->b == g->chainEnds.left) {
            g->chainEnds.left = d->a;
        } else {
            return -1;
        }
    }
    
    g->chain[g->chainLength++] = d->index;
    
    for (int i = dominoIdx; i < g->players[player].count - 1; i++) {
        g->players[player].hand[i] = g->players[player].hand[i + 1];
    }
    g->players[player].count--;
    
    g->lastPlayedBy = player;
    return 0;
}

int playRight(GameState* g, int player, int dominoIdx) {
    Domino* d = &g->players[player].hand[dominoIdx];
    
    if (g->chainLength == 0) {
        g->chainEnds.left = d->a;
        g->chainEnds.right = d->b;
    } else {
        if (d->a == g->chainEnds.right) {
            g->chainEnds.right = d->b;
        } else if (d->b == g->chainEnds.right) {
            g->chainEnds.right = d->a;
        } else {
            return -1;
        }
    }
    
    g->chain[g->chainLength++] = d->index;
    
    for (int i = dominoIdx; i < g->players[player].count - 1; i++) {
        g->players[player].hand[i] = g->players[player].hand[i + 1];
    }
    g->players[player].count--;
    
    g->lastPlayedBy = player;
    return 0;
}

int drawFromBoneyard(GameState* g, int player) {
    if (g->boneyardCount <= 0) return -1;
    
    int dIdx = rand() % g->boneyardCount;
    Domino d = g->boneyard[dIdx];
    
    g->boneyard[dIdx] = g->boneyard[--g->boneyardCount];
    g->players[player].hand[g->players[player].count++] = d;
    
    return 0;
}

int canPlayerPlay(GameState* g, int player) {
    if (g->chainLength == 0) return 1;
    for (int i = 0; i < g->players[player].count; i++) {
        Domino* d = &g->players[player].hand[i];
        if (d->a == g->chainEnds.left || d->b == g->chainEnds.left ||
            d->a == g->chainEnds.right || d->b == g->chainEnds.right) {
            return 1;
        }
    }
    return 0;
}

int aiChooseMove(GameState* g) {
    if (g->chainLength == 0) {
        int bestIdx = -1;
        int bestVal = -1;
        for (int i = 0; i < g->players[1].count; i++) {
            Domino* d = &g->players[1].hand[i];
            if (d->a == d->b && d->a > bestVal) {
                bestVal = d->a;
                bestIdx = i;
            }
        }
        return bestIdx;
    }
    
    int pips[7] = {0};
    for (int i = 0; i < g->players[1].count; i++) {
        Domino* d = &g->players[1].hand[i];
        pips[d->a]++;
        pips[d->b]++;
    }
    
    int left = g->chainEnds.left;
    int right = g->chainEnds.right;
    
    int playableLeft = -1, playableRight = -1;
    for (int i = 0; i < g->players[1].count; i++) {
        Domino* d = &g->players[1].hand[i];
        if (d->a == left || d->b == left) playableLeft = i;
        if (d->a == right || d->b == right) playableRight = i;
    }
    
    if (playableLeft >= 0 && playableRight >= 0) {
        if (pips[left] <= pips[right]) return playableLeft;
        else return playableRight;
    }
    if (playableLeft >= 0) return playableLeft;
    if (playableRight >= 0) return playableRight;
    
    return -1;
}

void checkGameOver(GameState* g) {
    for (int p = 0; p < MAX_PLAYERS; p++) {
        if (g->players[p].count == 0) {
            g->gameOver = 1;
            g->winner = p;
            return;
        }
    }
    
    if (g->boneyardCount == 0) {
        int p0CanPlay = canPlayerPlay(g, 0);
        int p1CanPlay = canPlayerPlay(g, 1);
        if (!p0CanPlay && !p1CanPlay) {
            g->gameOver = 1;
            if (g->players[0].count < g->players[1].count) g->winner = 0;
            else if (g->players[1].count < g->players[0].count) g->winner = 1;
            else g->winner = -1;
        }
    }
}
