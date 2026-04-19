
#include <iostream>
#include <vector>
using namespace std;

// LC518 Coin Change II - 组合数
int change(int amount, vector<int>& coins) {
    vector<int> dp(amount+1, 0);
    dp[0] = 1;
    for (int c: coins) {
        for (int i=c; i<=amount; i++) {
            dp[i] += dp[i-c];
        }
    }
    return dp[amount];
}

int main() {
    vector<int> c = {1,2,5};
    cout << change(5, c) << endl; // 4
    return 0;
}
