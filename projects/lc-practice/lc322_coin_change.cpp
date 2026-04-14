// LC322 零钱兑换 - 完全背包（每种硬币无限使用）
// 状态：dp[amount] = 凑成amount的最少硬币数
// 转移：dp[amount] = min(dp[amount], dp[amount-coin]+1)
// 初始化：dp[i]=INF, dp[0]=0
// 顺序：先金额后硬币（完全背包）
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

int coinChange(vector<int>& coins, int amount) {
    vector<int> dp(amount+1, INT_MAX);
    dp[0] = 0;
    for (int a = 1; a <= amount; a++) {
        for (int c : coins) {
            if (a >= c && dp[a-c] != INT_MAX) {
                dp[a] = min(dp[a], dp[a-c] + 1);
            }
        }
    }
    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

int main() {
    cout << "LC322 Coin Change" << endl;
    vector<int> c1 = {1,2,5};
    cout << "[1,2,5], 11: " << coinChange(c1, 11) << " (expected 3)" << endl;
    
    vector<int> c2 = {2};
    cout << "[2], 3: " << coinChange(c2, 3) << " (expected -1)" << endl;
    
    vector<int> c3 = {1};
    cout << "[1], 0: " << coinChange(c3, 0) << " (expected 0)" << endl;
    
    vector<int> c4 = {1};
    cout << "[1], 2: " << coinChange(c4, 2) << " (expected 2)" << endl;
    
    vector<int> c5 = {2,5,10,1};
    cout << "[2,5,10,1], 27: " << coinChange(c5, 27) << " (expected 4)" << endl;
    return 0;
}
