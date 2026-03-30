// LC188 Best Time to Buy and Sell Stock IV — Hard
// 最多完成k笔交易，获取最大利润
#include <cstdio>
#include <vector>
#include <algorithm>
using namespace std;

// DP解法: dp[t][i] = 第t笔交易在第i天结束后的最大利润
// dp[t][i] = max(dp[t][i-1], prices[i] + maxDiff)
// 其中 maxDiff = max(dp[t-1][j] - prices[j]) for all j < i
// 空间优化: O(k) space
int maxProfit(int k, vector<int>& prices) {
    int n = prices.size();
    if (n == 0 || k == 0) return 0;
    // 如果k >= n/2，变成无限次交易
    if (k >= n / 2) {
        int profit = 0;
        for (int i = 1; i < n; i++)
            profit += max(0, prices[i] - prices[i-1]);
        return profit;
    }
    vector<int> dp(k+1, 0);
    vector<int> maxDiff(k+1, -prices[0]);
    for (int i = 1; i < n; i++) {
        for (int t = k; t >= 1; t--) {
            dp[t] = max(dp[t], prices[i] + maxDiff[t]);
            maxDiff[t] = max(maxDiff[t], dp[t-1] - prices[i]);
        }
    }
    return dp[k];
}

// 优化：改用 2D DP 以避免 1D 的边界问题
int maxProfit2(int k, vector<int>& prices) {
    int n = prices.size();
    if (n == 0 || k == 0) return 0;
    if (k >= n/2) {
        int profit = 0;
        for (int i = 1; i < n; i++)
            profit += max(0, prices[i] - prices[i-1]);
        return profit;
    }
    vector<vector<int>> dp(k+1, vector<int>(n, 0));
    for (int t = 1; t <= k; t++) {
        int maxDiff = -prices[0];
        for (int i = 1; i < n; i++) {
            dp[t][i] = max(dp[t][i-1], prices[i] + maxDiff);
            maxDiff = max(maxDiff, dp[t-1][i] - prices[i]);
        }
    }
    return dp[k][n-1];
}

int main() {
    // Test
    vector<int> p1 = {2,4,1};
    printf("k=2, [2,4,1]: %d (expect 2)\n", maxProfit(2, p1));
    vector<int> p2 = {3,2,6,5,0,3};
    printf("k=2, [3,2,6,5,0,3]: %d (expect 7)\n", maxProfit(2, p2));
    return 0;
}
