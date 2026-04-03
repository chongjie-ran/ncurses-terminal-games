#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// LC198 House Robber - 1D DP (pick or skip pattern)
// Time: O(n), Space: O(1)
int rob(vector<int>& nums) {
    int prev2 = 0, prev1 = 0; // dp[i-2], dp[i-1]
    for (int n : nums) {
        int cur = max(prev1, prev2 + n);
        prev2 = prev1;
        prev1 = cur;
    }
    return prev1;
}

// LC213 House Robber II - circular array (house 0 and n-1 cannot both be picked)
// Time: O(n), Space: O(1)
int rob2(vector<int>& nums) {
    if (nums.size() == 1) return nums[0];
    // Case 1: rob [0..n-2], Case 2: rob [1..n-1]
    int prev2_1 = 0, prev1_1 = 0; // for [0..n-2]
    int prev2_2 = 0, prev1_2 = 0; // for [1..n-1]
    
    for (int i = 0; i < nums.size() - 1; i++) {
        int cur = max(prev1_1, prev2_1 + nums[i]);
        prev2_1 = prev1_1;
        prev1_1 = cur;
    }
    for (int i = 1; i < nums.size(); i++) {
        int cur = max(prev1_2, prev2_2 + nums[i]);
        prev2_2 = prev1_2;
        prev1_2 = cur;
    }
    return max(prev1_1, prev1_2);
}

int main() {
    // Test House Robber
    vector<int> t1 = {1,2,3,1};
    cout << "LC198: " << rob(t1) << " (expect 4)" << endl;
    vector<int> t2 = {2,7,9,3,1};
    cout << "LC198: " << rob(t2) << " (expect 12)" << endl;
    
    // Test House Robber II
    vector<int> t3 = {2,3,2};
    cout << "LC213: " << rob2(t3) << " (expect 3)" << endl;
    vector<int> t4 = {1,2,3,1};
    cout << "LC213: " << rob2(t4) << " (expect 4)" << endl;
    vector<int> t5 = {1,2,3};
    cout << "LC213: " << rob2(t5) << " (expect 3)" << endl;
    
    return 0;
}
