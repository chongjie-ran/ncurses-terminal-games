// LC704. Binary Search - Easy
#include <iostream>
#include <vector>
using namespace std;

int search(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

int main() {
    vector<int> nums = {-1,0,3,5,9,12};
    cout << search(nums, 9) << endl;   // 4
    cout << search(nums, 2) << endl;   // -1
    return 0;
}
