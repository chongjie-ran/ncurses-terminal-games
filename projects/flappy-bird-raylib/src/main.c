// Flappy Bird with Raylib - Main Entry
// Gravity physics + pipe gap navigation
#include "raylib.h"
#include "game.h"
#include <stdlib.h>
#include <time.h>

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Flappy Bird - Raylib Edition");
    SetTargetFPS(60);
    SetExitKey(0);  // no ESC exits

    srand((unsigned int)time(NULL));

    FlappyGame game;
    game_init(&game);

    while (!WindowShouldClose()) {
        // Jump input
        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) ||
            IsKeyPressed(KEY_UP) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            game_jump(&game);
        }

        game_update(&game);
        game_draw(&game);
    }

    game_free(&game);
    CloseWindow();
    return 0;
}
