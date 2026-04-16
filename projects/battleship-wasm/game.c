#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 玩家Board（追踪玩家对电脑的射击）
static Player player;
// 电脑Board（追踪电脑对玩家的射击 + 电脑自己的船）
static Player computer;

static Ship ships[NUM_SHIPS] = {
    {SHIP_CARRIER,    -1, -1, 0, 0, 0},
    {SHIP_BATTLESHIP, -1, -1, 0, 0, 0},
    {SHIP_CRUISER,    -1, -1, 0, 0, 0},
    {SHIP_SUBMARINE,  -1, -1, 0, 0, 0},
    {SHIP_DESTROYER,  -1, -1, 0, 0, 0},
};

// 电脑AI状态：hit模式追踪
static int ai_last_hit_r = -1, ai_last_hit_c = -1;
static int ai_mode = 0; // 0=search, 1=hunt
static int ai_dir_idx = 0;
static int ai_hit_stack[4][2];
static int ai_hit_count = 0;

// 方向向量（顺时针）
static const int DX[] = {0, 1, 0, -1};
static const int DY[] = {-1, 0, 1, 0};

static uint32_t rng_state;
static uint32_t rng_next(void) {
    rng_state = rng_state * 1103515245 + 12345;
    return (rng_state >> 16) & 0x7FFF;
}

static int try_place_ship_on_board(Player* p, int size, int row, int col, int horizontal) {
    int r, c;
    if (horizontal) {
        if (col + size > BOARD_SIZE) return 0;
        for (c = col; c < col + size; c++)
            if (p->real_board[row][c] == CELL_SHIP) return 0;
        for (c = col; c < col + size; c++)
            p->real_board[row][c] = CELL_SHIP;
    } else {
        if (row + size > BOARD_SIZE) return 0;
        for (r = row; r < row + size; r++)
            if (p->real_board[r][col] == CELL_SHIP) return 0;
        for (r = row; r < row + size; r++)
            p->real_board[r][col] = CELL_SHIP;
    }
    return 1;
}

static void random_place_ships(Player* p, Ship* ship_arr) {
    int r, c, h, tries, i;
    int sizes[NUM_SHIPS] = {SHIP_CARRIER, SHIP_BATTLESHIP, SHIP_CRUISER, SHIP_SUBMARINE, SHIP_DESTROYER};
    for (i = 0; i < NUM_SHIPS; i++) {
        int placed = 0;
        tries = 0;
        while (!placed && tries < 200) {
            r = rng_next() % BOARD_SIZE;
            c = rng_next() % BOARD_SIZE;
            h = rng_next() % 2;
            placed = try_place_ship_on_board(p, sizes[i], r, c, h);
            tries++;
        }
        if (placed && ship_arr != 0) {
            ship_arr[i].size = sizes[i];
            ship_arr[i].row = r;
            ship_arr[i].col = c;
            ship_arr[i].horizontal = h;
            ship_arr[i].hits = 0;
            ship_arr[i].sunk = 0;
        }
    }
}

static int check_all_sunk(Player* p) {
    int r, c;
    for (r = 0; r < BOARD_SIZE; r++)
        for (c = 0; c < BOARD_SIZE; c++)
            if (p->real_board[r][c] == CELL_SHIP && p->board[r][c] != CELL_HIT)
                return 0;
    return 1;
}

static void update_sunk(Player* p, Ship* ships) {
    int i, r, c;
    for (i = 0; i < NUM_SHIPS; i++) {
        if (ships[i].row < 0) continue;
        int hits = 0;
        if (ships[i].horizontal) {
            for (c = ships[i].col; c < ships[i].col + ships[i].size; c++)
                if (p->board[ships[i].row][c] == CELL_HIT) hits++;
        } else {
            for (r = ships[i].row; r < ships[i].row + ships[i].size; r++)
                if (p->board[r][ships[i].col] == CELL_HIT) hits++;
        }
        ships[i].hits = hits;
        if (hits >= ships[i].size) ships[i].sunk = 1;
    }
}

// --- 电脑AI射击逻辑（"命中率"启发式）---
static int ai_try_dir(int dir, int* r, int* c) {
    int nr = *r + DY[dir];
    int nc = *c + DX[dir];
    if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) return 0;
    if (computer.board[nr][nc] == CELL_HIT || computer.board[nr][nc] == CELL_MISS) return 0;
    *r = nr; *c = nc;
    return 1;
}

static int computer_shoot_impl(void) {
    int r, c;
    if (player.state != STATE_PLAYING) return -1;

    if (ai_mode == 1 && ai_hit_count > 0) {
        // Hunt模式：从命中栈中取一个方向搜索
        int found = 0;
        while (ai_dir_idx < 4 && !found) {
            int nr = ai_last_hit_r + DY[ai_dir_idx];
            int nc = ai_last_hit_c + DX[ai_dir_idx];
            if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE &&
                computer.board[nr][nc] != CELL_HIT && computer.board[nr][nc] != CELL_MISS) {
                r = nr; c = nc; found = 1;
            } else {
                ai_dir_idx++;
            }
        }
        if (!found) {
            // 所有方向都试过了，从栈中弹出一个老的hit点继续
            ai_hit_count--;
            if (ai_hit_count > 0) {
                ai_last_hit_r = ai_hit_stack[ai_hit_count-1][0];
                ai_last_hit_c = ai_hit_stack[ai_hit_count-1][1];
                ai_dir_idx = 0;
                // 递归找下一个方向
                return computer_shoot_impl();
            }
            ai_mode = 0;
        }
    }

    if (ai_mode == 0) {
        // Search模式：随机扫描
        int attempts = 0;
        do {
            // 优先选"十"字交叉点启发式
            if (attempts < 50) {
                r = rng_next() % BOARD_SIZE;
                c = rng_next() % BOARD_SIZE;
                // 跳过奇偶格以提高命中率
                if ((r + c) % 2 == 0) {
                    attempts++;
                    continue;
                }
            } else {
                r = rng_next() % BOARD_SIZE;
                c = rng_next() % BOARD_SIZE;
            }
            attempts++;
        } while ((computer.board[r][c] == CELL_HIT || computer.board[r][c] == CELL_MISS) && attempts < 200);
    }

    if (computer.board[r][c] == CELL_HIT || computer.board[r][c] == CELL_MISS) {
        r = -1; c = -1; // 无效射击
    }

    computer.board[r][c] = CELL_HIT; // 先默认miss

    if (player.real_board[r][c] == CELL_SHIP) {
        computer.board[r][c] = CELL_HIT;
        player.hit_count++;

        // 追踪AI命中
        if (ai_mode == 0) {
            ai_mode = 1;
            ai_last_hit_r = r; ai_last_hit_c = c;
            ai_hit_count = 1;
            ai_hit_stack[0][0] = r; ai_hit_stack[0][1] = c;
            ai_dir_idx = 0;
        } else {
            // 已在hunt模式，添加新的命中点
            if (ai_hit_count < 4) {
                ai_hit_stack[ai_hit_count][0] = r; ai_hit_stack[ai_hit_count][1] = c;
                ai_hit_count++;
            }
        }

        if (check_all_sunk(&player)) {
            player.state = STATE_LOSE;
        }
        return 1; // HIT
    } else {
        computer.board[r][c] = CELL_MISS;
        // 如果hunt模式中这个方向是miss，尝试下一个方向
        if (ai_mode == 1) {
            ai_dir_idx++;
        }
        return 0; // MISS
    }
}

// --- 导出函数 ---
static Ship computer_ships[NUM_SHIPS];

void battleship_init_impl(void) {
    battleship_reset_impl();
}

void battleship_reset_impl(void) {
    int r, c, i;
    rng_state = (uint32_t)(time(NULL) ^ (intptr_t)&battleship_init_impl);

    for (r = 0; r < BOARD_SIZE; r++)
        for (c = 0; c < BOARD_SIZE; c++) {
            player.board[r][c] = CELL_EMPTY;
            player.real_board[r][c] = CELL_EMPTY;
            computer.board[r][c] = CELL_EMPTY;
            computer.real_board[r][c] = CELL_EMPTY;
        }

    player.hit_count = 0; player.miss_count = 0;
    player.ships_sunk = 0; player.state = STATE_PLACING;
    player.ship_count = 0;

    computer.hit_count = 0; computer.miss_count = 0;
    computer.ships_sunk = 0; computer.ship_count = 0;

    ai_mode = 0; ai_last_hit_r = -1; ai_last_hit_c = -1;
    ai_dir_idx = 0; ai_hit_count = 0;

    for (i = 0; i < NUM_SHIPS; i++) {
        ships[i].hits = 0; ships[i].sunk = 0;
        ships[i].row = -1; ships[i].col = -1;
        computer_ships[i].hits = 0; computer_ships[i].sunk = 0;
        computer_ships[i].row = -1; computer_ships[i].col = -1;
    }
}

int battleship_get_cell_impl(int board_type, int row, int col) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return -1;
    if (board_type == 0) return player.board[row][col];         // 玩家追踪板
    if (board_type == 1) return player.real_board[row][col];    // 玩家船只（调试用）
    if (board_type == 2) return computer.board[row][col];       // 电脑追踪板
    return -1;
}

int battleship_get_game_state_impl(void) {
    return player.state;
}

int battleship_get_ships_sunk_impl(void) {
    return player.ships_sunk;
}

int battleship_get_hit_count_impl(void) {
    return player.hit_count;
}

int battleship_get_miss_count_impl(void) {
    return player.miss_count;
}

int battleship_get_ship_count_impl(void) {
    return player.ship_count;
}

// Computer ship positions (tracked when randomly placed)
// Count sunk ships by checking each computer ship's all cells are hit
static int count_computer_ships_sunk(void) {
    int sunk = 0;
    int r, c, i;
    for (i = 0; i < NUM_SHIPS; i++) {
        if (computer_ships[i].row < 0) continue;
        int all_hit = 1;
        if (computer_ships[i].horizontal) {
            for (c = computer_ships[i].col; c < computer_ships[i].col + computer_ships[i].size; c++) {
                if (player.board[computer_ships[i].row][c] != CELL_HIT) { all_hit = 0; break; }
            }
        } else {
            for (r = computer_ships[i].row; r < computer_ships[i].row + computer_ships[i].size; r++) {
                if (player.board[r][computer_ships[i].col] != CELL_HIT) { all_hit = 0; break; }
            }
        }
        if (all_hit) sunk++;
    }
    return sunk;
}

int battleship_get_computer_ships_sunk_impl(void) {
    return count_computer_ships_sunk();
}

int battleship_place_ship_impl(int ship_idx, int row, int col, int horizontal) {
    if (ship_idx < 0 || ship_idx >= NUM_SHIPS) return 0;
    if (player.ship_count >= NUM_SHIPS) return 0;

    int size = ships[ship_idx].size;
    int r, c, ok = 1;

    if (horizontal) {
        if (col + size > BOARD_SIZE) return 0;
        for (c = col; c < col + size; c++)
            if (player.real_board[row][c] == CELL_SHIP) { ok = 0; break; }
    } else {
        if (row + size > BOARD_SIZE) return 0;
        for (r = row; r < row + size; r++)
            if (player.real_board[r][col] == CELL_SHIP) { ok = 0; break; }
    }
    if (!ok) return 0;

    if (horizontal) {
        for (c = col; c < col + size; c++)
            player.real_board[row][c] = CELL_SHIP;
    } else {
        for (r = row; r < row + size; r++)
            player.real_board[r][col] = CELL_SHIP;
    }

    ships[ship_idx].row = row;
    ships[ship_idx].col = col;
    ships[ship_idx].horizontal = horizontal;
    ships[ship_idx].hits = 0;
    ships[ship_idx].sunk = 0;
    player.ship_count++;

    if (player.ship_count >= NUM_SHIPS) {
        player.state = STATE_PLAYING;
        // 电脑随机放置船只
        random_place_ships(&computer, computer_ships);
    }
    return 1;
}

int battleship_shoot_impl(int row, int col) {
    if (player.state != STATE_PLAYING) return -1;
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return -1;
    if (player.board[row][col] == CELL_HIT || player.board[row][col] == CELL_MISS) return -1;

    // 玩家射击电脑
    int cell = computer.real_board[row][col];
    if (cell == CELL_SHIP) {
        player.board[row][col] = CELL_HIT;
        player.hit_count++;
        if (check_all_sunk(&computer)) {
            player.state = STATE_WIN;
            return 1;
        }
        return 1; // HIT
    } else {
        player.board[row][col] = CELL_MISS;
        player.miss_count++;
        return 0; // MISS
    }
}

int battleship_computer_turn_impl(void) {
    // 执行电脑回合并返回射击结果（1=hit, 0=miss）
    return computer_shoot_impl();
}
