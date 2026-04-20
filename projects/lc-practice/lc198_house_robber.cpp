#include <iostream>
#include <vector>
using namespace std;

// LC198 House Robber
// 问题：选择不相邻的 houses 求最大金额
// 核心：dp[i] = max(dp[i-1], dp[i-2] + nums[i])
// 空间优化：只保留前两个状态

int rob(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    
    int prev2 = 0;  // dp[i-2]
    int prev1 = 0;  // dp[i-1]
    
    for (int i = 0; i < n; i++) {
        int curr = max(prev1, prev2 + nums[i]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

int main() {
    vector<int> nums1 = {1,2,3,1};
    cout << rob(nums1) << endl;  // 4 (2+3)
    
    vector<int> nums2 = {2,7,9,3,1};
    cout << rob(nums2) << endl;  // 12 (2+9+1)
    
    vector<int> nums3 = {2,1,1,2};
    cout << rob(nums3) << endl;  // 4 (2+2)
    
    return 0;
}
