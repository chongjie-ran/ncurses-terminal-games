// LC63 不同路径II - 带障碍物
// 障碍物处dp=0，其他正常转移
#include <iostream>
#include <vector>
using namespace std;

int uniquePathsWithObstacles(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    vector<vector<int>> dp(m, vector<int>(n, 0));
    
    // 起点或终点有障碍
    if (grid[0][0] == 1 || grid[m-1][n-1] == 1) return 0;
    dp[0][0] = 1;
    
    // 第一行
    for (int j = 1; j < n; j++) {
        dp[0][j] = (grid[0][j] == 0) ? dp[0][j-1] : 0;
    }
    // 第一列
    for (int i = 1; i < m; i++) {
        dp[i][0] = (grid[i][0] == 0) ? dp[i-1][0] : 0;
    }
    
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            if (grid[i][j] == 0) {
                dp[i][j] = dp[i-1][j] + dp[i][j-1];
            }
        }
    }
    return dp[m-1][n-1];
}

int main() {
    vector<vector<int>> grid1 = {
        {0,0,0},
        {0,1,0},
        {0,0,0}
    };
    cout << "LC63 Unique Paths II" << endl;
    cout << "grid1: " << uniquePathsWithObstacles(grid1) << " (expected 2)" << endl;
    
    vector<vector<int>> grid2 = {{0,1},{0,0}};
    cout << "grid2: " << uniquePathsWithObstacles(grid2) << " (expected 1)" << endl;
    
    vector<vector<int>> grid3 = {{1,0}};
    cout << "grid3: " << uniquePathsWithObstacles(grid3) << " (expected 0)" << endl;
    return 0;
}
