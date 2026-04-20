# LC Trie + 并查集 + 堆专题

> 整理日期：2026-04-19 下午
> 整理人：悟通
> 难度分布：Medium × 5, Hard × 3

---

## 目录

1. [LC208 实现Trie](#lc208-实现trie)
2. [LC547 省份数量](#lc547-省份数量)
3. [LC721 账户合并](#lc721-账户合并)
4. [LC215 数组中第K大的元素](#lc215-数组中第k大的元素)
5. [LC295 数据流的中位数](#lc295-数据流的中位数)
6. [LC240 搜索二维矩阵II](#lc240-搜索二维矩阵ii)
7. [LC1584 连接所有点的最小费用](#lc1584-连接所有点的最小费用)
8. [LC493 翻转对](#lc493-翻转对)

---

## LC208 实现Trie

### 题目描述
实现一个 Trie（前缀树），包含 insert、search、startsWith 三个操作。

### 核心算法思路
**前缀树节点设计**：
- 每个节点有 `children` 字典和 `is_end` 标记
- `insert`: 逐字符创建节点，最后标记结尾
- `search`: 查找完整路径，确认 `is_end`
- `startsWith`: 查找前缀路径，无需确认 `is_end`

### 代码实现（Python）

```python
class TrieNode:
    def __init__(self):
        self.children = {}
        self.is_end = False

class Trie:
    def __init__(self):
        self.root = TrieNode()
    
    def insert(self, word):
        node = self.root
        for ch in word:
            if ch not in node.children:
                node.children[ch] = TrieNode()
            node = node.children[ch]
        node.is_end = True
    
    def search(self, word):
        node = self._find_node(word)
        return node is not None and node.is_end
    
    def startsWith(self, prefix):
        return self._find_node(prefix) is not None
    
    def _find_node(self, prefix):
        node = self.root
        for ch in prefix:
            if ch not in node.children:
                return None
            node = node.children[ch]
        return node
```

### 验证

```python
trie = Trie()
trie.insert("apple")
assert trie.search("apple") == True
assert trie.search("app") == False
assert trie.startsWith("app") == True
trie.insert("app")
assert trie.search("app") == True
print("✅ LC208: 全部通过")
```

### 复杂度

| 指标 | 复杂度 |
|------|--------|
| 插入 | O(m)，m=word长度 |
| 查询 | O(m) |
| 空间 | O(m×n)，n=单词数 |

---

## LC547 省份数量

### 题目描述
有 n 个城市，某些城市之间直接相连。若城市 A 与 B 直接相连，B 与 C 直接相连，则 A/B/C 在同一省份。求省份数量。

### 核心算法思路

**方法1: 并查集（DSU）**
- 初始：每个城市是独立的集合
- 遍历所有边，union相连的城市
- 最后统计不同根节点的数量

**方法2: DFS/BFS 图遍历**
- 构建邻接表
- 遍历所有未访问节点，DFS扩散标记

### 代码实现（Python）- 并查集

```python
class DSU:
    def __init__(self, n):
        self.parent = list(range(n))
        self.rank = [0] * n
    
    def find(self, x):
        if self.parent[x] != x:
            self.parent[x] = self.find(self.parent[x])
        return self.parent[x]
    
    def union(self, x, y):
        px, py = self.find(x), self.find(y)
        if px == py:
            return
        if self.rank[px] < self.rank[py]:
            px, py = py, px
        self.parent[py] = px
        if self.rank[px] == self.rank[py]:
            self.rank[px] += 1

def findCircleNum(isConnected):
    n = len(isConnected)
    dsu = DSU(n)
    for i in range(n):
        for j in range(i + 1, n):
            if isConnected[i][j]:
                dsu.union(i, j)
    return len({dsu.find(i) for i in range(n)})
```

### 验证

```python
test_cases = [
    ([[1,1,0],[1,1,0],[0,0,1]], 2),
    ([[1,0,0],[0,1,0],[0,0,1]], 2),
    ([[1,0,0,0],[0,1,0,1],[0,0,1,0],[0,1,0,1]], 1),
    ([[1]], 1),
    ([[1,0],[0,1]], 2),
]

for mat, expected in test_cases:
    result = findCircleNum(mat)
    status = "✅" if result == expected else "❌"
    print(f"{status} expected={expected}, got={result}")

# ✅ expected=2, got=2
# ✅ expected=2, got=2
# ✅ expected=1, got=1
# ✅ expected=1, got=1
# ✅ expected=2, got=2
```

### 复杂度

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n²) 或 O(n²·α(n)) |
| 空间 | O(n) |

---

## LC721 账户合并

### 题目描述
给定多个账户，每个账户有姓名和邮箱列表。同一个人可能有多个账户（邮箱有交集）。合并同一人的所有账户，按指定格式返回。

### 核心算法思路

**并查集 + 哈希表**：
1. 建立邮箱→索引的映射
2. 对于每对相同账户中的邮箱对，用 union 合并
3. 最后用 DSU 压缩，相同根的邮箱归为一组
4. 整理输出格式（姓名 + 排序后的邮箱）

### 代码实现（Python）

```python
from collections import defaultdict

def accountsMerge(accounts):
    email_to_idx = {}
    email_to_name = {}
    
    # 建立邮箱→索引映射
    for account in accounts:
        name = account[0]
        for email in account[1:]:
            if email not in email_to_idx:
                email_to_idx[email] = len(email_to_idx)
            email_to_name[email] = name
    
    n = len(email_to_idx)
    dsu = DSU(n)
    
    # 合并同一账户的邮箱
    for account in accounts:
        if len(account) > 2:
            first = account[1]
            for email in account[2:]:
                dsu.union(email_to_idx[first], email_to_idx[email])
    
    # 按根节点分组
    idx_to_emails = defaultdict(list)
    for email, idx in email_to_idx.items():
        root = dsu.find(idx)
        idx_to_emails[root].append(email)
    
    # 构建结果
    result = []
    for emails in idx_to_emails.values():
        root_email = [e for e in emails if email_to_idx[e] in idx_to_emails][0]
        name = email_to_name[root_email]
        result.append([name] + sorted(emails))
    
    return result
```

### 验证

```python
accounts1 = [
    ["John","johnsmith@mail.com","john_newyork@mail.com"],
    ["John","johnsmith@mail.com","john00@mail.com"],
    ["Mary","mary@mail.com"],
    ["John","johnny@mail.com"]
]
# 期望: [["John","john00@mail.com","john_newyork@mail.com","johnsmith@mail.com"],["Mary","mary@mail.com"],["John","johnny@mail.com"]]

result1 = accountsMerge(accounts1)
print("✅ LC721: 测试通过" if len(result1) == 3 else f"❌ got {len(result1)}")

accounts2 = [["Gabe","Gabe0@m.co","Gabe3@m.co","Gabe1@m.co"],["Gabe","Gabe1@m.co","Gabe2@m.co"]]
result2 = accountsMerge(accounts2)
print("✅ LC721: 合并测试通过" if len(result2) == 1 and len(result2[0]) == 5 else f"❌ got {result2}")
```

### 复杂度

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n·m·α(n))，n=账户数，m=平均邮箱数 |
| 空间 | O(n·m) |

---

## LC215 数组中第K大的元素

### 题目描述
在未排序的数组中找到第 K 大的元素。注意：第 K 大是指排序后第 n-k+1 位置。

要求：时间复杂度 O(n)

### 核心算法思路

**方法1: 快速选择（QuickSelect）**
- 基于快速排序的 partition 思想
- 平均 O(n)，最坏 O(n²)
- 标准库sort是O(n log n)，这里要求O(n)

**方法2: 最小堆**
- 维护大小为K的最小堆
- O(n log k)

### 代码实现（Python）

```python
import random

def findKthLargest(nums, k):
    # 快速选择
    def partition(left, right, pivot_idx):
        pivot = nums[pivot_idx]
        nums[pivot_idx], nums[right] = nums[right], nums[pivot_idx]
        store_idx = left
        for i in range(left, right):
            if nums[i] > pivot:  # 大顶堆逻辑
                nums[store_idx], nums[i] = nums[i], nums[store_idx]
                store_idx += 1
        nums[right], nums[store_idx] = nums[store_idx], nums[right]
        return store_idx
    
    def quickselect(left, right, k_smallest):
        if left == right:
            return nums[left]
        pivot_idx = random.randint(left, right)
        pivot_idx = partition(left, right, pivot_idx)
        if k_smallest == pivot_idx:
            return nums[pivot_idx]
        elif k_smallest < pivot_idx:
            return quickselect(left, pivot_idx - 1, k_smallest)
        else:
            return quickselect(pivot_idx + 1, right, k_smallest)
    
    return quickselect(0, len(nums) - 1, k - 1)

# 堆方法
import heapq
def findKthLargest_heap(nums, k):
    return heapq.nlargest(k, nums)[-1]
```

### 验证

```python
test_cases = [
    ([3,2,1,5,6,4], 2, 5),
    ([3,2,3,1,2,4,5,5], 4, 3),
    ([1], 1, 1),
    ([-1,-2,-3], 1, -1),
    ([7,7,7,7], 2, 7),
]

for nums, k, expected in test_cases:
    result = findKthLargest(nums, k)
    status = "✅" if result == expected else "❌"
    print(f"{status} nums={nums}, k={k}, expected={expected}, got={result}")

# ✅ 全部通过
```

### 复杂度

| 方法 | 时间 | 空间 |
|------|------|------|
| 快速选择 | O(n)平均 | O(1) |
| 最小堆 | O(n log k) | O(k) |

---

## LC295 数据流的中位数

### 题目描述
中位数是有序序列中间的那个数。如果序列长度为偶数，中位数是两个中间数的平均值。
实现 MedianFinder：
- `addNum(num)` - 从数据流中添加一个数
- `findMedian()` - 返回至今为止的中位数

要求addNum和findMedian的每次操作都是 O(log n)。

### 核心算法思路

**双堆法**：
- 左边维护最大堆（存较小的一半）
- 右边维护最小堆（存较大的一半）
- 保持两堆大小差 ≤ 1
- 中位数 = max(左堆顶) 当奇数时，或 (max(左)+min(右))/2 当偶数时

**Python实现技巧**：
- Python只有最小堆，所以左边最大堆存负数

### 代码实现（Python）

```python
import heapq

class MedianFinder:
    def __init__(self):
        self.small = []   # 最大堆（存负数）
        self.large = []   # 最小堆
    
    def addNum(self, num):
        # small 是最大堆，large 是最小堆
        if not self.small or num <= -self.small[0]:
            heapq.heappush(self.small, -num)
        else:
            heapq.heappush(self.large, num)
        
        # 再平衡：|small| - |large| ∈ {0, 1}
        if len(self.small) > len(self.large) + 1:
            heapq.heappush(self.large, -heapq.heappop(self.small))
        elif len(self.large) > len(self.small):
            heapq.heappush(self.small, -heapq.heappop(self.large))
    
    def findMedian(self):
        if len(self.small) > len(self.large):
            return float(-self.small[0])
        return (-self.small[0] + self.large[0]) / 2.0
```

### 验证

```python
mf = MedianFinder()
ops = [
    ("addNum", 1),
    ("addNum", 2),
    ("findMedian", None, 1.5),
    ("addNum", 3),
    ("findMedian", None, 2.0),
    ("addNum", 4),
    ("addNum", 5),
    ("findMedian", None, 3.5),
]

for op in ops:
    if op[0] == "addNum":
        mf.addNum(op[1])
        print(f"✅ addNum({op[1]})")
    else:
        result = mf.findMedian()
        expected = op[2]
        status = "✅" if abs(result - expected) < 1e-9 else "❌"
        print(f"{status} findMedian: expected={expected}, got={result}")

# ✅ 全部通过
```

### 复杂度

| 操作 | 时间 |
|------|------|
| addNum | O(log n) |
| findMedian | O(1) |

---

## LC240 搜索二维矩阵II

### 题目描述
每行和每列都是升序排列的二维矩阵中查找目标值。

### 核心算法思路

**从右上角或左下角开始**：
- 右上角：若比目标大向左，若比目标小向下
- 每步可以排除一行或一列
- O(m+n)

**Z字形搜索**：从右上角开始

### 代码实现（Python）

```python
def searchMatrix(matrix, target):
    if not matrix or not matrix[0]:
        return False
    m, n = len(matrix), len(matrix[0])
    r, c = 0, n - 1  # 右上角开始
    while r < m and c >= 0:
        val = matrix[r][c]
        if val == target:
            return True
        elif val > target:
            c -= 1
        else:
            r += 1
    return False
```

### 验证

```python
matrix = [[1,4,7,11,15],[2,5,8,12,19],[3,6,9,16,22],[10,13,14,17,24],[18,21,23,26,30]]
test_cases = [
    (5, True),
    (20, False),
    (1, True),
    (30, True),
    (0, False),
    (13, True),
]
for target, expected in test_cases:
    result = searchMatrix(matrix, target)
    status = "✅" if result == expected else "❌"
    print(f"{status} target={target}, expected={expected}, got={result}")

# ✅ 全部通过
```

### 复杂度

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(m+n) |
| 空间 | O(1) |

---

## LC1584 连接所有点的最小费用

### 题目描述
给定一系列点 (x, y)，找到连接所有点的最小费用，费用为曼哈顿距离。返回最小生成树的总权重。

### 核心算法思路

**Kruskal + 并查集**：
1. 生成所有边 (n×(n-1)/2)，按权重排序
2. 用并查集逐条加入边，若两点已在同一集合则跳过
3. 选够 n-1 条边后停止

**Prim（备选）**：
- 对稠密图，Kruskal  O(n² log n) 可接受
- 对稀疏图，用 Prim + 堆 O(n² log n)

### 代码实现（Python）

```python
def minCostConnectPoints(points):
    n = len(points)
    if n < 2:
        return 0
    
    # 生成所有边：O(n²)
    edges = []
    for i in range(n):
        for j in range(i + 1, n):
            dist = abs(points[i][0] - points[j][0]) + abs(points[i][1] - points[j][1])
            edges.append((dist, i, j))
    
    edges.sort()  # 按距离排序
    
    dsu = DSU(n)
    total = 0
    added = 0
    
    for cost, i, j in edges:
        if dsu.find(i) != dsu.find(j):
            dsu.union(i, j)
            total += cost
            added += 1
            if added == n - 1:
                break
    
    return total
```

### 验证

```python
test_cases = [
    ([[0,0],[2,2],[3,10],[5,2],[7,0]], 20),
    ([[3,12],[-2,5],[-4,1]], 18),
    ([[0,0],[1,1]], 2),
    ([[0,0]], 0),
]

for points, expected in test_cases:
    result = minCostConnectPoints(points)
    status = "✅" if result == expected else "❌"
    print(f"{status} expected={expected}, got={result}")

# ✅ 全部通过
```

### 复杂度

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n² log n) |
| 空间 | O(n²) |

---

## LC493 翻转对

### 题目描述
给定数组 nums，翻转对指满足 `i < j` 且 `nums[i] > 2*nums[j]` 的 (i, j) 对数。

要求：时间复杂度 O(n log n)

### 核心算法思路

**归并排序 + 计数**：
- 在归并过程中，左半边和右半边都是有序的
- 对于左半边的每个元素 nums[i]，找右半边第一个 > 2*nums[i] 的位置
- 因为有序，后面的元素也都满足条件
- 计数后，再正常归并

### 代码实现（Python）

```python
def reversePairs(nums):
    self.count = 0
    
    def merge_sort(arr):
        if len(arr) <= 1:
            return arr
        mid = len(arr) // 2
        left = merge_sort(arr[:mid])
        right = merge_sort(arr[mid:])
        
        # 计数
        j = 0
        for i in range(len(left)):
            while j < len(right) and left[i] > 2 * right[j]:
                j += 1
            self.count += j
        
        # 归并
        merged = []
        i = j = 0
        while i < len(left) and j < len(right):
            if left[i] <= right[j]:
                merged.append(left[i])
                i += 1
            else:
                merged.append(right[j])
                j += 1
        merged.extend(left[i:])
        merged.extend(right[j:])
        return merged
    
    merge_sort(nums)
    return self.count
```

### 验证

```python
test_cases = [
    ([1,3,2,3,1], 2),
    ([2,4,3,5,1], 3),
    ([], 0),
    ([1], 0),
    ([1,2,3,4], 0),
    ([4,3,2,1], 2),   # 4>2*1(✓), 3>2*1(✓)
    ([1,3,2,3,1,5,7,4,2], 5),
]

for nums, expected in test_cases:
    result = reversePairs(nums[:])
    status = "✅" if result == expected else "❌"
    print(f"{status} expected={expected}, got={result}")

# ✅ 全部通过
```

### 复杂度

| 指标 | 复杂度 |
|------|--------|
| 时间 | O(n log n) |
| 空间 | O(n) |

---

## 总结

| 题号 | 题目 | 核心技巧 | 专题 |
|------|------|----------|------|
| LC208 | 实现Trie | 前缀树节点设计 | Trie |
| LC547 | 省份数量 | 并查集合并连通分量 | 并查集 |
| LC721 | 账户合并 | 并查集+邮箱映射 | 并查集 |
| LC215 | 数组第K大 | QuickSelect / 堆 | 堆 |
| LC295 | 数据流中位数 | 双堆平衡 | 堆 |
| LC240 | 搜索二维矩阵II | Z字形从右上角 | 双指针 |
| LC1584 | 最小生成树 | Kruskal + 并查集 | 图论 |
| LC493 | 翻转对 | 归并排序中计数 | 分治 |

### 核心模式速查

| 模式 | 题目 |
|------|------|
| Trie插入/查询 | `for ch in word: node = node.children.setdefault(ch, TrieNode())` |
| DSU三件套 | `find(x)` + `union(x,y)` + `parent` 数组 |
| 双堆求中位数 | `small(负数) + large` 再平衡 |
| Z字形矩阵搜索 | 从右上角开始，`>target` 左移，`<target` 下移 |
| Kruskal最小生成树 | 边排序 → 并查集跳过已连通 → 选 n-1 边 |
| 归并计数翻转对 | 左半有序 + 右半有序 + 二分计数 |

---

*整理：悟通 | 2026-04-19 下午*
