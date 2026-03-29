// Sokoban Raylib — 推箱子图形版
// 基于 ncurses sokoban 逻辑，Raylib 渲染
// 编译: clang++ -std=c++17 main.cpp -o sokoban-raylib -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -framework CoreVideo -framework Cocoa -framework IOKit -lm

#include <raylib.h>
#include <vector>
#include <string>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <cstdio>

using namespace std;

// ============ 关卡定义 ============
struct Level {
    const vector<string> layout;
    const char* name;
};

const vector<Level> LEVELS = {
    {
        {"########", "#      #", "# O $ .#", "#   $  #", "# .@   #", "########"},
        "Level 1 — Easy"
    },
    {
        {"  ##### ", "###   # ", "# $ # ##", "# #  . #", "#    # #", "## #   #", " ##$.  #", "  ##### "},
        "Level 2 — Medium"
    },
    {
        {"###########", "#    #    #", "# O  $ O  #", "#   $#$   #", "# O  #  . #", "#    #    #", "#  .@  O  #", "###########"},
        "Level 3 — Hard"
    },
    {
        {"  ####   ", "###  ####", "# $ $ $ #", "# .*.$. #", "# #@.## #", "# $ $ $ #", "###   ###", "  #####  "},
        "Level 4 — Expert"
    }
};

// ============ 游戏元素 ============
const char WALL    = '#';
const char FLOOR   = ' ';
const char BOX     = '$';
const char TARGET  = '.';
const char BOX_ON_TARGET = '*';
const char PLAYER  = '@';
const char PLAYER_ON_TARGET = '+';

const int CELL_SIZE = 52;
const int BOARD_X = 60;
const int BOARD_Y = 60;

int gx, gy;
vector<string> board;
int currentLevel = 0;
int moves = 0;
int pushes = 0;
bool levelComplete = false;
int completeTimer = 0;

const int dx[4] = {-1, 0, 1, 0};
const int dy[4] = {0, 1, 0, -1};

// ============ 工具函数 ============
string encodeBoard(const vector<string>& b, int px, int py) {
    string s;
    for (const auto& row : b) s += row;
    s += to_string(px) + "," + to_string(py);
    return s;
}

bool inBounds(int r, int c) {
    if (r < 0 || r >= (int)board.size()) return false;
    if (c < 0 || c >= (int)board[r].size()) return false;
    return true;
}

bool isWallCell(int r, int c) {
    if (!inBounds(r,c)) return true;
    return board[r][c] == WALL;
}

bool isBoxCell(int r, int c) {
    if (!inBounds(r,c)) return false;
    char ch = board[r][c];
    return ch == BOX || ch == BOX_ON_TARGET;
}

bool isTargetCell(int r, int c) {
    if (!inBounds(r,c)) return false;
    char ch = board[r][c];
    return ch == TARGET || ch == BOX_ON_TARGET || ch == PLAYER_ON_TARGET;
}

// ============ 关卡初始化 ============
void initLevel(int levelIdx) {
    currentLevel = levelIdx % LEVELS.size();
    const vector<string>& src = LEVELS[currentLevel].layout;
    board = src;
    moves = 0; pushes = 0;
    levelComplete = false; completeTimer = 0;

    for (int r = 0; r < (int)board.size(); ++r)
        for (int c = 0; c < (int)board[r].size(); ++c)
            if (board[r][c] == PLAYER || board[r][c] == PLAYER_ON_TARGET) {
                gx = r; gy = c;
            }
}

// ============ 移动逻辑 ============
bool tryMove(int dir) {
    if (levelComplete) return false;
    int nx = gx + dx[dir];
    int ny = gy + dy[dir];

    if (isWallCell(nx, ny)) return false;

    bool pushed = false;
    if (isBoxCell(nx, ny)) {
        int bx = nx + dx[dir];
        int by = ny + dy[dir];
        if (isWallCell(bx, by) || isBoxCell(bx, by)) return false;

        bool destIsTarget = isTargetCell(bx, by);
        board[bx][by] = destIsTarget ? BOX_ON_TARGET : BOX;
        board[nx][ny] = isTargetCell(nx, ny) ? TARGET : FLOOR;
        pushed = true;
    }

    bool wasOnTarget = isTargetCell(gx, gy);
    bool nextIsTarget = isTargetCell(nx, ny);
    board[gx][gy] = wasOnTarget ? TARGET : FLOOR;
    board[nx][ny] = nextIsTarget ? PLAYER_ON_TARGET : PLAYER;
    gx = nx; gy = ny;
    moves++;
    if (pushed) pushes++;

    for (int r = 0; r < (int)board.size(); ++r)
        for (int c = 0; c < (int)board[r].size(); ++c)
            if (board[r][c] == BOX) return true;
    levelComplete = true;
    return true;
}

// ============ BFS 自动求解 ============
struct BFSNode {
    int gx, gy;
    vector<string> board;
};

bool bfsSolve() {
    queue<BFSNode> q;
    unordered_set<string> visited;

    BFSNode start{gx, gy, board};
    q.push(start);
    visited.insert(encodeBoard(board, gx, gy));

    int expanded = 0;
    const int MAX_EXPAND = 200000;

    while (!q.empty() && expanded < MAX_EXPAND) {
        BFSNode cur = q.front(); q.pop();
        expanded++;

        // 检查完成
        bool done = true;
        for (const auto& row : cur.board)
            for (char c : row)
                if (c == BOX) { done = false; break; }
        if (done) return true;

        for (int dir = 0; dir < 4; ++dir) {
            int nx = cur.gx + dx[dir];
            int ny = cur.gy + dy[dir];

            if (cur.board[nx][ny] == WALL) continue;

            BFSNode nxt = cur;

            if (cur.board[nx][ny] == BOX || cur.board[nx][ny] == BOX_ON_TARGET) {
                int bx = nx + dx[dir];
                int by = ny + dy[dir];
                if (cur.board[bx][by] == WALL || cur.board[bx][by] == BOX || cur.board[bx][by] == BOX_ON_TARGET) continue;
                bool destTarget = (cur.board[bx][by] == TARGET);
                nxt.board[bx][by] = destTarget ? BOX_ON_TARGET : BOX;
                nxt.board[nx][ny] = (cur.board[nx][ny] == BOX_ON_TARGET) ? TARGET : FLOOR;
            }

            bool prevOnTarget = (cur.board[cur.gx][cur.gy] == PLAYER_ON_TARGET);
            bool nextOnTarget = (nxt.board[nx][ny] == TARGET);
            nxt.board[cur.gx][cur.gy] = prevOnTarget ? TARGET : FLOOR;
            nxt.board[nx][ny] = nextOnTarget ? PLAYER_ON_TARGET : PLAYER;
            nxt.gx = nx; nxt.gy = ny;

            string key = encodeBoard(nxt.board, nxt.gx, nxt.gy);
            if (visited.insert(key).second) q.push(nxt);
        }
    }
    return false;
}

// ============ 渲染 ============
Color colorForCell(char c) {
    switch (c) {
        case WALL:          return (Color){35, 35, 45, 255};
        case FLOOR:         return (Color){25, 25, 35, 255};
        case BOX:           return (Color){190, 130, 45, 255};
        case BOX_ON_TARGET: return (Color){40, 200, 120, 255};
        case TARGET:        return (Color){50, 50, 60, 255};
        case PLAYER:
        case PLAYER_ON_TARGET: return (Color){255, 90, 40, 255};
        default:            return (Color){25, 25, 35, 255};
    }
}

void drawCell(int r, int c, char cell) {
    Color col = colorForCell(cell);
    int px = BOARD_X + c * CELL_SIZE;
    int py = BOARD_Y + r * CELL_SIZE;

    DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, col);

    if (cell == WALL) {
        DrawRectangleLinesEx((Rectangle){(float)px, (float)py, CELL_SIZE, CELL_SIZE}, 2.0f, (Color){55, 55, 65, 255});
    }

    if (cell == TARGET) {
        DrawCircleLines(px + CELL_SIZE/2, py + CELL_SIZE/2, (int)(CELL_SIZE*0.33f), (Color){220, 80, 80, 180});
    }

    if (cell == BOX || cell == BOX_ON_TARGET) {
        float cx = px + CELL_SIZE/2.0f;
        float cy = py + CELL_SIZE/2.0f;
        float hw = CELL_SIZE * 0.38f;
        float hh = CELL_SIZE * 0.38f;
        DrawRectangleLinesEx((Rectangle){cx-hw, cy-hh, hw*2, hh*2}, 3.0f, (Color){255,210,50,220});
        int bx2 = (int)(cx-hw+5);
        int by2 = (int)(cy-hh+5);
        int bw2 = (int)(hw*2-10);
        int bh2 = (int)(hh*2-10);
        unsigned char br = (unsigned char)(col.r + 25 > 255 ? 255 : col.r + 25);
        unsigned char bg = (unsigned char)(col.g + 25 > 255 ? 255 : col.g + 25);
        unsigned char bb = (unsigned char)(col.b + 25 > 255 ? 255 : col.b + 25);
        DrawRectangle(bx2, by2, bw2, bh2, (Color){br, bg, bb, 255});
        DrawLine((int)cx, (int)(cy-hh+8), (int)cx, (int)(cy+hh-8), (Color){255,230,60,200});
        DrawLine((int)(cx-hw+8), (int)cy, (int)(cx+hw-8), (int)cy, (Color){255,230,60,200});
    }

    if (cell == PLAYER || cell == PLAYER_ON_TARGET) {
        float cx = px + CELL_SIZE/2.0f;
        float cy = py + CELL_SIZE/2.0f;
        float r_pl = CELL_SIZE * 0.36f;
        DrawCircle((int)cx, (int)cy, (int)r_pl, (Color){255, 80, 40, 255});
        DrawCircle((int)(cx-7), (int)(cy-5), 4, WHITE);
        DrawCircle((int)(cx+7), (int)(cy-5), 4, WHITE);
        DrawCircle((int)(cx-7), (int)(cy-5), 2, BLACK);
        DrawCircle((int)(cx+7), (int)(cy-5), 2, BLACK);
    }
}

void drawBoard() {
    for (int r = 0; r < (int)board.size(); ++r)
        for (int c = 0; c < (int)board[r].size(); ++c)
            drawCell(r, c, board[r][c]);
}

void drawUI() {
    DrawRectangle(0, 0, GetScreenWidth(), BOARD_Y, (Color){18, 18, 28, 255});

    DrawTextEx(GetFontDefault(), LEVELS[currentLevel].name, (Vector2){20, 12}, 24, 2, WHITE);

    char buf[200];
    snprintf(buf, sizeof(buf), "Moves: %d   Pushes: %d   [%d / %zu]", moves, pushes, currentLevel+1, LEVELS.size());
    DrawTextEx(GetFontDefault(), buf, (Vector2){20, 44}, 20, 2, (Color){160,160,160,255});

    const char* hints = "Arrows/WASD: Move   R: Reset   N: Next Level   P: Prev Level   B: Auto-Solve";
    DrawTextEx(GetFontDefault(), hints, (Vector2){20, (float)(GetScreenHeight()-35)}, 18, 2, (Color){90,90,90,255});
}

int main() {
    InitWindow(840, 680, "Sokoban — 推箱子");
    SetTargetFPS(60);
    initLevel(0);

    while (!WindowShouldClose()) {
        if (!levelComplete) {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) tryMove(0);
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) tryMove(1);
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) tryMove(2);
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) tryMove(3);
            if (IsKeyPressed(KEY_R)) initLevel(currentLevel);
        }
        if (IsKeyPressed(KEY_N)) initLevel(currentLevel + 1);
        if (IsKeyPressed(KEY_P)) initLevel(currentLevel - 1);
        if (IsKeyPressed(KEY_B) && !levelComplete) {
            if (bfsSolve()) levelComplete = true;
        }
        if (levelComplete) {
            completeTimer++;
            if (completeTimer > 120 && IsKeyPressed(KEY_ENTER))
                initLevel(currentLevel + 1);
        }

        BeginDrawing();
        ClearBackground((Color){12, 12, 18, 255});
        drawBoard();
        drawUI();

        if (levelComplete) {
            DrawRectangle(GetScreenWidth()/2-200, GetScreenHeight()/2-60, 400, 120, Fade((Color){0,0,0,200}, 0.9f));
            DrawRectangleLinesEx((Rectangle){(float)(GetScreenWidth()/2-200), (float)(GetScreenHeight()/2-60), 400, 120}, 3, (Color){50,220,100,255});
            const char* msg = "Level Complete!";
            Vector2 ts = MeasureTextEx(GetFontDefault(), msg, 32, 2);
            DrawTextEx(GetFontDefault(), msg, (Vector2){(float)(GetScreenWidth()/2 - ts.x/2), (float)(GetScreenHeight()/2 - 35)}, 32, 2, (Color){50,255,120,255});
            const char* sub = completeTimer > 120 ? "ENTER: Next Level" : "...";
            Vector2 ss = MeasureTextEx(GetFontDefault(), sub, 20, 2);
            DrawTextEx(GetFontDefault(), sub, (Vector2){(float)(GetScreenWidth()/2 - ss.x/2), (float)(GetScreenHeight()/2 + 10)}, 20, 2, WHITE);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
