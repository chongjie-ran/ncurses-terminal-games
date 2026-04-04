#include <emscripten.h>
#include "dominoes.h"
#include <stdlib.h>
#include <string.h>

static GameState g_game;

EMSCRIPTEN_KEEPALIVE
void newGame() {
    srand((unsigned int)((long)emscripten_get_now() % 10000));
    gameInit(&g_game);
    dealDominoes(&g_game);
    
    int highestDouble = -1;
    int firstPlayer = 0;
    for (int p = 0; p < MAX_PLAYERS; p++) {
        for (int i = 0; i < g_game.players[p].count; i++) {
            Domino* d = &g_game.players[p].hand[i];
            if (d->a == d->b && d->a > highestDouble) {
                highestDouble = d->a;
                firstPlayer = p;
            }
        }
    }
    g_game.currentPlayer = firstPlayer;
}

EMSCRIPTEN_KEEPALIVE
int getGameState(int* data) {
    data[0] = g_game.currentPlayer;
    data[1] = g_game.gameOver;
    data[2] = g_game.winner;
    data[3] = g_game.chainLength;
    data[4] = g_game.chainEnds.left;
    data[5] = g_game.chainEnds.right;
    data[6] = g_game.boneyardCount;
    data[7] = g_game.players[0].count;
    data[8] = g_game.players[1].count;
    data[9] = g_game.humanPassed;
    data[10] = g_game.aiPassed;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int getChainDomino(int pos, int* a, int* b) {
    if (pos < 0 || pos >= g_game.chainLength) return -1;
    int idx = g_game.chain[pos];
    int cnt = 0;
    for (int x = 0; x <= 6; x++) {
        for (int y = x; y <= 6; y++) {
            if (cnt == idx) { *a = x; *b = y; return 0; }
            cnt++;
        }
    }
    return -1;
}

EMSCRIPTEN_KEEPALIVE
int getHumanDomino(int pos, int* a, int* b) {
    if (pos < 0 || pos >= g_game.players[0].count) return -1;
    *a = g_game.players[0].hand[pos].a;
    *b = g_game.players[0].hand[pos].b;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int getAIDomino(int pos, int* a, int* b) {
    if (pos < 0 || pos >= g_game.players[1].count) return -1;
    *a = g_game.players[1].hand[pos].a;
    *b = g_game.players[1].hand[pos].b;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int canHumanPlay(int pos) {
    if (g_game.chainLength == 0) return 1;
    if (pos < 0 || pos >= g_game.players[0].count) return 0;
    Domino* d = &g_game.players[0].hand[pos];
    int left = g_game.chainEnds.left;
    int right = g_game.chainEnds.right;
    int canL = (d->a == left || d->b == left);
    int canR = (d->a == right || d->b == right);
    if (canL && canR) return 3;
    if (canL) return 1;
    if (canR) return 2;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int playHuman(int pos, int preferRight) {
    if (g_game.currentPlayer != 0) return -1;
    if (pos < 0 || pos >= g_game.players[0].count) return -1;
    
    Domino* d = &g_game.players[0].hand[pos];
    int left = g_game.chainEnds.left;
    int right = g_game.chainEnds.right;
    
    if (g_game.chainLength == 0) {
        g_game.chainEnds.left = d->a;
        g_game.chainEnds.right = d->b;
        g_game.chain[g_game.chainLength++] = d->index;
        for (int i = pos; i < g_game.players[0].count - 1; i++) {
            g_game.players[0].hand[i] = g_game.players[0].hand[i + 1];
        }
        g_game.players[0].count--;
        g_game.lastPlayedBy = 0;
        g_game.humanPassed = 0;
        checkGameOver(&g_game);
        if (!g_game.gameOver) g_game.currentPlayer = 1;
        return 0;
    }
    
    int canL = (d->a == left || d->b == left);
    int canR = (d->a == right || d->b == right);
    
    int played = 0;
    if (canL && (!canR || preferRight == 0)) {
        if (d->a == left) g_game.chainEnds.left = d->b;
        else g_game.chainEnds.left = d->a;
        g_game.chain[g_game.chainLength++] = d->index;
        played = 1;
    } else if (canR) {
        if (d->a == right) g_game.chainEnds.right = d->b;
        else g_game.chainEnds.right = d->a;
        g_game.chain[g_game.chainLength++] = d->index;
        played = 2;
    }
    
    if (!played) return -1;
    
    for (int i = pos; i < g_game.players[0].count - 1; i++) {
        g_game.players[0].hand[i] = g_game.players[0].hand[i + 1];
    }
    g_game.players[0].count--;
    g_game.lastPlayedBy = 0;
    g_game.humanPassed = 0;
    checkGameOver(&g_game);
    if (!g_game.gameOver) g_game.currentPlayer = 1;
    
    return played;
}

EMSCRIPTEN_KEEPALIVE
int drawTile() {
    if (g_game.currentPlayer != 0) return -1;
    if (g_game.boneyardCount == 0) return -1;
    if (g_game.chainLength > 0 && canPlayerPlay(&g_game, 0)) return -1;
    
    int dIdx = rand() % g_game.boneyardCount;
    Domino d = g_game.boneyard[dIdx];
    g_game.boneyard[dIdx] = g_game.boneyard[--g_game.boneyardCount];
    g_game.players[0].hand[g_game.players[0].count++] = d;
    
    g_game.currentPlayer = 1;
    return g_game.players[0].count - 1;
}

EMSCRIPTEN_KEEPALIVE
int aiTurn() {
    if (g_game.currentPlayer != 1) return -1;
    
    int move = aiChooseMove(&g_game);
    
    if (move >= 0) {
        Domino* d = &g_game.players[1].hand[move];
        
        if (g_game.chainLength == 0) {
            g_game.chainEnds.left = d->a;
            g_game.chainEnds.right = d->b;
            g_game.chain[g_game.chainLength++] = d->index;
        } else {
            int left = g_game.chainEnds.left;
            int right = g_game.chainEnds.right;
            int canL = (d->a == left || d->b == left);
            int canR = (d->a == right || d->b == right);
            
            if (canL) {
                if (d->a == left) g_game.chainEnds.left = d->b;
                else g_game.chainEnds.left = d->a;
                g_game.chain[g_game.chainLength++] = d->index;
            } else if (canR) {
                if (d->a == right) g_game.chainEnds.right = d->b;
                else g_game.chainEnds.right = d->a;
                g_game.chain[g_game.chainLength++] = d->index;
            }
        }
        
        for (int i = move; i < g_game.players[1].count - 1; i++) {
            g_game.players[1].hand[i] = g_game.players[1].hand[i + 1];
        }
        g_game.players[1].count--;
        g_game.aiPassed = 0;
        g_game.lastPlayedBy = 1;
    } else {
        if (g_game.boneyardCount > 0) {
            int dIdx = rand() % g_game.boneyardCount;
            Domino d = g_game.boneyard[dIdx];
            g_game.boneyard[dIdx] = g_game.boneyard[--g_game.boneyardCount];
            g_game.players[1].hand[g_game.players[1].count++] = d;
            g_game.aiPassed = 0;
        } else {
            g_game.aiPassed = 1;
        }
    }
    
    checkGameOver(&g_game);
    if (!g_game.gameOver) g_game.currentPlayer = 0;
    
    return move;
}
