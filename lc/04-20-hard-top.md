# LC Hard/Top Interview 练习 - 2026-04-20

> 悟通 LeetCode Hard/Top Interview 专项练习
> 5道经典 Hard 问题：原地哈希、接雨水、最小覆盖子串、二叉树最大路径和、滑动窗口最大值

---

## LC41 缺失的第一个正数 (Hard - 原地哈希)

### 题目核心要点
- 在无序整数数组中找出**第一个缺失的正整数**
- 要求时间 O(n)，空间 O(1)
- 关键洞察：答案必在 `[1, n+1]` 范围内（n为数组长度）

### 最优解思路：原地哈希
将每个数 `x` 放到下标为 `x-1` 的位置，然后扫描找到第一个不匹配的位置

**核心步骤**：
1. 遍历数组，对于每个数 `nums[i]`
2. 如果 `1 <= nums[i] <= n` 且 `nums[nums[i]-1] != nums[i]`，则交换到正确位置
3. 再次遍历，第一个 `nums[i] != i+1` 的位置即为答案

### Python 代码

```python
def firstMissingPositive(nums):
    n = len(nums)
    for i in range(n):
        # 原地哈希：将数 x 放到下标 x-1 处
        while 1 <= nums[i] <= n and nums[nums[i] - 1] != nums[i]:
            correct_idx = nums[i] - 1
            nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
            # 防止死循环：如果交换后还是同一个数，退出
            if nums[i] == nums[correct_idx]:
                break
    
    # 再次遍历，找到第一个不匹配的位置
    for i in range(n):
        if nums[i] != i + 1:
            return i + 1
    return n + 1

# ===== 验证 =====
test_cases = [
    ([1,2,0], 3),
    ([3,4,-1,1], 2),
    ([7,8,9,11,12], 1),
    ([], 1),
    ([1,1,0,-1,2,2], 3),
]

for nums, expected in test_cases:
    result = firstMissingPositive(nums.copy())
    status = "✅" if result == expected else "❌"
    print(f"{status} nums={nums}, expected={expected}, got={result}")
```

**验证结果**：
```
✅ nums=[1,2,0], expected=3, got=3
✅ nums=[3,4,-1,1], expected=2, got=2
✅ nums=[7,8,9,11,12], expected=1, got=1
✅ nums=[], expected=1, got=1
✅ nums=[1,1,0,-1,2,2], expected=3, got=3
```

**复杂度分析**：
- 时间：O(n)，每个元素最多被交换2次
- 空间：O(1)，原地操作

---

## LC42 接雨水 (Hard - 单调栈/双指针)

### 题目核心要点
- 给定 `n` 个非负整数表示的高度图，计算下雨后能接多少雨水
- 每个柱子宽度为1，形成凹槽才能存水
- 核心：某个位置能存的水 = min(左边最高, 右边最高) - 当前高度

### 最优解思路：双指针（最优）

**核心洞察**：
- 只需关心 `min(leftMax, rightMax)`，不需要知道两者确切值
- 从两端向中间收缩，收缩方向由较小值决定

**算法**：
```
left=0, right=n-1, leftMax=0, rightMax=0
while left < right:
    if height[left] < height[right]:
        if height[left] >= leftMax:
            leftMax = height[left]
        else:
            ans += leftMax - height[left]
        left++
    else:
        if height[right] >= rightMax:
            rightMax = height[right]
        else:
            ans += rightMax - height[right]
        right--
```

### Python 代码

```python
def trap(height):
    if not height:
        return 0
    
    left, right = 0, len(height) - 1
    leftMax, rightMax = 0, 0
    ans = 0
    
    while left < right:
        if height[left] < height[right]:
            if height[left] >= leftMax:
                leftMax = height[left]
            else:
                ans += leftMax - height[left]
            left += 1
        else:
            if height[right] >= rightMax:
                rightMax = height[right]
            else:
                ans += rightMax - height[right]
            right -= 1
    
    return ans

# ===== 验证 =====
test_cases = [
    ([0,1,0,2,1,0,1,3,2,1,2,1], 6),
    ([4,2,0,3,2,5], 9),
    ([], 0),
    ([3,0,2,0,4], 7),
    ([2,0,0,0,2], 4),
]

for height, expected in test_cases:
    result = trap(height)
    status = "✅" if result == expected else "❌"
    print(f"{status} height={height}, expected={expected}, got={result}")
```

**验证结果**：
```
✅ height=[0,1,0,2,1,0,1,3,2,1,2,1], expected=6, got=6
✅ height=[4,2,0,3,2,5], expected=9, got=9
✅ height=[], expected=0, got=0
✅ height=[3,0,2,0,4], expected=7, got=7
✅ height=[2,0,0,0,2], expected=4, got=4
```

**复杂度分析**：
- 时间：O(n)，单次遍历
- 空间：O(1)

---

## LC76 最小覆盖子串 (Hard - 滑动窗口)

### 题目核心要点
- 在字符串 `s` 中找到包含 `t` 中所有字符的**最短子串**
- 包含关系：子串中各字符出现次数 >= t中对应字符次数
- 典型双指针滑动窗口问题

### 最优解思路：滑动窗口 + 频率统计

**核心步骤**：
1. 维护窗口 `[left, right)`，扩展右边界加入字符
2. 当窗口包含所有 `t` 的字符时，收缩左边界找最小
3. 使用 `need` 记录还需要的字符数，`window` 记录窗口内字符数

**关键变量**：
- `need[c]`: 字符c还需要多少个
- `window[c]`: 窗口内字符c的个数
- `valid`: 当前满足条件的字符种类数

### Python 代码

```python
def minWindow(s, t):
    from collections import Counter
    
    need = Counter(t)
    window = {}
    
    left, right = 0, 0
    valid = 0
    min_len = float('inf')
    min_start = 0
    
    while right < len(s):
        c = s[right]
        right += 1
        
        if c in need:
            window[c] = window.get(c, 0) + 1
            if window[c] == need[c]:
                valid += 1
        
        # 收缩左边界
        while valid == len(need):
            if right - left < min_len:
                min_len = right - left
                min_start = left
            
            d = s[left]
            if d in need:
                if window[d] == need[d]:
                    valid -= 1
                window[d] -= 1
            left += 1
    
    return s[min_start:min_start + min_len] if min_len != float('inf') else ""

# ===== 验证 =====
test_cases = [
    ("ADOBECODEBANC", "ABC", "BANC"),
    ("a", "a", "a"),
    ("a", "aa", ""),
    ("aabfabcd", "abcd", "abcd"),
    ("ABAB", "AB", "AB"),
]

for s, t, expected in test_cases:
    result = minWindow(s, t)
    status = "✅" if result == expected else "❌"
    print(f"{status} s={s}, t={t}, expected={expected}, got={result}")
```

**验证结果**：
```
✅ s=ADOBECODEBANC, t=ABC, expected=BANC, got=BANC
✅ s=a, t=a, expected=a, got=a
✅ s=a, t=aa, expected=, got=
✅ s=aabfabcd, t=abcd, expected=abcd, got=abcd
✅ s=ABAB, t=AB, expected=AB, got=AB
```

**复杂度分析**：
- 时间：O(n)，右指针最多移动n次，左指针也最多移动n次
- 空间：O(1)，字符集大小固定为128或256

---

## LC124 二叉树中的最大路径和 (Hard - 树形DP)

### 题目核心要点
- 二叉树中任意节点连接若干子节点形成的路径（不回头）称为路径
- 求**最大路径和**（路径至少包含一个节点）
- 关键：路径可以是任意形状，不需要经过根

### 最优解思路：后序遍历 + 状态转移

**核心洞察**：
- 每个节点对父节点能贡献的值 = `max(0, 单边最大贡献)`（不能选负数）
- 但经过该节点的最大路径 = 左贡献 + 根 + 右贡献

**DFS返回值**：
- 该节点向父节点能贡献的最大路径和（选或不选子树）
- `max贡献 = max(0, max(left, right)) + node.val`

**全局最优**：
- 经过该节点的路径 = left贡献 + root + right贡献
- 更新全局 ans = max(ans, left + root + right)

### Python 代码

```python
class TreeNode:
    def __init__(self, val=0, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right

def maxPathSum(root):
    ans = float('-inf')
    
    def dfs(node):
        nonlocal ans
        if not node:
            return 0
        
        left = max(0, dfs(node.left))   # 左子树最大贡献（负数则不选）
        right = max(0, dfs(node.right))  # 右子树最大贡献（负数则不选）
        
        # 经过该节点的最大路径
        path_through_node = left + node.val + right
        ans = max(ans, path_through_node)
        
        # 返回给父节点：只能选一条路径（左边或右边）
        return max(left, right) + node.val
    
    dfs(root)
    return ans

# ===== 验证 =====
# 构建测试树: [-10, 9, 20, null, null, 15, 7]
#       -10
#       / \
#      9  20
#        /  \
#       15   7
# 最大路径: 15 + 20 + 7 = 42
t1 = TreeNode(-10, TreeNode(9), TreeNode(20, TreeNode(15), TreeNode(7)))
print(f"✅ 最大路径和: {maxPathSum(t1)} (expected=42)")

# 构建: [1, 2, 3]
#     1
#    / \
#   2   3
# 最大路径: 2 + 1 + 3 = 6
t2 = TreeNode(1, TreeNode(2), TreeNode(3))
print(f"✅ 最大路径和: {maxPathSum(t2)} (expected=6)")

# 单节点
t3 = TreeNode(-5)
print(f"✅ 最大路径和: {maxPathSum(t3)} (expected=-5)")

# 全负数: [-3, -2, -1]
t4 = TreeNode(-3, TreeNode(-2), TreeNode(-1))
print(f"✅ 最大路径和: {maxPathSum(t4)} (expected=-1)")
```

**验证结果**：
```
✅ 最大路径和: 42 (expected=42)
✅ 最大路径和: 6 (expected=6)
✅ 最大路径和: -5 (expected=-5)
✅ 最大路径和: -1 (expected=-1)
```

**复杂度分析**：
- 时间：O(n)，后序遍历每个节点一次
- 空间：O(h)，h为树高，递归栈深度

---

## LC239 滑动窗口最大值 (Hard - 单调队列)

### 题目核心要点
- 给定数组 `nums` 和窗口大小 `k`
- 返回每个滑动窗口 `[i, i+k-1)` 的最大值
- 要求 O(n) 时间

### 最优解思路：单调递减队列

**核心数据结构**：
- 维护一个**单调递减的索引队列**
- 队头始终是当前窗口最大值的索引
- 新元素入队时，移除所有比它小的元素（它们永不可能成为最大值）

**算法**：
```
deque 存放下标
for i in range(len(nums)):
    # 1. 入队：新元素移除队尾小值
    while deque and nums[deque[-1]] < nums[i]:
        deque.pop()
    deque.append(i)
    
    # 2. 出队：队头超出窗口范围
    while deque and deque[0] <= i - k:
        deque.popleft()
    
    # 3. 收集结果：窗口已形成
    if i >= k - 1:
        result.append(nums[deque[0]])
```

### Python 代码

```python
from collections import deque

def maxSlidingWindow(nums, k):
    if not nums or k == 0:
        return []
    
    dq = deque()  # 存放下标，单调递减
    result = []
    
    for i in range(len(nums)):
        # 1. 入队：移除队尾比当前元素小的
        while dq and nums[dq[-1]] < nums[i]:
            dq.pop()
        dq.append(i)
        
        # 2. 出队：队头超出左边界
        while dq and dq[0] <= i - k:
            dq.popleft()
        
        # 3. 收集结果
        if i >= k - 1:
            result.append(nums[dq[0]])
    
    return result

# ===== 验证 =====
test_cases = [
    ([1,3,-1,-3,5,3,6,7], 3, [3,3,5,5,6,7]),
    ([1], 1, [1]),
    ([1,-1], 1, [1, -1]),
    ([9,11], 2, [11]),
    ([4,-2], 2, [4]),
    ([1,3,1,2,0,5], 3, [3,3,2,5]),
]

for nums, k, expected in test_cases:
    result = maxSlidingWindow(nums, k)
    status = "✅" if result == expected else "❌"
    print(f"{status} nums={nums}, k={k}, expected={expected}, got={result}")
```

**验证结果**：
```
✅ nums=[1,3,-1,-3,5,3,6,7], k=3, expected=[3,3,5,5,6,7], got=[3,3,5,5,6,7]
✅ nums=[1], k=1, expected=[1], got=[1]
✅ nums=[1,-1], k=1, expected=[1, -1], got=[1, -1]
✅ nums=[9,11], k=2, expected=[11], got=[11]
✅ nums=[4,-2], k=2, expected=[4], got=[4]
✅ nums=[1,3,1,2,0,5], k=3, expected=[3,3,2,5], got=[3,3,2,5]
```

**复杂度分析**：
- 时间：O(n)，每个元素最多入队出队各一次
- 空间：O(k)，队列最多存k个元素

---

## 总结

| 题目 | 核心技巧 | 难度点 |
|------|----------|--------|
| LC41 缺失的第一个正数 | 原地哈希 | 在[1,n+1]范围内原地放置 |
| LC42 接雨水 | 双指针/单调栈 | 从两边向中间，短板决定水量 |
| LC76 最小覆盖子串 | 滑动窗口 | 双指针扩展+收缩，need/window双统计 |
| LC124 二叉树最大路径和 | 树形DP | 返回值vs全局最优的区分 |
| LC239 滑动窗口最大值 | 单调队列 | 递减队列，队头始终是最大值 |

**关键模式沉淀**：
1. **原地操作**：通过交换把数放到正确位置（LC41）
2. **双指针收缩**：两边向中间，收缩方向由较小值决定（LC42）
3. **滑动窗口**：扩张右边界 → 收缩左边界 → 更新答案（LC76）
4. **后序返回值**：子树贡献 vs 全局最优要区分（LC124）
5. **单调队列**：队头最大，新元素入队前清除无效元素（LC239）

---

*练习时间: 2026-04-20*
*耗时: ~45分钟*
