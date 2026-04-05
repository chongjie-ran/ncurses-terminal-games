// LC63 Unique Paths II (Medium)
// DP with obstacles: dp[i][j] = 0 if obstacle else dp[i-1][j] + dp[i][j-1]
#include <iostream>
#include <vector>
using namespace std;

int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
    int m = obstacleGrid.size();
    int n = obstacleGrid[0].size();
    vector<int> dp(n, 0);
    dp[0] = (obstacleGrid[0][0] == 0) ? 1 : 0;
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (obstacleGrid[i][j] == 1) {
                dp[j] = 0;
            } else if (j > 0) {
                dp[j] += dp[j-1];
            }
        }
    }
    return dp[n-1];
}

int main() {
    vector<vector<int>> g1 = {{0,0,0},{0,1,0},{0,0,0}};
    cout << "3x3 grid with middle obstacle: " << uniquePathsWithObstacles(g1) << " (expect 2)" << endl;
    
    vector<vector<int>> g2 = {{0,1},{0,0}};
    cout << "2x2 no obstacle: " << uniquePathsWithObstacles(g2) << " (expect 2)" << endl;
    
    vector<vector<int>> g3 = {{1,0}};
    cout << "1x2 start blocked: " << uniquePathsWithObstacles(g3) << " (expect 0)" << endl;
    
    vector<vector<int>> g4 = {{0,0},{1,1},{0,0}};
    cout << "3x2 with obstacle: " << uniquePathsWithObstacles(g4) << " (expect 0)" << endl;
    return 0;
}
