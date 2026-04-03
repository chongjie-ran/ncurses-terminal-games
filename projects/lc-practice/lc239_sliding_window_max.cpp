// LC239 Sliding Window Maximum
// Hard - Deque (Monotonic Queue)
// 核心: 维护一个递减 deque, 存索引
// deque[0] 永远是当前窗口最大值的索引

#include <iostream>
#include <vector>
#include <deque>
using namespace std;

vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    vector<int> res;
    deque<int> dq; // 存索引,保证 nums[dq[0]] 是当前窗口最大值

    for (int i = 0; i < nums.size(); i++) {
        // 1. 把超出窗口的索引从 front 去掉
        if (!dq.empty() && dq.front() <= i - k) {
            dq.pop_front();
        }

        // 2. 保持 deque 递减: 去掉后面所有比当前元素小的
        while (!dq.empty() && nums[dq.back()] <= nums[i]) {
            dq.pop_back();
        }

        // 3. 把当前索引加入
        dq.push_back(i);

        // 4. 窗口形成后记录答案
        if (i >= k - 1) {
            res.push_back(nums[dq.front()]);
        }
    }
    return res;
}

int main() {
    // Test 1
    vector<int> nums1 = {1,3,-1,-3,5,3,6,7};
    vector<int> res1 = maxSlidingWindow(nums1, 3);
    cout << "Test1: ";
    for (int x : res1) cout << x << " ";
    cout << endl;
    // Expected: 3 3 5 5 6 7

    // Test 2
    vector<int> nums2 = {1};
    vector<int> res2 = maxSlidingWindow(nums2, 1);
    cout << "Test2: ";
    for (int x : res2) cout << x << " ";
    cout << endl;
    // Expected: 1

    // Test 3: k=2
    vector<int> nums3 = {9,11};
    vector<int> res3 = maxSlidingWindow(nums3, 2);
    cout << "Test3: ";
    for (int x : res3) cout << x << " ";
    cout << endl;
    // Expected: 11

    // Test 4: decreasing
    vector<int> nums4 = {9,8,7,6,5,4,3,2,1};
    vector<int> res4 = maxSlidingWindow(nums4, 3);
    cout << "Test4: ";
    for (int x : res4) cout << x << " ";
    cout << endl;
    // Expected: 9 8 7 6 5 4 3 2

    return 0;
}
