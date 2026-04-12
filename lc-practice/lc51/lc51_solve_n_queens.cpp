// LC51. N-Queens
// 难度: Hard | 算法: Backtracking + 集合判重
// 核心: 回溯放皇后,三个集合判重(列/正斜线/反斜线)
// 关键洞察: 
//   - 正斜线 row-col 差相同: r-c ∈ [-(n-1), n-1], 统一加n-1映射到[0, 2n-2]
//   - 反斜线 row+col 和相同: r+c ∈ [0, 2n-2]
#include <bits/stdc++.h>
using namespace std;

void backtrack(int row, int n, vector<string>& board,
               unordered_set<int>& cols, unordered_set<int>& diag1,
               unordered_set<int>& diag2, vector<vector<string>>& res) {
    if (row == n) {
        res.push_back(board);
        return;
    }
    for (int col = 0; col < n; col++) {
        int d1 = row - col + n - 1;  // 正斜线
        int d2 = row + col;           // 反斜线
        if (cols.count(col) || diag1.count(d1) || diag2.count(d2)) continue;

        cols.insert(col); diag1.insert(d1); diag2.insert(d2);
        board[row][col] = 'Q';
        backtrack(row + 1, n, board, cols, diag1, diag2, res);
        board[row][col] = '.';
        cols.erase(col); diag1.erase(d1); diag2.erase(d2);
    }
}

vector<vector<string>> solveNQueens(int n) {
    vector<vector<string>> res;
    vector<string> board(n, string(n, '.'));
    unordered_set<int> cols, diag1, diag2;
    backtrack(0, n, board, cols, diag1, diag2, res);
    return res;
}

int main() {
    auto res = solveNQueens(4);
    cout << "Solutions for n=4: " << res.size() << endl;
    for (auto& board : res) {
        for (auto& row : board) cout << row << endl;
        cout << "---" << endl;
    }
    return 0;
}
