#ifndef GAME_H
#define GAME_H

#define GRID_W 14
#define GRID_H 15
#define CELL_SIZE 36

#define NUM_LANES 11
#define PLAYER_START_X 6
#define PLAYER_START_Y 13

#define GAME_STATE_PLAYING 0
#define GAME_STATE_DEAD 1
#define GAME_STATE_WIN 2
#define GAME_STATE_GAMEOVER 3

typedef struct {
    float x, y;
    int grid_x, grid_y;
    int lives;
    int score;
    int level;
    int state;
} Player;

typedef struct {
    float x, y;
    int grid_x, grid_y;
    int type; // 0=log, 1=turtle
    int direction; // -1=left, 1=right
    float speed;
    int width; // cells wide
} Obstacle;

void game_init(void);
int game_tick(void);
int game_click(int x, int y);
void game_restart(void);
int get_player_grid_x(void);
int get_player_grid_y(void);
int get_player_lives(void);
int get_player_score(void);
int get_player_level(void);
int get_player_state(void);
int get_obstacle_count(void);
float get_obstacle_x(int idx);
float get_obstacle_y(int idx);
int get_obstacle_type(int idx);
int get_obstacle_width(int idx);
int get_obstacle_dir(int idx);
float get_obstacle_speed(int idx);

#endif
