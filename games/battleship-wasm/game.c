#include "game.h"
#include <stdlib.h>
#include <string.h>

const int SHIP_LENGTHS[NUM_SHIPS] = {5, 4, 3, 3, 2};

void initGame(Game* g) {
    memset(g->grid, WATER, sizeof(g->grid));
    memset(g->enemyGrid, WATER, sizeof(g->enemyGrid));
    for (int i = 0; i < NUM_SHIPS; i++) {
        g->ships[i].length = SHIP_LENGTHS[i];
        g->ships[i].health = SHIP_LENGTHS[i];
        g->ships[i].pos.row = -1;
        g->ships[i].pos.col = -1;
        g->ships[i].horizontal = 1;
    }
    g->currentShip = 0;
    g->gameState = PLACING;
    g->playerScore = 0;
    g->enemyScore = 0;
    g->shipsSunk = 0;
    g->enemyShipsSunk = 0;
}

int placeShip(Game* g, int shipIndex, int row, int col, int horizontal) {
    if (shipIndex < 0 || shipIndex >= NUM_SHIPS) return 0;
    if (g->gameState != PLACING) return 0;
    
    int len = SHIP_LENGTHS[shipIndex];
    if (horizontal) {
        if (col + len > GRID_SIZE) return 0;
    } else {
        if (row + len > GRID_SIZE) return 0;
    }
    for (int i = 0; i < len; i++) {
        int r = horizontal ? row : row + i;
        int c = horizontal ? col + i : col;
        if (g->grid[r][c] != WATER) return 0;
    }
    g->ships[shipIndex].pos.row = row;
    g->ships[shipIndex].pos.col = col;
    g->ships[shipIndex].horizontal = horizontal;
    for (int i = 0; i < len; i++) {
        int r = horizontal ? row : row + i;
        int c = horizontal ? col + i : col;
        g->grid[r][c] = SHIP;
    }
    g->currentShip++;
    if (g->currentShip >= NUM_SHIPS) g->gameState = PLAYING;
    return 1;
}

int fireAt(Game* g, int row, int col) {
    if (g->gameState != PLAYING) return -1;
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) return -1;
    if (g->enemyGrid[row][col] != WATER) return 0;
    
    // Player fires at enemy ship
    int hit = 0;
    if (g->enemyGrid[row][col] == WATER) {
        g->enemyGrid[row][col] = (g->grid[row][col] == SHIP) ? HIT : MISS;
        if (g->enemyGrid[row][col] == HIT) {
            g->enemyScore++;
            hit = 1;
        }
    }
    
    // Enemy AI: hunt mode (target cells adjacent to hits)
    int er, ec;
    do {
        er = rand() % GRID_SIZE;
        ec = rand() % GRID_SIZE;
    } while (g->grid[er][ec] == HIT || g->grid[er][ec] == MISS);
    
    if (g->grid[er][ec] == SHIP) {
        g->grid[er][ec] = HIT;
        g->playerScore++;
    } else {
        g->grid[er][ec] = MISS;
    }
    
    return checkWin(g);
}

int checkWin(Game* g) {
    // Count actual sunk ships
    int enemySunk = 0, playerSunk = 0;
    for (int i = 0; i < NUM_SHIPS; i++) {
        int r = g->ships[i].pos.row;
        int c = g->ships[i].pos.col;
        int h = g->ships[i].horizontal;
        int len = SHIP_LENGTHS[i];
        if (r < 0) continue;
        int sunk = 1;
        for (int j = 0; j < len; j++) {
            int rr = h ? r : r + j;
            int cc = h ? c + j : c;
            if (g->enemyGrid[rr][cc] != HIT) { sunk = 0; break; }
        }
        if (sunk) enemySunk++;
    }
    g->enemyShipsSunk = enemySunk;
    
    // Player ships sunk
    int sunkCount = 0;
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            if (g->grid[r][c] == HIT) sunkCount++;
    int total = 0;
    for (int i = 0; i < NUM_SHIPS; i++) total += SHIP_LENGTHS[i];
    g->shipsSunk = sunkCount >= total ? NUM_SHIPS : 0;
    for (int i = 0; i < NUM_SHIPS; i++) {
        int r = g->ships[i].pos.row, c = g->ships[i].pos.col;
        int h = g->ships[i].horizontal, len = SHIP_LENGTHS[i];
        if (r < 0) continue;
        int sunk = 1;
        for (int j = 0; j < len; j++) {
            int rr = h ? r : r + j, cc = h ? c + j : c;
            if (g->grid[rr][cc] != HIT) { sunk = 0; break; }
        }
        if (sunk) playerSunk++;
    }
    g->shipsSunk = playerSunk;
    
    if (enemySunk >= NUM_SHIPS) return 1;
    if (playerSunk >= NUM_SHIPS) return 2;
    return 0;
}

void resetGame(Game* g) { initGame(g); }
