// LC198 House Robber - 动态规划
#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int rob(vector<int>& nums) {
        if (nums.empty()) return 0;
        if (nums.size() == 1) return nums[0];
        int prev2 = 0, prev1 = nums[0];
        for (int i = 1; i < nums.size(); i++) {
            int cur = max(prev1, prev2 + nums[i]);
            prev2 = prev1;
            prev1 = cur;
        }
        return prev1;
    }
};

int main() {
    Solution s;
    vector<int> nums1 = {1,2,3,1};
    cout << s.rob(nums1) << " (expect 4)" << endl;
    vector<int> nums2 = {2,7,9,3,1};
    cout << s.rob(nums2) << " (expect 12)" << endl;
    vector<int> nums3 = {0};
    cout << s.rob(nums3) << " (expect 0)" << endl;
    return 0;
}
