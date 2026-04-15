// LC188 买卖股票最佳时机IV - 困难
// 最多k次交易
#include <iostream>
#include <vector>
using namespace std;

// 空间优化版本 O(k*n)
int maxProfit(int k, vector<int>& prices) {
    if (prices.empty() || k == 0) return 0;
    int n = prices.size();
    if (k > n/2) k = n/2; // 最多n/2次完整交易
    
    vector<vector<int>> dp(k+1, vector<int>(2, 0));
    for (int j = 0; j < k; j++) dp[j][0] = -prices[0];
    
    for (int i = 1; i < n; i++) {
        for (int j = 1; j <= k; j++) {
            dp[j][0] = max(dp[j][0], dp[j-1][1] - prices[i]);
            dp[j][1] = max(dp[j][1], dp[j][0] + prices[i]);
        }
    }
    return dp[k][1];
}

int main() {
    vector<int> p1 = {3,2,6,5,0,3};
    cout << "k=2: " << maxProfit(2, p1) << endl; // 7
    
    vector<int> p2 = {2,4,1,7};
    cout << "k=2: " << maxProfit(2, p2) << endl; // 6: buy2-sell2(1->7) + buy5-sell6(0->3)? 
    // 实际上: buy@2 sell@4(2), buy@1 sell@7(6) = 8? 不对，因为卖了之后才能买
    // 正确: buy@2 sell@7(5), 第二笔交易需要等第一笔完成后才能开始
    
    vector<int> p3 = {1,2,3,4,5};
    cout << "k=2: " << maxProfit(2, p3) << endl; // 4: buy@1 sell@5
    
    vector<int> p4 = {7,6,4,3,1};
    cout << "k=2: " << maxProfit(2, p4) << endl; // 0
    
    return 0;
}
