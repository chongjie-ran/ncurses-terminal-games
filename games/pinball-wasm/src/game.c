#include <math.h>
#include <stdlib.h>
#include "../game.h"

static unsigned int seed = 12345;
static unsigned int simpleRand() {
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}
static float simpleRandFloat() {
    return simpleRand() / 32768.0f;
}

void initGame(Game* g) {
    g->score = 0;
    g->lives = 3;
    g->ballsLaunched = 0;
    g->totalBumperHits = 0;
    g->state = STATE_IDLE;
    g->paddleX = CANVAS_WIDTH / 2;
    g->launchTimer = 0;
    g->ball.x = CANVAS_WIDTH / 2;
    g->ball.y = PADDLE_Y - 50;
    g->ball.vx = 0;
    g->ball.vy = 0;
    g->ball.active = 0;
    g->leftFlipper.x = 100;
    g->leftFlipper.angle = 0.4f;
    g->leftFlipper.targetAngle = 0.4f;
    g->leftFlipper.side = 0;
    g->rightFlipper.x = CANVAS_WIDTH - 100;
    g->rightFlipper.angle = M_PI - 0.4f;
    g->rightFlipper.targetAngle = M_PI - 0.4f;
    g->rightFlipper.side = 1;
    float bumperPositions[NUM_BUMPERS][3] = {
        {CANVAS_WIDTH / 2.0f, 120, 0},
        {80, 200, 1},
        {CANVAS_WIDTH - 80, 200, 2},
        {130, 300, 3},
        {CANVAS_WIDTH - 130, 300, 4},
    };
    for (int i = 0; i < NUM_BUMPERS; i++) {
        g->bumpers[i].x = bumperPositions[i][0];
        g->bumpers[i].y = bumperPositions[i][1];
        g->bumpers[i].radius = BUMPER_RADIUS;
        g->bumpers[i].hit = 0;
        g->bumpers[i].score = 100 + i * 50;
    }
}

void launchBall(Game* g) {
    g->ball.x = g->paddleX;
    g->ball.y = PADDLE_Y - 20;
    g->ball.vx = (simpleRandFloat() - 0.5f) * 4;
    g->ball.vy = -10 - simpleRandFloat() * 3;
    g->ball.active = 1;
    g->ballsLaunched++;
    g->bumperHitCount = 0;
    g->state = STATE_PLAYING;
    for (int i = 0; i < NUM_BUMPERS; i++) {
        g->bumpers[i].hit = 0;
    }
}

int isBallLost(Game* g) {
    return g->ball.y > CANVAS_HEIGHT + BALL_RADIUS;
}

void resetBall(Game* g) {
    g->ball.x = g->paddleX;
    g->ball.y = PADDLE_Y - 20;
    g->ball.vx = 0;
    g->ball.vy = 0;
    g->ball.active = 0;
    if (g->ballsLaunched >= MAX_BALL_LAUNCHES) {
        g->state = STATE_GAME_OVER;
    } else {
        g->state = STATE_IDLE;
        g->launchTimer = 0;
    }
}
static int circleRectCollision(float cx, float cy, float radius,
                                float rx, float ry, float rw, float rh) {
    float closestX = fmaxf(rx, fminf(cx, rx + rw));
    float closestY = fmaxf(ry, fminf(cy, ry + rh));
    float dx = cx - closestX;
    float dy = cy - closestY;
    return (dx * dx + dy * dy) < (radius * radius);
}

static void bounceBallOffRect(float* bx, float* by, float* vx, float* vy, float radius,
                               float rx, float ry, float rw, float rh) {
    float closestX = fmaxf(rx, fminf(*bx, rx + rw));
    float closestY = fmaxf(ry, fminf(*by, ry + rh));
    float dx = *bx - closestX;
    float dy = *by - closestY;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist < 0.001f) dist = 0.001f;
    float nx = dx / dist;
    float ny = dy / dist;
    float dot = (*vx) * nx + (*vy) * ny;
    *vx = (*vx - 2 * dot * nx) * BOUNCE_DAMPING;
    *vy = (*vy - 2 * dot * ny) * BOUNCE_DAMPING;
    *bx = closestX + nx * (radius + 1);
    *by = closestY + ny * (radius + 1);
}
static void bounceBallOffCircle(float* bx, float* by, float* vx, float* vy, float radius,
                                  float cx, float cy, float cr) {
    float dx = *bx - cx;
    float dy = *by - cy;
    float dist = sqrtf(dx * dx + dy * dy);
    float minDist = radius + cr;
    if (dist < 0.001f) dist = 0.001f;
    float nx = dx / dist;
    float ny = dy / dist;
    float dot = (*vx) * nx + (*vy) * ny;
    *vx = (*vx - 2 * dot * nx) * BOUNCE_DAMPING;
    *vy = (*vy - 2 * dot * ny) * BOUNCE_DAMPING;
    *bx = cx + nx * (minDist + 1);
    *by = cy + ny * (minDist + 1);
}
static void bounceBallOffFlipper(float* bx, float* by, float* vx, float* vy, float radius,
                                  Flipper* f) {
    float halfLen = FLIPPER_WIDTH / 2.0f;
    float cx = f->x;
    float cy = FLIPPER_LOWER_Y;
    float cosA = cosf(-f->angle);
    float sinA = sinf(-f->angle);
    float localX = cosA * (*bx - cx) - sinA * (*by - cy);
    float localY = sinA * (*bx - cx) + cosA * (*by - cy);
    float localClosestX = fmaxf(-halfLen, fminf(localX, halfLen));
    float localClosestY = fmaxf(-FLIPPER_HEIGHT/2, fminf(localY, FLIPPER_HEIGHT/2));
    float dx = localX - localClosestX;
    float dy = localY - localClosestY;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist < radius && dist > 0.001f) {
        float nx = dx / dist;
        float ny = dy / dist;
        float localVx = cosA * (*vx) - sinA * (*vy);
        float localVy = sinA * (*vx) + cosA * (*vy);
        float dot = localVx * nx + localVy * ny;
        if (dot < 0) {
            localVx -= 2 * dot * nx;
            localVy -= 2 * dot * ny;
            localVy += FLIPPER_BOUNCE * (f->side == 0 ? 1 : -1) *
                       (f->angle - (f->side == 0 ? 0.4f : M_PI - 0.4f)) * 0.5f;
            cosA = cosf(f->angle);
            sinA = sinf(f->angle);
            *vx = cosA * localVx + sinA * localVy;
            *vy = -sinA * localVx + cosA * localVy;
            *vx *= BOUNCE_DAMPING;
            *vy *= BOUNCE_DAMPING;
            cosA = cosf(-f->angle);
            sinA = sinf(-f->angle);
            float outX = cx + cosA * localClosestX - sinA * localClosestY;
            float outY = cy + sinA * localClosestX + cosA * localClosestY;
            *bx = outX - nx * (radius + 2);
            *by = outY - ny * (radius + 2);
        }
    }
}
static void bounceOffWalls(float* x, float* y, float* vx, float* vy, float radius) {
    if (*x - radius < WALL_THICKNESS) {
        *x = WALL_THICKNESS + radius;
        *vx = fabsf(*vx) * BOUNCE_DAMPING;
    }
    if (*x + radius > CANVAS_WIDTH - WALL_THICKNESS) {
        *x = CANVAS_WIDTH - WALL_THICKNESS - radius;
        *vx = -fabsf(*vx) * BOUNCE_DAMPING;
    }
    if (*y - radius < WALL_THICKNESS) {
        *y = WALL_THICKNESS + radius;
        *vy = fabsf(*vy) * BOUNCE_DAMPING;
    }
}
static void bounceOffSlingshots(float* x, float* y, float* vx, float* vy, float radius) {
    if (*x < 60 && *y > 350 && *y < 500) {
        float targetX = 80;
        float targetY = 430;
        float dx = *x - targetX;
        float dy = *y - targetY;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < 30) {
            float nx = -dx/dist;
            float ny = dy/dist;
            float dot = (*vx)*nx + (*vy)*ny;
            if (dot < 0) {
                *vx = (*vx - 2*dot*nx) * 1.2f;
                *vy = (*vy - 2*dot*ny) * 1.2f;
                *x -= nx * 5;
                *y -= ny * 5;
            }
        }
    }
    if (*x > CANVAS_WIDTH - 60 && *y > 350 && *y < 500) {
        float targetX = CANVAS_WIDTH - 80;
        float targetY = 430;
        float dx = *x - targetX;
        float dy = *y - targetY;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist < 30) {
            float nx = -dx/dist;
            float ny = dy/dist;
            float dot = (*vx)*nx + (*vy)*ny;
            if (dot < 0) {
                *vx = (*vx - 2*dot*nx) * 1.2f;
                *vy = (*vy - 2*dot*ny) * 1.2f;
                *x -= nx * 5;
                *y -= ny * 5;
            }
        }
    }
}
void updateGame(Game* g, float dt) {
    if (dt > 0.05f) dt = 0.05f;
    if (g->state == STATE_IDLE) {
        g->launchTimer += dt;
        g->ball.x = g->paddleX;
        g->ball.y = PADDLE_Y - 15;
        return;
    }
    if (g->state != STATE_PLAYING) return;
    if (!g->ball.active) return;
    g->ball.vy += GRAVITY;
    g->ball.vx *= FRICTION;
    g->ball.vy *= FRICTION;
    float speed = sqrtf(g->ball.vx * g->ball.vx + g->ball.vy * g->ball.vy);
    if (speed > MAX_VELOCITY) {
        g->ball.vx = (g->ball.vx / speed) * MAX_VELOCITY;
        g->ball.vy = (g->ball.vy / speed) * MAX_VELOCITY;
    }
    g->ball.x += g->ball.vx;
    g->ball.y += g->ball.vy;
    bounceOffWalls(&g->ball.x, &g->ball.y, &g->ball.vx, &g->ball.vy, BALL_RADIUS);
    bounceOffSlingshots(&g->ball.x, &g->ball.y, &g->ball.vx, &g->ball.vy, BALL_RADIUS);
    for (int i = 0; i < NUM_BUMPERS; i++) {
        Bumper* b = &g->bumpers[i];
        float dx = g->ball.x - b->x;
        float dy = g->ball.y - b->y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist < BALL_RADIUS + b->radius) {
            float nx = dx / dist;
            float ny = dy / dist;
            float dot = g->ball.vx * nx + g->ball.vy * ny;
            if (dot < 0) {
                g->ball.vx -= 2 * dot * nx;
                g->ball.vy -= 2 * dot * ny;
                float bounceSpeed = 8;
                g->ball.vx += nx * bounceSpeed;
                g->ball.vy += ny * bounceSpeed;
            }
            float minDist = BALL_RADIUS + b->radius + 2;
            g->ball.x = b->x + nx * minDist;
            g->ball.y = b->y + ny * minDist;
            if (!b->hit) {
                b->hit = 1;
                g->score += b->score;
                g->bumperHitCount++;
                g->totalBumperHits++;
            }
        }
    }
    float paddleLeft = g->paddleX - PADDLE_WIDTH / 2;
    float paddleRight = g->paddleX + PADDLE_WIDTH / 2;
    if (g->ball.y + BALL_RADIUS > PADDLE_Y &&
        g->ball.y - BALL_RADIUS < PADDLE_Y + PADDLE_HEIGHT &&
        g->ball.x > paddleLeft && g->ball.x < paddleRight &&
        g->ball.vy > 0) {
        float hitPos = (g->ball.x - g->paddleX) / (PADDLE_WIDTH / 2);
        float angle = hitPos * 1.2f;
        speed = sqrtf(g->ball.vx * g->ball.vx + g->ball.vy * g->ball.vy);
        if (speed < 5) speed = 5;
        g->ball.vx = sinf(angle) * speed;
        g->ball.vy = -cosf(angle) * speed;
        g->ball.y = PADDLE_Y - BALL_RADIUS - 1;
    }
    bounceBallOffFlipper(&g->ball.x, &g->ball.y, &g->ball.vx, &g->ball.vy,
                          BALL_RADIUS, &g->leftFlipper);
    bounceBallOffFlipper(&g->ball.x, &g->ball.y, &g->ball.vx, &g->ball.vy,
                          BALL_RADIUS, &g->rightFlipper);
    if (isBallLost(g)) {
        g->ball.active = 0;
        resetBall(g);
    }
}
