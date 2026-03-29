#include "game2048.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <cstring>

using namespace std;

// ── Color helpers ──────────────────────────────────────────────────────────────
Color tileColor(int v) {
    if (v <= 0)    return (Color){238, 228, 218, 255};
    if (v == 2)    return (Color){238, 228, 218, 255};
    if (v == 4)    return (Color){237, 224, 200, 255};
    if (v == 8)    return (Color){242, 177, 121, 255};
    if (v == 16)   return (Color){245, 149, 99, 255};
    if (v == 32)   return (Color){246, 124, 95, 255};
    if (v == 64)   return (Color){246, 94, 59, 255};
    if (v == 128)  return (Color){237, 207, 114, 255};
    if (v == 256)  return (Color){237, 204, 97, 255};
    if (v == 512)  return (Color){237, 200, 80, 255};
    if (v == 1024) return (Color){237, 197, 63, 255};
    if (v == 2048) return (Color){237, 194, 46, 255};
    return (Color){60, 58, 50, 255};
}

Color tileTextColor(int v) {
    return (v <= 4) ? (Color){119, 110, 101, 255} : (Color){255, 255, 255, 255};
}

// ── Board helpers ─────────────────────────────────────────────────────────────
void rotateCW(int a[GRID_SIZE][GRID_SIZE]) {
    int b[GRID_SIZE][GRID_SIZE];
    for (int r = 0; r < GRID_SIZE; ++r)
        for (int c = 0; c < GRID_SIZE; ++c)
            b[c][GRID_SIZE - 1 - r] = a[r][c];
    for (int r = 0; r < GRID_SIZE; ++r)
        for (int c = 0; c < GRID_SIZE; ++c)
            a[r][c] = b[r][c];
}

void slideAndMerge(int row[GRID_SIZE], int* score) {
    int tmp[GRID_SIZE] = {0};
    int j = 0;
    for (int i = 0; i < GRID_SIZE; ++i)
        if (row[i] != 0) tmp[j++] = row[i];
    for (int i = 0; i < GRID_SIZE - 1; ++i) {
        if (tmp[i] == tmp[i + 1] && tmp[i] != 0) {
            tmp[i] *= 2;
            *score += tmp[i];
            tmp[i + 1] = 0;
            for (int k = i + 1; k < GRID_SIZE - 1; ++k) tmp[k] = tmp[k + 1];
            tmp[GRID_SIZE - 1] = 0;
        }
    }
    j = 0;
    int tmp2[GRID_SIZE] = {0};
    for (int i = 0; i < GRID_SIZE; ++i)
        if (tmp[i] != 0) tmp2[j++] = tmp[i];
    for (int i = 0; i < GRID_SIZE; ++i) row[i] = tmp2[i];
}

bool boardEquals(const int a[GRID_SIZE][GRID_SIZE], const int b[GRID_SIZE][GRID_SIZE]) {
    for (int r = 0; r < GRID_SIZE; ++r)
        for (int c = 0; c < GRID_SIZE; ++c)
            if (a[r][c] != b[r][c]) return false;
    return true;
}

// ── Game logic ────────────────────────────────────────────────────────────────
void initGame(Game2048* g) {
    for (int r = 0; r < GRID_SIZE; ++r)
        for (int c = 0; c < GRID_SIZE; ++c)
            g->board[r][c] = 0;
    g->score = 0;
    g->won = false;
    g->gameOver = false;
    g->keepPlaying = false;
    g->moved = false;
    g->spawnTimer = 0.0;
    loadBestScore(g);
    addRandomTile(g);
    addRandomTile(g);
}

void resetGame(Game2048* g) {
    for (int r = 0; r < GRID_SIZE; ++r)
        for (int c = 0; c < GRID_SIZE; ++c)
            g->board[r][c] = 0;
    g->score = 0;
    g->won = false;
    g->gameOver = false;
    g->keepPlaying = false;
    g->moved = false;
    addRandomTile(g);
    addRandomTile(g);
}

void loadBestScore(Game2048* g) {
    ifstream f(".best2048");
    if (f.is_open()) { f >> g->bestScore; f.close(); }
    else g->bestScore = 0;
}

void saveBestScore(Game2048* g) {
    if (g->score > g->bestScore) {
        g->bestScore = g->score;
        ofstream f(".best2048");
        if (f.is_open()) { f << g->bestScore << "\n"; f.close(); }
    }
}

void addRandomTile(Game2048* g) {
    vector<pair<int,int>> empty;
    for (int r = 0; r < GRID_SIZE; ++r)
        for (int c = 0; c < GRID_SIZE; ++c)
            if (g->board[r][c] == 0) empty.push_back({r,c});
    if (empty.empty()) return;
    auto p = empty[rand() % empty.size()];
    g->board[p.first][p.second] = (rand() % 10 < 9) ? 2 : 4;
    g->spawnTimer = 0.3;
}

bool canMove(const Game2048* g) {
    for (int r = 0; r < GRID_SIZE; ++r)
        for (int c = 0; c < GRID_SIZE; ++c)
            if (g->board[r][c] == 0) return true;
    for (int r = 0; r < GRID_SIZE; ++r)
        for (int c = 0; c < GRID_SIZE - 1; ++c)
            if (g->board[r][c] == g->board[r][c+1]) return true;
    for (int r = 0; r < GRID_SIZE - 1; ++r)
        for (int c = 0; c < GRID_SIZE; ++c)
            if (g->board[r][c] == g->board[r+1][c]) return true;
    return false;
}

bool move(Game2048* g, int dir) {
    int saved[GRID_SIZE][GRID_SIZE];
    memcpy(saved, g->board, sizeof(g->board));

    int nrot = 0;
    if (dir == 0) nrot = 1;      // up
    else if (dir == 1) nrot = 3; // down
    else if (dir == 3) nrot = 2; // right
    // left: nrot = 0

    for (int i = 0; i < nrot; ++i) rotateCW(g->board);
    for (int r = 0; r < GRID_SIZE; ++r) slideAndMerge(g->board[r], &g->score);
    for (int i = 0; i < (4 - nrot) % 4; ++i) rotateCW(g->board);

    bool changed = !boardEquals(saved, g->board);
    if (changed) {
        saveBestScore(g);
        if (!g->won) {
            for (int r = 0; r < GRID_SIZE; ++r)
                for (int c = 0; c < GRID_SIZE; ++c)
                    if (g->board[r][c] >= 2048) g->won = true;
        }
        addRandomTile(g);
        if (!canMove(g)) g->gameOver = true;
    }
    g->moved = changed;
    return changed;
}

// ── Drawing ───────────────────────────────────────────────────────────────────
void drawTile(int value, float x, float y, float size, double alpha) {
    Color bg = tileColor(value);
    Color fg = tileTextColor(value);
    bg.a = (unsigned char)(alpha * 255);
    float radius = size * 0.08f;
    DrawRectangleRounded((Rectangle){x, y, size, size}, radius, 6, bg);
    if (value == 0) return;

    const char* txt;
    char buf[32];
    if (value <= 32768) {
        static const char* labels[] = {"0","2","4","8","16","32","64","128","256","512","1024","2048","4096","8192","16384","32768"};
        txt = labels[(int)log2(value)];
    } else {
        snprintf(buf, sizeof(buf), "%d", value);
        txt = buf;
    }

    int fontSize = (value < 100) ? 40 : (value < 1000) ? 36 : (value < 10000) ? 28 : 22;
    Vector2 tsize = MeasureTextEx(GetFontDefault(), txt, fontSize, 2);
    float tx = x + (size - tsize.x) * 0.5f;
    float ty = y + (size - tsize.y) * 0.5f;
    DrawText(txt, (int)tx, (int)ty, fontSize, fg);
}

void drawBoard(const Game2048* g) {
    float bx = BOARD_PADDING;
    float by = BOARD_PADDING + 60;
    DrawRectangleRounded((Rectangle){
        bx - 5, by - 5,
        GRID_SIZE * CELL_SIZE + (GRID_SIZE + 1) * CELL_GAP + 10,
        GRID_SIZE * CELL_SIZE + (GRID_SIZE + 1) * CELL_GAP + 10},
        12, 8, CELL_BORDER);

    for (int r = 0; r < GRID_SIZE; ++r) {
        for (int c = 0; c < GRID_SIZE; ++c) {
            float x = bx + CELL_GAP + c * (CELL_SIZE + CELL_GAP);
            float y = by + CELL_GAP + r * (CELL_SIZE + CELL_GAP);
            double alpha = (g->spawnTimer > 0 && g->board[r][c] != 0) ? 0.5 + 0.5 * (1 - g->spawnTimer / 0.3) : 1.0;
            drawTile(g->board[r][c], x, y, CELL_SIZE, alpha);
        }
    }
}

void drawUI(const Game2048* g) {
    DrawText("2048", 20, 15, 40, (Color){119, 110, 101, 255});

    DrawRectangleRounded((Rectangle){WINDOW_W - 220, 10, 100, 50}, 6, 4, (Color){187, 173, 160, 255});
    DrawText("SCORE", WINDOW_W - 215, 14, 16, (Color){238, 228, 218, 255});
    DrawText(TextFormat("%d", g->score), WINDOW_W - 215, 32, 20, (Color){255, 255, 255, 255});

    DrawRectangleRounded((Rectangle){WINDOW_W - 110, 10, 100, 50}, 6, 4, (Color){187, 173, 160, 255});
    DrawText("BEST", WINDOW_W - 105, 14, 16, (Color){238, 228, 218, 255});
    DrawText(TextFormat("%d", g->bestScore), WINDOW_W - 105, 32, 20, (Color){255, 255, 255, 255});
}

void drawGameOver(const Game2048* g) {
    DrawRectangle(0, 0, WINDOW_W, WINDOW_H, (Color){238, 228, 218, 200});
    DrawText("Game Over!", WINDOW_W/2 - MeasureText("Game Over!", 48)/2, WINDOW_H/2 - 80, 48, (Color){119, 110, 101, 255});
    DrawText(TextFormat("Score: %d", g->score), WINDOW_W/2 - MeasureText(TextFormat("Score: %d", g->score), 24)/2, WINDOW_H/2, 24, (Color){119, 110, 101, 255});
    DrawText("Press R to restart", WINDOW_W/2 - MeasureText("Press R to restart", 20)/2, WINDOW_H/2 + 50, 20, (Color){119, 110, 101, 180});
}

void drawWinOverlay(void) {
    DrawRectangle(0, 0, WINDOW_W, WINDOW_H, (Color){250, 200, 80, 180});
    DrawText("You Win!", WINDOW_W/2 - MeasureText("You Win!", 48)/2, WINDOW_H/2 - 40, 48, (Color){255, 255, 255, 255});
    DrawText("Press C to continue", WINDOW_W/2 - MeasureText("Press C to continue", 20)/2, WINDOW_H/2 + 20, 20, (Color){255, 255, 255, 255});
    DrawText("Press R to restart", WINDOW_W/2 - MeasureText("Press R to restart", 20)/2, WINDOW_H/2 + 50, 20, (Color){255, 255, 255, 180});
}
