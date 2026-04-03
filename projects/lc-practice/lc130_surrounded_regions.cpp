// LC130 - Surrounded Regions (Medium)
// 思路: 先标记边界O及与之相邻的O(DFS/BFS),其余O都是被X包围的
// 剩余O改为X
#include <iostream>
#include <vector>
using namespace std;

void solve(vector<vector<char>>& board) {
    if (board.empty() || board[0].empty()) return;
    int n = (int)board.size(), m = (int)board[0].size();
    int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    function<void(int,int)> dfs = [&](int r, int c) {
        if (r<0||r>=n||c<0||c>=m||board[r][c]!='O') return;
        board[r][c] = '#';
        for (auto& d: dirs) dfs(r+d[0], c+d[1]);
    };
    for (int c=0;c<m;c++) {
        if (board[0][c]=='O') dfs(0,c);
        if (board[n-1][c]=='O') dfs(n-1,c);
    }
    for (int r=0;r<n;r++) {
        if (board[r][0]=='O') dfs(r,0);
        if (board[r][m-1]=='O') dfs(r,m-1);
    }
    for (int r=0;r<n;r++) {
        for (int c=0;c<m;c++) {
            if (board[r][c]=='#') board[r][c]='O';
            else if (board[r][c]=='O') board[r][c]='X';
        }
    }
}
