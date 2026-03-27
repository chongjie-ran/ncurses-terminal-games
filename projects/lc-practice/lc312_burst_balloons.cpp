// LC312 - Burst Balloons
// Hard DP: 区间 DP
// 戳破所有气球，获得最大金币数。戳破(i)得到 nums[i-1]*nums[i]*nums[i+1].
// 思路：区间 DP，枚举最后戳破的气球
// 时间: O(n^3) 空间: O(n^2)

#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int maxCoins(vector<int>& nums) {
        int n = nums.size();
        // 扩展数组，两端添加虚拟气球 1
        vector<int> a(n + 2, 1);
        for (int i = 0; i < n; ++i) a[i + 1] = nums[i];
        n += 2;
        
        // dp[i][j] = 戳破 (i+1...j-1) 范围内所有气球，获得的最大金币数
        // 即开区间 (i, j) 内的气球都被戳破
        vector<vector<int>> dp(n, vector<int>(n, 0));
        
        // 枚举区间长度
        for (int len = 3; len <= n; ++len) {
            for (int i = 0; i + len <= n; ++i) {
                int j = i + len - 1;
                // 枚举最后戳破的气球 k（i < k < j）
                // k 是最后戳破的，所以 (i,k) 和 (k,j) 区间已经戳完
                for (int k = i + 1; k < j; ++k) {
                    dp[i][j] = max(dp[i][j], dp[i][k] + dp[k][j] + a[i] * a[k] * a[j]);
                }
            }
        }
        
        return dp[0][n - 1];
    }
};

int main() {
    Solution sol;
    
    // Test1: [3,1,5,8] → 167
    // 戳破顺序: 1(->15) → 3(->16) → 2(->135) → 4(->17) = 167
    vector<int> t1 = {3, 1, 5, 8};
    cout << "Test1: " << sol.maxCoins(t1) << " (LeetCode expects 167)" << endl;
    
    // Test2: [1,5] → 10
    vector<int> t2 = {1, 5};
    cout << "Test2: " << sol.maxCoins(t2) << " (LeetCode expects 10)" << endl;
    
    return 0;
}
