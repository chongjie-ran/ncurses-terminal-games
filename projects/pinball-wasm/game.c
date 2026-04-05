#include "game.h"

// Table geometry
#define LEFT_WALL_X 20.0f
#define RIGHT_WALL_X (SCREEN_W - 20.0f)
#define TOP_WALL_Y 20.0f
#define BOTTOM_Y (SCREEN_H - 20.0f)
#define FLIPPER_Y (SCREEN_H - 100)
#define FLIPPER_PIVOT_DIST 50.0f
#define DRAIN_Y (SCREEN_H - 30)

// Bumper positions (pentagon-ish layout)
static float bumper_xs[BUMPERS] = {200, 120, 280, 80, 320};
static float bumper_ys[BUMPERS] = {180, 260, 260, 340, 340};

void init_game(Game* g) {
    g->state = STATE_READY;
    g->score = 0;
    g->balls_remaining = MAX_BALLS;
    g->launch_power = 0.5f;
    g->drain_timer = 0.0f;
    
    g->ball.active = false;
    g->ball.x = SCREEN_W - 40;
    g->ball.y = SCREEN_H - 60;
    
    // Left flipper - pivot at left-center
    g->left_flipper.x = SCREEN_W/2 - FLIPPER_PIVOT_DIST;
    g->left_flipper.y = FLIPPER_Y;
    g->left_flipper.angle = 0.4f;
    g->left_flipper.target_angle = 0.4f;
    g->left_flipper.len = FLIPPER_W;
    g->left_flipper.is_left = true;
    
    // Right flipper - pivot at right-center
    g->right_flipper.x = SCREEN_W/2 + FLIPPER_PIVOT_DIST;
    g->right_flipper.y = FLIPPER_Y;
    g->right_flipper.angle = M_PI - 0.4f;
    g->right_flipper.target_angle = M_PI - 0.4f;
    g->right_flipper.len = FLIPPER_W;
    g->right_flipper.is_left = false;
    
    for (int i = 0; i < BUMPERS; i++) {
        g->bumpers[i].x = bumper_xs[i];
        g->bumpers[i].y = bumper_ys[i];
        g->bumpers[i].r = BUMPER_R;
        g->bumpers[i].color_idx = i;
        g->bumper_timers[i] = 0.0f;
    }
}

void flipper_up(Flipper* f) {
    if (f->is_left) {
        f->target_angle = -0.5f;
    } else {
        f->target_angle = M_PI + 0.5f;
    }
}

void flipper_down(Flipper* f) {
    if (f->is_left) {
        f->target_angle = 0.4f;
    } else {
        f->target_angle = M_PI - 0.4f;
    }
}

void update_flipper(Flipper* f, float dt) {
    float speed = 12.0f;
    float diff = f->target_angle - f->angle;
    if (diff > 0.05f) {
        f->angle += speed * dt;
        if (f->angle > f->target_angle) f->angle = f->target_angle;
    } else if (diff < -0.05f) {
        f->angle -= speed * dt;
        if (f->angle < f->target_angle) f->angle = f->target_angle;
    }
}

bool circle_circle_hit(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float d = sqrtf(dx*dx + dy*dy);
    return d < r1 + r2;
}

// Get flipper end point given angle
static void get_flipper_end(Flipper* f, float angle, float* ex, float* ey) {
    *ex = f->x + cosf(angle) * f->len;
    *ey = f->y + sinf(angle) * f->len;
}

// Line segment to circle collision
static bool seg_circle_hit(float px, float py, float ex, float ey, float cx, float cy, float cr, float* nx, float* ny) {
    float dx = ex - px;
    float dy = ey - py;
    float fx = px - cx;
    float fy = py - cy;
    
    float a = dx*dx + dy*dy;
    float b = 2*(fx*dx + fy*dy);
    float c = fx*fx + fy*fy - cr*cr;
    
    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) return false;
    
    float t = (-b - sqrtf(discriminant)) / (2*a);
    if (t < 0 || t > 1) t = (-b + sqrtf(discriminant)) / (2*a);
    if (t < 0 || t > 1) return false;
    
    // Normal from circle center to closest point on line
    float closest_x = px + t*dx;
    float closest_y = py + t*dy;
    float len = sqrtf((closest_x-cx)*(closest_x-cx) + (closest_y-cy)*(closest_y-cy));
    if (len < 0.001f) len = 0.001f;
    *nx = (closest_x - cx) / len;
    *ny = (closest_y - cy) / len;
    return true;
}

void launch_ball(Game* g) {
    if (g->state != STATE_READY) return;
    g->ball.x = SCREEN_W - 40;
    g->ball.y = SCREEN_H - 60;
    g->ball.dx = -1.5f;
    g->ball.dy = -g->launch_power * 18.0f;
    g->ball.active = true;
    g->state = STATE_PLAYING;
}

void update_game(Game* g, float dt) {
    // Update bumper hit timers
    for (int i = 0; i < BUMPERS; i++) {
        if (g->bumper_timers[i] > 0) g->bumper_timers[i] -= dt;
    }
    
    // Update flippers
    update_flipper(&g->left_flipper, dt);
    update_flipper(&g->right_flipper, dt);
    
    if (g->state == STATE_DRAIN) {
        g->drain_timer += dt;
        if (g->drain_timer > 1.5f) {
            g->balls_remaining--;
            if (g->balls_remaining <= 0) {
                g->state = STATE_GAMEOVER;
            } else {
                g->state = STATE_READY;
                g->ball.x = SCREEN_W - 40;
                g->ball.y = SCREEN_H - 60;
                g->ball.active = false;
                g->drain_timer = 0.0f;
            }
        }
        return;
    }
    
    if (g->state == STATE_GAMEOVER) return;
    if (!g->ball.active) return;
    
    // Apply gravity
    g->ball.dy += GRAVITY;
    
    // Apply friction
    g->ball.dx *= FRICTION;
    g->ball.dy *= FRICTION;
    
    // Clamp speed
    float speed = sqrtf(g->ball.dx*g->ball.dx + g->ball.dy*g->ball.dy);
    if (speed > BALL_SPEED_MAX) {
        g->ball.dx = (g->ball.dx / speed) * BALL_SPEED_MAX;
        g->ball.dy = (g->ball.dy / speed) * BALL_SPEED_MAX;
    }
    
    // Move ball
    g->ball.x += g->ball.dx;
    g->ball.y += g->ball.dy;
    
    // Wall collisions
    if (g->ball.x - BALL_R < LEFT_WALL_X) {
        g->ball.x = LEFT_WALL_X + BALL_R;
        g->ball.dx = fabsf(g->ball.dx) * BOUNCE_DAMPING;
    }
    if (g->ball.x + BALL_R > RIGHT_WALL_X) {
        g->ball.x = RIGHT_WALL_X - BALL_R;
        g->ball.dx = -fabsf(g->ball.dx) * BOUNCE_DAMPING;
    }
    if (g->ball.y - BALL_R < TOP_WALL_Y) {
        g->ball.y = TOP_WALL_Y + BALL_R;
        g->ball.dy = fabsf(g->ball.dy) * BOUNCE_DAMPING;
    }
    
    // Angled top rails (decorate the upper area)
    // No collision for now - just open table
    
    // Bumper collisions
    for (int i = 0; i < BUMPERS; i++) {
        Bumper* b = &g->bumpers[i];
        if (circle_circle_hit(g->ball.x, g->ball.y, BALL_R, b->x, b->y, b->r)) {
            float nx, ny;
            float dx = g->ball.x - b->x;
            float dy = g->ball.y - b->y;
            float d = sqrtf(dx*dx + dy*dy);
            if (d < 0.001f) d = 0.001f;
            nx = dx/d; ny = dy/d;
            
            // Reflect velocity
            float dot = g->ball.dx * nx + g->ball.dy * ny;
            g->ball.dx = (g->ball.dx - 2*dot*nx) * 1.1f;
            g->ball.dy = (g->ball.dy - 2*dot*ny) * 1.1f;
            
            // Push ball out
            float overlap = BALL_R + b->r - d;
            g->ball.x += nx * (overlap + 1);
            g->ball.y += ny * (overlap + 1);
            
            // Score and hit effect
            g->score += BUMPER_SCORE;
            g->bumper_timers[i] = 0.15f;
        }
    }
    
    // Flipper collisions
    Flipper* flippers[2] = {&g->left_flipper, &g->right_flipper};
    for (int fi = 0; fi < 2; fi++) {
        Flipper* f = flippers[fi];
        float ex, ey;
        get_flipper_end(f, f->angle, &ex, &ey);
        
        float nx, ny;
        if (seg_circle_hit(f->x, f->y, ex, ey, g->ball.x, g->ball.y, BALL_R, &nx, &ny)) {
            // Reflect and boost
            float dot = g->ball.dx * nx + g->ball.dy * ny;
            g->ball.dx = (g->ball.dx - 2*dot*nx);
            g->ball.dy = (g->ball.dy - 2*dot*ny);
            
            // Additional boost when flipper is moving up
            bool moving_up = false;
            if (f->is_left && f->target_angle < f->angle) moving_up = true;
            if (!f->is_left && f->target_angle > f->angle) moving_up = true;
            
            if (moving_up) {
                float boost = (f->is_left ? -1.0f : 1.0f) * 3.0f;
                g->ball.dy -= 5.0f;
                g->ball.dx += boost;
            }
            
            // Push out
            g->ball.x -= nx * 2;
            g->ball.y -= ny * 2;
        }
    }
    
    // Drain detection
    if (g->ball.y > DRAIN_Y) {
        g->ball.active = false;
        g->state = STATE_DRAIN;
        g->drain_timer = 0.0f;
    }
}
