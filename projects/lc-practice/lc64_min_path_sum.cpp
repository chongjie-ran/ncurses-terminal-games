#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// LC64 Minimum Path Sum - 2D DP
// Time: O(m*n), Space: O(1) in-place
int minPathSum(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (i == 0 && j == 0) continue;
            int up = (i > 0) ? grid[i-1][j] : INT_MAX;
            int left = (j > 0) ? grid[i][j-1] : INT_MAX;
            grid[i][j] += min(up, left);
        }
    }
    return grid[m-1][n-1];
}

// LC322 Coin Change - Unbounded Knapsack DP
// Time: O(amount * n), Space: O(amount)
int coinChange(vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;
    for (int coin : coins) {
        for (int j = coin; j <= amount; j++) {
            if (dp[j - coin] != INT_MAX) {
                dp[j] = min(dp[j], dp[j - coin] + 1);
            }
        }
    }
    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

// LC518 Coin Change 2 - Count combinations (order matters: coins in outer loop!)
// Time: O(amount * n), Space: O(amount)
int change(int amount, vector<int>& coins) {
    vector<int> dp(amount + 1, 0);
    dp[0] = 1;
    for (int coin : coins) {
        for (int j = coin; j <= amount; j++) {
            dp[j] += dp[j - coin];
        }
    }
    return dp[amount];
}

int main() {
    // Test LC64
    vector<vector<int>> g1 = {{1,3,1},{1,5,1},{4,2,1}};
    cout << "LC64: " << minPathSum(g1) << " (expect 7)" << endl;
    vector<vector<int>> g2 = {{1,2,3},{4,5,6}};
    cout << "LC64: " << minPathSum(g2) << " (expect 12)" << endl;
    
    // Test LC322
    vector<int> c1 = {1,2,5}; int a1 = 11;
    cout << "LC322: " << coinChange(c1, a1) << " (expect 3)" << endl;
    vector<int> c2 = {2}; int a2 = 3;
    cout << "LC322: " << coinChange(c2, a2) << " (expect -1)" << endl;
    vector<int> c3 = {1}; int a3 = 0;
    cout << "LC322: " << coinChange(c3, a3) << " (expect 0)" << endl;
    
    // Test LC518
    vector<int> c4 = {1,2,5}; int a4 = 5;
    cout << "LC518: " << change(a4, c4) << " (expect 4)" << endl;
    vector<int> c5 = {2}; int a5 = 3;
    cout << "LC518: " << change(a5, c5) << " (expect 0)" << endl;
    
    return 0;
}
