// LC121: Best Time to Buy and Sell Stock - 1 transaction
// 核心: min_price记录最低点，max_profit记录最大差值
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

int maxProfit(vector<int>& prices) {
    int min_price = INT_MAX;
    int max_profit = 0;
    for (int price : prices) {
        min_price = min(min_price, price);
        max_profit = max(max_profit, price - min_price);
    }
    return max_profit;
}

int main() {
    vector<int> p1 = {7,1,5,3,6,4};
    cout << maxProfit(p1) << endl; // 5

    vector<int> p2 = {7,6,4,3,1};
    cout << maxProfit(p2) << endl; // 0

    vector<int> p3 = {3,3,5,0,0,3,1,4};
    cout << maxProfit(p3) << endl; // 4

    return 0;
}
