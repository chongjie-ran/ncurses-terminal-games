// LC121 Best Time to Buy and Sell Stock
// 问题：最多一次交易，求最大利润
// 思路：维护最低价格和最大利润
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int maxProfit(vector<int>& prices) {
    int minPrice = INT_MAX;
    int maxProfit = 0;
    for (int price : prices) {
        minPrice = min(minPrice, price);
        maxProfit = max(maxProfit, price - minPrice);
    }
    return maxProfit;
}

int main() {
    vector<int> prices = {7, 1, 5, 3, 6, 4};
    cout << "Input: [7,1,5,3,6,4] -> " << maxProfit(prices) << " (expected: 5)" << endl;
    
    prices = {7, 6, 4, 3, 1};
    cout << "Input: [7,6,4,3,1] -> " << maxProfit(prices) << " (expected: 0)" << endl;
    
    return 0;
}
