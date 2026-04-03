// LC213 House Robber II - Medium
// Topic: DP on circular array
// Key insight: house[0] and house[n-1] cannot be robbed together
// Solution: max(rob(0..n-2), rob(1..n-1))
// Reference: lc198 (house_robber) uses linear DP

#include <iostream>
#include <vector>
using namespace std;

// Linear house robber (standard DP)
// dp[i] = max amount robbed up to house i
// dp[i] = max(dp[i-1], dp[i-2] + nums[i])
int robLinear(const vector<int>& nums, int start, int end) {
    int prev2 = 0, prev1 = 0;
    for (int i = start; i < end; i++) {
        int cur = max(prev1, prev2 + nums[i]);
        prev2 = prev1;
        prev1 = cur;
    }
    return prev1;
}

int rob(vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    // Case 1: rob house[0..n-2] (exclude last)
    // Case 2: rob house[1..n-1] (exclude first)
    // They can't both be robbed because houses are circular
    return max(robLinear(nums, 0, n-1), robLinear(nums, 1, n));
}

int main() {
    // Test cases
    vector<int> t1 = {2,3,2};
    cout << "Test1: [2,3,2] => " << rob(t1) << " (expected 3)" << endl;
    
    vector<int> t2 = {1,2,3,1};
    cout << "Test2: [1,2,3,1] => " << rob(t2) << " (expected 4)" << endl;
    
    vector<int> t3 = {1,2,3};
    cout << "Test3: [1,2,3] => " << rob(t3) << " (expected 3)" << endl;
    
    vector<int> t4 = {0};
    cout << "Test4: [0] => " << rob(t4) << " (expected 0)" << endl;
    
    return 0;
}
