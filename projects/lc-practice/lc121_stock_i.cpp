// LC121 Best Time to Buy and Sell Stock - Easy
// Topic: Kadane-like (single pass)
// Key insight: Track min price seen so far, compute max profit
// Time: O(n), Space: O(1)

#include <iostream>
#include <vector>
using namespace std;

int maxProfit(const vector<int>& prices) {
    int minPrice = INT_MAX;
    int maxProfit = 0;
    for (int price : prices) {
        minPrice = min(minPrice, price);
        maxProfit = max(maxProfit, price - minPrice);
    }
    return maxProfit;
}

int main() {
    vector<int> t1 = {7,1,5,3,6,4};
    cout << "Test1: [7,1,5,3,6,4] => " << maxProfit(t1) << " (expected 5)" << endl;
    // Buy at 1, sell at 6 = 5
    
    vector<int> t2 = {7,6,4,3,1};
    cout << "Test2: [7,6,4,3,1] => " << maxProfit(t2) << " (expected 0)" << endl;
    
    vector<int> t3 = {2,1,4};
    cout << "Test3: [2,1,4] => " << maxProfit(t3) << " (expected 3)" << endl;
    // Buy at 1, sell at 4 = 3
    
    return 0;
}
