// LC122 Best Time to Buy and Sell Stock II
// 问题：可以多次交易，求最大利润
// 思路：贪心 - 所有上涨都要捕获
#include <iostream>
#include <vector>
using namespace std;

int maxProfit(vector<int>& prices) {
    int profit = 0;
    for (int i = 1; i < prices.size(); i++) {
        if (prices[i] > prices[i-1]) {
            profit += prices[i] - prices[i-1];
        }
    }
    return profit;
}

int main() {
    vector<int> prices = {7, 1, 5, 3, 6, 4};
    cout << "Input: [7,1,5,3,6,4] -> " << maxProfit(prices) << " (expected: 7)" << endl;
    
    prices = {1, 2, 3, 4, 5};
    cout << "Input: [1,2,3,4,5] -> " << maxProfit(prices) << " (expected: 4)" << endl;
    
    prices = {7, 6, 4, 3, 1};
    cout << "Input: [7,6,4,3,1] -> " << maxProfit(prices) << " (expected: 0)" << endl;
    
    return 0;
}
