// LC64 Minimum Path Sum - 经典DP
// 问题：网格中从左上到右下，路径和最小，只能向右或向下
// 状态：dp[i][j] = 到达(i,j)的最小路径和
// 转移：dp[i][j] = grid[i][j] + min(dp[i-1][j], dp[i][j-1])
// 初始化：dp[0][0]=grid[0][0], 第一行/列累加
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// O(m*n) space
int minPathSum(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    vector<vector<int>> dp(m, vector<int>(n, 0));
    dp[0][0] = grid[0][0];
    for (int i = 1; i < m; i++) dp[i][0] = dp[i-1][0] + grid[i][0];
    for (int j = 1; j < n; j++) dp[0][j] = dp[0][j-1] + grid[0][j];
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[i][j] = grid[i][j] + min(dp[i-1][j], dp[i][j-1]);
        }
    }
    return dp[m-1][n-1];
}

// O(n) space optimization
int minPathSumOptimized(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    vector<int> dp(n, INT_MAX);
    dp[0] = 0;
    for (int i = 0; i < m; i++) {
        dp[0] += grid[i][0];
        for (int j = 1; j < n; j++) {
            dp[j] = grid[i][j] + min(dp[j], dp[j-1]);
        }
    }
    return dp[n-1];
}

int main() {
    vector<vector<int>> grid = {
        {1,3,1},
        {1,5,1},
        {4,2,1}
    };
    cout << "LC64 Minimum Path Sum" << endl;
    cout << "Grid: [[1,3,1],[1,5,1],[4,2,1]]" << endl;
    cout << "minPathSum: " << minPathSum(grid) << " (expected 7)" << endl;
    cout << "minPathSumOptimized: " << minPathSumOptimized(grid) << " (expected 7)" << endl;
    
    vector<vector<int>> grid2 = {{1,2,3},{4,5,6}};
    cout << "Grid: [[1,2,3],[4,5,6]]" << endl;
    cout << "minPathSum: " << minPathSum(grid2) << " (expected 12)" << endl;
    return 0;
}
