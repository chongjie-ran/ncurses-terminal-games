// LC122 Best Time to Buy and Sell Stock II - Medium
// Topic: Greedy
// Key insight: Peak-valley approach or simple accumulation of positive diffs
// Profit = sum of all positive price differences
// Because you can complete as many transactions as you want

#include <iostream>
#include <vector>
using namespace std;

// Greedy: accumulate all positive differences
// Time: O(n), Space: O(1)
int maxProfit(const vector<int>& prices) {
    int profit = 0;
    for (int i = 1; i < prices.size(); i++) {
        if (prices[i] > prices[i-1]) {
            profit += prices[i] - prices[i-1];
        }
    }
    return profit;
}

int main() {
    // Test cases
    vector<int> t1 = {7,1,5,3,6,4};
    cout << "Test1: [7,1,5,3,6,4] => " << maxProfit(t1) << " (expected 7)" << endl;
    // Buy at 1, sell at 5 (profit 4), buy at 3, sell at 6 (profit 3) = 7
    
    vector<int> t2 = {1,2,3,4,5};
    cout << "Test2: [1,2,3,4,5] => " << maxProfit(t2) << " (expected 4)" << endl;
    // Buy at 1, sell at 5 = 4 (or accumulate: 1+1+1+1=4)
    
    vector<int> t3 = {7,6,4,3,1};
    cout << "Test3: [7,6,4,3,1] => " << maxProfit(t3) << " (expected 0)" << endl;
    
    vector<int> t4 = {2,1,2,0,1};
    cout << "Test4: [2,1,2,0,1] => " << maxProfit(t4) << " (expected 2)" << endl;
    // Buy at 1, sell at 2 (1), buy at 0, sell at 1 (1) = 2
    
    return 0;
}
