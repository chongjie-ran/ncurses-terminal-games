#include "game.h"
#include "draw.h"
#include <raylib.h>
#include <stdlib.h>

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Breakout - Raylib");
    SetTargetFPS(60);

    Game game;
    init_game(&game);

    while (!WindowShouldClose()) {
        // input
        float mx = GetMouseX();

        // paddle: mouse controls
        float px = mx - PADDLE_W / 2.0f;
        if (px < 0) px = 0;
        if (px + PADDLE_W > SCREEN_W) px = SCREEN_W - PADDLE_W;
        game.paddle.x = px;

        // keyboard paddle
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            game.paddle.x -= 8;
            if (game.paddle.x < 0) game.paddle.x = 0;
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            game.paddle.x += 8;
            if (game.paddle.x + PADDLE_W > SCREEN_W) game.paddle.x = SCREEN_W - PADDLE_W;
        }

        // state transitions
        if (game.state == STATE_MENU) {
            if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game.state = STATE_PLAYING;
                game.serve_timer = 1.5f;
                game.launch_ready = false;
            }
        } else if (game.state == STATE_PLAYING) {
            if (IsKeyPressed(KEY_P)) {
                game.state = STATE_PAUSED;
            }
            if (game.launch_ready && (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
                launch_ball(&game);
            }
        } else if (game.state == STATE_PAUSED) {
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_SPACE)) {
                game.state = STATE_PLAYING;
            }
        } else if (game.state == STATE_WIN) {
            if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game.level++;
                // restore all bricks
                for (int r = 0; r < BRICK_ROWS; r++) {
                    for (int c = 0; c < BRICK_COLS; c++) {
                        game.bricks[r][c].alive = true;
                    }
                }
                game.bricks_remaining = game.total_bricks;
                game.ball_count = 1;
                reset_ball(&game, 0);
                game.state = STATE_NEXT_LEVEL;
                game.next_ball_timer = 2.0f;
                game.serve_timer = 2.0f;
                game.launch_ready = false;
            }
        } else if (game.state == STATE_GAMEOVER) {
            if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                init_game(&game);
                game.state = STATE_PLAYING;
                game.serve_timer = 1.5f;
            }
        }

        // update
        float dt = GetFrameTime();
        if (game.state == STATE_PLAYING || game.state == STATE_NEXT_LEVEL) {
            update_game(&game, dt);
        }

        // draw
        draw_game(&game);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
