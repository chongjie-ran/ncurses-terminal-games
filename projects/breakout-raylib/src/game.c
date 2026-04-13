#include "game.h"
#include <stdlib.h>
#include <math.h>

void init_game(Game* g) {
    g->state = STATE_MENU;
    g->lives = 3;
    g->score = 0;
    g->level = 1;
    g->ball_count = 1;
    g->serve_timer = 0;
    g->launch_ready = false;

    g->paddle.x = SCREEN_W / 2.0f - PADDLE_W / 2.0f;
    g->paddle.y = PADDLE_Y;
    g->paddle.w = PADDLE_W;
    g->paddle.h = PADDLE_H;

    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            g->bricks[r][c].row = r;
            g->bricks[r][c].col = c;
            g->bricks[r][c].alive = true;
            g->bricks[r][c].color_idx = r;
        }
    }
    g->total_bricks = BRICK_ROWS * BRICK_COLS;
    g->bricks_remaining = g->total_bricks;

    reset_ball(g, 0);
    for (int i = 1; i < MAX_BALLS; i++) {
        g->balls[i].active = false;
    }
}

void reset_ball(Game* g, int ball_idx) {
    Ball* b = &g->balls[ball_idx];
    b->x = g->paddle.x + PADDLE_W / 2.0f;
    b->y = PADDLE_Y - BALL_R - 2;
    
    float angle = -60.0f + (rand() % 120);
    float rad = angle * M_PI / 180.0f;
    b->dx = BALL_SPEED * cosf(rad);
    b->dy = BALL_SPEED * sinf(rad);
    // always going up initially
    if (b->dy > 0) b->dy = -b->dy;
    
    b->active = true;
    g->launch_ready = true;
}

void launch_ball(Game* g) {
    g->launch_ready = false;
}

bool rect_circle_hit(float rx, float ry, int rw, int rh, float cx, float cy, float cr) {
    float nearest_x = fmaxf(rx, fminf(cx, rx + rw));
    float nearest_y = fmaxf(ry, fminf(cy, ry + rh));
    float dx = cx - nearest_x;
    float dy = cy - nearest_y;
    return (dx * dx + dy * dy) < (cr * cr);
}

void update_game(Game* g, float dt) {
    if (g->state == STATE_MENU) return;
    if (g->state == STATE_WIN || g->state == STATE_GAMEOVER) return;
    if (g->state == STATE_NEXT_LEVEL) {
        g->next_ball_timer -= dt;
        if (g->next_ball_timer <= 0) {
            g->state = STATE_PLAYING;
            g->serve_timer = 2.0f;
            g->launch_ready = true;
        }
        return;
    }
    if (g->state == STATE_PAUSED) return;

    // serve timer
    if (g->serve_timer > 0) {
        g->serve_timer -= dt;
        if (g->serve_timer <= 0) {
            g->launch_ready = true;
        }
    }

    // move paddle to ball if not launched
    if (g->launch_ready && g->balls[0].active) {
        g->balls[0].x = g->paddle.x + PADDLE_W / 2.0f;
        g->balls[0].y = PADDLE_Y - BALL_R - 2;
    }

    // update balls
    for (int bi = 0; bi < g->ball_count; bi++) {
        Ball* b = &g->balls[bi];
        if (!b->active) continue;

        b->x += b->dx;
        b->y += b->dy;

        // wall collisions
        if (b->x - BALL_R < 0) {
            b->x = BALL_R;
            b->dx = fabsf(b->dx);
        }
        if (b->x + BALL_R > SCREEN_W) {
            b->x = SCREEN_W - BALL_R;
            b->dx = -fabsf(b->dx);
        }
        if (b->y - BALL_R < 0) {
            b->y = BALL_R;
            b->dy = fabsf(b->dy);
        }

        // paddle collision
        if (rect_circle_hit(g->paddle.x, g->paddle.y, g->paddle.w, g->paddle.h, b->x, b->y, BALL_R)) {
            if (b->dy > 0) {
                b->dy = -fabsf(b->dy);
                // angle based on hit position
                float hit_pos = (b->x - g->paddle.x) / PADDLE_W; // 0..1
                float angle = 150.0f + hit_pos * 120.0f; // 150 to 270 degrees
                float rad = angle * M_PI / 180.0f;
                float speed = sqrtf(b->dx * b->dx + b->dy * b->dy);
                b->dx = speed * cosf(rad);
                b->dy = speed * sinf(rad);
                if (b->dy > -1.0f) b->dy = -1.0f * speed;
                b->y = g->paddle.y - BALL_R - 1;
            }
        }

        // brick collisions
        for (int r = 0; r < BRICK_ROWS; r++) {
            for (int c = 0; c < BRICK_COLS; c++) {
                Brick* br = &g->bricks[r][c];
                if (!br->alive) continue;

                float bx = BRICK_LEFT + c * (BRICK_W + BRICK_GAP);
                float by = BRICK_TOP + r * (BRICK_H + BRICK_GAP);

                if (rect_circle_hit(bx, by, BRICK_W, BRICK_H, b->x, b->y, BALL_R)) {
                    br->alive = false;
                    g->bricks_remaining--;
                    g->score += (BRICK_ROWS - r) * 10;

                    // determine bounce direction
                    float overlap_left = (bx + BRICK_W) - (b->x - BALL_R);
                    float overlap_right = (b->x + BALL_R) - bx;
                    float overlap_top = (by + BRICK_H) - (b->y - BALL_R);
                    float overlap_bottom = (b->y + BALL_R) - by;

                    float min_overlap_x = fminf(overlap_left, overlap_right);
                    float min_overlap_y = fminf(overlap_top, overlap_bottom);

                    if (min_overlap_x < min_overlap_y) {
                        b->dx = -b->dx;
                    } else {
                        b->dy = -b->dy;
                    }
                    break;
                }
            }
        }

        // ball fell
        if (b->y - BALL_R > SCREEN_H) {
            b->active = false;
        }
    }

    // check if all balls lost
    bool any_active = false;
    for (int i = 0; i < g->ball_count; i++) {
        if (g->balls[i].active) { any_active = true; break; }
    }

    if (!any_active) {
        g->lives--;
        if (g->lives <= 0) {
            g->state = STATE_GAMEOVER;
        } else {
            g->state = STATE_PLAYING;
            g->serve_timer = 1.5f;
            g->launch_ready = false;
            reset_ball(g, 0);
        }
    }

    // check win
    if (g->bricks_remaining <= 0) {
        g->state = STATE_WIN;
    }
}
