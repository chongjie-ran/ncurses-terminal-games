// LC154 寻找旋转排序数组中的最小值 II (Hard)
// 算法: 二分查找变体，处理重复值
// 关键: nums[mid] == nums[right] 时 right--，最坏O(N)但平均O(logN)
#include <vector>
#include <iostream>
using namespace std;

int findMin(vector<int>& nums) {
    int left = 0, right = (int)nums.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] > nums[right]) {
            left = mid + 1;
        } else if (nums[mid] < nums[right]) {
            right = mid;
        } else {
            // nums[mid] == nums[right]，无法判断，缩小右边界
            right--;
        }
    }
    return nums[left];
}

int main() {
    vector<int> nums1 = {2, 2, 2, 0, 1};
    cout << "Test1: " << findMin(nums1) << " (expected 0)" << endl;
    vector<int> nums2 = {1, 1, 1};
    cout << "Test2: " << findMin(nums2) << " (expected 1)" << endl;
    vector<int> nums3 = {1, 2, 3, 4, 5};
    cout << "Test3: " << findMin(nums3) << " (expected 1)" << endl;
    vector<int> nums4 = {1};
    cout << "Test4: " << findMin(nums4) << " (expected 1)" << endl;
    vector<int> nums5 = {2, 1};
    cout << "Test5: " << findMin(nums5) << " (expected 1)" << endl;
    return 0;
}
