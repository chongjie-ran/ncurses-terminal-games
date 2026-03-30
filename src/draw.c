#include "game.h"
#include <raylib.h>
#include <math.h>
#include <stdio.h>

#define HEADER_H 70

// Color palette
static const Color CLR_BG        = (Color){ 30,  30,  50,  255 };
static const Color CLR_CELL_HIDDEN = (Color){ 80,  80, 120,  255 };
static const Color CLR_CELL_REVEAL = (Color){ 45,  45,  65,  255 };
static const Color CLR_GRID_LINE  = (Color){ 20,  20,  40,  255 };
static const Color CLR_MINE       = (Color){ 220, 50,  50,  255 };
static const Color CLR_FLAG       = (Color){ 255, 80,  80,  255 };
static const Color CLR_NUM[9]     = {
    (Color){ 0,   0,   0,   0   },  // 0 unused
    (Color){ 30,  120, 255, 255 },  // 1 blue
    (Color){ 40,  180, 80,  255 },  // 2 green
    (Color){ 230, 60,  60,  255 },  // 3 red
    (Color){ 20,  80,  180, 255 },  // 4 dark blue
    (Color){ 150, 40,  40,  255 },  // 5 maroon
    (Color){ 30,  200, 200, 255 },  // 6 cyan
    (Color){ 0,   0,   0,   255 },  // 7 black
    (Color){ 128,128, 128, 255 },  // 8 gray
};

static void draw_header(MinesweeperGame *g) {
    // Background bar
    DrawRectangle(0, 0, WINDOW_W, HEADER_H, (Color){ 20, 20, 40, 255 });

    // Timer
    char timer_text[32];
    int mins = g->elapsed_seconds / 60;
    int secs = g->elapsed_seconds % 60;
    snprintf(timer_text, sizeof(timer_text), "%02d:%02d", mins, secs);

    // Left: Flag count
    char flag_text[16];
    snprintf(flag_text, sizeof(flag_text), "🚩 %d/%d", g->flagged_count, MINES);
    DrawText(flag_text, 15, 20, 22, CLR_FLAG);

    // Center: Timer
    int tw = MeasureText(timer_text, 26);
    DrawText(timer_text, (WINDOW_W - tw) / 2, 20, 26, (Color){ 255, 220, 100, 255 });

    // Right: Best time
    if (g->best_time >= 0) {
        char best_text[32];
        int bm = g->best_time / 60;
        int bs = g->best_time % 60;
        snprintf(best_text, sizeof(best_text), "BEST %02d:%02d", bm, bs);
        int bw = MeasureText(best_text, 18);
        DrawText(best_text, WINDOW_W - bw - 15, 25, 18, (Color){ 200, 200, 200, 180 });
    }
}

static void draw_cell(int r, int c, Cell *cell) {
    float x = GRID_PAD_X + c * CELL_SIZE;
    float y = GRID_PAD_Y + r * CELL_SIZE;
    int s = CELL_SIZE;

    if (cell->is_revealed) {
        DrawRectangle((int)x, (int)y, s, s, CLR_CELL_REVEAL);
        // Inner bevel highlight (top/left lighter)
        DrawRectangle((int)x, (int)y, s, 2, (Color){ 60, 60, 80, 255 });
        DrawRectangle((int)x, (int)y, 2, s, (Color){ 60, 60, 80, 255 });
        DrawRectangle((int)x, (int)y + s - 2, s, 2, (Color){ 30, 30, 50, 255 });
        DrawRectangle((int)x + s - 2, (int)y, 2, s, (Color){ 30, 30, 50, 255 });

        if (cell->is_mine) {
            // Draw mine
            int mx = (int)(x + s/2);
            int my = (int)(y + s/2);
            DrawCircle(mx, my, s/4, CLR_MINE);
            DrawCircle(mx, my, s/6, (Color){ 255, 100, 100, 255 });
            // Spokes
            for (int a = 0; a < 8; a++) {
                float angle = a * PI / 4.0f;
                int ex = mx + (int)(cosf(angle) * s/3);
                int ey = my + (int)(sinf(angle) * s/3);
                DrawLine(mx, my, ex, ey, CLR_MINE);
            }
        } else if (cell->adjacent_mines > 0) {
            char num[2] = { (char)('0' + cell->adjacent_mines), 0 };
            int nw = MeasureText(num, 22);
            DrawText(num, (int)(x + (s - nw)/2), (int)(y + s/2 - 14), 22, CLR_NUM[cell->adjacent_mines]);
        }
    } else {
        // Hidden cell — raised 3D look
        DrawRectangle((int)x, (int)y, s, s, CLR_CELL_HIDDEN);
        // Light bevel (top/left)
        DrawRectangle((int)x, (int)y, s, 3, (Color){ 110, 110, 150, 255 });
        DrawRectangle((int)x, (int)y, 3, s, (Color){ 110, 110, 150, 255 });
        // Dark bevel (bottom/right)
        DrawRectangle((int)x, (int)y + s - 3, s, 3, (Color){ 40, 40, 65, 255 });
        DrawRectangle((int)x + s - 3, (int)y, 3, s, (Color){ 40, 40, 65, 255 });

        if (cell->is_flagged) {
            // Draw flag
            int fx = (int)(x + s/2);
            int fy_top = (int)(y + 8);
            int fy_bot = (int)(y + s - 10);
            int fx_right = fx + 10;
            // Pole
            DrawLine(fx, fy_top, fx, fy_bot, (Color){ 180, 180, 180, 255 });
            // Flag
            DrawTriangle(
                (Vector2){ (float)fx, (float)fy_top },
                (Vector2){ (float)fx_right, (float)(fy_top + 6) },
                (Vector2){ (float)fx, (float)(fy_top + 12) },
                CLR_FLAG
            );
        }
    }

    // Border
    DrawRectangleLines((int)x, (int)y, s, s, CLR_GRID_LINE);
}

void draw_game(MinesweeperGame *g) {
    // Background
    ClearBackground(CLR_BG);

    draw_header(g);

    // Grid background
    DrawRectangle(GRID_PAD_X - 2, GRID_PAD_Y - 2,
                  COLS * CELL_SIZE + 4, ROWS * CELL_SIZE + 4,
                  (Color){ 15, 15, 35, 255 });

    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            draw_cell(r, c, &g->grid[r][c]);

    // Overlay messages
    if (g->state == STATE_MENU) {
        DrawRectangle(0, 0, WINDOW_W, WINDOW_H, (Color){ 0, 0, 0, 160 });
        const char *title = "MINESWEEPER";
        int tw = MeasureText(title, 44);
        DrawText(title, (WINDOW_W - tw)/2, 140, 44, (Color){ 255, 220, 60, 255 });

        const char *sub = "Click to Start";
        int sw = MeasureText(sub, 22);
        DrawText(sub, (WINDOW_W - sw)/2, 210, 22, (Color){ 200, 200, 200, 255 });

        const char *hint = "Left-click: Reveal  |  Right-click: Flag";
        int hw = MeasureText(hint, 18);
        DrawText(hint, (WINDOW_W - hw)/2, 260, 18, (Color){ 150, 150, 150, 200 });

        const char *info = "9x9  •  10 Mines";
        int iw = MeasureText(info, 18);
        DrawText(info, (WINDOW_W - iw)/2, 295, 18, (Color){ 120, 120, 160, 200 });
    }

    if (g->state == STATE_GAMEOVER) {
        DrawRectangle(0, WINDOW_H/2 - 60, WINDOW_W, 80, (Color){ 0, 0, 0, 180 });
        const char *msg = "GAME OVER";
        int mw = MeasureText(msg, 36);
        DrawText(msg, (WINDOW_W - mw)/2, WINDOW_H/2 - 50, 36, (Color){ 255, 80, 80, 255 });
        const char *sub = "Press R to Retry";
        int sw = MeasureText(sub, 20);
        DrawText(sub, (WINDOW_W - sw)/2, WINDOW_H/2 + 2, 20, (Color){ 200, 200, 200, 255 });
    }

    if (g->state == STATE_WIN) {
        DrawRectangle(0, WINDOW_H/2 - 60, WINDOW_W, 80, (Color){ 0, 0, 0, 180 });
        const char *msg = "YOU WIN!";
        int mw = MeasureText(msg, 36);
        DrawText(msg, (WINDOW_W - mw)/2, WINDOW_H/2 - 50, 36, (Color){ 80, 255, 120, 255 });
        char sub[64];
        snprintf(sub, sizeof(sub), "%02d:%02d  •  Press R to Retry", g->elapsed_seconds/60, g->elapsed_seconds%60);
        int sw = MeasureText(sub, 20);
        DrawText(sub, (WINDOW_W - sw)/2, WINDOW_H/2 + 2, 20, (Color){ 200, 255, 200, 255 });
    }
}
