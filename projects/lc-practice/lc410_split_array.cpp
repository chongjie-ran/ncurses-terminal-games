#include <iostream>
#include <vector>
using namespace std;

// LC410 Split Array Largest Sum
// Hard: 将数组分成m个子数组，使最大子数组和最小
// 思路：二分搜索答案 + 贪心验证
// 时间: O(N*log(sum)), 空间: O(1)

class Solution {
public:
    int splitArray(vector<int>& nums, int m) {
        long lo = 0, hi = 0;
        for (int x : nums) {
            lo = max(lo, (long)x);
            hi += x;
        }

        while (lo < hi) {
            long mid = (lo + hi) / 2;
            if (canSplit(nums, m, mid)) {
                hi = mid;
            } else {
                lo = mid + 1;
            }
        }
        return (int)lo;
    }

    bool canSplit(vector<int>& nums, int m, long maxSum) {
        int cnt = 1;
        long sum = 0;
        for (int x : nums) {
            if (sum + x <= maxSum) {
                sum += x;
            } else {
                cnt++;
                sum = x;
                if (cnt > m) return false;
            }
        }
        return true;
    }
};

int main() {
    Solution s;
    vector<int> nums1 = {7,2,5,10,8};
    cout << "Test1 (18): " << s.splitArray(nums1, 2) << endl;

    vector<int> nums2 = {1,2,3,4,5};
    cout << "Test2 (9): " << s.splitArray(nums2, 2) << endl;

    vector<int> nums3 = {1,4,4};
    cout << "Test3 (4): " << s.splitArray(nums3, 3) << endl;

    return 0;
}
