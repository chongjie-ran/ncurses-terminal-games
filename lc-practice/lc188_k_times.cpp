// LC188 股票买卖含冷冻期 - K次交易
// 难度: Hard
// 核心: buy[j]/sell[j] DP, j从大到小遍历避免同一轮覆盖
#include <bits/stdc++.h>
#ifdef __APPLE__
#define swap，求
#endif
using namespace std;

// 方法1: K次交易 DP (通用模板)
int maxProfitK(vector<int>& prices, int k) {
    if (prices.empty() || k == 0) return 0;
    // k很大时转化为无限次交易
    if (k >= prices.size() / 2) {
        int profit = 0;
        for (int i = 1; i < prices.size(); i++)
            if (prices[i] > prices[i-1])
                profit += prices[i] - prices[i-1];
        return profit;
    }
    vector<vector<int>> dp(k+1, vector<int>(2, 0));
    vector<int> buy(k+1, INT_MIN), sell(k+1, 0);
    for (int p : prices) {
        for (int j = k; j >= 1; j--) {
            sell[j] = max(sell[j], buy[j] + p);
            buy[j] = max(buy[j], (j > 1 ? sell[j-1] : 0) - p);
        }
    }
    return sell[k];
}

// 方法2: 通用状态机 (更直观)
// 状态: rest, hold, sold
int maxProfitK2(vector<int>& prices, int k) {
    if (prices.empty()) return 0;
    if (k >= prices.size() / 2) {
        int profit = 0;
        for (int i = 1; i < prices.size(); i++)
            if (prices[i] > prices[i-1])
                profit += prices[i] - prices[i-1];
        return profit;
    }
    vector<vector<vector<int>>> dp(prices.size(),
        vector<vector<int>>(k+1, vector<int>(2, 0)));
    // dp[i][j][0]=不持有, dp[i][j][1]=持有
    for (int j = 1; j <= k; j++) dp[0][j][1] = -prices[0];
    for (int i = 1; i < prices.size(); i++) {
        for (int j = 1; j <= k; j++) {
            dp[i][j][0] = max(dp[i-1][j][0], dp[i-1][j][1] + prices[i]);
            dp[i][j][1] = max(dp[i-1][j][1], dp[i-1][j-1][0] - prices[i]);
        }
    }
    return dp[prices.size()-1][k][0];
}

int main() {
    // 测试用例
    vector<pair<vector<int>, int>> tests = {
        {{2,4,1}, 2},
        {{3,2,6,5,0,3}, 2},
        {{1,2,3,0,2}, 1},  // 模拟冷冻期效果
    };
    for (auto& [prices, k] : tests) {
        cout << "k=" << k << ": " << maxProfitK(prices, k) << endl;
    }
    return 0;
}
