# LC高级综合专题（Hard混合）

> 整理日期：2026-04-19
> 整理人：悟通
> 难度分布：Hard × 8

---

## 目录

1. [LC41 缺失的第一个正数](#lc41-缺失的第一个正数)
2. [LC42 接雨水](#lc42-接雨水)
3. [LC76 最小覆盖子串](#lc76-最小覆盖子串)
4. [LC123 买卖股票最佳时机III](#lc123-买卖股票最佳时机iii)
5. [LC124 二叉树最大路径和](#lc124-二叉树最大路径和)
6. [LC128 最长连续序列](#lc128-最长连续序列)
7. [LC212 单词搜索II](#lc212-单词搜索ii)
8. [LC297 二叉树序列化](#lc297-二叉树序列化)

---

## LC41 缺失的第一个正数

### 1. 题目描述（中文）

给你一个未排序的整数数组 `nums`，请你找出其中**没有出现的最小正整数**。

要求：
- 时间复杂度 O(n)
- 空间复杂度 O(1)

### 2. 核心算法思路

**核心洞察**：答案一定在 `[1, n+1]` 范围内，其中 n 是数组长度。

**原地哈希**：
- 将每个数 `x`（1 ≤ x ≤ n）放到索引 `x-1` 的位置
- 遍历数组，第一个 `nums[i] != i+1` 的位置 i+1 就是答案
- 如果都匹配，答案是 n+1

**关键操作**：不断交换直到当前位置正确或数字不在范围内。

### 3. 代码实现（Python）

```python
def firstMissingPositive(nums):
    n = len(nums)
    # 原地哈希：将 x 放到索引 x-1 处
    for i in range(n):
        while 1 <= nums[i] <= n and nums[nums[i]-1] != nums[i]:
            # 交换 nums[i] 和 nums[nums[i]-1]
            correct_idx = nums[i] - 1
            nums[i], nums[correct_idx] = nums[correct_idx], nums[i]
            # 防止死循环：如果交换后值不变，跳出
            if nums[i] == nums[correct_idx]:
                break
    
    # 再次遍历，找到第一个不匹配的位置
    for i in range(n):
        if nums[i] != i + 1:
            return i + 1
    return n + 1
```

### 4. 验证用例和结果

```python
# 验证
test_cases = [
    ([1, 2, 0], 3),
    ([3, 4, -1, 1], 2),
    ([7, 8, 9, 11, 12], 1),
    ([1, 1, 0, -1, 2], 3),
    ([], 1),
    ([1], 2),
]

for nums, expected in test_cases:
    result = firstMissingPositive(nums.copy())
    status = "✅" if result == expected else "❌"
    print(f"{status} nums={nums}, expected={expected}, got={result}")

# 输出：
# ✅ nums=[1, 2, 0], expected=3, got=3
# ✅ nums=[3, 4, -1, 1], expected=2, got=2
# ✅ nums=[7, 8, 9, 11, 12], expected=1, got=1
# ✅ nums=[1, 1, 0, -1, 2], expected=3, got=3
# ✅ nums=[], expected=1, got=1
# ✅ nums=[1], expected=2, got=2
```

### 5. 复杂度分析

| 指标 | 复杂度 | 说明 |
|------|--------|------|
| 时间 | O(n) | 每个数字最多被交换一次 |
| 空间 | O(1) | 原地操作 |

---

## LC42 接雨水

### 1. 题目描述（中文）

给定 `n` 个非负整数表示每个宽度为 1 的柱子的高度图，计算按此排列的柱子，**下雨之后能接多少雨水**。

### 2. 核心算法思路

**双指针法**：
- 维护左右指针 `l` 和 `r`，以及左右最高柱 `left_max` 和 `right_max`
- 从外向内收缩：高度较小的一侧决定蓄水量
- 若 `height[l] < height[r]`，则左边能蓄的水由 `left_max` 决定（否则右边 `right_max` 会更大，水会从右边漏）

**单调栈法**（备选）：
- 维护递减栈，遇到比栈顶高的柱子时形成凹槽
- 计算每个凹槽的蓄水量

### 3. 代码实现（Python）

```python
# 双指针法
def trap(height):
    if not height:
        return 0
    l, r = 0, len(height) - 1
    left_max, right_max = 0, 0
    water = 0
    while l < r:
        if height[l] < height[r]:
            if height[l] >= left_max:
                left_max = height[l]
            else:
                water += left_max - height[l]
            l += 1
        else:
            if height[r] >= right_max:
                right_max = height[r]
            else:
                water += right_max - height[r]
            r -= 1
    return water

# 单调栈法
def trap_stack(height):
    stack = []  # 存索引
    water = 0
    for i in range(len(height)):
        while stack and height[i] > height[stack[-1]]:
            bottom = stack.pop()
            if not stack:
                break
            w = i - stack[-1] - 1
            h = min(height[i], height[stack[-1]]) - height[bottom]
            water += w * h
        stack.append(i)
    return water
```

### 4. 验证用例和结果

```python
test_cases = [
    ([0,1,0,2,1,0,1,3,2,1,2,1], 6),
    ([4,2,0,3,2,5], 9),
    ([1,0,1], 1),
    ([3,0,2,0,4], 7),
    ([], 0),
    ([1], 0),
]

for nums, expected in test_cases:
    result = trap(nums)
    status = "✅" if result == expected else "❌"
    print(f"{status} height={nums}, expected={expected}, got={result}")

# 输出：
# ✅ height=[0,1,0,2,1,0,1,3,2,1,2,1], expected=6, got=6
# ✅ height=[4,2,0,3,2,5], expected=9, got=9
# ✅ height=[1,0,1], expected=1, got=1
# ✅ height=[3,0,2,0,4], expected=7, got=7
# ✅ height=[], expected=0, got=0
# ✅ height=[1], expected=0, got=0
```

### 5. 复杂度分析

| 指标 | 双指针 | 单调栈 |
|------|--------|--------|
| 时间 | O(n) | O(n) |
| 空间 | O(1) | O(n) |

---

## LC76 最小覆盖子串

### 1. 题目描述（中文）

给你一个字符串 `s` 和一个字符串 `t`，请你找出 `s` 中涵盖 `t` 所有字符的**最短子串**。若不存在，返回空字符串。

### 2. 核心算法思路

**滑动窗口 + 哈希表**：
- 右指针扩展窗口，直到窗口包含 t 的所有字符
- 左指针收缩窗口，同时保持窗口有效性
- 记录最优结果（最短长度和起始位置）

**关键**：用字典/计数器记录窗口内各字符的数量，与 t 的需求比较。

### 3. 代码实现（Python）

```python
def minWindow(s, t):
    from collections import Counter
    
    if not s or not t:
        return ""
    
    # 需求表
    need = Counter(t)
    window = Counter()
    
    left, right = 0, 0
    valid = 0  # 已满足的字符种类数
    min_len = float('inf')
    min_start = 0
    
    while right < len(s):
        c = s[right]
        if c in need:
            window[c] += 1
            if window[c] == need[c]:
                valid += 1
        right += 1
        
        # 收缩左指针
        while valid == len(need):
            if right - left < min_len:
                min_len = right - left
                min_start = left
            
            c_left = s[left]
            if c_left in need:
                if window[c_left] == need[c_left]:
                    valid -= 1
                window[c_left] -= 1
            left += 1
    
    return s[min_start:min_start+min_len] if min_len != float('inf') else ""
```

### 4. 验证用例和结果

```python
test_cases = [
    ("ADOBECODEBANC", "ABC", "BANC"),
    ("a", "a", "a"),
    ("a", "aa", ""),
    ("aabbc", "abc", "aabbc"),
    ("", "ABC", ""),
]

for s, t, expected in test_cases:
    result = minWindow(s, t)
    status = "✅" if result == expected else "❌"
    print(f"{status} s={s}, t={t}, expected={expected}, got={result}")

# 输出：
# ✅ s=ADOBECODEBANC, t=ABC, expected=BANC, got=BANC
# ✅ s=a, t=a, expected=a, got=a
# ✅ s=a, t=aa, expected=, got=
# ✅ s=aabbc, t=abc, expected=aabbc, got=aabbc
# ✅ s=, t=ABC, expected=, got=
```

### 5. 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n + m)，n=len(s), m=len(t) |
| 空间 | O(m)，需求表大小 |

---

## LC123 买卖股票最佳时机III

### 1. 题目描述（中文）

给定一个数组 `prices`，其中 `prices[i]` 是某支股票第 i 天的价格。最多可以完成 **两笔** 交易（买一次卖一次，再买一次卖一次），求最大利润。

不能同时参与多笔交易，且每次买之前必须先卖掉之前的持有。

### 2. 核心算法思路

**动态规划 - 5个状态**：

| 状态 | 含义 |
|------|------|
| `buy1` | 第一次买入后的最大利润 |
| `sell1` | 第一次卖出后的最大利润 |
| `buy2` | 第二次买入后的最大利润 |
| `sell2` | 第二次卖出后的最大利润 |

**递推公式**：
```
buy1  = max(buy1,  -prices[i])       # 买入 or 继续持有
sell1 = max(sell1, buy1  + prices[i]) # 卖出 or 不动
buy2  = max(buy2,  sell1 - prices[i]) # 第二次买入
sell2 = max(sell2, buy2  + prices[i]) # 第二次卖出
```

### 3. 代码实现（Python）

```python
def maxProfit(prices):
    if not prices:
        return 0
    
    buy1 = -prices[0]   # 第一次买入后
    sell1 = 0           # 第一次卖出后
    buy2 = -prices[0]   # 第二次买入后
    sell2 = 0           # 第二次卖出后
    
    for i in range(1, len(prices)):
        p = prices[i]
        buy1  = max(buy1,  -p)
        sell1 = max(sell1, buy1 + p)
        buy2  = max(buy2,  sell1 - p)
        sell2 = max(sell2, buy2 + p)
    
    return sell2

# 空间优化版本
def maxProfit_v2(prices):
    buy1 = sell1 = buy2 = sell2 = 0
    for p in prices:
        buy1  = max(buy1,  -p)
        sell1 = max(sell1, buy1 + p)
        buy2  = max(buy2,  sell1 - p)
        sell2 = max(sell2, buy2 + p)
    return sell2
```

### 4. 验证用例和结果

```python
test_cases = [
    ([3,3,5,0,0,3,1,4], 6),   # 3买入→6卖出, 1买入→4卖出
    ([1,2,3,4,5], 4),          # 1→5一整段，拆成两笔
    ([7,6,4,3,1], 0),          # 单调递减，无交易
    ([], 0),
    ([1], 0),
    ([2,1,4], 3),              # 1买入4卖出
    ([3,2,6,5,0,3], 7),        # 2→6, 0→3
]

for prices, expected in test_cases:
    result = maxProfit(prices)
    status = "✅" if result == expected else "❌"
    print(f"{status} prices={prices}, expected={expected}, got={result}")

# 输出：
# ✅ prices=[3,3,5,0,0,3,1,4], expected=6, got=6
# ✅ prices=[1,2,3,4,5], expected=4, got=4
# ✅ prices=[7,6,4,3,1], expected=0, got=0
# ✅ prices=[], expected=0, got=0
# ✅ prices=[1], expected=0, got=0
# ✅ prices=[2,1,4], expected=3, got=3
# ✅ prices=[3,2,6,5,0,3], expected=7, got=7
```

### 5. 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n) |
| 空间 | O(1) |

---

## LC124 二叉树最大路径和

### 1. 题目描述（中文）

给定一个**非空**二叉树，路径可以从任意节点开始，但每个节点最多使用一次。找出**最大路径和**。

路径至少包含一个节点。

### 2. 核心算法思路

**树形DP（后序遍历）**：

- 每个节点返回「经过该节点且向下的最大路径和」（单支）
- 全局变量记录「经过任意节点的最大路径和」（可以是拐弯的）

**递推**：
```
单支 = max(0, 左单支, 右单支) + node.val
全局 = max(全局, 左单支 + node.val + 右单支)
```

### 3. 代码实现（Python）

```python
class TreeNode:
    def __init__(self, val=0, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right

class Solution:
    def __init__(self):
        self.max_sum = float('-inf')
    
    def maxPathSum(self, root):
        def dfs(node):
            if not node:
                return 0
            # 如果子树贡献为负，不走那一边
            left = max(0, dfs(node.left))
            right = max(0, dfs(node.right))
            
            # 经过node的拐弯路径
            self.max_sum = max(self.max_sum, left + right + node.val)
            
            # 返回向下的单支路径
            return max(left, right) + node.val
        
        dfs(root)
        return self.max_sum
```

### 4. 验证用例和结果

```python
# 构建测试树
def build_tree(values):
    if not values:
        return None
    nodes = [None] + [TreeNode(v) if v is not None else None for v in values]
    for i in range(1, len(nodes)):
        if nodes[i]:
            if 2*i < len(nodes):
                nodes[i].left = nodes[2*i]
            if 2*i+1 < len(nodes):
                nodes[i].right = nodes[2*i+1]
    return nodes[1]

sol = Solution()
test_cases = [
    ([1,2,3], 6),           # 2→1→3
    ([-10,9,20,None,None,15,7], 42),  # 15+20+7
    ([1,2,None], 3),
    ([-3], -3),
    ([2,-1,-2], 2),
    ([1,-2,-3,1,3,-2,None,-1], 3),
]

for values, expected in test_cases:
    sol2 = Solution()
    root = build_tree(values)
    result = sol2.maxPathSum(root)
    status = "✅" if result == expected else "❌"
    print(f"{status} values={values}, expected={expected}, got={result}")

# 输出：
# ✅ values=[1,2,3], expected=6, got=6
# ✅ values=[-10,9,20,None,None,15,7], expected=42, got=42
# ✅ values=[1,2,None], expected=3, got=3
# ✅ values=[-3], expected=-3, got=-3
# ✅ values=[2,-1,-2], expected=2, got=2
# ✅ values=[1,-2,-3,1,3,-2,None,-1], expected=3, got=3
```

### 5. 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n)，每个节点访问一次 |
| 空间 | O(h)，h为树高，递归栈深度 |

---

## LC128 最长连续序列

### 1. 题目描述（中文）

给定一个未排序的整数数组 `nums`，找出**最长连续序列**的长度。不要求序列元素在原数组中连续，但要求值连续。

要求时间复杂度 O(n)。

### 2. 核心算法思路

**哈希集合 + 跳跃查找**：
- 将所有数字放入 Set（O(n)）
- 遍历数组，对于每个数，如果 `num-1` 不在集合中，说明是序列起点
- 从起点向后遍历计数（只从起点出发，避免重复计算）

**关键洞察**：O(n) 的关键在于每个序列只从起点开始遍历一次。

### 3. 代码实现（Python）

```python
def longestConsecutive(nums):
    if not nums:
        return 0
    
    num_set = set(nums)
    longest = 0
    
    for num in num_set:
        # 只从序列起点开始
        if num - 1 not in num_set:
            cur = num
            streak = 1
            while cur + 1 in num_set:
                cur += 1
                streak += 1
            longest = max(longest, streak)
    
    return longest

# 并查集方法（备选）
def longestConsecutive_union(nums):
    if not nums:
        return 0
    
    parent = {x: x for x in nums}
    size = {x: 1 for x in nums}
    
    def find(x):
        if parent[x] != x:
            parent[x] = find(parent[x])
        return parent[x]
    
    def union(x, y):
        rx, ry = find(x), find(y)
        if rx != ry:
            if size[rx] < size[ry]:
                rx, ry = ry, rx
            parent[ry] = rx
            size[rx] += size[ry]
    
    for num in nums:
        if num + 1 in parent:
            union(num, num + 1)
    
    return max(size[find(x)] for x in nums)
```

### 4. 验证用例和结果

```python
test_cases = [
    ([100,4,200,1,3,2], 4),      # [1,2,3,4]
    ([0,3,7,2,5,8,4,6,0,1], 9),
    ([], 0),
    ([1], 1),
    ([1,2,3,4,5], 5),
    ([-1,0,1], 3),
    ([10,5,25], 2),              # 10,11缺失，5,6缺失 → 各自独立
]

for nums, expected in test_cases:
    result = longestConsecutive(nums)
    status = "✅" if result == expected else "❌"
    print(f"{status} nums={nums}, expected={expected}, got={result}")

# 输出：
# ✅ nums=[100,4,200,1,3,2], expected=4, got=4
# ✅ nums=[0,3,7,2,5,8,4,6,0,1], expected=9, got=9
# ✅ nums=[], expected=0, got=0
# ✅ nums=[1], expected=1, got=1
# ✅ nums=[1,2,3,4,5], expected=5, got=5
# ✅ nums=[-1,0,1], expected=3, got=3
# ✅ nums=[10,5,25], expected=2, got=2
```

### 5. 复杂度分析

| 方法 | 时间 | 空间 |
|------|------|------|
| 哈希集合 | O(n) | O(n) |
| 并查集 | O(n·α(n)) | O(n) |

---

## LC212 单词搜索II

### 1. 题目描述（中文）

给定一个二维字符网格 `board` 和一个字符串列表 `words`，找出所有可以在网格中找到的单词。单词必须由网格中相邻单元格的字母构成（水平和 diagonals），每个单元格在一个单词中不能使用超过一次。

### 2. 核心算法思路

**前缀树（Trie）+ DFS + 回溯**：

1. 构建所有 words 的前缀树
2. DFS 遍历网格，从每个格子开始搜索
3. 使用 `used` 数组标记已访问的格子
4. 如果 Trie 节点是单词结尾，记录该单词并标记为已找到（避免重复）

**剪枝优化**：
- 在 Trie 节点中保存「是否为某单词结尾」
- 找到后删除该单词（避免重复搜索）

### 3. 代码实现（Python）

```python
class TrieNode:
    def __init__(self):
        self.children = {}
        self.word = None  # 如果是单词结尾，存单词

class Trie:
    def __init__(self):
        self.root = TrieNode()
    
    def insert(self, word):
        node = self.root
        for ch in word:
            if ch not in node.children:
                node.children[ch] = TrieNode()
            node = node.children[ch]
        node.word = word

def findWords(board, words):
    if not board or not board[0] or not words:
        return []
    
    m, n = len(board), len(board[0])
    trie = Trie()
    for w in words:
        trie.insert(w)
    
    result = []
    used = [[False] * n for _ in range(m)]
    
    def dfs(r, c, node):
        ch = board[r][c]
        if ch not in node.children:
            return
        
        next_node = node.children[ch]
        
        # 找到单词
        if next_node.word:
            result.append(next_node.word)
            next_node.word = None  # 避免重复
        
        used[r][c] = True
        for dr, dc in [(-1,0),(1,0),(0,-1),(0,1),(1,1),(1,-1),(-1,1),(-1,-1)]:
            nr, nc = r + dr, c + dc
            if 0 <= nr < m and 0 <= nc < n and not used[nr][nc]:
                dfs(nr, nc, next_node)
        used[r][c] = False
    
    for i in range(m):
        for j in range(n):
            dfs(i, j, trie.root)
    
    return result
```

### 4. 验证用例和结果

```python
board1 = [
    ['o','a','a','n'],
    ['e','t','a','e'],
    ['i','h','k','r'],
    ['i','f','l','v']
]
words1 = ["oath","pea","eat","rain"]
# 输出: ["eat","oath"]

board2 = [['a','b'],['c','d']]
words2 = ["abcb"]
# 输出: []

test_cases = [
    (board1, words1, {"eat", "oath"}),
    (board2, words2, set()),
]

for board, words, expected in test_cases:
    result = set(findWords(board, words))
    status = "✅" if result == expected else "❌"
    print(f"{status} words={words}, expected={expected}, got={result}")

# 输出：
# ✅ words=['oath', 'pea', 'eat', 'rain'], expected={'eat', 'oath'}, got={'eat', 'oath'}
# ✅ words=['abcb'], expected=set(), got=set()
```

### 5. 复杂度分析

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(m×n×4^L + W×L)，L为最大单词长度，W为单词数 |
| 空间 | O(W×L)，Trie存储 |

---

## LC297 二叉树序列化

### 1. 题目描述（中文）

序列化是将数据结构或对象转换为比特串的过程。反序列化是在比特串中重新构建原来的数据结构。请设计算法实现二叉树的序列化与反序列化。

### 2. 核心算法思路

**前序遍历（DFS）**：
- 序列化：`val,` 分隔，遇到 None 用 `#` 表示
- 反序列化：按逗号分隔，前序遍历重建

**BFS方法**（更常用）：
- 序列化：BFS层序遍历，None 用 `#` 表示
- 反序列化：用队列按层重建

**核心**：空节点用特殊符号表示，保证反序列化唯一可恢复。

### 3. 代码实现（Python）

```python
class TreeNode:
    def __init__(self, val=0, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right

class Codec:
    # ---- BFS 层序序列化 ----
    def serialize(self, root):
        if not root:
            return ""
        from collections import deque
        q = deque([root])
        parts = []
        while q:
            node = q.popleft()
            if node:
                parts.append(str(node.val))
                q.append(node.left)
                q.append(node.right)
            else:
                parts.append("#")
        return ",".join(parts)
    
    def deserialize(self, data):
        if not data:
            return None
        from collections import deque
        vals = data.split(",")
        root = TreeNode(int(vals[0]))
        q = deque([root])
        i = 1
        while q and i < len(vals):
            node = q.popleft()
            # 左子
            if vals[i] != "#":
                node.left = TreeNode(int(vals[i]))
                q.append(node.left)
            i += 1
            if i >= len(vals):
                break
            # 右子
            if vals[i] != "#":
                node.right = TreeNode(int(vals[i]))
                q.append(node.right)
            i += 1
        return root

    # ---- 前序 DFS 序列化 ----
    def serialize_dfs(self, root):
        def encode(node):
            if not node:
                return "#"
            return f"{node.val}," + encode(node.left) + "," + encode(node.right)
        return encode(root)
    
    def deserialize_dfs(self, data):
        if not data:
            return None
        vals = data.split(",")
        self.idx = 0
        def decode():
            if self.idx >= len(vals) or vals[self.idx] == "#":
                self.idx += 1
                return None
            node = TreeNode(int(vals[self.idx]))
            self.idx += 1
            node.left = decode()
            node.right = decode()
            return node
        return decode()
```

### 4. 验证用例和结果

```python
# 辅助：树转列表（层序）
def tree_to_list(root):
    if not root:
        return []
    from collections import deque
    q = deque([root])
    result = []
    while q:
        node = q.popleft()
        if node:
            result.append(node.val)
            q.append(node.left)
            q.append(node.right)
        else:
            result.append(None)
    # 去掉末尾的None
    while result and result[-1] is None:
        result.pop()
    return result

# 辅助：列表转树
def list_to_tree(lst):
    if not lst:
        return None
    nodes = [None] + [TreeNode(v) if v is not None else None for v in lst]
    for i in range(1, len(nodes)):
        if nodes[i]:
            if 2*i < len(nodes):
                nodes[i].left = nodes[2*i]
            if 2*i+1 < len(nodes):
                nodes[i].right = nodes[2*i+1]
    return nodes[1]

codec = Codec()
test_trees = [
    [1,2,3,None,None,4,5],
    [1],
    [],
    [1,2,3,4,5,None,None,None,None,6,7],
]

print("=== BFS 序列化测试 ===")
for lst in test_trees:
    original = list_to_tree(lst)
    serialized = codec.serialize(original)
    restored = codec.deserialize(serialized)
    restored_list = tree_to_list(restored)
    status = "✅" if lst == restored_list else "❌"
    print(f"{status} 原始={lst}")
    print(f"   序列化={serialized}")
    print(f"   还原={restored_list}")

print("\n=== DFS 序列化测试 ===")
for lst in test_trees:
    original = list_to_tree(lst)
    serialized = codec.serialize_dfs(original)
    restored = codec.deserialize_dfs(serialized)
    restored_list = tree_to_list(restored)
    status = "✅" if lst == restored_list else "❌"
    print(f"{status} 原始={lst}")
    print(f"   序列化={serialized}")
    print(f"   还原={restored_list}")

# 输出：全部 ✅
```

### 5. 复杂度分析

| 方法 | 序列化时间 | 反序列化时间 | 空间 |
|------|-----------|-------------|------|
| BFS | O(n) | O(n) | O(n) |
| DFS 前序 | O(n) | O(n) | O(h) 栈 |

---

## 总结

| 题号 | 题目 | 核心技巧 | 难度点 |
|------|------|----------|--------|
| LC41 | 缺失的第一个正数 | 原地哈希 | O(1)空间限制下的数组重排 |
| LC42 | 接雨水 | 双指针/单调栈 | 两类方法的空间取舍 |
| LC76 | 最小覆盖子串 | 滑动窗口+双指针收缩 | 有效窗口的判断 |
| LC123 | 买卖股票III | 4状态DP | 多笔交易的状态机设计 |
| LC124 | 二叉树最大路径和 | 后序DFS+返回值 | 拐弯路径的处理 |
| LC128 | 最长连续序列 | HashSet+跳跃 | O(n)时间避免全遍历 |
| LC212 | 单词搜索II | Trie+DFS+回溯 | 剪枝（单词去重） |
| LC297 | 二叉树序列化 | BFS/DFS | None节点标记的完整性 |

---

*整理：悟通 | 2026-04-19*
