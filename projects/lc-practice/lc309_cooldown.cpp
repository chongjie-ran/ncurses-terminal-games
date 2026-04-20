// LC309: Best Time to Buy and Sell Stock with Cooldown
// 修正版：cooldown必须用昨天的sell，不能用今天的sell
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// 正确算法：cooldown用 prevSell（昨天的sell）
int maxProfit309(vector<int>& prices) {
    if (prices.empty()) return 0;
    int n = prices.size();
    int buy = -prices[0];
    int sell = 0;
    int prevSell = 0;  // 昨天的sell，用于冷冻期

    for (int i = 1; i < n; i++) {
        int prevBuy = buy;
        buy = max(buy, prevSell - prices[i]);  // 从冷冻期买入
        sell = max(sell, prevBuy + prices[i]); // 卖出
        prevSell = sell;                       // ??? 这还是有问题
    }
    return sell;
}

// 正确版本：标准三变量DP
int maxProfit309_correct(vector<int>& prices) {
    if (prices.empty()) return 0;
    int n = prices.size();
    int buy = -prices[0];   // 持有股票：max(-price[0], 从冷冻买入)
    int sell = 0;           // 不持有，上次卖出不在冷冻期
    int cooldown = 0;       // 冷冻期（=昨天卖出的sell）

    for (int i = 1; i < n; i++) {
        int newBuy = max(buy, cooldown - prices[i]);
        int newSell = max(sell, buy + prices[i]);
        int newCooldown = sell;   // 今天的sell成为明天的冷冻期
        buy = newBuy;
        sell = newSell;
        cooldown = newCooldown;
    }
    return sell;
}

// 测试验证
int main() {
    vector<pair<vector<int>, int>> tests = {
        {{1,2,3,0,2}, 3},    // buy@0,sell@2,cd@3,buy@4→profit=2+0=2 wait=3?
        {{1}, 0},              // edge: 1 day
        {{}, 0},               // edge: empty
        {{2,1,4}, 3},          // buy@1 sell@4 = 3
        {{1,2,4}, 3},          // buy@1 sell@4 = 3
        {{1,2,3,0,2}, 3},
    };

    printf("=== 正确版本测试 ===\n");
    int pass = 0, fail = 0;
    for (auto& [prices, expected] : tests) {
        int result = maxProfit309_correct(prices);
        if (result == expected) {
            printf("✅ [%s] = %d ✅\n",
                [&](){
                    string s=""; for(auto v:prices) s+=to_string(v)+",";
                    return s;
                }().c_str(), result);
            pass++;
        } else {
            printf("❌ [%s] = %d, expected %d ❌\n",
                [&](){
                    string s=""; for(auto v:prices) s+=to_string(v)+",";
                    return s;
                }().c_str(), result, expected);
            fail++;
        }
    }
    printf("\n结果: %d/%zu 通过\n", pass, tests.size());
    return fail;
}
