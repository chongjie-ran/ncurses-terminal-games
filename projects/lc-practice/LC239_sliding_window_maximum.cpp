
// LC239 滑动窗口最大值 - 单调队列
class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        deque<int> dq; // 存索引
        vector<int> res;
        for (int i = 0; i < nums.size(); i++) {
            // 移除超出窗口的索引
            if (!dq.empty() && dq.front() <= i - k) dq.pop_front();
            // 保持单调递减：新元素比队尾大就弹出
            while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
            dq.push_back(i);
            // 记录答案
            if (i >= k - 1) res.push_back(nums[dq.front()]);
        }
        return res;
    }
};
