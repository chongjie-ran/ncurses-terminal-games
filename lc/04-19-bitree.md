# LeetCode 树状数组(BIT)与线段树综合专题

> 日期: 2026-04-19 | 分类: 数据结构高级 | 难度: Hard为主
> 涵盖: BIT单点更新/区间和 | 线段树点更新/范围查询 | 动态前缀和/逆序对 | 区间合并

---

## 前置知识：BIT 与线段树对比

| 特性 | 树状数组(BIT) | 线段树 |
|------|-------------|--------|
| 区间查询 | O(log N) | O(log N) |
| 单点更新 | O(log N) | O(log N) |
| 区间修改 | 需换算(差分BIT) | O(log N) |
| 代码复杂度 | 低 | 中 |
| 适用范围 | 前缀和为主 | 区间合并/染色/最值 |

---

## 题目一：LC 307. 区域和检索 - 数组可修改

**链接**: https://leetcode.cn/problems/range-sum-query-mutable/
**难度**: ⭐⭐⭐ 中等
**分类**: BIT基础 / 线段树基础

### 1.1 问题描述
设计一个数据结构，支持：
- `update(index, val)`: 将 `nums[i]` 更新为 `val`
- `sumRange(left, right)`: 查询区间 `[left, right]` 的元素和

### 1.2 核心思路

**方法A - 树状数组(BIT)**：
- `update`: 单点增加 `delta`，向后传播
- `query(right)`: 前缀和 `Σ[0..right]`
- 区间和 = `query(right) - query(left-1)`

**方法B - 线段树**：
- 节点存储区间和
- `update`: 从叶子向上更新
- `query`: 递归区间合并

### 1.3 关键代码模板

```python
# ============ 方法A: 树状数组 ============
class BIT:
    def __init__(self, nums):
        self.n = len(nums)
        self.tree = [0] * (self.n + 1)
        for i, v in enumerate(nums):
            self._add(i + 1, v)

    def _add(self, idx, delta):
        while idx <= self.n:
            self.tree[idx] += delta
            idx += idx & -idx

    def _sum(self, idx):
        res = 0
        while idx > 0:
            res += self.tree[idx]
            idx -= idx & -idx
        return res

    def update(self, index, val):
        # val是新的目标值，不是增量delta
        delta = val - (self._sum(index) - self._sum(index - 1))
        self._add(index + 1, delta)

    def query(self, left, right):
        return self._sum(right + 1) - self._sum(left)

class NumArray:
    def __init__(self, nums):
        self.bit = BIT(nums)

    def update(self, index, val):
        self.bit.update(index, val)

    def sumRange(self, left, right):
        return self.bit.query(left, right)

# ============ 方法B: 线段树 ============
class SegTree:
    def __init__(self, nums):
        self.n = len(nums)
        self.tree = [0] * (4 * self.n)
        self._build(nums, 0, 0, self.n - 1)

    def _build(self, nums, node, l, r):
        if l == r:
            self.tree[node] = nums[l]
            return
        mid = (l + r) // 2
        self._build(nums, node * 2 + 1, l, mid)
        self._build(nums, node * 2 + 2, mid + 1, r)
        self.tree[node] = self.tree[node * 2 + 1] + self.tree[node * 2 + 2]

    def _add(self, node, l, r, idx, delta):
        if l == r:
            self.tree[node] += delta
            return
        mid = (l + r) // 2
        if idx <= mid:
            self._add(node * 2 + 1, l, mid, idx, delta)
        else:
            self._add(node * 2 + 2, mid + 1, r, idx, delta)
        self.tree[node] = self.tree[node * 2 + 1] + self.tree[node * 2 + 2]

    def _query(self, node, l, r, ql, qr):
        if ql <= l and r <= qr:
            return self.tree[node]
        mid = (l + r) // 2
        res = 0
        if ql <= mid:
            res += self._query(node * 2 + 1, l, mid, ql, qr)
        if qr > mid:
            res += self._query(node * 2 + 2, mid + 1, r, ql, qr)
        return res

    def update(self, index, val):
        delta = val - self._query(0, 0, self.n - 1, index, index)
        self._add(0, 0, self.n - 1, index, delta)

    def query(self, left, right):
        return self._query(0, 0, self.n - 1, left, right)

class NumArraySeg:
    def __init__(self, nums):
        self.seg = SegTree(nums)

    def update(self, index, val):
        self.seg.update(index, val)

    def sumRange(self, left, right):
        return self.seg.query(left, right)
```

### 1.4 复杂度分析
- 时间: `update` O(log N) + `query` O(log N)
- 空间: BIT O(N) / 线段树 O(4N)

---

## 题目二：LC 315. 计算右侧小于当前元素的个数

**链接**: https://leetcode.cn/problems/count-of-smaller-numbers-after-self/
**难度**: ⭐⭐⭐⭐ 困难
**分类**: BIT + 离散化 / 逆序对变形

### 2.1 问题描述
给定整数数组 `nums`，返回数组 `result`，其中 `result[i]` 是 `nums[i]` 右侧小于 `nums[i]` 的元素数目。

### 2.2 核心思路

**从右向左遍历 + BIT计数**：
1. 对所有元素**离散化**（排序去重 + 映射到1..N）
2. 从右往左扫描，对每个元素：
   - 查询当前BIT中比它小的元素个数（即 `query(val-1)`）
   - 将该元素加入BIT（`add(val, 1)`）
3. 结果就是「已出现元素中比当前值小的个数」

**为什么从右向左？** 确保只统计「右侧」元素。

### 2.3 关键代码模板

```python
from sortedcontainers import SortedList

class BIT:
    def __init__(self, n):
        self.n = n
        self.tree = [0] * (n + 1)

    def add(self, idx, delta):
        while idx <= self.n:
            self.tree[idx] += delta
            idx += idx & -idx

    def sum(self, idx):
        res = 0
        while idx > 0:
            res += self.tree[idx]
            idx -= idx & -idx
        return res

class Solution:
    def countSmaller(self, nums):
        if not nums:
            return []
        # ---- 离散化 ----
        sorted_vals = sorted(sorted(set(nums)))
        self.bit = BIT(len(sorted_vals))
        val_to_idx = {v: i + 1 for i, v in enumerate(sorted_vals)}

        result = [0] * len(nums)
        # ---- 从右往左扫描 ----
        for i in range(len(nums) - 1, -1, -1):
            idx = val_to_idx[nums[i]]
            # 查询当前索引之前（比它小）的元素个数
            result[i] = self.bit.sum(idx - 1)
            # 将当前元素加入BIT
            self.bit.add(idx, 1)
        return result

# ---- 附: 二分查找(TreeSet) 方法 ----
class Solution2:
    def countSmaller(self, nums):
        if not nums:
            return []
        sl = SortedList()
        result = []
        for v in reversed(nums):
            pos = sl.bisect_left(v)
            result.append(pos)
            sl.add(v)
        return result[::-1]
```

### 2.4 复杂度分析
- 时间: O(N log N) — 离散化O(N log N) + BIT操作O(N log N)
- 空间: O(N) — BIT树 + 离散化数组

---

## 题目三：LC 327. 区间和的个数

**链接**: https://leetcode.cn/problems/count-of-range-sum/
**难度**: ⭐⭐⭐⭐ 困难
**分类**: BIT + 前缀和 + 离散化

### 3.1 问题描述
给定整数数组 `nums` 和整数 `lower`、`upper`，返回在范围 `[lower, upper]` 内的区间和的个数。

### 3.2 核心思路

**前缀和 + BIT统计**：
1. 计算前缀和数组 `presum[0]=0, presum[i]=Σ[0..i-1]`
2. 区间和 `[j..i-1]` = `presum[i] - presum[j]`
3. 问题转化为：对每个 `i`，统计有多少个 `j < i` 满足 `lower ≤ presum[i] - presum[j] ≤ upper`
4. 即 `presum[i] - upper ≤ presum[j] ≤ presum[i] - lower`
5. 用**离散化**把所有前缀和及边界值映射到BIT，对每个i：
   - `add(val_to_idx(presum[j]), 1)` 记录已访问的前缀和
   - `count = query(idx_right) - query(idx_left - 1)` 计数满足条件的个数

### 3.3 关键代码模板

```python
class BIT:
    def __init__(self, n):
        self.n = n
        self.tree = [0] * (n + 2)  # 多开2位避免边界

    def add(self, idx, delta):
        while idx <= self.n:
            self.tree[idx] += delta
            idx += idx & -idx

    def sum(self, idx):
        res = 0
        while idx > 0:
            res += self.tree[idx]
            idx -= idx & -idx
        return res

class Solution:
    def countRangeSum(self, nums, lower, upper):
        if not nums:
            return 0
        # 前缀和
        presum = [0]
        for v in nums:
            presum.append(presum[-1] + v)

        # 离散化：所有前缀和 + 上下界偏移
        all_vals = set(presum)
        # 加入边界偏移值（upper/lower可能为负）
        for v in list(presum):
            all_vals.add(v - lower)
            all_vals.add(v - upper)
        sorted_vals = sorted(all_vals)
        val_to_idx = {v: i + 1 for i, v in enumerate(sorted_vals)}

        bit = BIT(len(sorted_vals) + 2)
        res = 0
        # 从左往右遍历前缀和
        for p in presum:
            # 查询 [p-upper, p-lower] 区间内有多少已记录的前缀和
            left_idx = val_to_idx[p - upper]
            right_idx = val_to_idx[p - lower]
            # sum(right) - sum(left-1) 即满足条件的个数
            res += bit.sum(right_idx) - bit.sum(left_idx - 1)
            # 把自己加入BIT（下次作为j时被统计）
            bit.add(val_to_idx[p], 1)
        return res
```

### 3.4 复杂度分析
- 时间: O(N log N)
- 空间: O(N)

---

## 题目四：LC 493. 翻转对

**链接**: https://leetcode.cn/problems/reverse-pairs/
**难度**: ⭐⭐⭐⭐ 困难
**分类**: BIT / 归并排序

### 4.1 问题描述
给定数组 `nums`，统计满足 `i < j` 且 `nums[i] > 2 * nums[j]` 的翻转对数量。

### 4.2 核心思路

**方法A - BIT（与LC315类似）**：
- 从右往左扫描
- 对 `nums[j]`，需要找所有满足 `nums[i] > 2 * nums[j]` 的 `i>j`
- 条件等价于：`nums[i] >= 2 * nums[j] + 1`
- 将 `2*v + 1` 离散化后，用BIT统计

**方法B - 归并排序（推荐，O(N log N) 且更稳定）**：
- 在归并过程中，左半部分和右半部分各自有序
- 双指针从左半和右半头部开始，统计满足条件的对数

### 4.3 关键代码模板

```python
# ============ 方法A: BIT ============
class BIT:
    def __init__(self, n):
        self.n = n
        self.tree = [0] * (n + 2)

    def add(self, idx, delta):
        while idx <= self.n:
            self.tree[idx] += delta
            idx += idx & -idx

    def sum(self, idx):
        res = 0
        while idx > 0:
            res += self.tree[idx]
            idx -= idx & -idx
        return res

class Solution:
    def reversePairs(self, nums):
        if not nums:
            return 0
        # 离散化: nums和 2*nums+1
        all_vals = set()
        for v in nums:
            all_vals.add(v)
            all_vals.add(2 * v + 1)
        sorted_vals = sorted(all_vals)
        val_to_idx = {v: i + 1 for i, v in enumerate(sorted_vals)}

        bit = BIT(len(sorted_vals) + 2)
        res = 0
        for j in range(len(nums) - 1, -1, -1):
            # 查询 > 2*nums[j] 的元素个数（即翻转对左半部分）
            idx = val_to_idx[2 * nums[j] + 1]
            res += bit.sum(len(sorted_vals)) - bit.sum(idx - 1)
            # 加入当前元素
            bit.add(val_to_idx[nums[j]], 1)
        return res

# ============ 方法B: 归并排序（推荐） ============
class Solution2:
    def reversePairs(self, nums):
        if not nums:
            return 0
        self.nums = nums
        return self._merge_sort(0, len(nums) - 1)

    def _merge_sort(self, l, r):
        if l >= r:
            return 0
        mid = (l + r) // 2
        count = self._merge_sort(l, mid) + self._merge_sort(mid + 1, r)

        # 统计翻转对
        j = mid + 1
        for i in range(l, mid + 1):
            while j <= r and self.nums[i] > 2 * self.nums[j]:
                j += 1
            count += j - (mid + 1)

        # 归并
        temp = []
        i, j = l, mid + 1
        while i <= mid and j <= r:
            if self.nums[i] <= self.nums[j]:
                temp.append(self.nums[i])
                i += 1
            else:
                temp.append(self.nums[j])
                j += 1
        temp.extend(self.nums[i:mid + 1])
        temp.extend(self.nums[j:r + 1])
        self.nums[l:r + 1] = temp
        return count
```

### 4.4 复杂度分析
- 时间: O(N log N)
- 空间: O(N) — 归并需要辅助数组

---

## 题目五：LC 218. 天际线问题

**链接**: https://leetcode.cn/problems/the-skyline-problem/
**难度**: ⭐⭐⭐⭐ 困难
**分类**: 线段树 / 扫描线

### 5.1 问题描述
给定建筑物的 `(left, right, height)`，返回建筑轮廓的关键点（天际线）。

### 5.2 核心思路

**扫描线算法**：
1. 将每个建筑拆成两个事件：`[left, height, +1]` 入事件，`[right, height, -1]` 出事件
2. 按 x 坐标排序，相同位置优先处理出事件（避免遮挡）
3. 用**最大堆**（按height降序）维护当前活跃建筑：
   - 当最高高度变化时，添加关键点
   - 用 (height, endTime) 记录建筑出堆时机

### 5.3 关键代码模板

```python
import heapq

class Solution:
    def getSkyline(self, buildings):
        # 事件点: (x, height, type)  type: +1入, -1出
        events = []
        for l, r, h in buildings:
            events.append((l, h, 1))    # 入
            events.append((r, h, -1))  # 出

        # 排序: x升序；同x处，type=-1优先（先处理出事件）；同type，height降序
        events.sort(key=lambda x: (x[0], -x[2], -x[1]))

        result = []
        max_heap = [(0, float('inf'))]  # (height, end_x)，初始高度0
        i = 0
        n = len(events)

        while i < n:
            x = events[i][0]
            # 处理所有x坐标上的事件
            while i < n and events[i][0] == x:
                _, h, typ = events[i]
                if typ == 1:
                    # 入事件: (height, right_x)
                    # 找对应的right
                    # 先找所有同建筑
                    heapq.heappush(max_heap, (-h, x))  # 用x临时存储
                i += 1

            # 清理已出堆的建筑（end <= 当前x）
            while max_heap and max_heap[0][1] <= x:
                heapq.heappop(max_heap)

            # 当前最高高度
            curr_height = -max_heap[0][0]
            if not result or curr_height != result[-1][1]:
                result.append([x, curr_height])

        return result
```

**更精确的版本**（使用真实right坐标）：

```python
class Solution:
    def getSkyline(self, buildings):
        # 构建事件: (x, -height, end) 入； (x, height, end) 出
        events = []
        for l, r, h in buildings:
            events.append((l, -h, r))
            events.append((r, h, r))
        events.sort()

        result = []
        # max_heap: (-height, end)
        heap = [(0, float('inf'))]
        i = 0
        n = len(events)

        while i < n:
            x = events[i][0]
            # 处理所有x相同的事件
            while i < n and events[i][0] == x:
                h, end = -events[i][1], events[i][2]
                if events[i][1] < 0:  # 入事件
                    heapq.heappush(heap, (-h, end))
                else:  # 出事件，这里不需要特殊处理，因为通过end清理
                i += 1

            # 移除已过期的建筑
            while heap and heap[0][1] <= x:
                heapq.heappop(heap)

            curr_h = -heap[0][0]
            if not result or curr_h != result[-1][1]:
                result.append([x, curr_h])

        return result
```

### 5.4 复杂度分析
- 时间: O(N log N)
- 空间: O(N)

---

## 题目六：LC 732. 我的日程安排表 III

**链接**: https://leetcode.cn/problems/my-calendar-iii/
**难度**: ⭐⭐⭐⭐ 困难
**分类**: 线段树区间染色/覆盖计数

### 6.1 问题描述
日程安排系统，支持 `book(start, end)` 预订日程，返回预订后**任意时间点被预约的最大次数**（即最大重叠数）。

### 6.2 核心思路

**差分数组（扫描线）**：
- 对区间 `[start, end)` 进行差分标记：
  - `diff[start] += 1`（开始预约）
  - `diff[end] -= 1`（结束预约）
- 遍历所有关键点，计算前缀和，即为任意点的重叠数
- 返回最大值

**线段树方法**（懒传播）：
- 区间加1，记录每个区间的最大覆盖次数

### 6.3 关键代码模板

```python
# ============ 方法A: 差分数组（推荐，简单有效）============
class MyCalendarThree:
    def __init__(self):
        self.diff = {}  # 差分数组

    def book(self, start, end):
        self.diff[start] = self.diff.get(start, 0) + 1
        self.diff[end] = self.diff.get(end, 0) - 1

        max_overlap = 0
        cur = 0
        for x in sorted(self.diff):
            cur += self.diff[x]
            max_overlap = max(max_overlap, cur)
        return max_overlap

# ============ 方法B: 线段树（懒传播） ============
class SegNode:
    def __init__(self, l, r):
        self.l, self.r = l, r
        self.left, self.right = None, None
        self.val = 0      # 当前区间被标记次数
        self.cover = 0    # 懒标记（尚未下推的增量）

class MyCalendarThreeSeg:
    def __init__(self):
        self.root = SegNode(0, 10**9 + 1)

    def update(self, node, l, r, ql, qr):
        if ql <= l and r <= qr:
            node.val += 1
            node.cover += 1
            return
        mid = (l + r) // 2
        if not node.left:
            node.left = SegNode(l, mid)
        if not node.right:
            node.right = SegNode(mid + 1, r)
        if ql <= mid:
            self.update(node.left, l, mid, ql, qr)
        if qr > mid:
            self.update(node.right, mid + 1, r, ql, qr)
        node.val = max(node.left.val if node.left else 0,
                       node.right.val if node.right else 0) + node.cover

    def query(self, node, l, r, ql, qr):
        if ql <= l and r <= qr:
            return node.val
        mid = (l + r) // 2
        res = node.cover  # 懒标记向下传递
        if ql <= mid and node.left:
            res = max(res, self.query(node.left, l, mid, ql, qr))
        if qr > mid and node.right:
            res = max(res, self.query(node.right, mid + 1, r, ql, qr))
        return res

    def book(self, start, end):
        self.update(self.root, 0, 10**9 + 1, start, end - 1)
        return self.query(self.root, 0, 10**9 + 1, start, end - 1)
```

### 6.4 复杂度分析
- 方法A（差分）: 每次book O(K log K)，K为已预订数；所有book总O(N²)
- 方法B（线段树）: 每次book O(log C)，C为坐标范围
- 空间: O(N)

---

## 题目七：LC 850. 矩形面积 II

**链接**: https://leetcode.cn/problems/rectangle-area-ii/
**难度**: ⭐⭐⭐⭐⭐ 极难
**分类**: 线段树 + 离散化 + 扫描线

### 7.1 问题描述
计算平面上所有矩形覆盖的**总面积**（可能有重叠）。

### 7.2 核心思路

**扫描线 + 线段树**：
1. 提取所有x坐标（离散化）
2. 对每两个相邻x坐标之间的「带」：
   - 用线段树维护y方向上被覆盖的长度
   - 面积 += 覆盖宽度 × 覆盖高度
3. 每个矩形的 `[x1, x2)` 区间用差分更新

### 7.3 关键代码模板

```python
class Solution:
    def rectangleArea(self, rectangles):
        MOD = 10**9 + 7
        # 收集所有x坐标
        xs = sorted(set([x for x, _, _, _ in rectangles] |
                        [x for _, _, x, _ in rectangles]))
        x_to_idx = {x: i for i, x in enumerate(xs)}

        # 差分数组: diff[y1] += 1, diff[y2] -= 1
        diff = [0] * (len(xs) - 1)

        # 事件: (x, y1, y2, +1/-1)
        events = []
        for x1, y1, x2, y2 in rectangles:
            events.append((x1, y1, y2, 1))
            events.append((x2, y1, y2, -1))
        events.sort()

        prev_x = events[0][0]
        ans = 0

        for x, y1, y2, typ in events:
            # ---- 计算当前覆盖面积 ----
            width = x - prev_x
            if width > 0:
                # 计算当前diff覆盖的y长度
                covered_y = 0
                for i, v in enumerate(diff):
                    if v > 0:
                        covered_y += xs[i + 1] - xs[i]
                ans += width * covered_y

            # ---- 更新差分数组 ----
            i1 = x_to_idx[y1]
            i2 = x_to_idx[y2]
            for i in range(i1, i2):
                diff[i] += typ

            prev_x = x

        return ans % MOD
```

### 7.4 复杂度分析
- 时间: O(N × Y) 其中Y是离散化后的y轴段数，最坏O(N²)但实际可接受
- 空间: O(N)

---

## 题目八：LC 699. 掉落的方块

**链接**: https://leetcode.cn/problems/falling-squares/
**难度**: ⭐⭐⭐⭐ 困难
**分类**: 线段树区间覆盖/最值

### 8.1 问题描述
顺序掉落正方形方块，每个方块 `(position, side_length)` 落在已有方块上，返回每个方块落下后的最大高度。

### 8.2 核心思路

**线段树 + 区间覆盖**：
1. 坐标范围很大（10^8），需要**离散化**
2. 对每个方块 `[p, p+len)`：
   - 查询区间 `[p, p+len-1)` 的当前最大高度 `h_max`
   - 新高度 = `h_max + len`
   - 将该区间**覆盖**为新高度（区间赋值）
   - 记录当前最大高度

**区间赋值需要懒传播**：标记覆盖值，pushdown时传递。

### 8.3 关键代码模板

```python
class SegTree:
    """线段树：区间赋值（覆盖）+ 区间最大值查询"""
    def __init__(self, coords):
        self.coords = sorted(coords)
        self.n = len(coords) - 1  # 区间段数
        self.tree = [0] * (4 * self.n)
        self.lazy = [0] * (4 * self.n)  # 懒标记：覆盖值

    def _push(self, node):
        if self.lazy[node]:
            self.tree[node * 2] = self.lazy[node]
            self.tree[node * 2 + 1] = self.lazy[node]
            self.lazy[node * 2] = self.lazy[node]
            self.lazy[node * 2 + 1] = self.lazy[node]
            self.lazy[node] = 0

    def _update(self, node, l, r, ql, qr, val):
        if ql <= l and r <= qr:
            self.tree[node] = val
            self.lazy[node] = val
            return
        self._push(node)
        mid = (l + r) // 2
        if ql <= mid:
            self._update(node * 2, l, mid, ql, qr, val)
        if qr > mid:
            self._update(node * 2 + 1, mid + 1, r, ql, qr, val)
        self.tree[node] = max(self.tree[node * 2], self.tree[node * 2 + 1])

    def _query(self, node, l, r, ql, qr):
        if ql <= l and r <= qr:
            return self.tree[node]
        self._push(node)
        mid = (l + r) // 2
        res = 0
        if ql <= mid:
            res = max(res, self._query(node * 2, l, mid, ql, qr))
        if qr > mid:
            res = max(res, self._query(node * 2 + 1, mid + 1, r, ql, qr))
        return res

    def update(self, l, r, val):
        self._update(1, 0, self.n - 1, l, r, val)

    def query(self, l, r):
        return self._query(1, 0, self.n - 1, l, r)


class Solution:
    def fallingSquares(self, positions):
        # 离散化坐标（区间端点）
        coords = set()
        for p, s in positions:
            coords.add(p)
            coords.add(p + s)
        sorted_coords = sorted(coords)
        c_to_i = {c: i for i, c in enumerate(sorted_coords)}
        # 合并相邻坐标段
        uniq = sorted_coords
        # 压缩：相邻坐标差>1的中间点也要加入
        all_pts = set()
        for p, s in positions:
            for x in range(p, p + s):
                all_pts.add(x)
        # 简化：用坐标映射到索引范围
        seg = SegTree(uniq)
        res = []
        cur_max = 0
        for p, s in positions:
            l = c_to_i[p]
            r = c_to_i[p + s] - 1  # 注意：覆盖[p, p+s)区间
            h = seg.query(l, r) + s
            seg.update(l, r, h)
            cur_max = max(cur_max, h)
            res.append(cur_max)
        return res
```

### 8.4 复杂度分析
- 时间: O(N log N)
- 空间: O(N)

---

## 总结：BIT 与线段树选择指南

| 问题场景 | 推荐数据结构 | 原因 |
|---------|------------|------|
| 动态前缀和/单点更新 | BIT | 代码简单，O(log N) |
| 逆序对/排名统计 | BIT + 离散化 | 从右往左扫描 |
| 区间和/计数 | BIT / 差分数组 | 差分尤其适合范围叠加 |
| 区间覆盖/赋值 | 线段树(懒传播) | 支持区间操作 |
| 区间合并/最值 | 线段树 | 支持max/min合并 |
| 区间染色/最大重叠 | 差分数组 / 线段树 | 差分更简洁 |
| 天际线/扫描线 | 堆+事件排序 | 维护最大堆 |

**通用模式记忆**：
```
BIT三件套：add(idx, delta) / sum(idx) / 离散化
线段树三件套：build / update / query（含懒传播push）
扫描线三件套：事件排序 / 差分更新 / 前缀最值
```

---

*本文件为悟空LeetCode专项练习产物 | 2026-04-19*
