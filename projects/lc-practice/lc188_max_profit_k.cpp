/*
LC188 买卖股票的最佳时机IV
Hard - DP

给定最多K次交易，在n天内买卖股票，求最大利润。
n=[0..1000], k=[0..100], price=[0..10000]

思路：三维DP
dp[i][j][0/1] = 第i天最多j次交易后，手里是否有股票的最大利润
- i: 0..n-1 天
- j: 0..k 次交易
- 0: 无股票, 1: 有股票

状态转移：
dp[i][j][1] = max(dp[i-1][j][1], dp[i-1][j-1][0] - price[i])  // 持有或买入
dp[i][j][0] = max(dp[i-1][j][0], dp[i-1][j][1] + price[i])  // 不持有或卖出

空间优化：滚动数组
*/

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int maxProfit(int k, vector<int>& prices) {
    int n = prices.size();
    if (n == 0 || k == 0) return 0;

    // 简化：k >= n/2 时等价于无限次交易
    if (k >= n / 2) {
        int profit = 0;
        for (int i = 1; i < n; i++) {
            profit += max(0, prices[i] - prices[i-1]);
        }
        return profit;
    }

    // dp[j][0/1] = 最多j次交易后无/有股票的最大利润
    vector<vector<int>> dp(k+1, vector<int>(2, 0));
    // 初始化：第0天持有股票 = -prices[0]
    for (int j = 1; j <= k; j++) dp[j][1] = -prices[0];

    for (int i = 1; i < n; i++) {
        for (int j = 1; j <= k; j++) {
            dp[j][1] = max(dp[j][1], dp[j-1][0] - prices[i]); // 买入
            dp[j][0] = max(dp[j][0], dp[j][1] + prices[i]);   // 卖出
        }
    }
    return dp[k][0];
}

int main() {
    // 测试
    vector<pair<int, vector<int>>> tests = {
        {2, {2,4,1}},
        {2, {3,2,6,5,0,3}},
        {0, {}},
    };
    for (auto& t : tests) {
        int k = t.first;
        vector<int> prices = t.second;
        cout << "k=" << k << ", prices=[";
        for (int i = 0; i < prices.size(); i++) {
            cout << prices[i] << (i<prices.size()-1?",":"");
        }
        cout << "] => " << maxProfit(k, prices) << endl;
    }
    // k=2, [2,4,1]: 最佳买2卖4利润=2
    // k=2, [3,2,6,5,0,3]: 最佳两次交易(2→6)+(0→3)=9
    return 0;
}
