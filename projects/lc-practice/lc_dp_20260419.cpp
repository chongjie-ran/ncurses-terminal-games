#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// LC188: k次交易 DP
int maxProfit_k(int k, vector<int>& prices) {
    int n = prices.size();
    if (n == 0 || k == 0) return 0;
    if (k >= n/2) {
        int profit = 0;
        for (int i = 1; i < n; i++)
            if (prices[i] > prices[i-1])
                profit += prices[i] - prices[i-1];
        return profit;
    }
    vector<vector<int>> hold(k+1, vector<int>(n, 0));
    vector<vector<int>> cash(k+1, vector<int>(n, 0));
    for (int t = 0; t <= k; t++) hold[t][0] = -prices[0];
    for (int i = 1; i < n; i++) {
        for (int t = 1; t <= k; t++) {
            cash[t][i] = max(cash[t][i-1], hold[t][i-1] + prices[i]);
            hold[t][i] = max(hold[t][i-1], cash[t-1][i-1] - prices[i]);
        }
    }
    return cash[k][n-1];
}

// LC983: 最低票价
int mincostTickets(vector<int>& days, vector<int>& costs) {
    int lastDay = days.back();
    vector<int> dp(lastDay + 1, 0);
    int dayIdx = 0;
    for (int i = 1; i <= lastDay; i++) {
        if (i == days[dayIdx]) {
            int cost1 = dp[i-1] + costs[0];
            int cost7 = dp[max(0, i-7)] + costs[1];
            int cost30 = dp[max(0, i-30)] + costs[2];
            dp[i] = min({cost1, cost7, cost30});
            dayIdx++;
        } else {
            dp[i] = dp[i-1];
        }
    }
    return dp[lastDay];
}

// LC474: 一和零（背包）
int findMaxForm(vector<string>& strs, int m, int n) {
    vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
    for (string& s : strs) {
        int zeros = 0, ones = 0;
        for (char c : s) { if (c == '0') zeros++; else ones++; }
        for (int i = m; i >= zeros; i--)
            for (int j = n; j >= ones; j--)
                dp[i][j] = max(dp[i][j], dp[i-zeros][j-ones] + 1);
    }
    return dp[m][n];
}

// LC343: 整数拆分
int integerBreak(int n) {
    vector<int> dp(n+1, 0);
    dp[1] = 1;
    for (int i = 2; i <= n; i++)
        for (int j = 1; j < i; j++)
            dp[i] = max({dp[i], j * (i-j), j * dp[i-j]});
    return dp[n];
}

int main() {
    vector<int> p1 = {2,4,1};
    cout << "LC188 k=2 [2,4,1]: " << maxProfit_k(2, p1) << " (expect 2)" << endl;
    vector<int> p2 = {3,2,6,5,0,3};
    cout << "LC188 k=2 [3,2,6,5,0,3]: " << maxProfit_k(2, p2) << " (expect 7)" << endl;
    vector<int> p3 = {1,2,3,4,5};
    cout << "LC188 k=2 [1,2,3,4,5]: " << maxProfit_k(2, p3) << " (expect 4)" << endl;
    vector<int> p4 = {3,3,5,0,0,3,1,4};
    cout << "LC188 k=100 [贪心]: " << maxProfit_k(100, p4) << " (expect 8)" << endl;
    
    vector<int> days = {1,4,6,7,8,20};
    vector<int> costs = {2,7,15};
    cout << "LC983: " << mincostTickets(days, costs) << " (expect 11)" << endl;
    
    vector<string> s1 = {"10","0001","111001","1","0"};
    cout << "LC474: " << findMaxForm(s1, 5, 3) << " (expect 4)" << endl;
    
    cout << "LC343 n=2: " << integerBreak(2) << " (expect 1)" << endl;
    cout << "LC343 n=10: " << integerBreak(10) << " (expect 36)" << endl;
    return 0;
}
