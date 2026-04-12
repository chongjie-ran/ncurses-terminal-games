#include "game.h"
#include <stdlib.h>
#include <math.h>

GameState g_game;

static int brick_color(int row) {
    static const int colors[6] = {1, 2, 3, 4, 5, 6};
    return colors[row % 6];
}

void game_reset(void) {
    g_game.ball.x = CANVAS_W / 2.0f;
    g_game.ball.y = CANVAS_H - 80;
    g_game.ball.speed = BALL_SPEED;
    float angle = -M_PI / 4.0f;
    g_game.ball.dx = sinf(angle) * g_game.ball.speed;
    g_game.ball.dy = -cosf(angle) * g_game.ball.speed;

    g_game.paddle.x = (CANVAS_W - PADDLE_W) / 2.0f;
    g_game.paddle.y = CANVAS_H - 40;
    g_game.paddle.w = PADDLE_W;
    g_game.paddle.h = PADDLE_H;

    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            Brick* b = &g_game.bricks[r * BRICK_COLS + c];
            b->row = r;
            b->col = c;
            b->hits = 1;
            b->color = brick_color(r);
        }
    }
    g_game.score = 0;
    g_game.lives = 3;
    g_game.state = 1;
    g_game.paused = 0;
    g_game.paddle_dir = 0;
}

void game_init(void) {
    game_reset();
    g_game.state = 0;  // MENU
}

static int check_brick_hit(float bx, float by, int* hit_row, int* hit_col) {
    for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++) {
        Brick* b = &g_game.bricks[i];
        if (b->hits <= 0) continue;
        int bx1 = b->col * (BRICK_W + BRICK_GAP) + BRICK_LEFT;
        int by1 = BRICK_TOP + b->row * (BRICK_H + BRICK_GAP);
        int bx2 = bx1 + BRICK_W;
        int by2 = by1 + BRICK_H;
        if (bx + BALL_R > bx1 && bx - BALL_R < bx2 &&
            by + BALL_R > by1 && by - BALL_R < by2) {
            *hit_row = b->row;
            *hit_col = b->col;
            return 1;
        }
    }
    return 0;
}

void game_tick(void) {
    if (g_game.state != 1 || g_game.paused) return;

    g_game.frame_count++;

    // Paddle movement
    if (g_game.paddle_dir != 0) {
        g_game.paddle.x += PADDLE_SPEED * g_game.paddle_dir;
        if (g_game.paddle.x < 0) g_game.paddle.x = 0;
        if (g_game.paddle.x + g_game.paddle.w > CANVAS_W)
            g_game.paddle.x = CANVAS_W - g_game.paddle.w;
    }

    // Move ball
    g_game.ball.x += g_game.ball.dx;
    g_game.ball.y += g_game.ball.dy;

    // Wall collisions
    if (g_game.ball.x - BALL_R < 0) {
        g_game.ball.x = BALL_R;
        g_game.ball.dx = fabsf(g_game.ball.dx);
    }
    if (g_game.ball.x + BALL_R > CANVAS_W) {
        g_game.ball.x = CANVAS_W - BALL_R;
        g_game.ball.dx = -fabsf(g_game.ball.dx);
    }
    if (g_game.ball.y - BALL_R < 0) {
        g_game.ball.y = BALL_R;
        g_game.ball.dy = fabsf(g_game.ball.dy);
    }

    // Ball out of bottom
    if (g_game.ball.y > CANVAS_H + BALL_R) {
        g_game.lives--;
        if (g_game.lives <= 0) {
            g_game.state = 2;
        } else {
            g_game.ball.x = CANVAS_W / 2.0f;
            g_game.ball.y = CANVAS_H - 80;
            float angle = -M_PI / 4.0f;
            g_game.ball.dx = sinf(angle) * g_game.ball.speed;
            g_game.ball.dy = -cosf(angle) * g_game.ball.speed;
        }
        return;
    }

    // Paddle collision
    Paddle* p = &g_game.paddle;
    if (g_game.ball.dy > 0 &&
        g_game.ball.y + BALL_R >= p->y &&
        g_game.ball.y - BALL_R <= p->y + p->h &&
        g_game.ball.x >= p->x &&
        g_game.ball.x <= p->x + p->w) {

        float hit_pos = (g_game.ball.x - p->x) / p->w;
        float angle = -M_PI / 6.0f - hit_pos * M_PI * 2.0f / 3.0f;
        float speed = g_game.ball.speed;
        if (speed < MAX_SPEED) {
            speed = fminf(speed * 1.003f, MAX_SPEED);
            g_game.ball.speed = speed;
        }
        g_game.ball.dx = sinf(angle) * speed;
        g_game.ball.dy = -cosf(angle) * speed;
        g_game.ball.y = p->y - BALL_R;
    }

    // Brick collisions
    int hit_r = -1, hit_c = -1;
    if (check_brick_hit(g_game.ball.x, g_game.ball.y, &hit_r, &hit_c)) {
        Brick* b = &g_game.bricks[hit_r * BRICK_COLS + hit_c];
        b->hits = 0;
        g_game.score += 10;

        int bx1 = hit_c * (BRICK_W + BRICK_GAP) + BRICK_LEFT;
        int by1 = BRICK_TOP + hit_r * (BRICK_H + BRICK_GAP);
        float brick_cx = bx1 + BRICK_W / 2.0f;
        float brick_cy = by1 + BRICK_H / 2.0f;
        float dx = g_game.ball.x - brick_cx;
        float dy = g_game.ball.y - brick_cy;
        if (fabsf(dx) * BRICK_H > fabsf(dy) * BRICK_W)
            g_game.ball.dx = -g_game.ball.dx;
        else
            g_game.ball.dy = -g_game.ball.dy;

        int remaining = 0;
        for (int i = 0; i < BRICK_ROWS * BRICK_COLS; i++)
            if (g_game.bricks[i].hits > 0) remaining++;
        if (remaining == 0) g_game.state = 3;
    }
}
