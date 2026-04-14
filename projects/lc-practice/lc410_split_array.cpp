// LC410 Split Array - 最大子数组和的最小值
// 关键词: 二分搜索
// 难度: Hard

#include <iostream>
#include <vector>
using namespace std;

// 二分搜索: 找最大子数组和的最小值
// m个子子数组，每个和 <= mid
bool canSplit(const vector<int>& nums, int m, int mid) {
    int count = 1;  // 至少1个子数组
    int sum = 0;
    for (int num : nums) {
        sum += num;
        if (sum > mid) {
            count++;
            sum = num;
            if (count > m) return false;
        }
    }
    return true;
}

int splitArray(vector<int>& nums, int m) {
    // 二分搜索范围: [max(nums), sum(nums)]
    int left = 0, right = 0;
    for (int num : nums) {
        left = max(left, num);
        right += num;
    }
    
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (canSplit(nums, m, mid)) {
            right = mid;  // 可以分割，尝试更小的最大值
        } else {
            left = mid + 1;  // 不行，需要更大的最大值
        }
    }
    return left;
}

// 测试
int main() {
    // 测试1: nums = [7,2,5,10,8], m = 2
    // 划分: [7,2,5] 和 [10,8] → 最大和=18
    // 划分: [7,2,5,10] 和 [8] → 最大和=24
    // 划分: [7,2] 和 [5,10,8] → 最大和=23
    // 最优: [7,2,5] 和 [10,8] → 18
    vector<int> nums1 = {7, 2, 5, 10, 8};
    cout << splitArray(nums1, 2) << endl;  // 18
    
    // 测试2: nums = [1,2,3,4,5], m = 2
    // 最优: [1,2,3] 和 [4,5] → 最大和=9
    vector<int> nums2 = {1, 2, 3, 4, 5};
    cout << splitArray(nums2, 2) << endl;  // 9
    
    // 测试3: nums = [1,4,4], m = 3
    // 划分: [1],[4],[4] → 最大和=4
    vector<int> nums3 = {1, 4, 4};
    cout << splitArray(nums3, 3) << endl;  // 4
    
    return 0;
}
