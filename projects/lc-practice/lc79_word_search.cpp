// LC79 Word Search - 回溯
// 核心：方向矩阵 + 标记已访问 + 撤销选择
#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Solution {
public:
    int m, n;
    vector<pair<int,int>> dirs = {{-1,0},{1,0},{0,-1},{0,1}};
    
    bool dfs(vector<vector<char>>& board, string& word, int idx, int x, int y, vector<vector<bool>>& vis) {
        if (idx == (int)word.size()) return true;
        if (x<0||x>=m||y<0||y>=n||vis[x][y]||board[x][y]!=word[idx]) return false;
        vis[x][y] = true;
        for (auto [dx,dy]: dirs) {
            if (dfs(board, word, idx+1, x+dx, y+dy, vis)) {
                vis[x][y] = false;
                return true;
            }
        }
        vis[x][y] = false;
        return false;
    }
    
    bool exist(vector<vector<char>>& board, string word) {
        if (word.empty()) return true;
        m=board.size(), n=board[0].size();
        vector<vector<bool>> vis(m, vector<bool>(n));
        for (int i=0;i<m;i++) {
            for (int j=0;j<n;j++) {
                if (dfs(board, word, 0, i, j, vis)) return true;
            }
        }
        return false;
    }
};

int main() {
    Solution s;
    vector<vector<char>> board = {{'A','B','C','E'},{'S','F','C','S'},{'A','D','E','E'}};
    cout << "ABCCED: " << s.exist(board, "ABCCED") << endl; // true
    cout << "SEE: " << s.exist(board, "SEE") << endl; // true
    cout << "ABCB: " << s.exist(board, "ABCB") << endl; // false
    cout << "ASF: " << s.exist(board, "ASF") << endl; // true
    cout << "ASFC: " << s.exist(board, "ASFC") << endl; // true
    return 0;
}
