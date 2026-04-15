// LC322 Coin Change - 完全背包/最小硬币数
#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
using namespace std;

// DP: 外层金额，内层硬币（组合数，顺序无关）
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        vector<int> dp(amount+1, amount+1);
        dp[0] = 0;
        for (int i = 1; i <= amount; i++) {
            for (int coin : coins) {
                if (i - coin >= 0) {
                    dp[i] = min(dp[i], dp[i-coin] + 1);
                }
            }
        }
        return dp[amount] == amount+1 ? -1 : dp[amount];
    }
};

int main() {
    Solution s;
    vector<int> coins = {1,2,5};
    cout << s.coinChange(coins, 11) << endl; // 3: 5+5+1
    vector<int> coins2 = {2};
    cout << s.coinChange(coins2, 3) << endl; // -1
    vector<int> coins3 = {1};
    cout << s.coinChange(coins3, 0) << endl; // 0
    vector<int> coins4 = {1};
    cout << s.coinChange(coins4, 2) << endl; // 2
    return 0;
}
