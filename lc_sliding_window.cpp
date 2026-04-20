#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
using namespace std;

// ============ LC239 滑动窗口最大值 ============
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq; // 存索引
    vector<int> res;
    for (int i = 0; i < (int)nums.size(); i++) {
        if (!dq.empty() && dq.front() <= i - k) dq.pop_front();
        while (!dq.empty() && nums[dq.back()] < nums[i]) dq.pop_back();
        dq.push_back(i);
        if (i >= k - 1) res.push_back(nums[dq.front()]);
    }
    return res;
}

// ============ LC42 接雨水 ============
int trap(vector<int>& h) {
    int l = 0, r = (int)h.size()-1, lm = 0, rm = 0, water = 0;
    while (l < r) {
        if (h[l] <= h[r]) {
            lm = max(lm, h[l]);
            water += lm - h[l++];
        } else {
            rm = max(rm, h[r]);
            water += rm - h[r--];
        }
    }
    return water;
}

// ============ 滑动窗口最小值（单调递增队列） ============
vector<int> minSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq; // 存索引
    vector<int> res;
    for (int i = 0; i < (int)nums.size(); i++) {
        if (!dq.empty() && dq.front() <= i - k) dq.pop_front();
        while (!dq.empty() && nums[dq.back()] > nums[i]) dq.pop_back();
        dq.push_back(i);
        if (i >= k - 1) res.push_back(nums[dq.front()]);
    }
    return res;
}

// ============ 测试 ============
int main() {
    // LC239 测试
    vector<int> nums1 = {1,3,-1,-3,5,3,6,7};
    vector<int> res1 = maxSlidingWindow(nums1, 3);
    cout << "LC239: ";
    for (int v : res1) cout << v << " "; // 期望: 3 3 5 5 6 7
    cout << endl;

    // LC42 测试
    vector<int> h = {0,1,0,2,1,0,1,3,2,1,2,1};
    cout << "LC42: " << trap(h) << " (期望6)" << endl;

    // 滑动窗口最小值测试
    vector<int> res2 = minSlidingWindow(nums1, 3);
    cout << "MinWindow: ";
    for (int v : res2) cout << v << " "; // 期望: -1 -3 -3 -3 3 3
    cout << endl;

    return 0;
}
