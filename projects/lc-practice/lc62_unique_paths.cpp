// LC62 不同路径 - 经典DP
// 状态：dp[i][j] = 到达(i,j)的路径数
// 转移：dp[i][j] = dp[i-1][j] + dp[i][j-1]
// 初始化：dp[0][j]=1, dp[i][0]=1
#include <iostream>
#include <vector>
using namespace std;

int uniquePaths(int m, int n) {
    vector<vector<int>> dp(m, vector<int>(n, 1));
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[i][j] = dp[i-1][j] + dp[i][j-1];
        }
    }
    return dp[m-1][n-1];
}

// 空间优化：O(n)
int uniquePathsOptimized(int m, int n) {
    vector<int> dp(n, 1);
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[j] += dp[j-1];
        }
    }
    return dp[n-1];
}

int main() {
    cout << "LC62 Unique Paths" << endl;
    cout << "3x7: " << uniquePaths(3, 7) << " (expected 28)" << endl;
    cout << "3x7 optimized: " << uniquePathsOptimized(3, 7) << endl;
    cout << "7x3: " << uniquePaths(7, 3) << " (expected 28)" << endl;
    cout << "3x3: " << uniquePaths(3, 3) << " (expected 6)" << endl;
    return 0;
}
