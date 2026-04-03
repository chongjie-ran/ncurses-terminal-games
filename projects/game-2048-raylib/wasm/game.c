#include "game.h"
#include <stdlib.h>
#include <string.h>

static int rand_range(int n) {
    return rand() % n;
}

static void shuffle(int* arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand_range(i + 1);
        int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }
}

void Game2048_Init(struct Game2048* g) {
    memset(g, 0, sizeof(*g));
    g->state = STATE_MENU;
    g->bestScore = 0;
}

void Game2048_Reset(struct Game2048* g) {
    memset(g->board, 0, sizeof(g->board));
    g->score = 0;
    g->won = 0;
    g->moves = 0;
    g->state = STATE_PLAYING;
    Game2048_AddRandomTile(g);
    Game2048_AddRandomTile(g);
}

void Game2048_AddRandomTile(struct Game2048* g) {
    int empty[GRID_SIZE * GRID_SIZE];
    int cnt = 0;
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            if (g->board[r][c] == 0) empty[cnt++] = r * GRID_SIZE + c;
    if (cnt == 0) return;
    int idx = empty[rand_range(cnt)];
    g->board[idx / GRID_SIZE][idx % GRID_SIZE] = (rand_range(10) < 9) ? 2 : 4;
}

int boardFull(const int a[GRID_SIZE][GRID_SIZE]) {
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            if (a[r][c] == 0) return 0;
    return 1;
}

int canMergeRow(const int row[GRID_SIZE]) {
    for (int c = 0; c < GRID_SIZE - 1; c++)
        if (row[c] == row[c + 1]) return 1;
    return 0;
}

int Game2048_CanMove(const struct Game2048* g) {
    if (!boardFull(g->board)) return 1;
    for (int r = 0; r < GRID_SIZE; r++)
        if (canMergeRow(g->board[r])) return 1;
    int tmp[GRID_SIZE][GRID_SIZE];
    memcpy(tmp, g->board, sizeof(tmp));
    for (int c = 0; c < GRID_SIZE; c++) {
        int col[GRID_SIZE];
        for (int r = 0; r < GRID_SIZE; r++) col[r] = g->board[r][c];
        if (canMergeRow(col)) return 1;
    }
    return 0;
}

// Slide one row to the left, accumulate score
void slideRowLeft(int row[GRID_SIZE], int* score) {
    int dst[GRID_SIZE];
    int j = 0;
    // collect non-zero
    for (int i = 0; i < GRID_SIZE; i++)
        if (row[i] != 0) dst[j++] = row[i];
    while (j < GRID_SIZE) dst[j++] = 0;
    // merge
    for (int i = 0; i < GRID_SIZE - 1; i++) {
        if (dst[i] != 0 && dst[i] == dst[i + 1]) {
            dst[i] *= 2;
            *score += dst[i];
            // shift left
            for (int k = i + 1; k < GRID_SIZE - 1; k++) dst[k] = dst[k + 1];
            dst[GRID_SIZE - 1] = 0;
        }
    }
    for (int i = 0; i < GRID_SIZE; i++) row[i] = dst[i];
}

void rotateCW(int a[GRID_SIZE][GRID_SIZE]) {
    int tmp[GRID_SIZE][GRID_SIZE];
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            tmp[c][GRID_SIZE - 1 - r] = a[r][c];
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            a[r][c] = tmp[r][c];
}

int Game2048_Move(struct Game2048* g, int dir) {
    if (g->state != STATE_PLAYING) return 0;
    int prev[GRID_SIZE][GRID_SIZE];
    memcpy(prev, g->board, sizeof(prev));

    // Normalize to "slide left": rotate board so direction -> left
    int rot = dir; // 0=up,1=down,2=left,3=right
    // up=3, down=1 rotations CW
    if (dir == 0) rot = 3; // up
    else if (dir == 1) rot = 1; // down
    else if (dir == 2) rot = 0; // left
    else rot = 2; // right

    for (int i = 0; i < rot; i++) rotateCW(g->board);

    int oldScore = g->score;
    for (int r = 0; r < GRID_SIZE; r++)
        slideRowLeft(g->board[r], &g->score);

    for (int i = 0; i < rot; i++) rotateCW(g->board);
    rotateCW(g->board); rotateCW(g->board); rotateCW(g->board); // rotate CCW rot times

    if (g->score > oldScore) {
        if (g->score > g->bestScore) g->bestScore = g->score;
    }

    if (memcmp(prev, g->board, sizeof(prev)) == 0) return 0; // no change

    g->moves++;
    Game2048_AddRandomTile(g);

    // Check win: any cell == 2048
    if (!g->won) {
        for (int r = 0; r < GRID_SIZE; r++)
            for (int c = 0; c < GRID_SIZE; c++)
                if (g->board[r][c] == 2048) g->won = 1;
    }

    if (!Game2048_CanMove(g)) g->state = STATE_GAMEOVER;

    return 1;
}

int Game2048_GetState(const struct Game2048* g) { return g->state; }
int Game2048_GetScore(const struct Game2048* g) { return g->score; }
int Game2048_GetBestScore(const struct Game2048* g) { return g->bestScore; }
void Game2048_SetBestScore(struct Game2048* g, int score) { g->bestScore = score; }
int Game2048_GetCell(const struct Game2048* g, int row, int col) {
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) return 0;
    return g->board[row][col];
}
int Game2048_GetWon(const struct Game2048* g) { return g->won; }
