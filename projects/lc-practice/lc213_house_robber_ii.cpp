#include <iostream>
#include <vector>
using namespace std;

// LC213 House Robber II - 环形房屋
// 问题：首尾相连的 houses 求最大金额
// 核心：两种情况 - 不抢第一间 vs 不抢最后一间

int robRange(vector<int>& nums, int start, int end) {
    int prev2 = 0, prev1 = 0;
    for (int i = start; i <= end; i++) {
        int curr = max(prev1, prev2 + nums[i]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

int rob(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    // 不抢第一间 or 不抢最后一间
    return max(robRange(nums, 0, n-2), robRange(nums, 1, n-1));
}

int main() {
    vector<int> nums1 = {2,3,2};
    cout << rob(nums1) << endl;  // 3 (2+1不work, 选3)
    
    vector<int> nums2 = {1,2,3,1};
    cout << rob(nums2) << endl;  // 4 (2+3)
    
    vector<int> nums3 = {1,2,3};
    cout << rob(nums3) << endl;  // 3 (2+1不work, 选3)
    
    return 0;
}
