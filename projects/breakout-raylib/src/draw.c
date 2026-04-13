#include "game.h"
#include <raylib.h>
#include <stdio.h>

static const Color BRICK_COLOR_TABLE[5] = {
    {255, 60, 60, 255},   // red
    {255, 140, 40, 255},  // orange
    {255, 220, 40, 255},  // yellow
    {60, 220, 60, 255},    // green
    {60, 140, 255, 255}    // blue
};

static const Color CLR_PADDLE = {200, 170, 130, 255};
static const Color CLR_PADDLE_TOP = {255, 220, 180, 255};
static const Color CLR_PADDLE_BOT = {100, 80, 60, 255};
static const Color CLR_BALL = {255, 255, 255, 255};
static const Color CLR_BALL_CORE = {255, 240, 180, 255};
static const Color CLR_BG = {15, 15, 30, 255};
static const Color CLR_TEXT = {240, 240, 240, 255};
static const Color CLR_ACCENT = {255, 200, 60, 255};
static const Color CLR_DIM = {100, 100, 120, 255};

static void draw_brick(Brick* b) {
    if (!b->alive) return;
    float x = BRICK_LEFT + b->col * (BRICK_W + BRICK_GAP);
    float y = BRICK_TOP + b->row * (BRICK_H + BRICK_GAP);
    Color col = BRICK_COLOR_TABLE[b->color_idx];

    DrawRectangle(x, y, BRICK_W, BRICK_H, col);
    // highlight
    DrawRectangle(x, y, BRICK_W, 3, (Color){255,255,255,100});
    DrawRectangle(x, y, 3, BRICK_H, (Color){255,255,255,80});
    // shadow
    DrawRectangle(x, y + BRICK_H - 3, BRICK_W, 3, (Color){0,0,0,80});
    DrawRectangle(x + BRICK_W - 3, y, 3, BRICK_H, (Color){0,0,0,60});
}

static void draw_paddle(Paddle* p) {
    // body
    DrawRectangle(p->x, p->y, p->w, p->h, CLR_PADDLE);
    // top highlight
    DrawRectangle(p->x, p->y, p->w, 3, CLR_PADDLE_TOP);
    // left highlight
    DrawRectangle(p->x, p->y, 3, p->h, CLR_PADDLE_TOP);
    // bottom shadow
    DrawRectangle(p->x, p->y + p->h - 3, p->w, 3, CLR_PADDLE_BOT);
    // right shadow
    DrawRectangle(p->x + p->w - 3, p->y, 3, p->h, CLR_PADDLE_BOT);
    // center grip
    int mid = p->x + p->w / 2;
    DrawRectangle(mid - 15, p->y + 4, 30, p->h - 8, (Color){180,150,110,255});
}

static void draw_ball(Ball* b) {
    if (!b->active) return;
    DrawCircle(b->x, b->y, BALL_R, CLR_BALL);
    DrawCircle(b->x - 2, b->y - 2, BALL_R * 0.5f, CLR_BALL_CORE);
}

static void draw_hud(Game* g) {
    char buf[64];
    snprintf(buf, sizeof(buf), "SCORE: %d", g->score);
    DrawText(buf, 20, 20, 22, CLR_ACCENT);

    snprintf(buf, sizeof(buf), "LEVEL: %d", g->level);
    DrawText(buf, SCREEN_W / 2 - 50, 20, 22, CLR_DIM);

    snprintf(buf, sizeof(buf), "LIVES:");
    DrawText(buf, SCREEN_W - 200, 20, 22, CLR_TEXT);
    for (int i = 0; i < g->lives; i++) {
        DrawCircle(SCREEN_W - 120 + i * 25, 30, 8, CLR_ACCENT);
    }

    if (g->state == STATE_PLAYING && g->launch_ready) {
        const char* tip = "SPACE or CLICK to launch  |  P = Pause";
        int tw = MeasureText(tip, 18);
        DrawText(tip, (SCREEN_W - tw) / 2, SCREEN_H - 30, 18, (Color){150,150,150,200});
    }
}

static void draw_menu(Game* g) {
    const char* title = "BREAKOUT";
    int ts = MeasureText(title, 72);
    DrawText(title, (SCREEN_W - ts) / 2, 160, 72, CLR_ACCENT);

    const char* sub = "CLICK or SPACE to Start";
    int ss = MeasureText(sub, 24);
    DrawText(sub, (SCREEN_W - ss) / 2, 270, 24, CLR_TEXT);

    const char* hint = "Mouse or Arrow Keys to move paddle";
    int hs = MeasureText(hint, 18);
    DrawText(hint, (SCREEN_W - hs) / 2, 320, 18, CLR_DIM);

    char buf[64];
    snprintf(buf, sizeof(buf), "Best: %d  |  Level: %d", g->score, g->level);
    int bs = MeasureText(buf, 18);
    DrawText(buf, (SCREEN_W - bs) / 2, 380, 18, CLR_DIM);
}

static void draw_win(Game* g) {
    const char* msg = "YOU WIN!";
    int ms = MeasureText(msg, 60);
    DrawText(msg, (SCREEN_W - ms) / 2, 200, 60, (Color){60, 255, 100, 255});

    char buf[64];
    snprintf(buf, sizeof(buf), "Score: %d", g->score);
    int bs = MeasureText(buf, 28);
    DrawText(buf, (SCREEN_W - bs) / 2, 290, 28, CLR_TEXT);

    const char* sub = "Click or SPACE for next level";
    int ss = MeasureText(sub, 20);
    DrawText(sub, (SCREEN_W - ss) / 2, 350, 20, CLR_DIM);
}

static void draw_gameover(Game* g) {
    const char* msg = "GAME OVER";
    int ms = MeasureText(msg, 60);
    DrawText(msg, (SCREEN_W - ms) / 2, 200, 60, (Color){255, 60, 60, 255});

    char buf[64];
    snprintf(buf, sizeof(buf), "Final Score: %d", g->score);
    int bs = MeasureText(buf, 28);
    DrawText(buf, (SCREEN_W - bs) / 2, 290, 28, CLR_TEXT);

    const char* sub = "Click or SPACE to Restart";
    int ss = MeasureText(sub, 20);
    DrawText(sub, (SCREEN_W - ss) / 2, 350, 20, CLR_DIM);
}

static void draw_paused(Game* g) {
    (void)g;
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 150});
    const char* msg = "PAUSED";
    int ms = MeasureText(msg, 60);
    DrawText(msg, (SCREEN_W - ms) / 2, SCREEN_H / 2 - 30, 60, CLR_TEXT);
    const char* sub = "P = Resume";
    int ss = MeasureText(sub, 20);
    DrawText(sub, (SCREEN_W - ss) / 2, SCREEN_H / 2 + 30, 20, CLR_DIM);
}

static void draw_next_level(Game* g) {
    char buf[64];
    snprintf(buf, sizeof(buf), "LEVEL %d", g->level);
    int bs = MeasureText(buf, 52);
    DrawText(buf, (SCREEN_W - bs) / 2, SCREEN_H / 2 - 26, 52, CLR_ACCENT);
}

void draw_game(Game* g) {
    ClearBackground(CLR_BG);

    if (g->state == STATE_MENU) {
        for (int r = 0; r < BRICK_ROWS; r++) {
            for (int c = 0; c < BRICK_COLS; c++) {
                draw_brick(&g->bricks[r][c]);
            }
        }
        draw_menu(g);
        return;
    }

    // draw bricks
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            draw_brick(&g->bricks[r][c]);
        }
    }

    // draw paddle
    draw_paddle(&g->paddle);

    // draw balls
    for (int i = 0; i < g->ball_count; i++) {
        draw_ball(&g->balls[i]);
    }

    // draw HUD
    draw_hud(g);

    // draw overlays
    if (g->state == STATE_WIN) draw_win(g);
    if (g->state == STATE_GAMEOVER) draw_gameover(g);
    if (g->state == STATE_PAUSED) draw_paused(g);
    if (g->state == STATE_NEXT_LEVEL) draw_next_level(g);
}
