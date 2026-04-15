// LC79 单词搜索 - 回溯
#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    bool exist(vector<vector<char>>& board, string word) {
        if (board.empty() || word.empty()) return false;
        int m = board.size(), n = board[0].size();
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (backtrack(board, word, 0, i, j)) return true;
            }
        }
        return false;
    }
private:
    bool backtrack(vector<vector<char>>& board, const string& word, int idx, int i, int j) {
        if (idx == word.size()) return true;
        if (i < 0 || i >= board.size() || j < 0 || j >= board[0].size()) return false;
        if (board[i][j] != word[idx]) return false;
        
        char temp = board[i][j];
        board[i][j] = '#';  // 标记已访问
        
        bool found = backtrack(board, word, idx+1, i+1, j) ||
                     backtrack(board, word, idx+1, i-1, j) ||
                     backtrack(board, word, idx+1, i, j+1) ||
                     backtrack(board, word, idx+1, i, j-1);
        
        board[i][j] = temp;  // 恢复
        return found;
    }
};

int main() {
    Solution sol;
    vector<vector<char>> board = {
        {'A','B','C','E'},
        {'S','F','C','S'},
        {'A','D','E','E'}
    };
    cout << sol.exist(board, "ABCCED") << endl;  // 1
    cout << sol.exist(board, "SEE") << endl;      // 1
    cout << sol.exist(board, "ABCB") << endl;     // 0
    return 0;
}
