// LC79 - Word Search
// 分类: DFS + Backtracking
// 难度: Medium
// 思路: 在2D网格中找单词，允许上下左右移动，不能重复使用同一格子
// 对每个起点DFS，用原位标记(改为'#')防止重复访问，回溯时恢复

#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Solution {
public:
    int m, n;
    bool exist(vector<vector<char>>& board, string word) {
        m = board.size(), n = board[0].size();
        for(int i=0;i<m;i++)
            for(int j=0;j<n;j++)
                if(dfs(board, word, i, j, 0))
                    return true;
        return false;
    }
    bool dfs(vector<vector<char>>& board, const string& word, int i, int j, int idx) {
        if(idx == word.size()) return true;
        if(i<0||i>=m||j<0||j>=n||board[i][j]!=word[idx]) return false;
        char c = board[i][j];
        board[i][j] = '#';
        bool res = dfs(board, word, i+1, j, idx+1)
                 || dfs(board, word, i-1, j, idx+1)
                 || dfs(board, word, i, j+1, idx+1)
                 || dfs(board, word, i, j-1, idx+1);
        board[i][j] = c;
        return res;
    }
};
