// LC79 Word Search
// 给定 m x n 棋盘和单词，判断单词是否存在于棋盘中
// 移动: 上下左右相邻格子
// 每个格子只能使用一次

#include <bitset>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Solution {
public:
    bool exist(vector<vector<char>>& board, string word) {
        if (word.empty()) return true;
        m = board.size();
        n = board[0].size();
        if (m == 0 || n == 0) return false;
        
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < n; ++j)
                if (dfs(board, word, 0, i, j))
                    return true;
        return false;
    }
    
private:
    int m, n;
    const int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    
    bool dfs(vector<vector<char>>& board, const string& word, int idx, int x, int y) {
        if (board[x][y] != word[idx]) return false;
        if (idx == word.size() - 1) return true;
        
        char original = board[x][y];
        board[x][y] = '#'; // 标记已访问
        
        for (auto& d : dirs) {
            int nx = x + d[0], ny = y + d[1];
            if (nx >= 0 && nx < m && ny >= 0 && ny < n) {
                if (dfs(board, word, idx + 1, nx, ny)) {
                    board[x][y] = original;
                    return true;
                }
            }
        }
        
        board[x][y] = original; // 恢复
        return false;
    }
};

// 测试
int main() {
    Solution s;
    
    // Test 1: LeetCode 示例
    vector<vector<char>> board1 = {
        {'A','B','C','E'},
        {'S','F','C','S'},
        {'A','D','E','E'}
    };
    cout << (s.exist(board1, "ABCCED") ? "true" : "false") << endl;  // true
    cout << (s.exist(board1, "SEE") ? "true" : "false") << endl;     // true
    cout << (s.exist(board1, "ABCB") ? "true" : "false") << endl;    // false
    
    // Test 2: 单格
    vector<vector<char>> board2 = {{'a'}};
    cout << (s.exist(board2, "a") ? "true" : "false") << endl;       // true
    cout << (s.exist(board2, "b") ? "true" : "false") << endl;       // false
    
    // Test 3: 蛇形路径
    vector<vector<char>> board3 = {
        {'A','B'},
        {'D','C'}
    };
    cout << (s.exist(board3, "ABDC") ? "true" : "false") << endl;   // true (A->B->C->D 不能走回头)
    cout << (s.exist(board3, "ABCD") ? "true" : "false") << endl;   // false
    
    return 0;
}
