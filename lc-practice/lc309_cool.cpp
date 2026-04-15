// LC309 股票买卖含冷冻期
// 核心: 三状态 DP
// states: hold(持有), sold(刚卖出), rest(休息/不持有)
// 转移: hold=max(hold, sold-p), sold=rest, rest=max(rest, sold)
#include <iostream>
#include <vector>
using namespace std;

int maxProfit(vector<int>& prices) {
    if (prices.empty()) return 0;
    int hold = -prices[0], sold = 0, rest = 0;
    for (int i = 1; i < prices.size(); i++) {
        int prev_hold = hold;
        hold = max(hold, rest - prices[i]);  // 买入: 从rest来
        rest = max(rest, sold);               // rest更新
        sold = prev_hold + prices[i];        // 今天卖出→明天冷冻
    }
    return max(hold, max(sold, rest));
}

// 通用冷冻期模板
int maxProfitGeneral(vector<int>& prices, int cooldown) {
    if (prices.empty()) return 0;
    vector<vector<int>> dp(prices.size()+1, vector<int>(2, 0));
    dp[0][1] = -prices[0];
    for (int i = 1; i < prices.size(); i++) {
        dp[i][0] = max(dp[i-1][0], dp[i-1][1] + prices[i]);
        dp[i][1] = max(dp[i-1][1], (i >= cooldown ? dp[i-cooldown][0] : 0) - prices[i]);
    }
    return dp[prices.size()-1][0];
}

int main() {
    vector<vector<int>> tests = {{1,2,3,0,2}, {1}};
    for (auto& p : tests)
        cout << maxProfit(p) << " ";
    cout << endl;
    return 0;
}
