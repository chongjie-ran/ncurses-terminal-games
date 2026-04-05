#ifndef GAME_H
#define GAME_H

// Canvas dimensions
#define CANVAS_WIDTH 400
#define CANVAS_HEIGHT 700

// Ball
#define BALL_RADIUS 8
#define GRAVITY 0.15
#define FRICTION 0.999
#define BOUNCE_DAMPING 0.75
#define MAX_VELOCITY 15

// Paddle
#define PADDLE_WIDTH 80
#define PADDLE_HEIGHT 14
#define PADDLE_Y (CANVAS_HEIGHT - 60)
#define PADDLE_SPEED 8

// Flippers
#define FLIPPER_WIDTH 60
#define FLIPPER_HEIGHT 12
#define FLIPPER_LOWER_Y (CANVAS_HEIGHT - 40)
#define FLIPPER_ANGULAR_VELOCITY 0.3
#define FLIPPER_BOUNCE 1.5

// Bumpers (3 circular bumpers)
#define NUM_BUMPERS 5
#define BUMPER_RADIUS 25

// Walls
#define WALL_THICKNESS 10

// Game states
#define STATE_IDLE 0       // Waiting to launch
#define STATE_PLAYING 1     // Ball in play
#define STATE_GAME_OVER 2   // No balls left
#define STATE_WIN 3         // All bumpers hit

// Bumper hit tracking
#define MAX_BALL_LAUNCHES 3

typedef struct {
    float x, y;           // Center position
    float vx, vy;         // Velocity
    int active;           // Is ball in play
} Ball;

typedef struct {
    float x;              // Center X
    float angle;          // Current angle (radians)
    float targetAngle;    // Target angle
    int side;             // 0=left, 1=right
} Flipper;

typedef struct {
    float x, y;
    int radius;
    int hit;              // Has been hit this ball
    int score;            // Points value
} Bumper;

typedef struct {
    Ball ball;
    Flipper leftFlipper;
    Flipper rightFlipper;
    Bumper bumpers[NUM_BUMPERS];
    float paddleX;        // Paddle center X
    int state;
    int score;
    int lives;
    int ballsLaunched;
    int totalBumperHits;
    int bumperHitCount;    // Hits this ball
    float launchTimer;     // Auto-launch timer
} Game;

void initGame(Game* g);
void updateGame(Game* g, float dt);
void launchBall(Game* g);
int isBallLost(Game* g);
void resetBall(Game* g);

#endif
