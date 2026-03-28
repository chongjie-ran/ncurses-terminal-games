// LC51 N-Queens
// 将 N 个皇后放在 N×N 棋盘上，使任意两个皇后不互相攻击
// 皇后可以攻击同行/列/对角线

#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Solution {
public:
    vector<vector<string>> solveNQueens(int n) {
        results.clear();
        board.assign(n, string(n, '.'));
        cols.assign(n, false);
        diag1.assign(2*n-1, false);
        diag2.assign(2*n-1, false);
        this->n = n;
        backtrack(0);
        return results;
    }

private:
    int n;
    vector<vector<string>> results;
    vector<string> board;
    vector<bool> cols, diag1, diag2;

    void backtrack(int row) {
        if (row == n) {
            results.push_back(board);
            return;
        }
        for (int col = 0; col < n; ++col) {
            int d1 = row - col + n - 1;  // 主对角线 (row-col 唯一)
            int d2 = row + col;           // 副对角线 (row+col 唯一)
            if (cols[col] || diag1[d1] || diag2[d2]) continue;

            board[row][col] = 'Q';
            cols[col] = diag1[d1] = diag2[d2] = true;
            backtrack(row + 1);
            board[row][col] = '.';
            cols[col] = diag1[d1] = diag2[d2] = false;
        }
    }
};

int main() {
    Solution s;

    // Test 1: n=4 → 2 解
    auto r4 = s.solveNQueens(4);
    cout << "n=4: " << r4.size() << " solutions" << endl;
    for (auto& board : r4) {
        for (auto& row : board) cout << row << endl;
        cout << "---" << endl;
    }

    // Test 2: n=1 → 1 解
    auto r1 = s.solveNQueens(1);
    cout << "n=1: " << r1.size() << " solutions" << endl;
    for (auto& row : r1[0]) cout << row << endl;

    // Test 3: n=2 → 0 解
    auto r2 = s.solveNQueens(2);
    cout << "n=2: " << r2.size() << " solutions" << endl;

    // Test 4: n=3 → 0 解
    auto r3 = s.solveNQueens(3);
    cout << "n=3: " << r3.size() << " solutions" << endl;

    // Test 5: n=8 → 92 解
    auto r8 = s.solveNQueens(8);
    cout << "n=8: " << r8.size() << " solutions (expected 92)" << endl;

    return 0;
}
