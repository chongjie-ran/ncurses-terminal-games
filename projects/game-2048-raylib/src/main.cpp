#include "game2048.h"
#include <stdio.h>

int main() {
    srand((unsigned)time(NULL));

    InitWindow(WINDOW_W, WINDOW_H, "2048 — Raylib Edition");
    SetTargetFPS(60);

    Game2048 game;
    initGame(&game);

    while (!WindowShouldClose()) {
        // Update
        float dt = GetFrameTime();
        if (game.spawnTimer > 0) game.spawnTimer -= dt;

        if (!game.gameOver && (!game.won || game.keepPlaying)) {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))     move(&game, 0);
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))  move(&game, 1);
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))  move(&game, 2);
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))  move(&game, 3);
        }

        if (IsKeyPressed(KEY_R)) resetGame(&game);
        if (game.won && IsKeyPressed(KEY_C)) game.keepPlaying = true;

        // Draw
        BeginDrawing();
        ClearBackground(BG_COLOR);

        drawBoard(&game);
        drawUI(&game);

        if (game.won && !game.keepPlaying) drawWinOverlay();
        if (game.gameOver) drawGameOver(&game);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
