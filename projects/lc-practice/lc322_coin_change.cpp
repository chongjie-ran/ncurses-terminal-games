// LC322 Coin Change - Medium
// 问题: 给定硬币面额数组 coins 和总额 amount，求最少硬币数凑成 amount
// 如果无法凑成，返回 -1
// 思路: DP - 完全背包问题
// dp[i] = 凑成金额 i 所需的最少硬币数
// dp[i] = min(dp[i], dp[i - coin] + 1) for all coins

#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>
using namespace std;

// ==================== Solution 1: DP 1D ====================
int coinChange(vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;
    
    for (int i = 1; i <= amount; i++) {
        for (int coin : coins) {
            if (i - coin >= 0 && dp[i - coin] != INT_MAX) {
                dp[i] = min(dp[i], dp[i - coin] + 1);
            }
        }
    }
    
    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

// ==================== Solution 2: BFS (最快) ====================
// 从 amount 开始递减 BFS，找到的第一层就是最少硬币数
int coinChangeBFS(vector<int>& coins, int amount) {
    if (amount == 0) return 0;
    
    vector<int> visited(amount + 1, 0);
    queue<int> q;
    q.push(amount);
    visited[amount] = 1;
    int steps = 0;
    
    while (!q.empty()) {
        int sz = q.size();
        steps++;
        for (int i = 0; i < sz; i++) {
            int cur = q.front(); q.pop();
            for (int coin : coins) {
                int next = cur - coin;
                if (next == 0) return steps;
                if (next > 0 && !visited[next]) {
                    visited[next] = 1;
                    q.push(next);
                }
            }
        }
    }
    return -1;
}

// ==================== Tests ====================
int main() {
    // Test 1
    vector<int> coins1 = {1, 2, 5};
    int result1 = coinChange(coins1, 11);
    cout << "Test1: " << result1 << " (expected 3: 5+5+1)" << endl;
    
    // Test 2
    vector<int> coins2 = {2};
    int result2 = coinChange(coins2, 3);
    cout << "Test2: " << result2 << " (expected -1)" << endl;
    
    // Test 3
    vector<int> coins3 = {1};
    int result3 = coinChange(coins3, 0);
    cout << "Test3: " << result3 << " (expected 0)" << endl;
    
    // Test 4 - BFS
    int result4 = coinChangeBFS(coins1, 11);
    cout << "Test4 BFS: " << result4 << " (expected 3)" << endl;
    
    // Test 5
    vector<int> coins5 = {1, 5, 11, 13, 30};
    int result5 = coinChange(coins5, 72);
    cout << "Test5: " << result5 << endl;
    
    return 0;
}
