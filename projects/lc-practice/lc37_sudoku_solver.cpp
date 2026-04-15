/*
LC37 Sudoku Solver (2026-04-15)
回溯 + 状态压缩
*/
#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    void solveSudoku(vector<vector<char>>& board) {
        rows.assign(9, 0);
        cols.assign(9, 0);
        boxes.assign(9, 0);
        
        // 初始化
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (board[i][j] != '.') {
                    int num = board[i][j] - '0';
                    int box = (i/3)*3 + j/3;
                    rows[i] |= 1 << num;
                    cols[j] |= 1 << num;
                    boxes[box] |= 1 << num;
                }
            }
        }
        backtrack(board, 0);
    }
    
private:
    vector<int> rows, cols, boxes;
    
    bool backtrack(vector<vector<char>>& board, int pos) {
        if (pos == 81) return true;
        
        int i = pos / 9, j = pos % 9;
        if (board[i][j] != '.') return backtrack(board, pos + 1);
        
        int box = (i/3)*3 + j/3;
        for (int num = 1; num <= 9; num++) {
            int bit = 1 << num;
            if (rows[i] & bit || cols[j] & bit || boxes[box] & bit) continue;
            
            board[i][j] = '0' + num;
            rows[i] |= bit; cols[j] |= bit; boxes[box] |= bit;
            
            if (backtrack(board, pos + 1)) return true;
            
            board[i][j] = '.';
            rows[i] &= ~bit; cols[j] &= ~bit; boxes[box] &= ~bit;
        }
        return false;
    }
};

void printBoard(vector<vector<char>>& board) {
    for (auto& row : board) {
        for (char c : row) cout << c << ' ';
        cout << endl;
    }
}

int main() {
    Solution sol;
    
    // 测试用例：经典数独
    vector<vector<char>> board = {
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
    
    cout << "=== LC37 Sudoku Solver ===" << endl;
    cout << "Input:" << endl;
    printBoard(board);
    
    sol.solveSudoku(board);
    
    cout << "\nSolution:" << endl;
    printBoard(board);
    
    return 0;
}
