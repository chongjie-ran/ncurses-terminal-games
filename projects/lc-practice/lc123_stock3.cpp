// LC123 Best Time to Buy and Sell Stock III
// 问题：最多两次交易，求最大利润
// 思路：DP五状态
// buy1: 第一次买入后最大利润
// sell1: 第一次卖出后最大利润  
// buy2: 第二次买入后最大利润
// sell2: 第二次卖出后最大利润
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int maxProfit(vector<int>& prices) {
    if (prices.empty()) return 0;
    
    int buy1 = -prices[0], sell1 = 0;
    int buy2 = -prices[0], sell2 = 0;
    
    for (int i = 1; i < prices.size(); i++) {
        buy1 = max(buy1, -prices[i]);
        sell1 = max(sell1, buy1 + prices[i]);
        buy2 = max(buy2, sell1 - prices[i]);
        sell2 = max(sell2, buy2 + prices[i]);
    }
    
    return sell2;
}

int main() {
    vector<int> prices = {3, 3, 5, 0, 0, 3, 1, 4};
    cout << "Input: [3,3,5,0,0,3,1,4] -> " << maxProfit(prices) << " (expected: 6)" << endl;
    
    prices = {1, 2, 3, 4, 5};
    cout << "Input: [1,2,3,4,5] -> " << maxProfit(prices) << " (expected: 4)" << endl;
    
    return 0;
}
