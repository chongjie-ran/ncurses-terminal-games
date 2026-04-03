#ifndef GAME_H
#define GAME_H

#define GRID_SIZE 10
#define NUM_SHIPS 5

// Ship lengths: Carrier=5, Battleship=4, Cruiser=3, Submarine=3, Destroyer=2
extern const int SHIP_LENGTHS[NUM_SHIPS];

// Cell states
#define WATER 0
#define HIT 1
#define MISS 2
#define SHIP 3   // ship cell (not hit yet)

// Game states
#define PLACING 0
#define PLAYING 1
#define GAME_OVER 2

typedef struct {
    int row, col;
} Position;

typedef struct {
    int length;
    int health;
    Position pos;
    int horizontal;
} Ship;

typedef struct {
    int grid[GRID_SIZE][GRID_SIZE];
    int enemyGrid[GRID_SIZE][GRID_SIZE];
    Ship ships[NUM_SHIPS];
    int currentShip;
    int gameState;
    int playerScore;
    int enemyScore;
    int shipsSunk;
    int enemyShipsSunk;
} Game;

void initGame(Game* g);
int placeShip(Game* g, int shipIndex, int row, int col, int horizontal);
int fireAt(Game* g, int row, int col);
int checkWin(Game* g);
void resetGame(Game* g);

#endif
