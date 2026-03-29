// Tetris — Raylib Graphics Version
// Developed by 悟通 (Developer Agent) on 2026-03-30
// SRS (Super Rotation System) with wall kick support

#include "raylib.h"
#include <vector>
#include <cstring>
#include <cstdlib>

// ── Constants ──────────────────────────────────────────────
#define CELL  30
#define COLS  10
#define ROWS  20
#define WIDTH  (COLS * CELL)
#define HEIGHT (ROWS * CELL)
#define BOARD_X 60
#define BOARD_Y 40

// Tetromino colors
static const Color TET_COLORS[8] = {
    {30,  30,  40,  255},  // 0: empty
    {0,   240, 255, 255},  // I - cyan
    {255, 240, 0,   255},  // O - yellow
    {160, 0,   255, 255},  // T - purple
    {0,   255, 0,   255},  // S - green
    {255, 0,   0,   255},  // Z - red
    {0,   0,   200, 255},  // J - blue
    {255, 130, 0,   255},  // L - orange
};

// ── Tetromino shapes (4 rotations × 4×4) ─────────────────
// Format: PIECES[type][rotation][row][col] = 0/1
static const unsigned char PIECES[7][4][4][4] = {
    // I
    {{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
     {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
     {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}},
     {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}},
    // O
    {{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},
    // T
    {{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
     {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
     {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}},
    // S
    {{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
     {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
     {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}},
    // Z
    {{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
     {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
     {{0,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0}}},
    // J
    {{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},
     {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}},
    // L
    {{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
     {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},
     {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
     {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}},
};

// ── SRS Wall Kick Data ────────────────────────────────────
// Format: WALL_KICKS[piece_group][from_rotation][kick_index][x_or_y]
// piece_group: 0=JLSTZ, 1=I, 2=O
// from_rotation: 0,1,2,3 (current state)
// kick_index: 0-4 (up to 5 test positions)
// x_or_y: 0=x, 1=y
// Each row has 5 (x,y) test pairs for clockwise rotation
static const int WALL_KICKS[3][4][5][2] = {
    // JLSTZ wall kicks
    {
        {{ 0, 0}, {-1, 0}, {-1, 1}, { 0,-2}, {-1,-2}}, // 0->1
        {{ 0, 0}, { 1, 0}, { 1,-1}, { 0, 2}, { 1, 2}}, // 1->2
        {{ 0, 0}, { 1, 0}, { 1, 1}, { 0,-2}, { 1,-2}}, // 2->3
        {{ 0, 0}, {-1, 0}, {-1,-1}, { 0, 2}, {-1, 2}}, // 3->0
    },
    // I wall kicks
    {
        {{ 0, 0}, {-2, 0}, { 1, 0}, {-2,-1}, { 1, 2}},
        {{ 0, 0}, {-1, 0}, { 2, 0}, {-1, 2}, { 2,-1}},
        {{ 0, 0}, { 2, 0}, {-1, 0}, { 2, 1}, {-1,-2}},
        {{ 0, 0}, { 1, 0}, {-2, 0}, { 1,-2}, {-2, 1}},
    },
    // O — no kicks (just offset adjustment in game logic)
    {
        {{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}},
        {{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}},
        {{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}},
        {{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}},
    },
};

// ── Game State ─────────────────────────────────────────────
struct Piece {
    int type;    // 0-6: I,O,T,S,Z,J,L
    int rot;     // 0-3
    int x, y;    // top-left of 4×4 bounding box
};

struct Game {
    int board[ROWS][COLS];
    Piece cur, nxt;
    int score, level, lines;
    bool over;
    float dropTimer, dropInterval;
    int bag[7];
    int bagPos;
};

static Game G;

// ── 7-Bag Randomizer ───────────────────────────────────────
static void shuffleBag() {
    for (int i = 0; i < 7; ++i) G.bag[i] = i;
    for (int i = 6; i > 0; --i) {
        int j = GetRandomValue(0, i);
        int t = G.bag[i]; G.bag[i] = G.bag[j]; G.bag[j] = t;
    }
    G.bagPos = 0;
}

static int nextFromBag() {
    if (G.bagPos >= 7) shuffleBag();
    return G.bag[G.bagPos++];
}

// ── Piece helpers ───────────────────────────────────────────
static inline bool cellGet(int type, int rot, int c, int r) {
    return PIECES[type][rot][r][c];
}

static bool validPos(int type, int rot, int px, int py) {
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (cellGet(type, rot, c, r)) {
                int bx = px + c;
                int by = py + r;
                if (bx < 0 || bx >= COLS) return false;
                if (by >= ROWS) return false;
                if (by >= 0 && G.board[by][bx] != 0) return false;
            }
    return true;
}

static void lockPiece() {
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (cellGet(G.cur.type, G.cur.rot, c, r)) {
                int bx = G.cur.x + c;
                int by = G.cur.y + r;
                if (by >= 0 && by < ROWS && bx >= 0 && bx < COLS)
                    G.board[by][bx] = G.cur.type + 1;
            }

    int cleared = 0;
    for (int rr = ROWS - 1; rr >= 0; --rr) {
        bool full = true;
        for (int cc = 0; cc < COLS; ++cc)
            if (G.board[rr][cc] == 0) { full = false; break; }
        if (full) {
            ++cleared;
            for (int r2 = rr; r2 > 0; --r2)
                memcpy(G.board[r2], G.board[r2-1], sizeof(G.board[r2]));
            memset(G.board[0], 0, sizeof(G.board[0]));
            ++rr;
        }
    }

    if (cleared > 0) {
        static const int LINE_PTS[] = {0,100,300,500,800};
        G.score += LINE_PTS[cleared] * G.level;
        G.lines += cleared;
        G.level = G.lines / 10 + 1;
        G.dropInterval = (G.dropInterval > 0.05f) ? G.dropInterval - 0.002f * cleared : 0.05f;
    }

    G.cur = G.nxt;
    G.nxt.type = nextFromBag();
    G.nxt.rot = 0; G.nxt.x = 3;
    G.nxt.y = (G.nxt.type == 0) ? -1 : 0;
    G.cur.rot = 0; G.cur.x = 3;
    G.cur.y = (G.cur.type == 0) ? -1 : 0;

    if (!validPos(G.cur.type, G.cur.rot, G.cur.x, G.cur.y))
        G.over = true;
}

static int ghostY() {
    int gy = G.cur.y;
    while (validPos(G.cur.type, G.cur.rot, G.cur.x, gy + 1)) ++gy;
    return gy;
}

static void spawnFirst() {
    G.nxt.type = nextFromBag();
    G.nxt.rot = 0; G.nxt.x = 3;
    G.nxt.y = (G.nxt.type == 0) ? -1 : 0;
    G.cur.type = nextFromBag();
    G.cur.rot = 0; G.cur.x = 3;
    G.cur.y = (G.cur.type == 0) ? -1 : 0;
}

// ── Rotation ────────────────────────────────────────────────
static int kickGroup() {
    if (G.cur.type == 0) return 1; // I
    if (G.cur.type == 1) return 2; // O
    return 0;                        // JLSTZ
}

static bool tryRotate(int newRot) {
    int grp = kickGroup();
    const int (*kicks)[2] = WALL_KICKS[grp][G.cur.rot];
    for (int i = 0; i < 5; ++i) {
        int nx = G.cur.x + kicks[i][0];
        int ny = G.cur.y + kicks[i][1];
        // O piece: adjust offset
        if (G.cur.type == 1) { nx -= 1; ny += 1; }
        if (validPos(G.cur.type, newRot, nx, ny)) {
            G.cur.rot = newRot;
            G.cur.x = nx;
            G.cur.y = ny;
            return true;
        }
    }
    return false;
}

static void tryMove(int dx, int dy) {
    if (validPos(G.cur.type, G.cur.rot, G.cur.x + dx, G.cur.y + dy)) {
        G.cur.x += dx;
        G.cur.y += dy;
    }
}

// ── Input ──────────────────────────────────────────────────
static void handleInput() {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_X))
        tryRotate((G.cur.rot + 1) % 4);
    if (IsKeyPressed(KEY_Z) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT_CONTROL))
        tryRotate((G.cur.rot + 3) % 4);

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_H))
        tryMove(-1, 0);
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_L))
        tryMove(1, 0);

    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        if (validPos(G.cur.type, G.cur.rot, G.cur.x, G.cur.y + 1)) {
            ++G.cur.y;
            G.score += 1;
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        int gy = ghostY();
        G.score += (gy - G.cur.y) * 2;
        G.cur.y = gy;
        lockPiece();
    }

    if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
        int tmp = G.cur.type;
        G.cur.type = G.nxt.type;
        G.nxt.type = tmp;
        G.cur.rot = 0; G.cur.x = 3;
        G.cur.y = (G.cur.type == 0) ? -1 : 0;
        if (!validPos(G.cur.type, G.cur.rot, G.cur.x, G.cur.y))
            G.over = true;
    }
}

// ── Draw helpers ─────────────────────────────────────────────
static void drawCell(int col, int row, int colorIdx, float alpha) {
    if (row < 0) return;
    Color c = TET_COLORS[colorIdx];
    c.a = (unsigned char)(alpha * 255);
    int px = BOARD_X + col * CELL;
    int py = BOARD_Y + row * CELL;
    DrawRectangle(px+1, py+1, CELL-2, CELL-2, c);
    // Inner border highlight
    Color hi = {255,255,255,(unsigned char)(alpha*50)};
    DrawRectangleLinesEx((Rectangle){(float)px+2,(float)py+2,(float)CELL-4,(float)CELL-4}, 1, hi);
}

static void drawBoard() {
    DrawRectangle(BOARD_X, BOARD_Y, WIDTH, HEIGHT, (Color){15,15,25,255});
    for (int r = 0; r <= ROWS; ++r)
        DrawLine(BOARD_X, BOARD_Y+r*CELL, BOARD_X+WIDTH, BOARD_Y+r*CELL, (Color){35,35,50,255});
    for (int c = 0; c <= COLS; ++c)
        DrawLine(BOARD_X+c*CELL, BOARD_Y, BOARD_X+c*CELL, BOARD_Y+HEIGHT, (Color){35,35,50,255});
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            if (G.board[r][c] != 0)
                drawCell(c, r, G.board[r][c], 1.0f);
}

static void drawGhost() {
    int gy = ghostY();
    if (gy == G.cur.y) return;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (cellGet(G.cur.type, G.cur.rot, c, r))
                drawCell(G.cur.x + c, gy + r, G.cur.type + 1, 0.2f);
}

static void drawCurrent() {
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (cellGet(G.cur.type, G.cur.rot, c, r))
                drawCell(G.cur.x + c, G.cur.y + r, G.cur.type + 1, 1.0f);
}

static void drawNext() {
    int nx = BOARD_X + WIDTH + 30;
    int ny = BOARD_Y + 10;
    DrawText("NEXT", nx, ny, 20, WHITE);
    ny += 28;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (PIECES[G.nxt.type][0][r][c])
                DrawRectangle(nx + c*20, ny + r*20, 18, 18, TET_COLORS[G.nxt.type+1]);
}

static void drawInfo() {
    int ix = BOARD_X + WIDTH + 30;
    int iy = BOARD_Y + 170;
    char buf[64];

    snprintf(buf, sizeof(buf), "SCORE");
    DrawText(buf, ix, iy, 16, GRAY); iy += 22;
    snprintf(buf, sizeof(buf), "%d", G.score);
    DrawText(buf, ix, iy, 22, WHITE); iy += 46;

    snprintf(buf, sizeof(buf), "LEVEL");
    DrawText(buf, ix, iy, 16, GRAY); iy += 22;
    snprintf(buf, sizeof(buf), "%d", G.level);
    DrawText(buf, ix, iy, 22, WHITE); iy += 46;

    snprintf(buf, sizeof(buf), "LINES");
    DrawText(buf, ix, iy, 16, GRAY); iy += 22;
    snprintf(buf, sizeof(buf), "%d", G.lines);
    DrawText(buf, ix, iy, 22, WHITE);
}

static void drawControls() {
    int cx = BOARD_X + WIDTH + 30;
    int cy = BOARD_Y + HEIGHT - 200;
    const char* lines[] = {
        "CONTROLS",
        "LEFT/RIGHT  Move",
        "UP/X        Rotate R",
        "Z/CTRL      Rotate L",
        "DOWN/S      Soft drop",
        "SPACE       Hard drop",
        "C/SHIFT     Hold",
        "R           Restart",
    };
    for (size_t i = 0; i < 8; ++i) {
        Color col = (i == 0) ? WHITE : (Color){140,140,140,255};
        DrawText(lines[i], cx, cy, 14, col);
        cy += 22;
    }
}

static void drawGameOver() {
    DrawRectangle(BOARD_X + 20, BOARD_Y + HEIGHT/2 - 55, WIDTH - 40, 110, (Color){0,0,0,210});
    const char* msg = "GAME OVER";
    int tw = MeasureText(msg, 30);
    DrawText(msg, BOARD_X + (WIDTH - tw)/2, BOARD_Y + HEIGHT/2 - 40, 30, RED);
    const char* sub = "Press R to restart";
    int sw = MeasureText(sub, 16);
    DrawText(sub, BOARD_X + (WIDTH - sw)/2, BOARD_Y + HEIGHT/2 + 5, 16, GRAY);
}

static void drawStartScreen() {
    const char* title = "TETRIS";
    int tw = MeasureText(title, 64);
    DrawText(title, (GetScreenWidth()-tw)/2, 80, 64, WHITE);

    const char* sub = "Press ENTER to start";
    int sw = MeasureText(sub, 20);
    DrawText(sub, (GetScreenWidth()-sw)/2, 200, 20, GRAY);

    const char* hint = "SRS Wall Kick + Ghost Piece + Bag Randomizer";
    int hw = MeasureText(hint, 14);
    DrawText(hint, (GetScreenWidth()-hw)/2, 240, 14, (Color){100,100,100,255});
}

static void initGame() {
    memset(&G, 0, sizeof(G));
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c < COLS; ++c)
            G.board[r][c] = 0;
    G.score = 0; G.level = 1; G.lines = 0;
    G.over = false;
    G.dropTimer = 0;
    G.dropInterval = 0.8f;
    G.bagPos = 7; // trigger shuffle on first call
    spawnFirst();
}

// ── Main ───────────────────────────────────────────────────
int main() {
    int W = BOARD_X*2 + WIDTH + 170;
    int H = BOARD_Y*2 + HEIGHT + 20;
    InitWindow(W, H, "Tetris — SRS with Raylib");
    SetTargetFPS(60);

    bool started = false;
    initGame();

    while (!WindowShouldClose()) {
        if (!started) {
            if (IsKeyPressed(KEY_ENTER)) started = true;
        } else if (G.over) {
            if (IsKeyPressed(KEY_R)) initGame();
        } else {
            handleInput();
            float dt = GetFrameTime();
            G.dropTimer += dt;
            if (G.dropTimer >= G.dropInterval) {
                G.dropTimer = 0;
                if (validPos(G.cur.type, G.cur.rot, G.cur.x, G.cur.y + 1))
                    ++G.cur.y;
                else
                    lockPiece();
            }
        }

        BeginDrawing();
        ClearBackground((Color){10,10,16,255});
        if (!started) {
            drawStartScreen();
        } else {
            drawBoard();
            if (!G.over) drawGhost();
            if (!G.over) drawCurrent();
            drawNext();
            drawInfo();
            drawControls();
            if (G.over) drawGameOver();
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
