# LeetCode 贪心算法 - 区间问题综合专题

> 悟空 LC 练习 | 2026-04-19
> 专题：区间贪心（区间调度、区间覆盖、区间合并、射箭问题、会议室问题）

---

## 一、通用模板：区间排序Comparator

**几乎所有区间问题第一步都是排序**，排序策略决定解法：

```cpp
// 按右端点排序（区间调度、射箭问题）
sort(intervals.begin(), intervals.end(), [](auto& a, auto& b) {
    return a[1] < b[1];
});

// 按左端点排序（合并区间、区间交集）
sort(intervals.begin(), intervals.end(), [](auto& a, auto& b) {
    return a[0] < b[0];
});

// 按左端点降序，右端点升序（部分覆盖问题）
sort(intervals.begin(), intervals.end(), [](auto& a, auto& b) {
    if (a[0] != b[0]) return a[0] > b[0];
    return a[1] < b[1];
});
```

---

## 二、区间调度问题（最多不相交区间）

### LC435. Non-overlapping Intervals

**链接**: https://leetcode.com/problems/non-overlapping-intervals/
**难度**: 🟡 中等
**标签**: 区间调度、贪心

#### 核心思路

**贪心策略**：按右端点排序，选择结束最早的区间，贪心最大化剩余空间。

1. 按区间右端点升序排列
2. 依次选择不重叠的区间（当前区间左端点 > 上一个选中区间右端点）
3. 需要移除的区间数 = 总区间数 - 选择的最大不相交区间数

**关键洞察**：结束越早的区间越不可能影响后续区间的选择 —— 这是区间调度问题的核心贪心选择。

#### 关键代码

```cpp
int eraseOverlapIntervals(vector<vector<int>>& intervals) {
    if (intervals.empty()) return 0;

    // 【核心】按右端点排序
    sort(intervals.begin(), intervals.end(),
         [](const vector<int>& a, const vector<int>& b) {
             return a[1] < b[1];
         });

    int count = 1;                           // 第一个区间必选
    int end = intervals[0][1];                // 当前选中区间右端点

    for (int i = 1; i < intervals.size(); i++) {
        int start = intervals[i][0];

        if (start >= end) {                   // 不重叠，选择它
            count++;
            end = intervals[i][1];
        }
        // 重叠则跳过（不计数，不更新end）
    }

    return intervals.size() - count;           // 需要移除的区间数
}
```

#### 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n log n) — 排序主导 |
| 空间 | O(1) — 原地排序，O(log n) 若sort用递归栈 |

#### 变形题

| 题号 | 问题 | 贪心策略 |
|------|------|----------|
| LC646 | Maximum Length of Pair Chain | 同理按右端点，选择更多链 |
| LC452 | Min Arrows to Burst Balloons | 按右端点，选择重叠区间 |

---

## 三、射箭问题

### LC452. Minimum Number of Arrows to Burst Balloons

**链接**: https://leetcode.com/problems/minimum-number-of-arrows-to-burst-balloons/
**难度**: 🟡 中等
**标签**: 射箭问题、区间重叠、贪心

#### 核心思路

**贪心策略**：按右端点排序，一箭射在重叠区间的最右端。

1. 按右端点排序所有气球区间
2. 第一支箭射在第一个气球的右端点
3. 如果后续气球的左端点 > 当前箭的位置，需要新射一支箭

**关键洞察**：箭射在每个重叠区间最右端，可以同时引爆所有左端点 ≤ 该位置的区间。

#### 关键代码

```cpp
int findMinArrowShots(vector<vector<int>>& points) {
    if (points.empty()) return 0;

    // 【核心】按右端点排序
    sort(points.begin(), points.end(),
         [](const vector<int>& a, const vector<int>& b) {
             return a[1] < b[1];
         });

    int arrows = 1;                           // 第一支箭
    int arrowPos = points[0][1];              // 射在第一个区间右端点

    for (int i = 1; i < points.size(); i++) {
        int start = points[i][0];
        int end = points[i][1];

        // 当前气球左端点 > 箭的位置，无法引爆，需新射一支
        if (start > arrowPos) {
            arrows++;
            arrowPos = end;                   // 新箭射在新气球右端点
        }
        // 否则当前气球被箭引爆，无需额外操作
    }

    return arrows;
}
```

#### 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n log n) — 排序 |
| 空间 | O(1) |

#### 关键区别 vs LC435

| 对比项 | LC435 | LC452 |
|--------|-------|-------|
| 重叠时 | 跳过/移除 | 一箭引爆 |
| 贪心选择 | 选择不重叠 | 箭射在重叠区最右 |
| 结果 | 需要移除多少 | 需要多少箭 |

---

## 四、区间合并问题

### LC56. Merge Intervals

**链接**: https://leetcode.com/problems/merge-overlapping-intervals/
**难度**: 🟡 中等
**标签**: 区间合并、排序

#### 核心思路

1. 按左端点升序排序
2. 依次合并：如果当前区间左端点 ≤ 上个合并区间右端点 → 合并（取max右端点）
3. 否则：新开一个合并区间

**变形**：如果是 `insertInterval`（LC57），先二分找插入位置，再分三段处理。

#### 关键代码

```cpp
vector<vector<int>> merge(vector<vector<int>>& intervals) {
    if (intervals.size() <= 1) return intervals;

    // 【核心】按左端点排序
    sort(intervals.begin(), intervals.end(),
         [](const vector<int>& a, const vector<int>& b) {
             return a[0] < b[0];
         });

    vector<vector<int>> result;
    int currL = intervals[0][0];
    int currR = intervals[0][1];

    for (int i = 1; i < intervals.size(); i++) {
        int L = intervals[i][0];
        int R = intervals[i][1];

        if (L <= currR) {
            // 【核心合并】左端点在当前区间内，合并
            currR = max(currR, R);
        } else {
            // 不重叠，保存当前区间，开新区间
            result.push_back({currL, currR});
            currL = L;
            currR = R;
        }
    }
    result.push_back({currL, currR});         // 别漏最后一个

    return result;
}
```

#### 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n log n) — 排序 |
| 空间 | O(n) — 结果存储 |

---

## 五、区间交集问题

### LC986. Interval List Intersections

**链接**: https://leetcode.com/problems/interval-list-intersections/
**难度**: 🟡 中等
**标签**: 双指针、区间交集

#### 核心思路

**双指针贪心**：两个有序区间列表，每次比较两个指针区间：

- 交集 = `[max(A[i][0], B[j][0]), min(A[i][1], B[j][1])]`（仅当 max ≤ min）
- 移动策略：右端点较小的区间指针前移（因为它的区间已经不可能和其他区间有更大交集）

```text
A: [0,2] [5,10]
B: [1,5] [8,12]
      ↑   ↑
  指针i  指针j
交集: [1,2] → 右端点2 < 5，i++
交集: [5,5] → 右端点相等，都++
```

#### 关键代码

```cpp
vector<vector<int>> intervalIntersection(
        vector<vector<int>>& A,
        vector<vector<int>>& B) {

    vector<vector<int>> result;
    int i = 0, j = 0;

    while (i < A.size() && j < B.size()) {
        int aL = A[i][0], aR = A[i][1];
        int bL = B[j][0], bR = B[j][1];

        // 计算交集
        int L = max(aL, bL);
        int R = min(aR, bR);
        if (L <= R) {                         // 有交集
            result.push_back({L, R});
        }

        // 【贪心移动】右端点小的指针前移
        if (aR < bR) {
            i++;
        } else {
            j++;
        }
    }

    return result;
}
```

#### 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n + m) — 每个指针最多走完一个列表 |
| 空间 | O(1) 额外 |

---

## 六、会议室问题（最少会议室）

### LC253. Meeting Rooms II

**链接**: https://leetcode.com/problems/meeting-rooms-ii/
**难度**: 🔴 困难
**标签**: 最小会议室、堆/优先队列

#### 核心思路

**方法1：最小堆（推荐）**

- 按开始时间排序
- 用最小堆维护各会议室的结束时间
- 新会议如果 start ≥ 堆顶（最早结束），复用该会议室（pop再push新结束时间）
- 否则开新会议室（push新结束时间）

**方法2：扫描线（O(n)空间）**

- 把所有时间点标记为 (time, type)，type: +1开始, -1结束
- 排序后线性扫描，当前会议室数 = max(扫描过程中最大值)

#### 关键代码（堆方法）

```cpp
int minMeetingRooms(vector<vector<int>>& intervals) {
    if (intervals.empty()) return 0;

    // 按开始时间排序
    sort(intervals.begin(), intervals.end(),
         [](const vector<int>& a, const vector<int>& b) {
             return a[0] < b[0];
         });

    // 最小堆：存储各会议室的结束时间
    priority_queue<int, vector<int>, greater<int>> minHeap;
    minHeap.push(intervals[0][1]);            // 第一个会议的结束时间

    for (int i = 1; i < intervals.size(); i++) {
        int start = intervals[i][0];
        int end = intervals[i][1];

        if (start >= minHeap.top()) {
            // 【复用】新会议开始时，最早结束的会议室已空
            minHeap.pop();
        }
        // 否则需要新会议室，直接push新结束时间
        minHeap.push(end);
    }

    return minHeap.size();                    // 堆大小即最少会议室数
}
```

#### 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n log n) — 排序 + 堆操作 |
| 空间 | O(n) — 堆最坏存n个会议室 |

---

## 七、区间划分问题（Partition Labels）

### LC763. Partition Labels

**链接**: https://leetcode.com/problems/partition-labels/
**难度**: 🟡 中等
**标签**: 区间划分、贪心

#### 核心思路

**两步贪心**：

1. **第一遍**：记录每个字符最后出现的位置 `last[c]`
2. **第二遍**：遍历字符串，维护当前片段的右边界 `end`
   - 每次扩展 `end = max(end, last[s[i]])`
   - 当 i == end，当前片段划分完成

**本质**：每次划分的是一个"不会在后面再出现"的片段，即该片段的最右边界。

```text
s = "ababcbacadefegde"
last: a=8, b=5, c=4, d=14, e=16, f=11, g=13

片段1: i=0→end=8 → "ababcbacadefegde" 中最远a在位置8 → "ababcbacadefegde" 还没到8继续
... → i=8时 i==end → 片段1长度=9 → "ababcbacadef"
片段2: i=9→end=14 → d在14 → i=14时 → 片段2长度=6 → "fegdeh"
```

#### 关键代码

```cpp
vector<int> partitionLabels(string s) {
    // 【第一步】记录每个字符最后出现的位置
    unordered_map<char, int> last;
    for (int i = 0; i < s.size(); i++) {
        last[s[i]] = i;
    }

    vector<int> result;
    int end = 0;                              // 当前片段最右边界
    int start = 0;                            // 当前片段起始

    for (int i = 0; i < s.size(); i++) {
        end = max(end, last[s[i]]);           // 扩展右边界

        if (i == end) {                       // 【划分点】i到达当前片段最右
            result.push_back(end - start + 1);
            start = i + 1;                     // 下一个片段从下一个字符开始
        }
    }

    return result;
}
```

#### 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n) — 两遍遍历 |
| 空间 | O(1) — 26字母数组，或O(字符集) |

---

## 八、区间插入问题（Insert & Summary）

### LC228. Summary Ranges

**链接**: https://leetcode.com/problems/summary-ranges/
**难度**: 🟢 简单
**标签**: 区间归类、连续序列

#### 核心思路

连续整数归类为区间格式：

```text
[0,1,2,3,6,7,9] → ["0->4", "6->7", "9->9"]
```

遍历时维护连续起点 `start`，当出现断链(i.e., `nums[i] != nums[i-1] + 1`)时输出一个区间。

#### 关键代码

```cpp
vector<string> summaryRanges(vector<int>& nums) {
    vector<string> result;

    for (int i = 0; i < nums.size(); i++) {
        int start = nums[i];

        // 找到连续终点
        while (i + 1 < nums.size() && nums[i + 1] == nums[i] + 1) {
            i++;
        }

        if (start == nums[i]) {
            result.push_back(to_string(start));
        } else {
            result.push_back(to_string(start) + "->" + to_string(nums[i]));
        }
    }

    return result;
}
```

#### 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n) |
| 空间 | O(1) |

---

## 九、综合练习题

### LC57. Insert Interval

**链接**: https://leetcode.com/problems/insert-interval/
**难度**: 🔴 困难
**标签**: 区间插入、合并

#### 核心思路

**三分法**：

1. `intervals[i].end < newInterval.start` → 直接复制左边
2. `intervals[i].start > newInterval.end` → 直接复制右边
3. 否则 → 合并（取min左端点，max右端点）

#### 关键代码

```cpp
vector<vector<int>> insert(
        vector<vector<int>>& intervals,
        vector<int>& newInterval) {

    vector<vector<int>> result;
    int i = 0, n = intervals.size();

    // 【第一段】原区间在新区间左边（无重叠）
    while (i < n && intervals[i][1] < newInterval[0]) {
        result.push_back(intervals[i++]);
    }

    // 【第二段】有重叠的区间合并
    while (i < n && intervals[i][0] <= newInterval[1]) {
        newInterval[0] = min(newInterval[0], intervals[i][0]);
        newInterval[1] = max(newInterval[1], intervals[i][1]);
        i++;
    }
    result.push_back(newInterval);

    // 【第三段】原区间在新区间右边（无重叠）
    while (i < n) {
        result.push_back(intervals[i++]);
    }

    return result;
}
```

---

## 十、区间问题贪心模板总结

| 问题类型 | 排序策略 | 贪心选择 | 关键代码模式 |
|----------|----------|----------|--------------|
| 最多不相交区间 (LC435) | 按右端点 | 选择右端点最早的 | `start >= end` |
| 射箭引爆气球 (LC452) | 按右端点 | 箭射在重叠区最右 | `start > arrowPos` |
| 合并区间 (LC56) | 按左端点 | 左端点≤currR则合并 | `L <= currR` |
| 区间交集 (LC986) | 原序双指针 | 右端点小者前移 | `aR < bR ? i++ : j++` |
| 最少会议室 (LC253) | 按开始时间 | 堆顶复用 | `start >= minHeap.top()` |
| 划分字母 (LC763) | 无需排序 | i==end划分 | `i == end` |
| 插入区间 (LC57) | 无需排序 | 三段分类合并 | `min/max` |

**通用套路**：
1. **排序** — 几乎所有区间问题第一步
2. **一次遍历** — 维护边界变量
3. **贪心选择** — 选最早结束 / 选重叠区最右 / 选右端点最小

---

*悟空 LC 练习 | 2026-04-19 | 贪心算法区间问题专题*
