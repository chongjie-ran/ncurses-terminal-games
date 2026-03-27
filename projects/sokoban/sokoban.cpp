// Sokoban - 推箱子游戏
// 经典仓库番，BFS/DFS 状态空间搜索
// 编译: g++ -std=c++17 -o sokoban sokoban.cpp -lncurses

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <ncurses.h>

using namespace std;

// 游戏元素定义
enum Cell {
    WALL = '#',
    FLOOR = ' ',
    BOX = 'O',      // 箱子
    TARGET = '.',   // 目标位置
    BOX_ON_TARGET = '*',  // 箱子在目标上
    PLAYER = '@',   // 玩家
    PLAYER_ON_TARGET = '+' // 玩家在目标上
};

// 方向: 上右下下
const int dx[4] = {-1, 0, 1, 0};
const int dy[4] = {0, 1, 0, -1};
const char dir_names[4] = {'U', 'R', 'D', 'L'};

// 关卡示例
const vector<string> LEVEL_1 = {
    "########",
    "#      #",
    "# O $ .#",
    "#   $  #",
    "# .@   #",
    "########"
};

const vector<string> LEVEL_2 = {
    "  ##### ",
    "###   # ",
    "# $ # ##",
    "# #  . #",
    "#    # #",
    "## #   #",
    " ##$.  #",
    "  ##### "
};

// 游戏状态
struct State {
    vector<string> board;  // 当前板状态
    int px, py;           // 玩家位置
    int pushes;            // 推动次数
    vector<pair<int,int>> moves; // 移动历史
    
    // 状态序列化（用于去重和比较）
    string encode() const {
        string s;
        for (const auto& row : board)
            s += row;
        s += to_string(px) + "," + to_string(py);
        return s;
    }
};

// 检查是否完成（所有箱子都在目标上）
bool is_complete(const vector<string>& board) {
    for (const auto& row : board) {
        for (char c : row) {
            if (c == BOX) return false;  // 还有箱子不在目标上
        }
    }
    return true;
}

// 获取箱子位置列表（用于状态编码）
string get_box_signature(const vector<string>& board) {
    string sig;
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            if (board[i][j] == BOX || board[i][j] == BOX_ON_TARGET) {
                sig += char(i) + string(",") + char(j) + ";";
            }
        }
    }
    return sig;
}

// BFS 求解推箱子
struct SolverResult {
    bool solved;
    int moves;
    int pushes;
    vector<pair<int,int>> path;
    int expanded;
};

SolverResult bfs_solve(const vector<string>& start_board, int sx, int sy) {
    queue<State> q;
    unordered_set<string> visited;
    
    State init;
    init.board = start_board;
    init.px = sx;
    init.py = sy;
    init.pushes = 0;
    
    q.push(init);
    visited.insert(init.encode());
    
    SolverResult result = {false, 0, 0, {}, 0};
    
    while (!q.empty()) {
        State cur = q.front();
        q.pop();
        result.expanded++;
        
        if (is_complete(cur.board)) {
            result.solved = true;
            result.moves = cur.moves.size();
            result.pushes = cur.pushes;
            result.path = cur.moves;
            return result;
        }
        
        // 限制搜索规模
        if (result.expanded > 500000) {
            return result;  // 超时
        }
        
        // 四个方向尝试
        for (int dir = 0; dir < 4; dir++) {
            int nx = cur.px + dx[dir];
            int ny = cur.py + dy[dir];
            
            // 检查边界
            if (nx < 0 || nx >= cur.board.size() || ny < 0 || ny >= cur.board[nx].size())
                continue;
            
            char c = cur.board[nx][ny];
            
            // 墙壁
            if (c == WALL) continue;
            
            // 空地或目标：直接移动
            if (c == FLOOR || c == TARGET) {
                State nxt = cur;
                nxt.board[nx][ny] = (c == TARGET) ? PLAYER_ON_TARGET : PLAYER;
                nxt.board[cur.px][cur.py] = (cur.board[cur.px][cur.py] == PLAYER_ON_TARGET) ? TARGET : FLOOR;
                nxt.px = nx;
                nxt.py = ny;
                nxt.moves.push_back({dir, 0}); // 0=移动, 1=推箱子
                
                string key = nxt.encode();
                if (!visited.count(key)) {
                    visited.insert(key);
                    q.push(nxt);
                }
            }
            // 箱子：检查箱子后面是否可以推
            else if (c == BOX || c == BOX_ON_TARGET) {
                int bx = nx + dx[dir];
                int by = ny + dy[dir];
                
                if (bx < 0 || bx >= cur.board.size() || by < 0 || by >= cur.board[bx].size())
                    continue;
                
                char bc = cur.board[bx][by];
                if (bc == WALL || bc == BOX || bc == BOX_ON_TARGET)
                    continue;  // 不能推到墙壁或另一个箱子上
                
                State nxt = cur;
                // 移动玩家
                nxt.board[nx][ny] = (c == BOX_ON_TARGET) ? PLAYER_ON_TARGET : PLAYER;
                nxt.board[cur.px][cur.py] = (cur.board[cur.px][cur.py] == PLAYER_ON_TARGET) ? TARGET : FLOOR;
                // 移动箱子
                nxt.board[bx][by] = (bc == TARGET) ? BOX_ON_TARGET : BOX;
                nxt.px = nx;
                nxt.py = ny;
                nxt.pushes++;
                nxt.moves.push_back({dir, 1}); // 推箱子
                
                string key = nxt.encode();
                if (!visited.count(key)) {
                    visited.insert(key);
                    q.push(nxt);
                }
            }
        }
    }
    
    return result;
}

// ncurses 渲染
void render(const vector<string>& board, int pushes, int moves, bool solved) {
    erase();
    
    int h = board.size();
    int start_y = (LINES - h) / 2;
    int start_x = (COLS - board[0].size()) / 2;
    
    attron(COLOR_PAIR(1));
    mvprintw(0, 0, "=== Sokoban 推箱子 ===");
    attroff(COLOR_PAIR(1));
    
    mvprintw(1, 0, "推动: %d  移动: %d", pushes, moves);
    
    if (solved) {
        attron(COLOR_PAIR(3));
        mvprintw(2, 0, "🎉 通关！按 Q 退出");
        attroff(COLOR_PAIR(3));
    } else {
        mvprintw(2, 0, "方向键移动 | R 重置 | S 解算 | Q 退出");
    }
    
    // 绘制游戏板
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < board[i].size(); j++) {
            char c = board[i][j];
            int color = 1;
            
            if (c == WALL) color = 1;        // 墙壁
            else if (c == TARGET) color = 4; // 目标（蓝色）
            else if (c == BOX) color = 2;    // 箱子（黄色）
            else if (c == BOX_ON_TARGET) color = 3; // 箱子在目标上（绿色）
            else if (c == PLAYER_ON_TARGET) color = 3; // 玩家在目标上
            else if (c == PLAYER) color = 2; // 玩家
            
            attron(COLOR_PAIR(color));
            mvaddch(start_y + i, start_x + j, c);
            attroff(COLOR_PAIR(color));
        }
    }
    
    refresh();
}

int main() {
    // 初始化 ncurses
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    
    // 颜色初始化
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   // 墙壁/默认
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);   // 箱子/玩家
    init_pair(3, COLOR_GREEN, COLOR_BLACK);   // 完成
    init_pair(4, COLOR_CYAN, COLOR_BLACK);    // 目标
    
    // 选择关卡
    vector<string> board = LEVEL_1;
    
    // 找到玩家初始位置
    int px = -1, py = -1;
    for (int i = 0; i < board.size() && px < 0; i++)
        for (int j = 0; j < board[i].size(); j++)
            if (board[i][j] == PLAYER || board[i][j] == PLAYER_ON_TARGET) {
                px = i; py = j;
                board[i][j] = (board[i][j] == PLAYER_ON_TARGET) ? PLAYER_ON_TARGET : PLAYER;
            }
    
    int pushes = 0, moves = 0;
    bool solved = false;
    SolverResult solver_result = {false, 0, 0, {}, 0};
    
    render(board, pushes, moves, solved);
    
    int ch;
    while ((ch = getch()) != 'q' && ch != 'Q') {
        if (ch == 'r' || ch == 'R') {
            // 重置
            for (int i = 0; i < board.size(); i++)
                board[i] = LEVEL_1[i];
            for (int i = 0; i < board.size() && px < 0; i++)
                for (int j = 0; j < board[i].size(); j++)
                    if (board[i][j] == PLAYER || board[i][j] == PLAYER_ON_TARGET) {
                        px = i; py = j;
                    }
            pushes = moves = 0;
            solved = false;
        }
        else if (ch == 's' || ch == 'S') {
            // BFS 自动求解
            solver_result = bfs_solve(board, px, py);
            if (solver_result.solved) {
                mvprintw(LINES-2, 0, "BFS 求解成功: %d 步, %d 推动, 扩展 %d 节点",
                    solver_result.moves, solver_result.pushes, solver_result.expanded);
                refresh();
                getch();
            } else {
                mvprintw(LINES-2, 0, "BFS 未找到解（扩展 %d 节点）", solver_result.expanded);
                refresh();
                getch();
            }
            continue;
        }
        else if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT) {
            if (solved) continue;
            
            int dir = -1;
            if (ch == KEY_UP) dir = 0;
            else if (ch == KEY_RIGHT) dir = 1;
            else if (ch == KEY_DOWN) dir = 2;
            else if (ch == KEY_LEFT) dir = 3;
            
            int nx = px + dx[dir];
            int ny = py + dy[dir];
            
            if (nx < 0 || nx >= board.size() || ny < 0 || ny >= board[nx].size())
                continue;
            
            char c = board[nx][ny];
            
            if (c == WALL) continue;
            
            if (c == FLOOR || c == TARGET) {
                // 直接移动
                board[px][py] = (board[px][py] == PLAYER_ON_TARGET) ? TARGET : FLOOR;
                board[nx][ny] = (c == TARGET) ? PLAYER_ON_TARGET : PLAYER;
                px = nx; py = ny;
                moves++;
            }
            else if (c == BOX || c == BOX_ON_TARGET) {
                // 推箱子
                int bx = nx + dx[dir];
                int by = ny + dy[dir];
                
                if (bx < 0 || bx >= board.size() || by < 0 || by >= board[bx].size())
                    continue;
                
                char bc = board[bx][by];
                if (bc == WALL || bc == BOX || bc == BOX_ON_TARGET)
                    continue;
                
                // 移动玩家
                board[px][py] = (board[px][py] == PLAYER_ON_TARGET) ? TARGET : FLOOR;
                board[nx][ny] = (c == BOX_ON_TARGET) ? PLAYER_ON_TARGET : PLAYER;
                // 移动箱子
                board[bx][by] = (bc == TARGET) ? BOX_ON_TARGET : BOX;
                px = nx; py = ny;
                pushes++;
                moves++;
            }
            
            if (is_complete(board)) solved = true;
        }
        
        render(board, pushes, moves, solved);
    }
    
    endwin();
    return 0;
}
