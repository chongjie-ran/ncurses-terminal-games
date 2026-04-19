// LC188: Best Time to Buy and Sell Stock IV - k transactions
// 核心: DP[i][j] = max(DP[i][j-1], prices[j] + maxDiff)
//       maxDiff = max(maxDiff, DP[i-1][j] - prices[j])
// 空间优化: 2D→2行
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

int maxProfit(int k, vector<int>& prices) {
    if (prices.empty() || k == 0) return 0;
    int n = prices.size();
    if (k > n/2) { // unlimited transactions
        int profit = 0;
        for (int i = 1; i < n; i++)
            if (prices[i] > prices[i-1])
                profit += prices[i] - prices[i-1];
        return profit;
    }
    vector<int> dp(k+1, 0), maxDiff(k+1, INT_MIN);
    for (int j = 0; j < n; j++) {
        for (int i = 1; i <= k; i++) {
            maxDiff[i] = max(maxDiff[i], dp[i-1] - prices[j]);
            dp[i] = max(dp[i], prices[j] + maxDiff[i]);
        }
    }
    return dp[k];
}

int main() {
    vector<int> p1 = {2,4,1};
    cout << maxProfit(2, p1) << endl; // 2

    vector<int> p2 = {3,2,6,5,0,3};
    cout << maxProfit(2, p2) << endl; // 7

    vector<int> p3 = {1,2,3,4,5};
    cout << maxProfit(2, p3) << endl; // 4

    vector<int> p4 = {7,6,4,3,1};
    cout << maxProfit(2, p4) << endl; // 0

    vector<int> p5 = {5,11,3,50,60,90};
    cout << maxProfit(2, p5) << endl; // 93

    return 0;
}
