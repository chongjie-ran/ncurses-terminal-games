#include <emscripten.h>
#include "game.h"

EMSCRIPTEN_KEEPALIVE int get_game_state(void) { return g_game.state; }
EMSCRIPTEN_KEEPALIVE int get_score(void) { return g_game.score; }
EMSCRIPTEN_KEEPALIVE int get_lives(void) { return g_game.lives; }
EMSCRIPTEN_KEEPALIVE int get_paused(void) { return g_game.paused; }
EMSCRIPTEN_KEEPALIVE float get_ball_x(void) { return g_game.ball.x; }
EMSCRIPTEN_KEEPALIVE float get_ball_y(void) { return g_game.ball.y; }
EMSCRIPTEN_KEEPALIVE float get_ball_dx(void) { return g_game.ball.dx; }
EMSCRIPTEN_KEEPALIVE float get_ball_dy(void) { return g_game.ball.dy; }
EMSCRIPTEN_KEEPALIVE float get_paddle_x(void) { return g_game.paddle.x; }
EMSCRIPTEN_KEEPALIVE float get_paddle_y(void) { return g_game.paddle.y; }
EMSCRIPTEN_KEEPALIVE int get_paddle_w(void) { return g_game.paddle.w; }
EMSCRIPTEN_KEEPALIVE int get_paddle_h(void) { return g_game.paddle.h; }
EMSCRIPTEN_KEEPALIVE int get_brick_color(int idx) { return g_game.bricks[idx].hits > 0 ? g_game.bricks[idx].color : 0; }
EMSCRIPTEN_KEEPALIVE int get_ball_r(void) { return BALL_R; }
EMSCRIPTEN_KEEPALIVE int get_frame_count(void) { return g_game.frame_count; }
EMSCRIPTEN_KEEPALIVE void init_game(void) { game_init(); }
EMSCRIPTEN_KEEPALIVE void tick_game(void) { game_tick(); }
EMSCRIPTEN_KEEPALIVE void game_set_paddle_dir(int dir) { g_game.paddle_dir = dir; }
