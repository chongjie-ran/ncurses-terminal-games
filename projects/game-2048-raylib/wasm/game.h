#ifndef GAME2048_H
#define GAME2048_H

#define GRID_SIZE 4

enum GameState { STATE_MENU = 0, STATE_PLAYING = 1, STATE_GAMEOVER = 2, STATE_WIN = 3 };

struct Game2048 {
    int board[GRID_SIZE][GRID_SIZE];
    int score;
    int bestScore;
    int state;     // 0=MENU,1=PLAYING,2=GAMEOVER,3=WIN
    int won;       // reached 2048
    int moves;     // move counter
};

// Core game logic (pure C, no raylib)
void Game2048_Init(struct Game2048* g);
void Game2048_Reset(struct Game2048* g);
void Game2048_AddRandomTile(struct Game2048* g);
int  Game2048_Move(struct Game2048* g, int dir); // 0=up,1=down,2=left,3=right; returns 1 if moved
int  Game2048_CanMove(const struct Game2048* g);
int  Game2048_GetState(const struct Game2048* g);
int  Game2048_GetScore(const struct Game2048* g);
int  Game2048_GetBestScore(const struct Game2048* g);
void Game2048_SetBestScore(struct Game2048* g, int score);
int  Game2048_GetCell(const struct Game2048* g, int row, int col);
int  Game2048_GetWon(const struct Game2048* g);

// Internal helpers
void slideRowLeft(int row[GRID_SIZE], int* score);
void rotateCW(int a[GRID_SIZE][GRID_SIZE]);
int boardFull(const int a[GRID_SIZE][GRID_SIZE]);
int canMergeRow(const int row[GRID_SIZE]);

#endif
