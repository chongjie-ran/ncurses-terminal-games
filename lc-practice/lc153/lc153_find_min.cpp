// LC153. Find Minimum in Rotated Sorted Array - Medium
#include <iostream>
#include <vector>
using namespace std;

int findMin(vector<int>& nums) {
    int left = 0, right = nums.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] > nums[right]) left = mid + 1;
        else right = mid;
    }
    return nums[left];
}

int main() {
    vector<int> nums = {3,4,5,1,2};
    cout << findMin(nums) << endl;  // 1
    return 0;
}
