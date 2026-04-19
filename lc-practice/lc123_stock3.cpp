// LC123: Best Time to Buy and Sell Stock III - 2 transactions
// 核心: DP - 分割点 + 两边最大利润
//       left[i] = max profit [0..i] (1 transaction)
//       right[i] = max profit [i..n-1] (1 transaction)
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

int maxProfit(vector<int>& prices) {
    if (prices.empty()) return 0;
    int n = prices.size();

    vector<int> left(n, 0), right(n, 0);

    // left: 1 transaction max profit in [0..i]
    int min_price = prices[0];
    for (int i = 1; i < n; i++) {
        min_price = min(min_price, prices[i]);
        left[i] = max(left[i-1], prices[i] - min_price);
    }

    // right: 1 transaction max profit in [i..n-1]
    int max_price = prices[n-1];
    for (int i = n-2; i >= 0; i--) {
        max_price = max(max_price, prices[i]);
        right[i] = max(right[i+1], max_price - prices[i]);
    }

    // split at i: left[i] + right[i]
    int profit = 0;
    for (int i = 0; i < n; i++) {
        profit = max(profit, left[i] + right[i]);
    }
    return profit;
}

int main() {
    vector<int> p1 = {3,3,5,0,0,3,1,4};
    cout << maxProfit(p1) << endl; // 6

    vector<int> p2 = {1,2,3,4,5};
    cout << maxProfit(p2) << endl; // 4

    vector<int> p3 = {7,6,4,3,1};
    cout << maxProfit(p3) << endl; // 0

    vector<int> p4 = {1,2,4};
    cout << maxProfit(p4) << endl; // 4

    return 0;
}
