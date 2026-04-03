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
    
    // Check bounds
    if (horizontal) {
        if (col + len > GRID_SIZE) return 0;
    } else {
        if (row + len > GRID_SIZE) return 0;
    }
    
    // Check overlap
    for (int i = 0; i < len; i++) {
        int r = horizontal ? row : row + i;
        int c = horizontal ? col + i : col;
        if (g->grid[r][c] != WATER) return 0;
    }
    
    // Place ship
    g->ships[shipIndex].pos.row = row;
    g->ships[shipIndex].pos.col = col;
    g->ships[shipIndex].horizontal = horizontal;
    
    for (int i = 0; i < len; i++) {
        int r = horizontal ? row : row + i;
        int c = horizontal ? col + i : col;
        g->grid[r][c] = SHIP;
    }
    
    g->currentShip++;
    if (g->currentShip >= NUM_SHIPS) {
        g->gameState = PLAYING;
    }
    return 1;
}

int fireAt(Game* g, int row, int col) {
    if (g->gameState != PLAYING) return -1;
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) return -1;
    if (g->enemyGrid[row][col] != WATER) return 0; // Already fired
    
    // Simple AI: random shots avoiding already tried
    int targetRow = row;
    int targetCol = col;
    
    if (g->enemyGrid[targetRow][targetCol] == WATER) {
        // Mark as fired
        if (g->grid[targetRow][targetCol] == SHIP) {
            g->enemyGrid[targetRow][targetCol] = HIT;
            g->enemyScore++;
            // Check if ship sunk
            for (int i = 0; i < NUM_SHIPS; i++) {
                int r = g->ships[i].pos.row;
                int c = g->ships[i].pos.col;
                int h = g->ships[i].horizontal;
                int len = SHIP_LENGTHS[i];
                int sunk = 1;
                for (int j = 0; j < len; j++) {
                    int rr = h ? r : r + j;
                    int cc = h ? c + j : c;
                    if (g->enemyGrid[rr][cc] != HIT) {
                        sunk = 0;
                        break;
                    }
                }
                if (sunk) {
                    g->enemyShipsSunk++;
                    break;
                }
            }
        } else {
            g->enemyGrid[targetRow][targetCol] = MISS;
        }
    }
    
    // Simple enemy AI: random shot
    int enemyRow = rand() % GRID_SIZE;
    int enemyCol = rand() % GRID_SIZE;
    while (g->grid[enemyRow][enemyCol] == HIT || g->grid[enemyRow][enemyCol] == MISS) {
        enemyRow = rand() % GRID_SIZE;
        enemyCol = rand() % GRID_SIZE;
    }
    
    if (g->grid[enemyRow][enemyCol] == SHIP) {
        g->grid[enemyRow][enemyCol] = HIT;
        g->playerScore++;
    } else {
        g->grid[enemyRow][enemyCol] = MISS;
    }
    
    return checkWin(g);
}

int checkWin(Game* g) {
    if (g->enemyShipsSunk >= NUM_SHIPS) return 1; // Player wins
    if (g->shipsSunk >= NUM_SHIPS) return 2;      // Enemy wins
    return 0;
}

void resetGame(Game* g) {
    initGame(g);
}
