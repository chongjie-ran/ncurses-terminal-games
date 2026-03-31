// LC329 - Longest Increasing Path in a Matrix
// 分类: DFS + Memoization (Matrix DP)
// 难度: Hard
// 思路: 从每个格子出发DFS，用memo记录以(i,j)为起点的最长递增路径长度
// 四个方向扩展，dp[i][j] = 1 + max(dp[ni][nj]) for all valid neighbors

#include <iostream>
#include <vector>
#include <functional>
using namespace std;

class Solution {
public:
    int longestIncreasingPath(vector<vector<int>>& matrix) {
        if(matrix.empty() || matrix[0].empty()) return 0;
        int m = matrix.size(), n = matrix[0].size();
        vector<vector<int>> memo(m, vector<int>(n, -1));
        vector<vector<int>> dirs = {{1,0},{-1,0},{0,1},{0,-1}};
        
        function<int(int,int)> dfs = [&](int i, int j) -> int {
            if(memo[i][j] != -1) return memo[i][j];
            int best = 1;
            for(auto& d: dirs) {
                int ni = i + d[0], nj = j + d[1];
                if(ni>=0 && ni<m && nj>=0 && nj<n && matrix[ni][nj] > matrix[i][j]) {
                    best = max(best, 1 + dfs(ni, nj));
                }
            }
            memo[i][j] = best;
            return best;
        };
        
        int ans = 0;
        for(int i=0;i<m;i++) for(int j=0;j<n;j++)
            ans = max(ans, dfs(i,j));
        return ans;
    }
};
