// LC239 Sliding Window Maximum - Hard
// 单调递减双端队列 O(n)
// deque存储索引，队首永远是最大值
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        deque<int> dq; // 存储索引，值单调递减
        vector<int> ans;

        for (int i = 0; i < nums.size(); i++) {
            // 移除队尾小于当前元素的（永远不可能成为最大值）
            while (!dq.empty() && nums[dq.back()] <= nums[i])
                dq.pop_back();
            dq.push_back(i);

            // 移除队首超出窗口范围的
            if (dq.front() <= i - k)
                dq.pop_front();

            // 窗口形成后记录答案
            if (i >= k - 1)
                ans.push_back(nums[dq.front()]);
        }
        return ans;
    }
};
