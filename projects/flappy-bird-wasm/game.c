#include "game.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>

// ---- Game logic ----

void flappy_init(FlappyGame *g, int best_score) {
    g->bird.x = (float)BIRD_X;
    g->bird.y = (float)(SCREEN_H / 2 - GROUND_H / 2);
    g->bird.vy = 0.0f;
    g->pipe_count = 0;
    g->score = 0;
    g->best_score = best_score;
    g->state = STATE_MENU;
    g->scroll_x = 0.0f;
    g->bird_rot = 0.0f;
    for (int i = 0; i < PIPE_CAP; i++) {
        g->pipes[i].x = -9999.0f;
        g->pipes[i].gap_y = 0.0f;
        g->pipes[i].passed = false;
    }
}

void flappy_start(FlappyGame *g) {
    g->bird.x = (float)BIRD_X;
    g->bird.y = (float)(SCREEN_H / 2 - GROUND_H / 2);
    g->bird.vy = 0.0f;
    g->pipe_count = 0;
    g->score = 0;
    g->state = STATE_PLAYING;
    g->scroll_x = 0.0f;
    g->bird_rot = 0.0f;
    for (int i = 0; i < PIPE_CAP; i++) {
        g->pipes[i].x = -9999.0f;
        g->pipes[i].gap_y = 0.0f;
        g->pipes[i].passed = false;
    }
    // Spawn first pipe immediately
    Pipe *p = &g->pipes[0];
    p->x = (float)(SCREEN_W + 20);
    p->gap_y = 80.0f + rand() % (SCREEN_H - GROUND_H - 200);
    p->passed = false;
    g->pipe_count = 1;
}

void flappy_jump(FlappyGame *g) {
    if (g->state == STATE_MENU) {
        flappy_start(g);
        return;
    }
    if (g->state == STATE_PLAYING) {
        g->bird.vy = JUMP_VELOCITY;
    }
    if (g->state == STATE_GAMEOVER) {
        // restart
        flappy_start(g);
    }
}

static void spawn_pipe(FlappyGame *g) {
    if (g->pipe_count >= PIPE_CAP) return;
    Pipe *p = &g->pipes[g->pipe_count];
    p->x = (float)SCREEN_W + PIPE_WIDTH;
    // Random gap_y: between 80 and (SCREEN_H - GROUND_H - 80 - PIPE_GAP)
    int min_y = 80;
    int max_y = SCREEN_H - GROUND_H - (int)PIPE_GAP - 80;
    if (max_y > min_y) {
        p->gap_y = (float)(min_y + rand() % (max_y - min_y));
    } else {
        p->gap_y = (float)(SCREEN_H / 2 - GROUND_H / 2 - PIPE_GAP / 2);
    }
    p->passed = false;
    g->pipe_count++;
}

static bool circle_rect_hit(float cx, float cy, float cr,
                            float rx, float ry, float rw, float rh) {
    float nearest_x = fmaxf(rx, fminf(cx, rx + rw));
    float nearest_y = fmaxf(ry, fminf(cy, ry + rh));
    float dx = cx - nearest_x;
    float dy = cy - nearest_y;
    return (dx * dx + dy * dy) < (cr * cr);
}

void flappy_update(FlappyGame *g, int dt_ms) {
    if (g->state != STATE_PLAYING) return;

    float dt = dt_ms / 1000.0f;

    // Bird physics
    g->bird.vy += GRAVITY;
    g->bird.y += g->bird.vy;

    // Bird rotation (visual)
    g->bird_rot = fmaxf(-30.0f, fminf(90.0f, g->bird.vy * 6.0f));

    // Ground collision
    float ground_y = (float)(SCREEN_H - GROUND_H);
    if (g->bird.y + BIRD_RADIUS > ground_y) {
        g->bird.y = ground_y - BIRD_RADIUS;
        g->state = STATE_GAMEOVER;
        if (g->score > g->best_score) g->best_score = g->score;
        return;
    }

    // Ceiling collision
    if (g->bird.y - BIRD_RADIUS < 0.0f) {
        g->bird.y = (float)BIRD_RADIUS;
        g->bird.vy = 0.0f;
    }

    // Move pipes and check scoring/collision
    for (int i = 0; i < g->pipe_count; i++) {
        Pipe *p = &g->pipes[i];
        p->x -= PIPE_SPEED;

        // Remove off-screen pipes
        if (p->x + PIPE_WIDTH < -10.0f) {
            // Shift pipes left
            for (int j = i; j < g->pipe_count - 1; j++) {
                g->pipes[j] = g->pipes[j + 1];
            }
            g->pipe_count--;
            i--;
            continue;
        }

        // Collision with top pipe
        if (circle_rect_hit(g->bird.x, g->bird.y, (float)BIRD_RADIUS,
                            p->x, 0.0f, (float)PIPE_WIDTH, p->gap_y)) {
            g->state = STATE_GAMEOVER;
            if (g->score > g->best_score) g->best_score = g->score;
            return;
        }

        // Collision with bottom pipe
        {
            float top_of_bottom = p->gap_y + PIPE_GAP;
            float bottom_pipe_bottom = (float)SCREEN_H - GROUND_H;
            if (circle_rect_hit(g->bird.x, g->bird.y, (float)BIRD_RADIUS,
                                p->x, top_of_bottom, (float)PIPE_WIDTH,
                                bottom_pipe_bottom - top_of_bottom)) {
                g->state = STATE_GAMEOVER;
                if (g->score > g->best_score) g->best_score = g->score;
                return;
            }
        }

        // Score when passing pipe
        if (!p->passed && g->bird.x > p->x + PIPE_WIDTH) {
            p->passed = true;
            g->score++;
        }
    }

    // Spawn new pipes
    if (g->pipe_count > 0) {
        Pipe *last = &g->pipes[g->pipe_count - 1];
        if ((float)SCREEN_W - last->x >= PIPE_INTERVAL) {
            spawn_pipe(g);
        }
    } else {
        spawn_pipe(g);
    }
}
