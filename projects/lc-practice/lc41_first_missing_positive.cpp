// LC41 缺失的第一个正数 (Hard)
// 算法: 原地哈希（桶排序思想）
// 关键: 将数字i放到索引i-1位置，最后扫描找第一个不满足的位置
#include <vector>
#include <iostream>
using namespace std;

int firstMissingPositive(vector<int>& nums) {
    int n = (int)nums.size();
    for (int i = 0; i < n; i++) {
        while (nums[i] > 0 && nums[i] <= n && nums[nums[i] - 1] != nums[i]) {
            swap(nums[i], nums[nums[i] - 1]);
        }
    }
    for (int i = 0; i < n; i++) {
        if (nums[i] != i + 1) return i + 1;
    }
    return n + 1;
}

int main() {
    vector<int> nums1 = {1, 2, 0};
    cout << "Test1: " << firstMissingPositive(nums1) << " (expected 3)" << endl;
    vector<int> nums2 = {3, 4, -1, 1};
    cout << "Test2: " << firstMissingPositive(nums2) << " (expected 2)" << endl;
    vector<int> nums3 = {7, 8, 9, 11, 12};
    cout << "Test3: " << firstMissingPositive(nums3) << " (expected 1)" << endl;
    vector<int> nums4 = {1, 2, 3, 4, 5};
    cout << "Test4: " << firstMissingPositive(nums4) << " (expected 6)" << endl;
    vector<int> nums5 = {2};
    cout << "Test5: " << firstMissingPositive(nums5) << " (expected 1)" << endl;
    return 0;
}
