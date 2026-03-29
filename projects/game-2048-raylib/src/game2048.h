#ifndef GAME2048_H
#define GAME2048_H

#include <raylib.h>
#include <vector>
#include <string>

const int GRID_SIZE = 4;
const int CELL_SIZE = 100;
const int CELL_GAP = 10;
const int BOARD_PADDING = 20;
const int WINDOW_W = GRID_SIZE * CELL_SIZE + (GRID_SIZE + 1) * CELL_GAP + BOARD_PADDING * 2;
const int WINDOW_H = WINDOW_W + 100; // extra for score/title
const Color BG_COLOR = {250, 248, 239, 255};        // #FAF8EF
const Color CELL_BG = {204, 193, 173, 255};         // empty cell
const Color CELL_BORDER = {187, 173, 160, 255};

Color tileColor(int value);
Color tileTextColor(int value);

struct Game2048 {
    int board[GRID_SIZE][GRID_SIZE];
    int score;
    int bestScore;
    bool won;          // reached 2048
    bool gameOver;
    bool keepPlaying;  // after winning, keep going
    bool moved;        // animation trigger
    double spawnTimer; // for new tile animation
};

void initGame(Game2048* g);
void resetGame(Game2048* g);
void loadBestScore(Game2048* g);
void saveBestScore(Game2048* g);

bool canMove(const Game2048* g);
void addRandomTile(Game2048* g);
bool move(Game2048* g, int dir); // 0=up,1=down,2=left,3=right
void rotateCW(int a[GRID_SIZE][GRID_SIZE]);
void slideAndMerge(int row[GRID_SIZE], int* score);
bool boardEquals(const int a[GRID_SIZE][GRID_SIZE], const int b[GRID_SIZE][GRID_SIZE]);

void drawTile(int value, float x, float y, float size, double alpha);
void drawBoard(const Game2048* g);
void drawUI(const Game2048* g);
void drawGameOver(const Game2048* g);
void drawWinOverlay(void);

#endif
