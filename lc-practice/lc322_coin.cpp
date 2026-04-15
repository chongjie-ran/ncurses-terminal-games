// LC322 硬币找零 - 完全背包
// 难度: Medium
// 核心: dp[i] = min(dp[i], dp[i-coin]+1)
// 特点: 求最小数量，不是最大价值
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

// 方法1: DP数组 (一维,正序遍历)
int coinChange(vector<int>& coins, int amount) {
    vector<int> dp(amount+1, INT_MAX);
    dp[0] = 0;
    for (int coin : coins) {
        for (int i = coin; i <= amount; i++) {
            if (dp[i-coin] != INT_MAX)
                dp[i] = min(dp[i], dp[i-coin] + 1);
        }
    }
    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

// 方法2: BFS (最少硬币数)
// 层级遍历，第一次达到target就是最少
int coinChangeBFS(vector<int>& coins, int amount) {
    if (amount == 0) return 0;
    vector<int> visited(amount+1, 0);
    queue<int> q; q.push(0); visited[0] = 1;
    int steps = 0;
    while (!q.empty()) {
        int sz = q.size();
        steps++;
        while (sz--) {
            int cur = q.front(); q.pop();
            for (int c : coins) {
                int nxt = cur + c;
                if (nxt == amount) return steps;
                if (nxt < amount && !visited[nxt]) {
                    visited[nxt] = 1;
                    q.push(nxt);
                }
            }
        }
    }
    return -1;
}

int main() {
    vector<pair<vector<int>, int>> tests = {
        {{1,2,5}, 11},    // 3 (5+5+1)
        {{2}, 3},         // -1
        {{1}, 0},         // 0
        {{1,5,11}, 15},   // 3 (5+5+5 or 11+1+1+1+1)
    };
    for (auto& [coins, amt] : tests) {
        cout << coinChange(coins, amt) << " ";
    }
    cout << endl;
    return 0;
}
