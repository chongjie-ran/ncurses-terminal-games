#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int maxCoins(vector<int>& nums) {
    int n = nums.size();
    vector<vector<int>> dp(n+2, vector<int>(n+2, 0));
    vector<int> val(n+2);
    val[0] = val[n+1] = 1;
    for (int i = 1; i <= n; i++) val[i] = nums[i-1];
    for (int len = 2; len <= n+1; len++) {
        for (int i = 0; i + len <= n+2; i++) {
            int j = i + len;
            for (int k = i+1; k < j; k++) {
                dp[i][j] = max(dp[i][j], dp[i][k] + dp[k][j] + val[i]*val[k]*val[j]);
            }
        }
    }
    return dp[0][n+1];
}
int main() {
    vector<int> t1 = {3,1,5,8};
    cout << maxCoins(t1) << endl; // 167
    vector<int> t2 = {1,5};
    cout << maxCoins(t2) << endl; // 10
}
