// LC123 买卖股票最佳时机III - 困难
// 最多2次交易
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// 四状态DP
// buy1: 第一次买入后的最大利润
// sell1: 第一次卖出后的最大利润  
// buy2: 第二次买入后的最大利润
// sell2: 第二次卖出后的最大利润
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

// 测试
int main() {
    vector<int> p1 = {3,3,5,0,0,3,1,4};
    cout << maxProfit(p1) << endl; // 6: 0,3买入3,4卖出 + 1买入4卖出
    
    vector<int> p2 = {1,2,3,4,5};
    cout << maxProfit(p2) << endl; // 4: 1买入5卖出
    
    vector<int> p3 = {7,6,4,3,1};
    cout << maxProfit(p3) << endl; // 0
    
    return 0;
}
