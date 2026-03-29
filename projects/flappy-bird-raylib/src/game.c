#include "game.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#define SCORE_FILE "flappy_best.txt"

int best_score_load(void) {
    FILE *f = fopen(SCORE_FILE, "r");
    if (!f) return 0;
    int v = 0;
    if (fscanf(f, "%d", &v) != 1) v = 0;
    fclose(f);
    return v;
}

void best_score_save(int s) {
    FILE *f = fopen(SCORE_FILE, "w");
    if (!f) return;
    fprintf(f, "%d\n", s);
    fclose(f);
}

static float randf(void) {
    return (float)rand() / (float)RAND_MAX;
}

static float pipe_base_x(int idx) {
    return (float)(idx * PIPE_INTERVAL);
}

float get_pipe_x(FlappyGame *g, int idx) {
    return pipe_base_x(idx) - g->scroll_x;
}

void game_init(FlappyGame *g) {
    memset(g, 0, sizeof(*g));
    g->bird.y = SCREEN_H / 2.0f;
    g->bird.x = 100.0f;
    g->state = STATE_MENU;
    g->best_score = best_score_load();

    g->pipe_cap = 8;
    g->pipes = calloc(g->pipe_cap, sizeof(Pipe));
    for (int i = 0; i < 4; i++) {
        g->pipes[i].x = pipe_base_x(i);
        g->pipes[i].gap_y = 160.0f + randf() * (SCREEN_H - PIPE_GAP - 320.0f);
        g->pipes[i].passed = false;
        g->pipe_count = i + 1;
    }
}

void game_free(FlappyGame *g) {
    free(g->pipes);
}

void game_jump(FlappyGame *g) {
    if (g->state == STATE_MENU) {
        g->state = STATE_PLAYING;
        g->score = 0;
        g->scroll_x = 0;
        g->pipe_count = 0;
        for (int i = 0; i < 4; i++) {
            g->pipes[i].x = pipe_base_x(i);
            g->pipes[i].gap_y = 160.0f + randf() * (SCREEN_H - PIPE_GAP - 320.0f);
            g->pipes[i].passed = false;
            g->pipe_count = i + 1;
        }
        g->bird.y = SCREEN_H / 2.0f;
        g->bird.vy = JUMP_VELOCITY;
        return;
    }
    if (g->state == STATE_GAMEOVER) {
        g->state = STATE_PLAYING;
        g->score = 0;
        g->scroll_x = 0;
        g->pipe_count = 0;
        for (int i = 0; i < 4; i++) {
            g->pipes[i].x = pipe_base_x(i);
            g->pipes[i].gap_y = 160.0f + randf() * (SCREEN_H - PIPE_GAP - 320.0f);
            g->pipes[i].passed = false;
            g->pipe_count = i + 1;
        }
        g->bird.y = SCREEN_H / 2.0f;
        g->bird.vy = JUMP_VELOCITY;
        return;
    }
    g->bird.vy = JUMP_VELOCITY;
}

static void add_pipe_pair(FlappyGame *g, float world_x) {
    if (g->pipe_count >= g->pipe_cap) {
        g->pipe_cap *= 2;
        g->pipes = realloc(g->pipes, g->pipe_cap * sizeof(Pipe));
    }
    g->pipes[g->pipe_count].x = world_x;
    g->pipes[g->pipe_count].gap_y = 160.0f + randf() * (SCREEN_H - PIPE_GAP - 320.0f);
    g->pipes[g->pipe_count].passed = false;
    g->pipe_count++;
}

void game_update(FlappyGame *g) {
    if (g->state != STATE_PLAYING) return;

    g->bird.vy += GRAVITY;
    g->bird.y += g->bird.vy;

    if (g->bird.y - BIRD_RADIUS < 0) {
        g->bird.y = BIRD_RADIUS;
        g->bird.vy = 0;
    }
    if (g->bird.y + BIRD_RADIUS > SCREEN_H) {
        g->state = STATE_GAMEOVER;
        if (g->score > g->best_score) {
            g->best_score = g->score;
            best_score_save(g->best_score);
        }
        return;
    }

    g->scroll_x += PIPE_SPEED;

    // Recycle pipes that scrolled off screen
    for (int i = 0; i < g->pipe_count; i++) {
        if (get_pipe_x(g, i) < -PIPE_WIDTH - 10) {
            float max_x = 0;
            for (int j = 0; j < g->pipe_count; j++) {
                if (g->pipes[j].x > max_x) max_x = g->pipes[j].x;
            }
            g->pipes[i].x = max_x + PIPE_INTERVAL;
            g->pipes[i].gap_y = 160.0f + randf() * (SCREEN_H - PIPE_GAP - 320.0f);
            g->pipes[i].passed = false;
        }
    }

    // Add new pipe pair as we scroll
    float last_pipe_x = 0;
    for (int i = 0; i < g->pipe_count; i++) {
        if (g->pipes[i].x > last_pipe_x) last_pipe_x = g->pipes[i].x;
    }
    if (last_pipe_x - g->scroll_x < SCREEN_W + PIPE_INTERVAL) {
        add_pipe_pair(g, last_pipe_x + PIPE_INTERVAL);
    }

    // Collision & scoring
    for (int i = 0; i < g->pipe_count; i++) {
        float px = get_pipe_x(g, i);
        float bx = g->bird.x;
        float by = g->bird.y;
        float gy = g->pipes[i].gap_y;

        if (bx + BIRD_RADIUS > px && bx - BIRD_RADIUS < px + PIPE_WIDTH) {
            if (by - BIRD_RADIUS < gy || by + BIRD_RADIUS > gy + PIPE_GAP) {
                g->state = STATE_GAMEOVER;
                if (g->score > g->best_score) {
                    g->best_score = g->score;
                    best_score_save(g->best_score);
                }
                return;
            }
        }

        if (!g->pipes[i].passed && px + PIPE_WIDTH < bx - BIRD_RADIUS) {
            g->pipes[i].passed = true;
            g->score++;
        }
    }
}
