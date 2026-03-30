#include "game.h"
#include <raylib.h>

int main(void) {
    InitWindow(WINDOW_W, WINDOW_H, "Minesweeper — Raylib");
    SetTargetFPS(60);

    MinesweeperGame game;
    game_init(&game);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // ---- Input ----
        if (IsKeyPressed(KEY_R)) {
            game_reset(&game);
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            int r, c;
            if (game_get_mouse_cell(mp, &r, &c) == 0) {
                if (game.state == STATE_MENU) {
                    game.state = STATE_PLAYING;
                    game_place_mines(&game, r, c);
                    game_reveal(&game, r, c);
                } else if (game.state == STATE_PLAYING) {
                    if (!game.grid[r][c].is_flagged) {
                        game_reveal(&game, r, c);
                    }
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            int r, c;
            if (game_get_mouse_cell(mp, &r, &c) == 0) {
                if (game.state == STATE_PLAYING) {
                    game_toggle_flag(&game, r, c);
                }
            }
        }

        // ---- Update ----
        game_update(&game, dt);

        // ---- Draw ----
        BeginDrawing();
        draw_game(&game);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
