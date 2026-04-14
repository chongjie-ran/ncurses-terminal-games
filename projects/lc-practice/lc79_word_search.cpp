// LC79 Word Search
// 回溯算法 - 2D board中搜索单词路径
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Solution {
public:
    bool exist(vector<vector<char>>& board, string word) {
        if (board.empty() || word.empty()) return false;
        m = board.size(), n = board[0].size();
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (dfs(board, word, 0, i, j)) return true;
            }
        }
        return false;
    }
private:
    int m, n;
    const int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    
    bool dfs(vector<vector<char>>& board, const string& word, int idx, int x, int y) {
        if (idx == word.size()) return true;
        if (x < 0 || x >= m || y < 0 || y >= n || board[x][y] != word[idx]) return false;
        
        char c = board[x][y];
        board[x][y] = '#'; // 标记访问
        for (auto& d : dirs) {
            if (dfs(board, word, idx+1, x+d[0], y+d[1])) {
                board[x][y] = c; // 撤销
                return true;
            }
        }
        board[x][y] = c; // 撤销
        return false;
    }
};

int main() {
    Solution sol;
    vector<vector<char>> board = {
        {'A','B','C','E'},
        {'S','F','C','S'},
        {'A','D','E','E'}
    };
    
    cout << (sol.exist(board, "ABCCED") ? "true" : "false") << endl;
    cout << (sol.exist(board, "SEE") ? "true" : "false") << endl;
    cout << (sol.exist(board, "ABCB") ? "true" : "false") << endl;
    return 0;
}
