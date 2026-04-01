// LC416 Partition Equal Subset Sum - Medium
// 问题: 判断能否将数组分成两部分，使得两部分和相等
// 思路: 转化为背包问题 - 是否能从数组中选出若干数凑成 sum/2
// 约束: 1 <= nums[i] <= 100, 数组长度 <= 200, sum <= 20000

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ==================== Solution 1: DP 1D (空间优化) ====================
// dp[i] = 是否能凑成和 i
bool canPartition(vector<int>& nums) {
    int sum = 0;
    for (int n : nums) sum += n;
    if (sum % 2 != 0) return false;
    
    int target = sum / 2;
    // 剪枝: 如果最大数大于 target，直接 false
    sort(nums.begin(), nums.end(), greater<int>());
    if (nums[0] > target) return false;
    
    vector<char> dp(target + 1, 0);
    dp[0] = 1;
    
    for (int num : nums) {
        for (int i = target; i >= num; i--) {
            dp[i] = dp[i] || dp[i - num];
        }
    }
    
    return dp[target];
}

// ==================== Solution 2: 递归 + 记忆化 ====================
// 对已排序数组剪枝
bool dfsMemo(const vector<int>& nums, int idx, int target, vector<char>& memo) {
    if (target == 0) return true;
    if (idx >= (int)nums.size() || target < 0) return false;
    if (memo[idx * 10001 + target] != -1) return memo[idx * 10001 + target];
    
    bool result = dfsMemo(nums, idx + 1, target - nums[idx], memo) 
               || dfsMemo(nums, idx + 1, target, memo);
    memo[idx * 10001 + target] = result;
    return result;
}

bool canPartitionMemo(vector<int>& nums) {
    int sum = 0;
    for (int n : nums) sum += n;
    if (sum % 2 != 0) return false;
    
    sort(nums.begin(), nums.end(), greater<int>());
    if (nums[0] > sum / 2) return false;
    
    vector<char> memo(nums.size() * 10001, -1);
    return dfsMemo(nums, 0, sum / 2, memo);
}

// ==================== Tests ====================
int main() {
    // Test 1: [1,5,11,13,30] -> sum=69, target=34.5 无法平分
    vector<int> nums1 = {1, 5, 11, 13, 30};
    cout << "Test1: " << canPartition(nums1) << " (expected 0)" << endl;
    
    // Test 2: [1,5,11,13] -> sum=30, target=15 -> 1+5+13=19, 11 不行
    vector<int> nums2 = {1, 5, 11, 13};
    cout << "Test2: " << canPartition(nums2) << " (expected 0)" << endl;
    
    // Test 3: [1,5,11] -> sum=17 无法平分
    vector<int> nums3 = {1, 5, 11};
    cout << "Test3: " << canPartition(nums3) << " (expected 0)" << endl;
    
    // Test 4: [2,2,3,5] -> sum=12, target=6 -> 2+2+2=6 不行，2+3+5=10 不行
    vector<int> nums4 = {2, 2, 3, 5};
    cout << "Test4: " << canPartition(nums4) << " (expected 0)" << endl;
    
    // Test 5: [1,2,3,4,5,6,7] -> sum=28, target=14 -> 1+6+7=14 or 2+4+8...
    vector<int> nums5 = {1, 2, 3, 4, 5, 6, 7};
    cout << "Test5: " << canPartition(nums5) << " (expected 1)" << endl;
    
    // Test 6: [1,2,5] -> sum=8 无法平分
    vector<int> nums6 = {1, 2, 5};
    cout << "Test6: " << canPartition(nums6) << " (expected 0)" << endl;
    
    // Test 7: Large array
    vector<int> nums7(200, 1); // sum=200, target=100
    cout << "Test7: " << canPartition(nums7) << " (expected 1)" << endl;
    
    return 0;
}
