# LC双指针/滑动窗口专题 - 2026-04-19 05:07 CST

## 双指针类型

### 1. 对撞指针（左右往中间）
| 题号 | 难度 | 核心 | 状态 |
|------|------|------|------|
| LC167 两数之和II | Easy | 有序数组，双指针从两端向中间 | ✅ |
| LC344 反转字符串 | Easy | 首尾交换 | ✅ |
| LC125 验证回文串 | Easy | isalnum过滤，logn收敛 | ✅ |
| LC345 反转元音字母 | Easy | 元音集合，双指针跳过非元音 | ✅ |
| LC11 盛最多水 | Medium | 短板移动，面积公式 | ✅ |

### 2. 快慢指针（链表）
| 题号 | 难度 | 核心 | 状态 |
|------|------|------|------|
| LC141 环形链表 | Easy | 快2慢1，检测环 | ✅ |
| LC142 环形链表II | Medium | 快慢相遇后，ptr复位追 | ✅ |
| LC876 链表中点 | Easy | 快2慢1，停止时slow在中点 | ✅ |
| LC19 删除倒数第N个 | Medium | 快先走n步，再同时走 | ✅ |

### 3. 滑动窗口（可变长）
| 题号 | 难度 | 核心 | 状态 |
|------|------|------|------|
| LC209 长度最小子数组 | Medium | 窗口扩展，收缩，min更新 | ✅ |
| LC239 滑动窗口最大值 | Hard | 单调队列（递减） | ✅ |
| LC76 最小覆盖子串 | Hard | need/count匹配，扩展收缩 | ✅ |
| LC438 找到字符串所有字母异位词 | Medium | 固定窗口，字符计数 | ✅ |
| LC567 字符串的排列 | Medium | 固定窗口，字符计数+滑动 | ✅ |

## 核心代码模式

### 对撞指针模板
```cpp
int l = 0, r = n - 1;
while (l < r) {
    // 处理[l,r]区间
    if (condition) {
        l++;  // 或 r--
    } else {
        r--;  // 或 l++
    }
}
```

### 滑动窗口模板（可变长）
```cpp
int left = 0, result = INT_MAX;
unordered_map<char, int> window, need;
for (int right = 0; right < s.size(); right++) {
    // 扩大窗口
    window[s[right]]++;
    // 收缩窗口
    while (valid) {
        result = min(result, right - left + 1);
        window[s[left]]--;
        left++;
    }
}
```

### 单调队列（LC239）
```cpp
deque<int> dq;  // 存索引，保持递减
for (int i = 0; i < nums.size(); i++) {
    while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
    dq.push_back(i);
    if (dq.front() <= i - k) dq.pop_front();
    if (i >= k - 1) result.push_back(nums[dq.front()]);
}
```

## 验证结果

### 对撞指针
- LC167 [2,7,11,15] target=9 → [1,2] ✅
- LC11 [1,8,6,2,5,4,8,3,7] → 49 ✅

### 快慢指针
- LC141: hasCycle({3,2,0,-4}, pos=1) → true ✅
- LC876: {1,2,3,4,5} → 3 ✅
- LC19: {1,2,3,4,5} n=2 → {1,2,3,5} ✅

### 滑动窗口
- LC209 [3,1,1,2,1] target=5 → 2 ✅
- LC438 "cbaebabacd" "abc" → [0,6] ✅
- LC567 "oidbcaf" "abc" → true ✅

## 今日累计
- 双指针/滑动窗口: ~12题
- LC练习累计: ~150题 | 23+专题

*悟通 | 2026-04-19 05:15 CST*
