#include <iostream>
#include <vector>
using namespace std;

// LC312 戳气球 - 区间DP
// 添加边界0和n+1，dp[i][j] = max(dp[i][j], dp[i][k]+dp[k][j]+nums[i]*nums[k]*nums[j])
int maxCoins(vector<int>& nums) {
    int n = nums.size();
    vector<int> a(n+2, 1);
    for (int i = 0; i < n; i++) a[i+1] = nums[i];
    n += 2;
    vector<vector<int>> dp(n, vector<int>(n, 0));
    for (int len = 2; len < n; len++) {
        for (int i = 0; i + len < n; i++) {
            int j = i + len;
            for (int k = i+1; k < j; k++) {
                dp[i][j] = max(dp[i][j], dp[i][k] + dp[k][j] + a[i]*a[k]*a[j]);
            }
        }
    }
    return dp[0][n-1];
}

int main() {
    vector<vector<int>> tests = {{3,1,5,8}, {1,5}, {}};
    vector<int> expected = {167, 10, 0};
    for (int i = 0; i < tests.size(); i++) {
        int result = maxCoins(tests[i]);
        cout << "Test" << i << ": " << result << " (expected " << expected[i] << ") " << (result==expected[i]?"✓":"✗") << endl;
    }
    return 0;
}
