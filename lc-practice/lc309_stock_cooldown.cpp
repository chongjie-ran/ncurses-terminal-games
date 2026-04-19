// LC309: Best Time to Buy and Sell Stock with Cooldown
// 核心: 状态机DP
// sell[i] = max profit up to i ending with SELL (or cooldown)
// buy[i] = max profit up to i ending with BUY
// rest[i] = max profit up to i ending with REST
// 转移: buy[i] = max(buy[i-1], sell[i-2] - prices[i])
//       sell[i] = buy[i-1] + prices[i]
//       rest[i] = max(sell[i-1], rest[i-1])
// 最终: sell[n-1] (最后必须卖出)
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int maxProfit(vector<int>& prices) {
    if (prices.empty()) return 0;
    int n = prices.size();
    int sell = 0, prev_sell = 0, buy = -prices[0];
    for (int i = 1; i < n; i++) {
        int prev_buy = buy;
        buy = max(buy, prev_sell - prices[i]);
        prev_sell = sell;
        sell = prev_buy + prices[i];
    }
    return sell;
}

int main() {
    vector<int> p1 = {1,2,3,0,2};
    cout << maxProfit(p1) << endl; // 3

    vector<int> p2 = {1};
    cout << maxProfit(p2) << endl; // 0

    vector<int> p3 = {1,2,4};
    cout << maxProfit(p3) << endl; // 3

    vector<int> p4 = {2,1,4,4,2,5};
    cout << maxProfit(p4) << endl; // 6

    return 0;
}
