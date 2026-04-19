// LC714: Best Time to Buy and Sell Stock with Transaction Fee
// 核心: 状态DP + fee在买入时扣 or 卖出时扣
// buy[j] = max(buy[j-1], sell[j-1] - prices[j] - fee)
// sell[j] = max(sell[j-1], buy[j-1] + prices[j])
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int maxProfit(vector<int>& prices, int fee) {
    if (prices.empty()) return 0;
    int sell = 0, buy = -prices[0];
    for (int i = 1; i < prices.size(); i++) {
        buy = max(buy, sell - prices[i] - fee);
        sell = max(sell, buy + prices[i]);
    }
    return sell;
}

int main() {
    vector<int> p1 = {1,3,2,8,4,9};
    cout << maxProfit(p1, 2) << endl; // 8

    vector<int> p2 = {1,3,7,5,10,3};
    cout << maxProfit(p2, 3) << endl; // 6

    vector<int> p3 = {1,2,3,4,5};
    cout << maxProfit(p3, 2) << endl; // 1

    vector<int> p4 = {2,1,4,4,2,5};
    cout << maxProfit(p4, 2) << endl; // 5

    return 0;
}
