# LC练习 - 滑动窗口进阶 + 双指针 | 2026-04-18 14:50

## 专题: 滑动窗口进阶

### LC76 最小覆盖子串 (Hard)
**问题**: s中找包含t所有字符的最小子串
**核心**: 变长滑动窗口，双指针扩展收缩

```cpp
string minWindow(string s, string t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;
    int left = 0, right = 0, valid = 0, start = 0, len = INT_MAX;
    while (right < s.size()) {
        char c = s[right++];
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) valid++;
        }
        while (valid == need.size()) {
            if (right - left < len) { start = left; len = right - left; }
            char d = s[left++];
            if (need.count(d)) {
                if (window[d] == need[d]) valid--;
                window[d]--;
            }
        }
    }
    return len == INT_MAX ? "" : s.substr(start, len);
}
```

**验证**: s="ADOBECODEBANC", t="ABC" → "BANC" ✅

---

### LC239 滑动窗口最大值 (Hard)
**问题**: 数组每个窗口最大值
**核心**: 单调递减队列（deque）

```cpp
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq; // 存索引
    vector<int> res;
    for (int i = 0; i < nums.size(); i++) {
        while (dq.size() && nums[dq.back()] <= nums[i]) dq.pop_back();
        dq.push_back(i);
        if (dq.front() <= i - k) dq.pop_front();
        if (i >= k - 1) res.push_back(nums[dq.front()]);
    }
    return res;
}
```

**验证**: [1,3,-1,-3,5,3,6,7], k=3 → [3,3,5,5,6,7] ✅

---

### LC480 滑动窗口中位数 (Hard)
**问题**: 滑动窗口中位数
**核心**: 两个堆（低区堆+高区堆）平衡

```cpp
// 思路：维持两个堆平衡，中位数即 maxHeap.top() 或 两堆顶平均
```

---

### LC3 无重复字符最长子串 (Medium)
**问题**: 无重复字符的最长子串长度
**核心**: 定长滑动窗口 + 哈希set

```cpp
int lengthOfLongestSubstring(string s) {
    unordered_set<char> ws;
    int left = 0, ans = 0;
    for (int right = 0; right < s.size(); right++) {
        while (ws.count(s[right])) ws.erase(s[left++]);
        ws.insert(s[right]);
        ans = max(ans, right - left + 1);
    }
    return ans;
}
```

**验证**: "abcabcbb" → 3 ("abc") ✅
**验证**: "bbbbb" → 1 ✅

---

## 专题: 双指针进阶

### LC15 三数之和 (Medium)
**问题**: 数组中三数之和为0，返回所有不重复三元组
**核心**: 排序 + 双指针，a+b+c=0

```cpp
vector<vector<int>> threeSum(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> res;
    for (int i = 0; i < nums.size(); i++) {
        if (i > 0 && nums[i] == nums[i-1]) continue;
        int target = -nums[i];
        int left = i + 1, right = nums.size() - 1;
        while (left < right) {
            int sum = nums[left] + nums[right];
            if (sum == target) {
                res.push_back({nums[i], nums[left], nums[right]});
                while (left < right && nums[left] == nums[left+1]) left++;
                while (left < right && nums[right] == nums[right-1]) right--;
                left++; right--;
            } else if (sum < target) left++;
            else right--;
        }
    }
    return res;
}
```

**验证**: [-1,0,1,2,-1,-4] → [[-1,-1,2],[-1,0,1]] ✅

---

### LC18 四数之和 (Medium)
**问题**: 四数之和为目标值
**核心**: 双指针两层遍历，剪枝去重

```cpp
vector<vector<int>> fourSum(vector<int>& nums, int target) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> res;
    int n = nums.size();
    for (int a = 0; a < n; a++) {
        if (a > 0 && nums[a] == nums[a-1]) continue;
        for (int b = a+1; b < n; b++) {
            if (b > a+1 && nums[b] == nums[b-1]) continue;
            long newTarget = (long)target - nums[a] - nums[b];
            int left = b+1, right = n-1;
            while (left < right) {
                int sum = nums[left] + nums[right];
                if (sum == newTarget) {
                    res.push_back({nums[a], nums[b], nums[left], nums[right]});
                    while (left < right && nums[left] == nums[left+1]) left++;
                    while (left < right && nums[right] == nums[right-1]) right--;
                    left++; right--;
                } else if (sum < newTarget) left++;
                else right--;
            }
        }
    }
    return res;
}
```

---

### LC11 盛最多水的容器 (Medium)
**问题**: 两指针从两端向中间，收窄时更新最大面积
**核心**: 木桶原理：宽固定，高由短边决定

```cpp
int maxArea(vector<int>& height) {
    int left = 0, right = height.size() - 1;
    int ans = 0;
    while (left < right) {
        int area = (right - left) * min(height[left], height[right]);
        ans = max(ans, area);
        if (height[left] < height[right]) left++;
        else right--;
    }
    return ans;
}
```

**验证**: [1,8,6,2,5,4,8,3,7] → 49 ✅

---

## 验证结果

| 题目 | 难度 | 专题 | 验证 |
|------|------|------|------|
| LC76 最小覆盖子串 | Hard | 滑动窗口 | ✅ |
| LC239 滑动窗口最大值 | Hard | 单调队列 | ✅ |
| LC3 无重复字符最长子串 | Medium | 滑动窗口 | ✅ |
| LC15 三数之和 | Medium | 双指针 | ✅ |
| LC18 四数之和 | Medium | 双指针 | ✅ |
| LC11 盛最多水 | Medium | 双指针 | ✅ |

---

*悟通LC练习 | 2026-04-18 14:50 CST*
