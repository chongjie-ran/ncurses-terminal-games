#include <math.h>
#include <stdlib.h>
#include "game.h"

PongGame g_state;

void game_init(PongGame *g) {
    g->left.x = 20;
    g->left.y = SCREEN_H / 2.0f - PADDLE_H / 2.0f;
    g->left.score = 0;
    g->right.x = SCREEN_W - 20 - PADDLE_W;
    g->right.y = SCREEN_H / 2.0f - PADDLE_H / 2.0f;
    g->right.score = 0;
    g->ball.x = SCREEN_W / 2.0f;
    g->ball.y = SCREEN_H / 2.0f;
    g->ball.dx = 0;
    g->ball.dy = 0;
    g->ball.radius = BALL_R;
    g->state = 0;
    g->win_score = 7;
    g->menu_selection = 0;
}

void game_launch_ball(PongGame *g) {
    g->ball.x = SCREEN_W / 2.0f;
    g->ball.y = SCREEN_H / 2.0f;
    float angle = (rand() % 60 - 30) * M_PI / 180.0f;
    int dir = (rand() % 2 == 0) ? 1 : -1;
    g->ball.dx = dir * BALL_SPEED * cosf(angle);
    g->ball.dy = BALL_SPEED * sinf(angle);
}

static void ai_move(PongGame *g) {
    float target = g->ball.y;
    float center = g->right.y + PADDLE_H / 2.0f;
    float diff = target - center;
    if (diff > AI_SPEED) {
        g->right.y += AI_SPEED;
    } else if (diff < -AI_SPEED) {
        g->right.y -= AI_SPEED;
    }
    if (g->right.y < 0) g->right.y = 0;
    if (g->right.y > SCREEN_H - PADDLE_H) g->right.y = SCREEN_H - PADDLE_H;
}

void game_update(PongGame *g, float dt) {
    (void)dt;
    if (g->state == 0 || g->state == 2 || g->state == 3) return;

    g->ball.x += g->ball.dx;
    g->ball.y += g->ball.dy;

    if (g->ball.y - BALL_R < 0) {
        g->ball.y = BALL_R;
        g->ball.dy = fabsf(g->ball.dy);
    }
    if (g->ball.y + BALL_R > SCREEN_H) {
        g->ball.y = SCREEN_H - BALL_R;
        g->ball.dy = -fabsf(g->ball.dy);
    }

    if (g->ball.dx < 0 &&
        g->ball.x - BALL_R < g->left.x + PADDLE_W &&
        g->ball.x + BALL_R > g->left.x &&
        g->ball.y > g->left.y &&
        g->ball.y < g->left.y + PADDLE_H) {
        float hit_pos = (g->ball.y - g->left.y) / PADDLE_H;
        float angle = (hit_pos - 0.5f) * 60.0f * M_PI / 180.0f;
        float speed = sqrtf(g->ball.dx * g->ball.dx + g->ball.dy * g->ball.dy);
        speed *= 1.05f;
        if (speed > BALL_SPEED * 2.0f) speed = BALL_SPEED * 2.0f;
        g->ball.dx = speed * cosf(angle);
        g->ball.dy = speed * sinf(angle);
        if (g->ball.dx < 0) g->ball.dx = -g->ball.dx;
        g->ball.x = g->left.x + PADDLE_W + BALL_R + 1;
    }

    if (g->ball.dx > 0 &&
        g->ball.x + BALL_R > g->right.x &&
        g->ball.x - BALL_R < g->right.x + PADDLE_W &&
        g->ball.y > g->right.y &&
        g->ball.y < g->right.y + PADDLE_H) {
        float hit_pos = (g->ball.y - g->right.y) / PADDLE_H;
        float angle = (hit_pos - 0.5f) * 60.0f * M_PI / 180.0f;
        float speed = sqrtf(g->ball.dx * g->ball.dx + g->ball.dy * g->ball.dy);
        speed *= 1.05f;
        if (speed > BALL_SPEED * 2.0f) speed = BALL_SPEED * 2.0f;
        g->ball.dx = -speed * cosf(angle);
        g->ball.dy = speed * sinf(angle);
        if (g->ball.dx > 0) g->ball.dx = -g->ball.dx;
        g->ball.x = g->right.x - BALL_R - 1;
    }

    ai_move(g);

    if (g->ball.x < 0) {
        g->right.score++;
        if (g->right.score >= g->win_score) g->state = 3;
        else { g->state = 2; g->ball.dx = 0; g->ball.dy = 0; }
    }
    if (g->ball.x > SCREEN_W) {
        g->left.score++;
        if (g->left.score >= g->win_score) g->state = 3;
        else { g->state = 2; g->ball.dx = 0; g->ball.dy = 0; }
    }
}
