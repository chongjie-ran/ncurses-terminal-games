/**
 * Hangman - Terminal Version
 * 
 * 猜单词游戏，使用 ncurses 库实现
 * 
 * 编译: g++ -std=c++17 -Wall -Wextra -O2 -o hangman hangman.cpp -lncurses
 * 运行: ./hangman
 * 
 * 操作:
 *   - 输入字母进行猜测
 *   - Q 退出游戏
 */

#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <set>
#include <cctype>

using namespace std;

// 游戏状态
enum GameState { PLAYING, WON, LOST };

// 单词列表
const vector<string> WORD_BANK = {
    "programming", "computer", "algorithm", "developer", "keyboard",
    "software", "hardware", "network", "database", "compiler",
    "debugging", "function", "variable", "recursive", "binary",
    "terminal", "terminal", "compile", "execute", "runtime",
    "python", "javascript", "algorithm", "datastructure", "artificial",
    "intelligence", "machine", "learning", "deep", "neural",
    "network", "processor", "memory", "storage", "display",
    "resolution", "keyboard", "mouse", "monitor", "speaker",
    "graphics", "physics", "engine", "render", "shader",
    "animation", "simulation", "physics", "collision", "velocity",
    "gravity", "friction", "momentum", "acceleration", "trajectory"
};

// 单词类别
const vector<string> CATEGORIES = {
    "Programming", "Technology", "AI/ML", "Computer Parts", "Game Dev"
};

// 每类单词对应的索引范围
const vector<pair<int,int>> CATEGORY_RANGES = {
    {0, 4},    // Programming
    {5, 14},   // Technology  
    {15, 23},  // AI/ML
    {24, 32},  // Computer Parts
    {33, 44}   // Game Dev
};

// Hangman ASCII art stages (6 mistakes allowed)
const vector<string> HANGMAN_STAGES = {
    // Stage 0: Empty
    R"(
    +-------+
    |       |
    |
    |
    |
    |
======)",
    
    // Stage 1: Head
    R"(
    +-------+
    |       |
    |       O
    |
    |
    |
======)",
    
    // Stage 2: Body
    R"(
    +-------+
    |       |
    |       O
    |       |
    |
    |
======)",
    
    // Stage 3: Left arm
    R"(
    +-------+
    |       |
    |       O
    |      /|
    |
    |
======)",
    
    // Stage 4: Right arm
    R"(
    +-------+
    |       |
    |       O
    |      /|\
    |
    |
======)",
    
    // Stage 5: Left leg
    R"(
    +-------+
    |       |
    |       O
    |      /|\
    |      /
    |
======)",
    
    // Stage 6: Right leg (GAME OVER)
    R"(
    +-------+
    |       |
    |       O
    |      /|\
    |      / \
    |
======)"
};

class Hangman {
public:
    Hangman() {
        srand(time(nullptr));
        init_game();
    }
    
    void init_game() {
        // 随机选择单词类别
        int cat_idx = rand() % CATEGORY_RANGES.size();
        const auto& range = CATEGORY_RANGES[cat_idx];
        int word_idx = range.first + rand() % (range.second - range.first + 1);
        
        word = WORD_BANK[word_idx];
        category = CATEGORIES[cat_idx];
        
        // 转换为大写
        for (char& c : word) {
            c = toupper(c);
        }
        
        // 初始化显示
        display.resize(word.length(), '_');
        guessed_letters.clear();
        wrong_guesses = 0;
        state = PLAYING;
        
        // 随机显示1-2个字母作为提示
        vector<int> indices(word.length());
        for (int i = 0; i < (int)indices.size(); ++i) indices[i] = i;
        shuffle(indices.begin(), indices.end(), default_random_engine(rand()));
        
        int hint_count = min(2, max(1, (int)word.length() / 4));
        for (int i = 0; i < hint_count && i < (int)indices.size(); ++i) {
            int pos = indices[i];
            display[pos] = word[pos];
            guessed_letters.insert(word[pos]);
        }
    }
    
    bool guess(char letter) {
        letter = toupper(letter);
        
        if (guessed_letters.count(letter)) {
            return false; // 已经猜过
        }
        
        guessed_letters.insert(letter);
        
        bool found = false;
        for (int i = 0; i < (int)word.length(); ++i) {
            if (word[i] == letter) {
                display[i] = letter;
                found = true;
            }
        }
        
        if (!found) {
            ++wrong_guesses;
        }
        
        update_state();
        return found;
    }
    
    void update_state() {
        // 检查是否获胜
        bool all_revealed = true;
        for (size_t i = 0; i < word.length(); ++i) {
            if (display[i] == '_') {
                all_revealed = false;
                break;
            }
        }
        
        if (all_revealed) {
            state = WON;
        } else if (wrong_guesses >= 6) {
            state = LOST;
        }
    }
    
    const string& get_word() const { return word; }
    const string& get_category() const { return category; }
    const vector<char>& get_display() const { return display; }
    const set<char>& get_guessed_letters() const { return guessed_letters; }
    int get_wrong_guesses() const { return wrong_guesses; }
    GameState get_state() const { return state; }
    
    string get_display_string() const {
        string result;
        for (char c : display) {
            result += c;
            result += ' ';
        }
        return result;
    }
    
    string get_wrong_letters_string() const {
        string result;
        string wrong;
        for (char c : guessed_letters) {
            bool is_wrong = true;
            for (char wc : word) {
                if (wc == c) {
                    is_wrong = false;
                    break;
                }
            }
            if (is_wrong) wrong += c;
        }
        sort(wrong.begin(), wrong.end());
        return wrong;
    }
    
private:
    string word;
    string category;
    vector<char> display;
    set<char> guessed_letters;
    int wrong_guesses;
    GameState state;
};

class HangmanUI {
public:
    HangmanUI() {
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        curs_set(0);
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_CYAN, COLOR_BLACK);
    }
    
    ~HangmanUI() {
        endwin();
    }
    
    void draw(const Hangman& game) {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        
        clear();
        
        // 标题
        attron(COLOR_PAIR(4) | A_BOLD);
        mvprintw(1, (max_x - 20) / 2, "=== HANGMAN ===");
        attroff(COLOR_PAIR(4) | A_BOLD);
        
        // 类别
        attron(COLOR_PAIR(3));
        mvprintw(3, 2, "Category: %s", game.get_category().c_str());
        attroff(COLOR_PAIR(3));
        
        // 剩余次数
        int remaining = 6 - game.get_wrong_guesses();
        mvprintw(3, max_x - 25, "Remaining: %d/6", remaining);
        if (remaining <= 2) {
            attron(COLOR_PAIR(2));
            mvprintw(3, max_x - 8, "!");
            attroff(COLOR_PAIR(2));
        }
        
        // Hangman 图形
        int stage = game.get_wrong_guesses();
        mvprintw(5, 2, "%s", HANGMAN_STAGES[stage].c_str());
        
        // 显示单词
        string display_str = game.get_display_string();
        mvprintw(5, max_x / 2 + 5, "Word: %s", display_str.c_str());
        
        // 字母状态
        mvprintw(7, max_x / 2 + 5, "Guessed: ");
        int x_pos = max_x / 2 + 15;
        for (char c = 'A'; c <= 'Z'; ++c) {
            if (game.get_guessed_letters().count(c)) {
                bool is_wrong = true;
                for (char wc : game.get_word()) {
                    if (wc == c) {
                        is_wrong = false;
                        break;
                    }
                }
                if (is_wrong) {
                    attron(COLOR_PAIR(2));
                    mvaddch(7, x_pos, c);
                    attroff(COLOR_PAIR(2));
                } else {
                    attron(COLOR_PAIR(1));
                    mvaddch(7, x_pos, c);
                    attroff(COLOR_PAIR(1));
                }
            } else {
                mvaddch(7, x_pos, '_');
            }
            ++x_pos;
        }
        
        // 错误字母
        string wrong_letters = game.get_wrong_letters_string();
        if (!wrong_letters.empty()) {
            mvprintw(9, max_x / 2 + 5, "Wrong: ");
            attron(COLOR_PAIR(2));
            mvprintw(9, max_x / 2 + 12, "%s", wrong_letters.c_str());
            attroff(COLOR_PAIR(2));
        }
        
        // 游戏结束信息
        if (game.get_state() == WON) {
            attron(COLOR_PAIR(1) | A_BOLD);
            mvprintw(14, (max_x - 20) / 2, "YOU WIN! CONGRATULATIONS!");
            attroff(COLOR_PAIR(1) | A_BOLD);
            mvprintw(15, (max_x - 30) / 2, "Press SPACE to play again");
        } else if (game.get_state() == LOST) {
            attron(COLOR_PAIR(2) | A_BOLD);
            mvprintw(14, (max_x - 20) / 2, "GAME OVER! YOU LOST!");
            attroff(COLOR_PAIR(2) | A_BOLD);
            mvprintw(15, (max_x - 25) / 2, "The word was: %s", game.get_word().c_str());
            mvprintw(16, (max_x - 30) / 2, "Press SPACE to play again");
        }
        
        // 提示
        mvprintw(max_y - 2, 2, "Press Q to quit | SPACE to restart when game over");
        
        refresh();
    }
    
    char get_input() {
        return getch();
    }
};

int main() {
    Hangman game;
    HangmanUI ui;
    
    ui.draw(game);
    
    while (true) {
        int ch = ui.get_input();
        
        if (ch == 'q' || ch == 'Q') {
            break;
        }
        
        if ((game.get_state() == WON || game.get_state() == LOST) && 
            (ch == ' ' || ch == KEY_ENTER || ch == 10 || ch == 13)) {
            // 重新开始
            game.init_game();
            ui.draw(game);
            continue;
        }
        
        if (game.get_state() == PLAYING && isalpha(ch)) {
            game.guess(ch);
            ui.draw(game);
            
            if (game.get_state() != PLAYING) {
                // 游戏结束，等待操作
            }
        }
    }
    
    return 0;
}
