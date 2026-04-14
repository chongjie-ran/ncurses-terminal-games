/*
LC312 Burst Balloons - 区间DP
问题: 给定n个气球，戳破气球i获得nums[left]*nums[i]*nums[right]金币
     求最大金币数 (边界气球不能戳)
思路: 区间DP
     - 添加虚拟气球nums[0]=1和nums[n+1]=1
     - dp[l][r] = 区间(l,r)最大金币数
     - 枚举最后戳的气球k: dp[l][r] = max(dp[l][k] + dp[k][r] + nums[l]*nums[k]*nums[r])
复杂度: O(n^3)
*/
#include <vector>
#include <iostream>
using namespace std;

class Solution {
public:
    int maxCoins(vector<int>& nums) {
        int n = nums.size();
        // 添加虚拟边界气球
        vector<int> val(n + 2, 1);
        for (int i = 0; i < n; i++) val[i + 1] = nums[i];
        
        // dp[l][r] = 戳破(l,r)间所有气球的最大金币
        vector<vector<int>> dp(n + 2, vector<int>(n + 2, 0));
        
        // 区间长度从2到n+1 (即考虑至少一个真实气球)
        for (int len = 2; len <= n + 1; len++) {
            for (int l = 0; l + len <= n + 1; l++) {
                int r = l + len;
                // 枚举最后戳的气球k (l<k<r)
                for (int k = l + 1; k < r; k++) {
                    dp[l][r] = max(dp[l][r], 
                        dp[l][k] + dp[k][r] + val[l] * val[k] * val[r]);
                }
            }
        }
        return dp[0][n + 1];
    }
};

// 测试
int main() {
    Solution sol;
    
    // Test 1: [3,1,5,8] -> 167
    vector<int> nums1 = {3, 1, 5, 8};
    cout << "Test [3,1,5,8]: " << sol.maxCoins(nums1) << " (expected: 167)" << endl;
    
    // Test 2: [1,5] -> 10
    vector<int> nums2 = {1, 5};
    cout << "Test [1,5]: " << sol.maxCoins(nums2) << " (expected: 10)" << endl;
    
    // Test 3: [] -> 0
    vector<int> nums3 = {};
    cout << "Test []: " << sol.maxCoins(nums3) << " (expected: 0)" << endl;
    
    return 0;
}
