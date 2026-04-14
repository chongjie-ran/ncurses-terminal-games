// LC300 Longest Increasing Subsequence
// 问题：最长递增子序列长度
// 思路：DP O(n²) 或 Binary Search O(nlogn)

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// DP版本 O(n²)
int lengthOfLIS_dp(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    
    vector<int> dp(n, 1); // dp[i] = 以nums[i]结尾的最长递增子序列长度
    int result = 1;
    
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i]) {
                dp[i] = max(dp[i], dp[j] + 1);
            }
        }
        result = max(result, dp[i]);
    }
    return result;
}

// Binary Search版本 O(nlogn) - 维护一个有序数组
int lengthOfLIS_bs(vector<int>& nums) {
    vector<int> tails; // tails[i] = 长度为i+1的递增子序列的最小结尾元素
    
    for (int num : nums) {
        auto it = lower_bound(tails.begin(), tails.end(), num);
        if (it == tails.end()) {
            tails.push_back(num); // 比所有元素都大，延长序列
        } else {
            *it = num; // 替换，找更小的结尾元素
        }
    }
    return tails.size();
}

// 测试
int main() {
    vector<int> nums1 = {10, 9, 2, 5, 3, 7, 101, 18};
    cout << "Example 1: " << lengthOfLIS_dp(nums1) << endl; // 4: [2,3,7,101] or [2,3,7,18]
    cout << "Example 1 BS: " << lengthOfLIS_bs(nums1) << endl;
    
    vector<int> nums2 = {0, 1, 0, 3, 2, 3};
    cout << "Example 2: " << lengthOfLIS_dp(nums2) << endl; // 4: [0,1,2,3]
    
    vector<int> nums3 = {7, 7, 7, 7, 7, 7, 7};
    cout << "Example 3: " << lengthOfLIS_dp(nums3) << endl; // 1
    
    vector<int> nums4 = {};
    cout << "Example 4: " << lengthOfLIS_dp(nums4) << endl; // 0
    
    vector<int> nums5 = {1, 2, 3, 4, 5};
    cout << "Example 5: " << lengthOfLIS_dp(nums5) << " BS:" << lengthOfLIS_bs(nums5) << endl; // 5
    
    return 0;
}
