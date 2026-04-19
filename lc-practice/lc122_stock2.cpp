// LC122: Best Time to Buy and Sell Stock II - unlimited transactions
// 核心: 贪心 - 所有上涨区间都买卖
#include <iostream>
#include <vector>
using namespace std;

int maxProfit(vector<int>& prices) {
    int profit = 0;
    for (int i = 1; i < prices.size(); i++) {
        if (prices[i] > prices[i-1]) {
            profit += prices[i] - prices[i-1];
        }
    }
    return profit;
}

int main() {
    vector<int> p1 = {7,1,5,3,6,4};
    cout << maxProfit(p1) << endl; // 7

    vector<int> p2 = {1,2,3,4,5};
    cout << maxProfit(p2) << endl; // 4

    vector<int> p3 = {7,6,4,3,1};
    cout << maxProfit(p3) << endl; // 0

    return 0;
}
