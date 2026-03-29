#include "game.h"
#include <raylib.h>
#include <math.h>

#define DARK_BG   (Color){ 50, 180, 220, 255 }
#define DIRT      (Color){ 200, 140, 60, 255 }
#define PIPE_COL  (Color){ 50, 200, 80, 255 }
#define PIPE_DARK (Color){ 30, 150, 50, 255 }
#define BIRD_COL  (Color){ 255, 220, 30, 255 }
#define BIRD_WING (Color){ 255, 150, 30, 255 }
#define TEXT_COL  (Color){ 255, 255, 255, 255 }
#define SHADOW    (Color){ 0, 0, 0, 100 }

static void draw_bg(void) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, DARK_BG);

    static const Color cloud = { 255, 255, 255, 180 };
    static const struct { int x, y, r; } clouds[3] = {
        { 60, 100, 40 },
        { 200, 80, 55 },
        { 350, 140, 35 },
    };
    for (int c = 0; c < 3; c++) {
        int cx = clouds[c].x, cy = clouds[c].y, cr = clouds[c].r;
        DrawCircle(cx, cy, cr, cloud);
        DrawCircle(cx + cr * 2, cy, cr * 3 / 4, cloud);
        DrawCircle(cx - cr * 2, cy + cr / 2, cr / 2, cloud);
    }

    int ground_h = 60;
    DrawRectangle(0, SCREEN_H - ground_h, SCREEN_W, ground_h, DIRT);
    DrawRectangle(0, SCREEN_H - ground_h, SCREEN_W, 8, (Color){ 100, 200, 50, 255 });
}

static void draw_pipes(FlappyGame *g) {
    for (int i = 0; i < g->pipe_count; i++) {
        float px = get_pipe_x(g, i);
        if (px > SCREEN_W + 10 || px + PIPE_WIDTH < -10) continue;
        float gy = g->pipes[i].gap_y;

        // Top pipe body
        DrawRectangle((int)px, 0, PIPE_WIDTH, (int)gy, PIPE_COL);
        DrawRectangle((int)px + 4, 0, PIPE_WIDTH - 8, (int)gy, PIPE_DARK);
        // Top cap
        DrawRectangle((int)px - 4, (int)gy - 20, PIPE_WIDTH + 8, 20, PIPE_COL);
        DrawRectangle((int)px, (int)gy - 18, PIPE_WIDTH - 4, 16, PIPE_DARK);

        // Bottom pipe body
        int bot_y = (int)(gy + PIPE_GAP);
        DrawRectangle((int)px, bot_y, PIPE_WIDTH, SCREEN_H - bot_y, PIPE_COL);
        DrawRectangle((int)px + 4, bot_y, PIPE_WIDTH - 8, SCREEN_H - bot_y, PIPE_DARK);
        // Bottom cap
        DrawRectangle((int)px - 4, bot_y, PIPE_WIDTH + 8, 20, PIPE_COL);
        DrawRectangle((int)px, bot_y + 2, PIPE_WIDTH - 4, 16, PIPE_DARK);
    }
}

static void draw_bird(FlappyGame *g) {
    float bx = g->bird.x;
    float by = g->bird.y;

    DrawCircle((int)bx + 3, (int)by + 3, BIRD_RADIUS, SHADOW);
    DrawCircle((int)bx, (int)by, BIRD_RADIUS, BIRD_COL);

    // Wing animation
    int wing_y = (int)by + (g->bird.vy < 0 ? -6 : 4);
    DrawEllipse((int)bx - 2, wing_y, 12, 7, BIRD_WING);

    // Eye
    DrawCircle((int)bx + 5, (int)by - 4, 5, WHITE);
    DrawCircle((int)bx + 6, (int)by - 4, 2, BLACK);

    // Beak
    DrawTriangle(
        (Vector2){ bx + BIRD_RADIUS, by },
        (Vector2){ bx + BIRD_RADIUS + 12, by + 4 },
        (Vector2){ bx + BIRD_RADIUS, by + 8 },
        (Color){ 255, 150, 50, 255 }
    );
}

void game_draw(FlappyGame *g) {
    BeginDrawing();
    ClearBackground(DARK_BG);

    draw_bg();
    draw_pipes(g);
    draw_bird(g);

    if (g->state == STATE_MENU) {
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, 100 });
        DrawText("FLAPPY BIRD", SCREEN_W / 2 - 100, SCREEN_H / 2 - 80, 28, GOLD);
        DrawText("Raylib Edition", SCREEN_W / 2 - 80, SCREEN_H / 2 - 45, 18, LIGHTGRAY);
        DrawText("SPACE / TAP to Start", SCREEN_W / 2 - 115, SCREEN_H / 2 + 10, 20, TEXT_COL);
        DrawText("SPACE / WASD / Click to Jump", SCREEN_W / 2 - 150, SCREEN_H / 2 + 50, 20, TEXT_COL);
        if (g->best_score > 0)
            DrawText(TextFormat("Best: %d", g->best_score), SCREEN_W / 2 - 55, SCREEN_H / 2 + 90, 18, GOLD);
    }

    if (g->state == STATE_PLAYING || g->state == STATE_GAMEOVER) {
        DrawRectangle(10, 10, 130, 55, (Color){ 0, 0, 0, 130 });
        DrawText(TextFormat("Score: %d", g->score), 20, 18, 22, TEXT_COL);
        DrawText(TextFormat("Best: %d", g->best_score), 20, 48, 16, GOLD);
    }

    if (g->state == STATE_GAMEOVER) {
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, 140 });
        DrawText("GAME OVER", SCREEN_W / 2 - 95, SCREEN_H / 2 - 90, 36, RED);
        DrawText(TextFormat("Score: %d", g->score), SCREEN_W / 2 - 70, SCREEN_H / 2 - 30, 28, TEXT_COL);
        DrawText(TextFormat("Best:  %d", g->best_score), SCREEN_W / 2 - 70, SCREEN_H / 2 + 10, 28, GOLD);
        DrawText("SPACE / Click to Restart", SCREEN_W / 2 - 130, SCREEN_H / 2 + 60, 20, TEXT_COL);
    }

    EndDrawing();
}
