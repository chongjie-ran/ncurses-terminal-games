// LC123 Best Time to Buy and Sell Stock III
// 状态机 DP：最多两笔交易
// 思路：四种状态
// hold1: 持有一支股票（买入第一笔后的最大利润）
// sold1: 卖出一支股票（第一笔交易完成）
// hold2: 持有第二支股票（买入第二笔后的最大利润）
// sold2: 卖出两支股票（两笔交易完成）
// 转移：
// hold1 = max(hold1, -prices[i])              // 之前就持有 或 今天刚买第一笔
// sold1 = max(sold1, hold1 + prices[i])       // 之前就卖了 或 今天刚卖第一笔
// hold2 = max(hold2, sold1 - prices[i])       // 之前就持有 或 今天刚买第二笔
// sold2 = max(sold2, hold2 + prices[i])       // 之前就卖了 或 今天刚卖第二笔

#include <iostream>
#include <vector>
#include <climits>
using namespace std;

class Solution {
public:
    int maxProfit(vector<int>& prices) {
        if (prices.empty()) return 0;
        
        // 空间优化版：只需要4个变量
        int hold1 = INT_MIN;  // 买入第一笔后
        int sold1 = 0;        // 卖出第一笔后
        int hold2 = INT_MIN;  // 买入第二笔后
        int sold2 = 0;        // 卖出第二笔后
        
        for (int price : prices) {
            hold1 = max(hold1, -price);              // -price: 第一笔买入
            sold1 = max(sold1, hold1 + price);       // 卖出第一笔
            hold2 = max(hold2, sold1 - price);        // 用第一笔收益买入第二笔
            sold2 = max(sold2, hold2 + price);         // 卖出第二笔
        }
        
        return sold2;  // 卖出两笔后手里现金最多
    }
};

int main() {
    Solution sol;
    
    // Test 1: [3,3,5,0,0,3,1,4]
    // 最佳: 3买(第一笔) 5卖(第一笔) + 0买(第二笔) 4卖(第二笔) = 6
    vector<int> p1 = {3,3,5,0,0,3,1,4};
    cout << sol.maxProfit(p1) << " (期望 6)" << endl;
    
    // Test 2: [1,2,3,4,5]
    // 1买 5卖 = 4
    vector<int> p2 = {1,2,3,4,5};
    cout << sol.maxProfit(p2) << " (期望 4)" << endl;
    
    // Test 3: [7,6,4,3,1]
    // 下跌市，不交易: 0
    vector<int> p3 = {7,6,4,3,1};
    cout << sol.maxProfit(p3) << " (期望 0)" << endl;
    
    // Test 4: 单个元素
    vector<int> p4 = {1};
    cout << sol.maxProfit(p4) << " (期望 0)" << endl;
    
    return 0;
}
