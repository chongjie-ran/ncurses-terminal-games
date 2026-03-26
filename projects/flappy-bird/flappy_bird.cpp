#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

const int WIDTH = 40;
const int HEIGHT = 20;
const int PIPE_WIDTH = 7;
const int PIPE_GAP = 6;
const int GRAVITY = 1;
const int JUMP_VELOCITY = -8;
const int PIPE_SPEED = 1;
const int PIPE_INTERVAL = 15; // frames between pipes

struct FlappyBirdPipe {
    int x;
    int gap_y;
    bool scored;
};

class FlappyBird {
public:
    FlappyBird() : bird_y(HEIGHT / 2), velocity(0), score(0), frame(0), game_over(false) {
        srand(time(nullptr));
        initPipes();
    }
    
    void initPipes();
    void addPipe(int x_pos);
    
    void jump() {
        if (!game_over) velocity = JUMP_VELOCITY;
    }
    
    void update();
    
    bool isGameOver() const { return game_over; }
    int getScore() const { return score; }
    int getBirdY() const { return bird_y; }
    const std::vector<FlappyBirdPipe>& getPipes() const { return pipes; }
    void restart();
    
private:
    int bird_y;
    int velocity;
    int score;
    int frame;
    bool game_over;
    std::vector<FlappyBirdPipe> pipes;
};

void FlappyBird::initPipes() {
    pipes.clear();
    for (int i = 0; i < 3; ++i) {
        addPipe(WIDTH + i * PIPE_INTERVAL);
    }
}

void FlappyBird::addPipe(int x_pos) {
    int min_gap_y = 3;
    int max_gap_y = HEIGHT - PIPE_GAP - 3;
    int gap_y = min_gap_y + rand() % (max_gap_y - min_gap_y);
    pipes.push_back({x_pos, gap_y, false});
}

void FlappyBird::update() {
    if (game_over) return;
    
    ++frame;
    velocity += GRAVITY;
    bird_y += velocity;
    
    if (bird_y < 0) bird_y = 0;
    if (bird_y >= HEIGHT - 1) bird_y = HEIGHT - 2;
    
    for (auto& p : pipes) p.x -= PIPE_SPEED;
    
    pipes.erase(std::remove_if(pipes.begin(), pipes.end(),
        [](const FlappyBirdPipe& p){ return p.x + PIPE_WIDTH < 0; }), pipes.end());
    
    if (frame % PIPE_INTERVAL == 0) {
        addPipe(WIDTH);
    }
    
    if (bird_y >= HEIGHT - 1) {
        game_over = true;
        return;
    }
    
    int bird_x = 5;
    for (const auto& p : pipes) {
        if (bird_x + 2 > p.x && bird_x < p.x + PIPE_WIDTH) {
            if (bird_y < p.gap_y) { game_over = true; return; }
            if (bird_y > p.gap_y + PIPE_GAP) { game_over = true; return; }
        }
    }
    
    for (auto& p : pipes) {
        if (!p.scored && p.x + PIPE_WIDTH < bird_x) {
            p.scored = true;
            ++score;
        }
    }
}

void FlappyBird::restart() {
    bird_y = HEIGHT / 2;
    velocity = 0;
    score = 0;
    frame = 0;
    game_over = false;
    initPipes();
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    
    FlappyBird game;
    int ch;
    
    while (true) {
        ch = getch();
        if (ch == 'q' || ch == 'Q') break;
        if (ch == ' ' || ch == KEY_UP || ch == 'w' || ch == 'W') {
            if (game.isGameOver()) {
                game.restart();
            } else {
                game.jump();
            }
        }
        
        game.update();
        
        erase();
        
        // Border
        mvaddch(0, 0, '+');
        mvaddch(0, WIDTH - 1, '+');
        mvaddch(HEIGHT - 1, 0, '+');
        mvaddch(HEIGHT - 1, WIDTH - 1, '+');
        for (int x = 1; x < WIDTH - 1; ++x) {
            mvaddch(0, x, '-');
            mvaddch(HEIGHT - 1, x, '-');
        }
        for (int y = 1; y < HEIGHT - 1; ++y) {
            mvaddch(y, 0, '|');
            mvaddch(y, WIDTH - 1, '|');
        }
        
        // Pipes
        for (const auto& p : game.getPipes()) {
            if (p.x < 1 || p.x >= WIDTH - 1) continue;
            for (int y = 1; y < p.gap_y; ++y) {
                for (int dx = 0; dx < PIPE_WIDTH; ++dx) {
                    int px = p.x + dx;
                    if (px >= 1 && px < WIDTH - 1) mvaddch(y, px, '#');
                }
            }
            for (int y = p.gap_y + PIPE_GAP; y < HEIGHT - 1; ++y) {
                for (int dx = 0; dx < PIPE_WIDTH; ++dx) {
                    int px = p.x + dx;
                    if (px >= 1 && px < WIDTH - 1) mvaddch(y, px, '#');
                }
            }
        }
        
        // Bird
        int by = game.getBirdY();
        if (by >= 1 && by < HEIGHT - 1) {
            mvaddch(by, 5, '@');
            mvaddch(by - 1, 5, '^');
        }
        
        mvprintw(1, 2, "Score: %d", game.getScore());
        mvprintw(1, 16, "[SPACE] jump  [Q] quit");
        
        if (game.isGameOver()) {
            mvprintw(HEIGHT / 2 - 1, WIDTH / 2 - 5, "GAME OVER!");
            mvprintw(HEIGHT / 2, WIDTH / 2 - 8, "Score: %d", game.getScore());
            mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 10, "Press SPACE to restart");
        }
        
        refresh();
        timeout(50);
    }
    
    endwin();
    return 0;
}
