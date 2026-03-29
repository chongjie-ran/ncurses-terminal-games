// LC37 Sudoku Solver — 解数独
// 回溯 + 剪枝：按行遍历空格，尝试1-9，检查行/列/3x3宫是否合法
// 时间: O(9^空格数) 实际极快因为剪枝严格
// 空间: O(1) 原地修改

#include <iostream>
#include <vector>
#include <unordered_set>
using namespace std;

class Solution {
public:
    void solveSudoku(vector<vector<char>>& board) {
        solve(board);
    }
    
    bool solve(vector<vector<char>>& board) {
        for (int i = 0; i < 9; ++i)
            for (int j = 0; j < 9; ++j)
                if (board[i][j] == '.') {
                    for (char c = '1'; c <= '9'; ++c) {
                        if (isValid(board, i, j, c)) {
                            board[i][j] = c;
                            if (solve(board)) return true;
                            board[i][j] = '.';
                        }
                    }
                    return false;  // 没有有效数字
                }
        return true;  // 所有格子填满
    }
    
    bool isValid(const vector<vector<char>>& board, int row, int col, char num) {
        // 检查行
        for (int j = 0; j < 9; ++j)
            if (board[row][j] == num) return false;
        // 检查列
        for (int i = 0; i < 9; ++i)
            if (board[i][col] == num) return false;
        // 检查 3x3 宫
        int br = (row / 3) * 3, bc = (col / 3) * 3;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (board[br + i][bc + j] == num) return false;
        return true;
    }
};

void printBoard(const vector<vector<char>>& board) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) cout << board[i][j] << ' ';
        cout << '\n';
    }
}

int main() {
    // LeetCode Test 1: 标准数独
    vector<vector<char>> board1 = {
        {'5','3','.', '.','7','.', '.','.','.'},
        {'6','.','.', '1','9','5', '.','.','.'},
        {'.','9','8', '.','.','.', '.','6','.'},
        {'8','.','.', '.','6','.', '.','.','3'},
        {'4','.','.', '8','.','3', '.','.','1'},
        {'7','.','.', '.','2','.', '.','.','6'},
        {'.','6','.', '.','.','.', '2','8','.'},
        {'.','.','.', '4','1','9', '.','.','5'},
        {'.','.','.', '.','8','.', '.','7','9'}
    };
    Solution sol;
    sol.solveSudoku(board1);
    cout << "=== Test 1 ===" << endl;
    printBoard(board1);
    
    // 验证：检查解是否完整（无'.')
    bool complete = true;
    for (auto& row : board1)
        for (char c : row)
            if (c == '.') complete = false;
    cout << "完整: " << (complete ? "是" : "否") << endl;
    
    // LeetCode Test 2: 全空对角线
    vector<vector<char>> board2 = {
        {'.','.','.','.','.','.','.','.','.'},
        {'.','.','.','.','.','.','.','.','.'},
        {'.','.','.','.','.','.','.','.','.'},
        {'.','.','.','.','.','.','.','.','.'},
        {'.','.','.','.','.','.','.','.','.'},
        {'.','.','.','.','.','.','.','.','.'},
        {'.','.','.','.','.','.','.','.','.'},
        {'.','.','.','.','.','.','.','.','.'},
        {'.','.','.','.','.','.','.','.','.'}
    };
    sol.solveSudoku(board2);
    cout << "\n=== Test 2 (全空) ===" << endl;
    printBoard(board2);
    
    complete = true;
    for (auto& row : board2)
        for (char c : row)
            if (c == '.') complete = false;
    cout << "完整: " << (complete ? "是" : "否") << endl;
    
    return 0;
}
