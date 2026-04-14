// LC416 Partition Equal Subset Sum - 01背包
// 问题：能否将数组分成两个和相等的子集
// 思路：转换为01背包 - 是否能装满sum/2的背包

#include <iostream>
#include <vector>
#include <numeric>
using namespace std;

// DP版本 - 01背包
bool canPartition(vector<int>& nums) {
    int n = nums.size();
    int sum = accumulate(nums.begin(), nums.end(), 0);
    if (sum % 2 != 0) return false;
    
    int target = sum / 2;
    vector<bool> dp(target + 1, false);
    dp[0] = true;
    
    // 01背包：倒序遍历容量
    for (int num : nums) {
        for (int j = target; j >= num; j--) {
            dp[j] = dp[j] || dp[j - num];
        }
    }
    return dp[target];
}

// 测试
int main() {
    vector<int> nums1 = {1, 5, 11, 5};
    cout << "Example 1: " << (canPartition(nums1) ? "true" : "false") << endl; // true: [1,5,5] vs [11]
    
    vector<int> nums2 = {1, 2, 3, 5};
    cout << "Example 2: " << (canPartition(nums2) ? "true" : "false") << endl; // false
    
    vector<int> nums3 = {1, 2, 3, 4, 5, 6, 7};
    cout << "Example 3: " << (canPartition(nums3) ? "true" : "false") << endl; // true: [1,6,7] vs [2,3,4,5]
    
    vector<int> nums4 = {2, 2, 2, 2};
    cout << "Example 4: " << (canPartition(nums4) ? "true" : "false") << endl; // true: [2,2] vs [2,2]
    
    vector<int> nums5 = {100};
    cout << "Example 5: " << (canPartition(nums5) ? "true" : "false") << endl; // false
    
    return 0;
}
