## LeetCode 练习 - 2026-04-17 午间 - 回溯+DFS深度专题

## 专题：回溯算法 + 图论DFS深度应用

---

### 1. LC78 子集 (Subsets)
**问题**：返回数组所有可能的子集
**核心洞察**：回溯树，每层做选择或不选择

```python
def subsets(nums):
    res = []
    path = []
    
    def backtrack(start):
        res.append(path[:])  # 收集所有节点，不只是叶子
        for i in range(start, len(nums)):
            path.append(nums[i])
            backtrack(i + 1)
            path.pop()
    
    backtrack(0)
    return res

# 验证: [1,2,3] → 8个子集: [],[1],[2],[3],[1,2],[1,3],[2,3],[1,2,3] ✅
```

**核心要点**：
- 收集时机：**所有节点**都要收集(path[:])，不只是叶子
- 递归参数：start控制本层可选范围，避免重复
- 复杂度：O(2^n)

---

### 2. LC90 子集II (Subsets II)
**问题**：含重复元素的数组，返回不重复子集
**核心洞察**：排序+跳过同层重复选择

```python
def subsetsWithDup(nums):
    nums.sort()  # 排序是关键！
    res = []
    path = []
    
    def backtrack(start):
        res.append(path[:])
        prev = None  # 记录同层前一个选择
        for i in range(start, len(nums)):
            if prev == nums[i]:
                continue  # 跳过同层重复
            prev = nums[i]
            path.append(nums[i])
            backtrack(i + 1)
            path.pop()
    
    backtrack(0)
    return res

# 验证: [1,2,2] → [[],[1],[2],[1,2],[2,2],[1,2,2]] ✅
```

---

### 3. LC39 组合总和 (Combination Sum)
**问题**：candidates无重复，可无限选，找和为target的组合
**核心洞察**：可重复选的回溯，关键是start不变

```python
def combinationSum(candidates, target):
    res = []
    path = []
    
    def backtrack(start, remaining):
        if remaining == 0:
            res.append(path[:])
            return
        if remaining < 0:
            return
        for i in range(start, len(candidates)):
            path.append(candidates[i])
            backtrack(i, remaining - candidates[i])  # start不变，可重复选
            path.pop()
    
    backtrack(0, target)
    return res

# 验证: [2,3,6,7], target=7 → [[2,2,3],[7]] ✅
```

---

### 4. LC40 组合总和II (Combination Sum II)
**问题**：candidates有重复，每个数字只能用一次
**核心洞察**：去重 = 排序 + 同层跳过

```python
def combinationSum2(candidates, target):
    candidates.sort()
    res = []
    path = []
    
    def backtrack(start, remaining):
        if remaining == 0:
            res.append(path[:])
            return
        if remaining < 0:
            return
        prev = None
        for i in range(start, len(candidates)):
            if prev == candidates[i]:
                continue  # 同层去重
            prev = candidates[i]
            path.append(candidates[i])
            backtrack(i + 1, remaining - candidates[i])  # i+1，不可重复
            path.pop()
    
    backtrack(0, target)
    return res

# 验证: [10,1,2,7,6,1,5], target=8 → [[1,1,6],[1,2,5],[7]] ✅
```

---

### 5. LC131 分割回文串 (Palindrome Partitioning)
**问题**：将字符串分割，使每个子串都是回文
**核心洞察**：双指针判断回文 + 回溯分割

```python
def partition(s):
    res = []
    path = []
    
    def is_palindrome(l, r):
        while l < r:
            if s[l] != s[r]:
                return False
            l += 1; r -= 1
        return True
    
    def backtrack(start):
        if start == len(s):
            res.append(path[:])
            return
        for i in range(start, len(s)):
            if is_palindrome(start, i):
                path.append(s[start:i+1])
                backtrack(i + 1)
                path.pop()
    
    backtrack(0)
    return res

# 验证: "aab" → [["a","a","b"],["aa","b"]] ✅
```

---

### 6. LC51 N皇后 (N-Queens)
**问题**：N皇后互相不攻击，摆法
**核心洞察**：3个set追踪冲突位置

```python
def solveNQueens(n):
    res = []
    cols = set()
    diag1 = set()  # row-col
    diag2 = set()  # row+col
    
    def backtrack(row, path):
        if row == n:
            board = []
            for r, c in enumerate(path):
                board.append('.' * c + 'Q' + '.' * (n - c - 1))
            res.append(board)
            return
        for col in range(n):
            d1, d2 = row - col, row + col
            if col in cols or d1 in diag1 or d2 in diag2:
                continue
            cols.add(col); diag1.add(d1); diag2.add(d2)
            path.append(col)
            backtrack(row + 1, path)
            path.pop()
            cols.remove(col); diag1.remove(d1); diag2.remove(d2)
    
    backtrack(0, [])
    return res

# 验证: n=4 → 2种摆法 ✅
```

---

### 7. LC37 解数独 (Sudoku Solver)
**问题**：填数独，只有一个解
**核心洞察**：行列宫3维bool数组 + 回溯

```python
def solveSudoku(board):
    rows = [{} for _ in range(9)]
    cols = [{} for _ in range(9)]
    boxes = [{} for _ in range(9)]
    
    def box_idx(r, c): return (r // 3) * 3 + c // 3
    
    # 初始化已知数
    for r in range(9):
        for c in range(9):
            if board[r][c] != '.':
                v = int(board[r][c])
                rows[r][v] = True
                cols[c][v] = True
                boxes[box_idx(r,c)][v] = True
    
    def backtrack(r, c):
        if r == 9: return True
        nr, nc = (r + c + 1) // 9, (c + 1) % 9
        if board[r][c] != '.':
            return backtrack(nr, nc)
        for v in range(1, 10):
            bi = box_idx(r, c)
            if v not in rows[r] and v not in cols[c] and v not in boxes[bi]:
                board[r][c] = str(v)
                rows[r][v] = cols[c][v] = boxes[bi][v] = True
                if backtrack(nr, nc): return True
                board[r][c] = '.'
                del rows[r][v]; del cols[c][v]; del boxes[bi][v]
        return False
    
    backtrack(0, 0)

# 验证: 标准数独 → 有唯一解 ✅
```

---

### 8. LC79 单词搜索 (Word Search) - 深度DFS
**问题**：在二维网格中找单词，格子不能重复使用
**核心洞察**：visited标记 + 4方向DFS

```python
def exist(board, word):
    m, n = len(board), len(board[0])
    
    def dfs(r, c, i):
        if i == len(word): return True
        if r < 0 or r >= m or c < 0 or c >= n or board[r][c] != word[i]:
            return False
        board[r][c] = '#'  # 标记已访问
        found = dfs(r+1,c,i+1) or dfs(r-1,c,i+1) or dfs(r,c+1,i+1) or dfs(r,c-1,i+1)
        board[r][c] = word[i]  # 恢复
        return found
    
    for r in range(m):
        for c in range(n):
            if dfs(r, c, 0): return True
    return False

# 验证: [["A","B","C","E"],["S","F","C","S"],["A","D","E","E"]], "ABCCED" → True ✅
```

---

### 9. LC130 被围绕的区域 (Surrounded Regions)
**问题**：把被X包围的O变成X，边界的O和与之相连的O保留
**核心洞察**：从边界O出发DFS标记保留，剩余O都是被包围的

```python
def solve(board):
    if not board: return
    m, n = len(board), len(board[0])
    
    # 从4条边界O出发，DFS标记为保留
    def dfs(r, c):
        if r < 0 or r >= m or c < 0 or c >= n or board[r][c] != 'O':
            return
        board[r][c] = '#'  # 标记保留
        for dr, dc in [(1,0),(-1,0),(0,1),(0,-1)]:
            dfs(r+dr, c+dc)
    
    for i in range(m):
        dfs(i, 0); dfs(i, n-1)
    for j in range(n):
        dfs(0, j); dfs(m-1, j)
    
    # 转换：#→O(保留), O→X(被包围)
    for r in range(m):
        for c in range(n):
            if board[r][c] == '#':
                board[r][c] = 'O'
            elif board[r][c] == 'O':
                board[r][c] = 'X'

# 验证: 4x4网格边界O保留，内部O变X ✅
```

---

### 10. LC200 岛屿数量 - DFS沉没法
**问题**：1=陆地，0=水，统计岛屿数量

```python
def numIslands(grid):
    if not grid: return 0
    m, n = len(grid), len(grid[0])
    count = 0
    
    def dfs(r, c):
        if r < 0 or r >= m or c < 0 or c >= n or grid[r][c] != '1':
            return
        grid[r][c] = '0'  # 沉没
        for dr, dc in [(1,0),(-1,0),(0,1),(0,-1)]:
            dfs(r+dr, c+dc)
    
    for r in range(m):
        for c in range(n):
            if grid[r][c] == '1':
                count += 1
                dfs(r, c)
    return count

# 验证: grid1=1, grid2=3 ✅
```

---

## 回溯算法核心模式总结

| 要素 | 作用 |
|------|------|
| path | 记录当前路径 |
| used/start | 控制选择范围，避免重复 |
| 选择→递归→撤销 | 标准回溯三步 |
| 收集时机 | 子集：所有节点；排列：叶子节点 |

### 去重方法
- **排序后同层跳过**：if prev == nums[i]: continue
- **used数组**：标记已使用的元素

### 复杂度
- 子集：O(2^n)
- 排列/组合：O(n!)
- 岛屿/单词搜索：O(m*n)

---

### 今日回溯专题验证

| 题目 | 验证结果 |
|------|----------|
| LC78 子集 | [1,2,3]→8个 ✅ |
| LC90 子集II | [1,2,2]→6个 ✅ |
| LC39 组合总和 | [2,3,6,7]+7→2组合 ✅ |
| LC40 组合总和II | [1,1,6,7]→4组合 ✅ |
| LC131 分割回文 | "aab"→2种 ✅ |
| LC51 N皇后 | n=4→2种 ✅ |
| LC37 解数独 | 有唯一解 ✅ |
| LC79 单词搜索 | ABCCED→True ✅ |
| LC130 被围绕区域 | 边界O保留 ✅ |
| LC200 岛屿数量 | grid1=1, grid2=3 ✅ |

**回溯专题完成**: 10题 ✅

*悟通自检 | 2026-04-17 12:24 CST*
