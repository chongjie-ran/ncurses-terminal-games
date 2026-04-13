// LC33: Search in Rotated Sorted Array
// 关键洞察：旋转点将数组分成两段升序，二分查找时总有一半是有序的
// 时间O(log n)，空间O(1)
#include <iostream>
#include <vector>
using namespace std;

int search(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return mid;
        // 左半边有序
        if (nums[left] <= nums[mid]) {
            if (nums[left] <= target && target < nums[mid])
                right = mid - 1;
            else
                left = mid + 1;
        } else { // 右半边有序
            if (nums[mid] < target && target <= nums[right])
                left = mid + 1;
            else
                right = mid - 1;
        }
    }
    return -1;
}

// 测试
int main() {
    vector<pair<vector<int>, int>> tests = {
        {{4,5,6,7,0,1,2}, 0},    // 4
        {{4,5,6,7,0,1,2}, 3},    // -1
        {{1}, 0},                  // -1
        {{1}, 1},                  // 0
        {{3,1}, 1},               // 1
    };
    for (auto& [nums, target] : tests) {
        cout << target << " in " << "→ " << search(nums, target) << endl;
    }
    return 0;
}
