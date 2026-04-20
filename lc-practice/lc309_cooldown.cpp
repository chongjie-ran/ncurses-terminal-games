// LC309: Best Time to Buy and Sell Stock with Cooldown
// 核心: 状态机DP
// 状态: hold[i] = max profit ending with stock held on day i
//       sold[i] = max profit ending with sold on day i
//       rest[i] = max profit ending with rest on day i
// 转移: hold[i] = max(hold[i-1], rest[i-1] - prices[i])  // keep or buy
//       sold[i] = hold[i-1] + prices[i]                   // sell today
//       rest[i] = max(rest[i-1], sold[i-1])               // rest or cooldown
// 空间优化: 只需要3个变量
#include <iostream>
#include <vector>
using namespace std;

int maxProfit(vector<int>& prices) {
    if (prices.empty()) return 0;
    int n = prices.size();
    
    int hold = -prices[0];   // 持有股票
    int sold = 0;            // 刚卖出 (cooldown)
    int rest = 0;            // 休息/冷冻期
    
    for (int i = 1; i < n; i++) {
        int prev_hold = hold;
        int prev_sold = sold;
        int prev_rest = rest;
        
        hold = max(prev_hold, prev_rest - prices[i]);  // 保持 或 买入
        sold = prev_hold + prices[i];                   // 卖出
        rest = max(prev_rest, prev_sold);               // 休息 或 解冻
        
        // sold变成rest (cooldown)
        // 但更清晰的是: cooldown只在sold后一天rest
    }
    
    return max(sold, rest);
}

// 更清晰的3状态版本
int maxProfitV2(vector<int>& prices) {
    if (prices.empty()) return 0;
    int n = prices.size();
    
    int hold = -prices[0];  // 持有
    int sold = 0;            // 卖出（不在冷冻期）
    int cool = 0;           // 冷冻期
    
    for (int i = 1; i < n; i++) {
        int new_hold = max(hold, cool - prices[i]);
        int new_sold = hold + prices[i];
        int new_cool = max(sold, cool);
        hold = new_hold;
        sold = new_sold;
        cool = new_cool;
    }
    
    return max(sold, cool);
}

int main() {
    vector<int> p1 = {1,2,3,0,2};
    cout << maxProfit(p1) << " | " << maxProfitV2(p1) << endl; // 3
    
    vector<int> p2 = {1};
    cout << maxProfit(p2) << " | " << maxProfitV2(p2) << endl; // 0
    
    vector<int> p3 = {1,2,4};
    cout << maxProfit(p3) << " | " << maxProfitV2(p3) << endl; // 3
    
    vector<int> p4 = {1,2,3,0,2,1,5,0,3};
    cout << maxProfit(p4) << " | " << maxProfitV2(p4) << endl; // 6
    
    return 0;
}
