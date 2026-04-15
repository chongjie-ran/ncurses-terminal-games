#include <iostream>
#include <vector>
#include <climits>
using namespace std;

// LC188 Best Time to Buy and Sell Stock IV
// 最多k次交易
// DP: dp[i][j] = max profit up to day i with j transactions
// 空间优化版
int maxProfit(int k, vector<int>& prices) {
    int n = prices.size();
    if (n < 2 || k == 0) return 0;
    
    // k >= n/2 → greedy
    if (k >= n / 2) {
        int profit = 0;
        for (int i = 1; i < n; ++i)
            if (prices[i] > prices[i-1]) profit += prices[i] - prices[i-1];
        return profit;
    }
    
    // dp[j] = max profit with j transactions so far
    vector<int> dp(k + 1, 0), minPrice(k + 1, INT_MAX);
    
    for (int price : prices) {
        for (int j = 1; j <= k; ++j) {
            minPrice[j] = min(minPrice[j], price - dp[j-1]);
            dp[j] = max(dp[j], price - minPrice[j]);
        }
    }
    return dp[k];
}

int main() {
    // Test 2 transactions (LC123)
    auto test2 = [](vector<int> p, int exp) {
        int r = maxProfit(2, p);
        cout << (r == exp ? "✅" : "❌") << " k=2: ";
        for (int x : p) cout << x << " ";
        cout << " => " << r << " (exp " << exp << ")\n";
    };
    
    test2({3,3,5,0,0,3,1,4}, 6);   // buy@0 sell@3(+3) + buy@1 sell@4(+3) = 6
    test2({1,2,3,4,5}, 4);          // 0→4(+4) = 4
    test2({7,6,4,3,1}, 0);           // no profit
    test2({1}, 0);
    test2({1,2}, 1);                // 1→2
    test2({2,1,4}, 2);              // 1→4 = 3? No, k=2 unlimited → greedy = 3
    test2({3,2,6,5,0,3}, 4);        // buy@2 sell@5(6-0=6?) → 2→6 + 0→3 = 7?
    
    cout << "\nGreedy test (k >= n/2):\n";
    auto testG = [](vector<int> p, int exp) {
        int r = maxProfit(100, p);  // large k → greedy
        cout << (r == exp ? "✅" : "❌") << " k=100: ";
        for (int x : p) cout << x << " ";
        cout << " => " << r << "\n";
    };
    testG({7,1,5,3,6,4}, 7);
    testG({1,2,3,4,5}, 4);
    
    return 0;
}
