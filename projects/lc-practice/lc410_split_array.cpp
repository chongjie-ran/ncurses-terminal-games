// LC410 Split Array Largest Sum - Hard - Binary Search
// 找最小化的最大子数组和，二分搜索答案
#include <iostream>
#include <vector>
using namespace std;

bool canSplit(vector<int>& nums, int m, long long maxSum) {
    int count = 1;
    long long cur = 0;
    for(int x: nums) {
        if(cur + x <= maxSum) cur += x;
        else { count++; cur = x; if(count > m) return false; }
    }
    return true;
}

int splitArray(vector<int>& nums, int m) {
    long long lo = 0, hi = 0;
    for(int x: nums) { lo = max(lo, (long long)x); hi += x; }
    while(lo < hi) {
        long long mid = (lo + hi) / 2;
        if(canSplit(nums, m, mid)) hi = mid;
        else lo = mid + 1;
    }
    return (int)lo;
}

int main() {
    vector<int> nums = {7,2,5,10,8};
    cout << splitArray(nums, 2) << endl; // 18
    return 0;
}
