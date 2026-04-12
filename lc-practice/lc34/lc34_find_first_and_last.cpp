// LC34. Find First and Last Position - Medium
#include <iostream>
#include <vector>
using namespace std;

vector<int> searchRange(vector<int>& nums, int target) {
    vector<int> res(2, -1);
    if (nums.empty()) return res;
    int left = 0, right = nums.size() - 1;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid] < target) left = mid + 1;
        else right = mid;
    }
    if (nums[left] == target) res[0] = left;
    left = 0; right = nums.size() - 1;
    while (left < right) {
        int mid = left + (right - left + 1) / 2;
        if (nums[mid] > target) right = mid - 1;
        else left = mid;
    }
    if (nums[right] == target) res[1] = right;
    return res;
}

int main() {
    vector<int> nums = {5,7,7,7,7,8,8,10};
    auto r = searchRange(nums, 7);
    cout << r[0] << " " << r[1] << endl;  // 1 4
    return 0;
}
