# LeetCode 高级算法练习 - 2026-04-20

## LC212 单词搜索 II (Hard - Trie+DFS回溯)

### 题目核心要点
- 给定一个 m×n 的字符网格和一个单词列表
- 找到所有可以在网格中被形成的单词
- 单词必须由相邻单元格的字母构成（上下左右四连通）
- 每个单元格在构建单词时不能被重复使用

### 最优解思路
1. **构建 Trie 前缀树**：将所有单词插入 Trie，便于快速查找前缀
2. **DFS回溯**：从每个单元格开始DFS，剪枝条件包括：
   - 当前字符不在 Trie 路径上
   - 已访问过该单元格
3. **优化**：使用 visited 集合记录已访问位置，避免重复搜索

### Python 代码

```python
from typing import List, Set

class TrieNode:
    def __init__(self):
        self.children = {}
        self.word = None

class Solution:
    def findWords(self, board: List[List[str]], words: List[str]) -> List[str]:
        # 构建 Trie
        self.root = TrieNode()
        for word in words:
            node = self.root
            for ch in word:
                if ch not in node.children:
                    node.children[ch] = TrieNode()
                node = node.children[ch]
            node.word = word
        
        self.result = []
        self.m, self.n = len(board), len(board[0])
        
        # 从每个单元格开始DFS
        for i in range(self.m):
            for j in range(self.n):
                if board[i][j] in self.root.children:
                    self._dfs(i, j, self.root, board)
        
        return self.result
    
    def _dfs(self, i: int, j: int, node: TrieNode, board: List[List[str]]):
        ch = board[i][j]
        cur_node = node.children[ch]
        
        # 如果到达单词结尾，记录结果
        if cur_node.word:
            self.result.append(cur_node.word)
            cur_node.word = None  # 避免重复添加
        
        # 标记为已访问
        board[i][j] = '#'
        
        # 四个方向遍历
        for di, dj in [(0,1),(0,-1),(1,0),(-1,0)]:
            ni, nj = i + di, j + dj
            if 0 <= ni < self.m and 0 <= nj < self.n:
                if board[ni][nj] in cur_node.children:
                    self._dfs(ni, nj, cur_node, board)
        
        # 恢复状态
        board[i][j] = ch
        
        # 剪枝：如果当前节点没有子节点，删除它
        if not cur_node.children:
            del node.children[ch]
```

### 验证

```python
# 测试用例
board = [
    ['o','a','a','n'],
    ['e','t','a','e'],
    ['i','h','k','r'],
    ['i','f','l','v']
]
words = ["oath","pea","eat","rain"]
sol = Solution()
print(sol.findWords(board, words))  # Expected: ["eat","oath"]
```

---

## LC297 二叉树的序列化与反序列化 (Hard - BFS/DFS)

### 题目核心要点
- 设计一种算法将二叉树序列化为字符串
- 设计算法从字符串反序列化重建二叉树
- 序列化后应能完整恢复原二叉树结构

### 最优解思路
**方法1：前序遍历 + 哨兵标记**
- 前序遍历：根-左-右
- 使用 '#' 标记空节点
- 反序列化时按前序重建

**方法2：BFS层序遍历**
- 按层遍历，记录空节点
- 更直观，序列化紧凑

### Python 代码

```python
import collections

class TreeNode:
    def __init__(self, x):
        self.val = x
        self.left = None
        self.right = None

class Codec:
    def serialize(self, root: TreeNode) -> str:
        """前序遍历序列化"""
        if not root:
            return "#"
        return str(root.val) + "," + self.serialize(root.left) + "," + self.serialize(root.right)
    
    def deserialize(self, data: str) -> TreeNode:
        """前序遍历反序列化"""
        nodes = collections.deque(data.split(","))
        return self._build(nodes)
    
    def _build(self, nodes: collections.deque) -> TreeNode:
        val = nodes.popleft()
        if val == "#":
            return None
        node = TreeNode(int(val))
        node.left = self._build(nodes)
        node.right = self._build(nodes)
        return node

# 测试
root = TreeNode(1)
root.left = TreeNode(2)
root.right = TreeNode(3)
root.right.left = TreeNode(4)
root.right.right = TreeNode(5)

codec = Codec()
serialized = codec.serialize(root)
print(f"序列化: {serialized}")  # "1,2,#,#,3,4,#,#,5,#,#"

deserialized = codec.deserialize(serialized)

# 验证前序遍历相同
def preorder(node):
    if not node:
        return []
    return [node.val] + preorder(node.left) + preorder(node.right)

print(f"原树前序: {preorder(root)}")
print(f"恢复后前序: {preorder(deserialized)}")
print(f"验证通过: {preorder(root) == preorder(deserialized)}")
```

---

## LC329 矩阵中的最长递增路径 (Hard - DFS+DP+拓扑排序)

### 题目核心要点
- 给定一个 m×n 整数矩阵
- 找到最长的递增路径的长度
- 每次只能朝上下左右四个方向移动
- 必须严格递增

### 最优解思路
**方法：DFS + 记忆化（Top-Down DP）**
1. 对每个单元格进行DFS
2. dp[i][j] 表示从 (i,j) 出发的最长递增路径
3. 递归公式：dp[i][j] = 1 + max(dp[neighbors]) if matrix[neighbor] > matrix[i][j]
4. 使用记忆化避免重复计算

**时间复杂度**：O(m×n)
**空间复杂度**：O(m×n)

### Python 代码

```python
from typing import List

class Solution:
    def longestIncreasingPath(self, matrix: List[List[int]]) -> int:
        if not matrix or not matrix[0]:
            return 0
        
        self.m, self.n = len(matrix), len(matrix[0])
        self.matrix = matrix
        self.dp = [[0] * self.n for _ in range(self.m)]
        self.dirs = [(0,1),(0,-1),(1,0),(-1,0)]
        self.result = 0
        
        for i in range(self.m):
            for j in range(self.n):
                self.result = max(self.result, self._dfs(i, j))
        
        return self.result
    
    def _dfs(self, i: int, j: int) -> int:
        if self.dp[i][j] != 0:
            return self.dp[i][j]
        
        max_len = 1
        for di, dj in self.dirs:
            ni, nj = i + di, j + dj
            if 0 <= ni < self.m and 0 <= nj < self.n:
                if self.matrix[ni][nj] > self.matrix[i][j]:
                    max_len = max(max_len, 1 + self._dfs(ni, nj))
        
        self.dp[i][j] = max_len
        return max_len

# 测试用例
sol = Solution()

# 测试1: 标准递增路径
matrix1 = [
    [9, 9, 4],
    [6, 6, 8],
    [2, 1, 1]
]
print(f"测试1: {sol.longestIncreasingPath(matrix1)}")  # Expected: 4 ([1,2,6,9])

# 测试2: 多个最长路径
matrix2 = [
    [3, 4, 5],
    [3, 2, 6],
    [2, 2, 1]
]
print(f"测试2: {sol.longestIncreasingPath(matrix2)}")  # Expected: 4 ([3,4,5,6] or [3,2,6])

# 测试3: 单个元素
matrix3 = [[1]]
print(f"测试3: {sol.longestIncreasingPath(matrix3)}")  # Expected: 1
```

---

## LC17 电话号码的字母组合 (Medium+ - 回溯)

### 题目核心要点
- 给定一个只包含数字 2-9 的字符串
- 每个数字映射到一些字母（像老式电话按键）
- 返回数字字符串能表示的所有字母组合

### 最优解思路
**回溯算法**：
1. 建立数字到字母的映射表
2. 从第一个数字开始，依次尝试每个字母
3. 递归处理剩余数字
4. 当组合长度等于数字长度时，添加结果

### Python 代码

```python
from typing import List

class Solution:
    def __init__(self):
        self.phone = {
            '2': 'abc',
            '3': 'def',
            '4': 'ghi',
            '5': 'jkl',
            '6': 'mno',
            '7': 'pqrs',
            '8': 'tuv',
            '9': 'wxyz'
        }
    
    def letterCombinations(self, digits: str) -> List[str]:
        self.result = []
        if not digits:
            return self.result
        
        self._backtrack(digits, 0, "")
        return self.result
    
    def _backtrack(self, digits: str, index: int, current: str):
        if index == len(digits):
            self.result.append(current)
            return
        
        letters = self.phone[digits[index]]
        for letter in letters:
            self._backtrack(digits, index + 1, current + letter)

# 测试
sol = Solution()

# 测试1: "23"
result1 = sol.letterCombinations("23")
print(f"测试1 '23': {result1}")
# Expected: ["ad","ae","af","bd","be","bf","cd","ce","cf"]

# 测试2: ""
result2 = sol.letterCombinations("")
print(f"测试2 '': {result2}")  # Expected: []

# 测试3: "2"
result3 = sol.letterCombinations("2")
print(f"测试3 '2': {result3}")  # Expected: ["a","b","c"]

# 验证组合数量
print(f"\n'23' 组合数: {len(result1)} (expected: 9)")
print(f"'234' 组合数: {len(sol.letterCombinations('234'))} (expected: 27)")
```

---

## LC51 N皇后 (Hard - 回溯)

### 题目核心要点
- 在 n×n 的棋盘上放置 n 个皇后
- 皇后可以攻击同一行、同一列、同一对角线的其他皇后
- 求所有可能的皇后放置方案

### 最优解思路
**回溯算法 + 集合剪枝**：
1. 使用三个集合记录已占用的列、正斜线、反斜线
2. 递归尝试在每一行放置皇后
3. 剪枝条件：检查列和对角线是否冲突

**时间复杂度**：O(n!)
**空间复杂度**：O(n)

### Python 代码

```python
from typing import List, Set

class Solution:
    def solveNQueens(self, n: int) -> List[List[str]]:
        self.result = []
        self.n = n
        
        # 三个集合用于剪枝
        self.cols = set()      # 已占用列
        self.diag1 = set()     # 主对角线 (row - col)
        self.diag2 = set()     # 副对角线 (row + col)
        
        self._backtrack(0, [])
        return self.result
    
    def _backtrack(self, row: int, current: List[str]):
        if row == self.n:
            self.result.append(current[:])
            return
        
        for col in range(self.n):
            # 剪枝：检查列和对角线冲突
            if col in self.cols or (row - col) in self.diag1 or (row + col) in self.diag2:
                continue
            
            # 做选择
            self.cols.add(col)
            self.diag1.add(row - col)
            self.diag2.add(row + col)
            
            # 添加当前行
            current.append('.' * col + 'Q' + '.' * (self.n - col - 1))
            
            # 递归
            self._backtrack(row + 1, current)
            
            # 撤销选择
            current.pop()
            self.cols.remove(col)
            self.diag1.remove(row - col)
            self.diag2.remove(row + col)
    
    def print_solution(self, boards: List[List[str]]):
        """打印所有解决方案"""
        for i, board in enumerate(boards):
            print(f"方案 {i+1}:")
            for row in board:
                print(f"  {row}")
            print()

# 测试
sol = Solution()

# 测试 n=4
result4 = sol.solveNQueens(4)
print(f"4皇后解法数: {len(result4)} (expected: 2)")
sol.print_solution(result4)

# 测试 n=1
result1 = sol.solveNQueens(1)
print(f"1皇后解法数: {len(result1)} (expected: 1)")

# 测试 n=8
result8 = sol.solveNQueens(8)
print(f"8皇后解法数: {len(result8)} (expected: 92)")
```

---

## 总结

| 题号 | 题目 | 难度 | 核心技巧 | 时间复杂度 |
|------|------|------|----------|------------|
| LC212 | 单词搜索 II | Hard | Trie + DFS回溯 | O(m×n×4^L) |
| LC297 | 二叉树的序列化与反序列化 | Hard | 前序遍历 + 递归重建 | O(n) |
| LC329 | 矩阵中的最长递增路径 | Hard | DFS + 记忆化DP | O(m×n) |
| LC17 | 电话号码的字母组合 | Medium+ | 回溯 | O(4^n) |
| LC51 | N皇后 | Hard | 回溯 + 集合剪枝 | O(n!) |

### 关键模式总结
1. **回溯三要素**：选择列表、路径、结束条件
2. **剪枝技巧**：集合判断冲突、限制搜索范围
3. **记忆化**：DFS+DP避免重复计算
4. **Trie应用**：前缀匹配、路径压缩

---

*练习时间: 2026-04-20*
*Agent: 悟空 LC练习助手*
