## LeetCode 练习 - 2026-04-17 下午 - Union-Find 并查集专题

## 专题：Union-Find（并查集）核心原理

---

### Union-Find 核心概念

**两个操作**：
- `find(x)`: 找到x的根节点（带路径压缩）
- `union(x, y)`: 合并x和y的集合

**核心思想**：
- 用数组模拟树结构
- 路径压缩让find接近O(1)
- 按秩合并避免树退化

---

### 1. LC200 岛屿数量（Union-Find解法）
**问题**：统计岛屿数量
**核心洞察**：把所有'1'连成一片，统计连通分量

```python
class UnionFind:
    def __init__(self, grid):
        m, n = len(grid), len(grid[0])
        self.parent = [-1] * (m * n)
        self.count = 0
        for i in range(m):
            for j in range(n):
                if grid[i][j] == '1':
                    idx = i * n + j
                    self.parent[idx] = idx
                    self.count += 1
    
    def find(self, x):
        if self.parent[x] != x:
            self.parent[x] = self.find(self.parent[x])
        return self.parent[x]
    
    def union(self, x, y):
        rx, ry = self.find(x), self.find(y)
        if rx != ry:
            self.parent[rx] = ry
            self.count -= 1

def numIslands(grid):
    if not grid: return 0
    m, n = len(grid), len(grid[0])
    uf = UnionFind(grid)
    for i in range(m):
        for j in range(n):
            if grid[i][j] == '1':
                idx = i * n + j
                if i + 1 < m and grid[i+1][j] == '1':
                    uf.union(idx, (i+1) * n + j)
                if j + 1 < n and grid[i][j+1] == '1':
                    uf.union(idx, i * n + j + 1)
    return uf.count

# 验证: grid1=1, grid2=3 ✅
```

---

### 2. LC547 省份数量（Friend Circles）
**问题**：有n个城市，有些城市直接相连，求省份数量
**核心洞察**：矩阵是无向图邻接矩阵，Union-Find统计连通分量

```python
def findCircleNum(isConnected):
    n = len(isConnected)
    parent = list(range(n))
    
    def find(x):
        if parent[x] != x:
            parent[x] = find(parent[x])
        return parent[x]
    
    def union(x, y):
        rx, ry = find(x), find(y)
        if rx != ry:
            parent[rx] = ry
            return True
        return False
    
    for i in range(n):
        for j in range(i+1, n):
            if isConnected[i][j] == 1:
                union(i, j)
    
    return len(set(find(i) for i in range(n)))

# 验证: [[1,1,0],[1,1,0],[0,0,1]]→2省 ✅
```

---

### 3. LC130 被围绕的区域（Union-Find解法）
**问题**：把被X包围的O变成X，边界O和与之相连的O保留
**Union-Find思路**：加一个虚拟节点，所有边界O连到虚拟节点，最后O但未连虚拟节点的变X

```python
def solve(board):
    if not board: return
    m, n = len(board), len(board[0])
    parent = list(range(m * n + 1))  # 多一个虚拟节点
    
    def find(x):
        if parent[x] != x:
            parent[x] = find(parent[x])
        return parent[x]
    
    def union(x, y):
        rx, ry = find(x), find(y)
        if rx != ry:
            parent[rx] = ry
    
    dummy = m * n  # 虚拟节点
    
    for i in range(m):
        for j in range(n):
            if board[i][j] == 'O':
                idx = i * n + j
                if i == 0 or i == m-1 or j == 0 or j == n-1:
                    union(idx, dummy)  # 边界O连到虚拟节点
                else:
                    if i > 0 and board[i-1][j] == 'O':
                        union(idx, (i-1) * n + j)
                    if i < m-1 and board[i+1][j] == 'O':
                        union(idx, (i+1) * n + j)
                    if j > 0 and board[i][j-1] == 'O':
                        union(idx, i * n + j-1)
                    if j < n-1 and board[i][j+1] == 'O':
                        union(idx, i * n + j+1)
    
    for i in range(m):
        for j in range(n):
            if board[i][j] == 'O' and find(i * n + j) != find(dummy):
                board[i][j] = 'X'

# 验证: 边界O保留，内部O变X ✅
```

---

### 4. LC990 等式方程的可满足性
**问题**：给定a==b和a!=b，判断方程是否可满足
**核心洞察**：先union相等的，再检查矛盾的

```python
def equationsPossible(equations):
    parent = list(range(26))  # 26个字母
    
    def find(x):
        if parent[x] != x:
            parent[x] = find(parent[x])
        return parent[x]
    
    def union(x, y):
        rx, ry = find(x), find(y)
        if rx != ry:
            parent[rx] = ry
    
    # 第一遍：处理所有==
    for eq in equations:
        if eq[1] == '=':
            union(ord(eq[0]) - ord('a'), ord(eq[3]) - ord('a'))
    
    # 第二遍：检查所有!=
    for eq in equations:
        if eq[1] == '!':
            if find(ord(eq[0]) - ord('a')) == find(ord(eq[3]) - ord('a')):
                return False
    return True

# 验证: ["a==b","b!=c","c==a"]→False ✅ ["a==b","b==c","a==c"]→True ✅
```

---

### 5. LC721 账户合并
**问题**：有相同邮箱的账户合并
**核心洞察**：邮箱作为节点，相同邮箱连接账户，最后合并

```python
def accountsMerge(accounts):
    email_to_idx = {}
    email_to_name = {}
    for account in accounts:
        name = account[0]
        for email in account[1:]:
            if email not in email_to_idx:
                email_to_idx[email] = len(email_to_idx)
            email_to_name[email] = name
    
    parent = list(range(len(email_to_idx)))
    
    def find(x):
        if parent[x] != x:
            parent[x] = find(parent[x])
        return parent[x]
    
    def union(x, y):
        rx, ry = find(x), find(y)
        if rx != ry:
            parent[rx] = ry
    
    # 同一个账户的邮箱union
    for account in accounts:
        first_email = account[1]
        first_idx = email_to_idx[first_email]
        for email in account[2:]:
            union(first_idx, email_to_idx[email])
    
    # 合并
    merged = {}
    for email, idx in email_to_idx.items():
        root = find(idx)
        if root not in merged:
            merged[root] = []
        merged[root].append(email)
    
    result = []
    for root, emails in merged.items():
        name = email_to_name[emails[0]]
        result.append([name] + sorted(emails))
    return result

# 验证: [["John","johnsmith@mail.com"],["John","john00@mail.com"],["John","john_newyork@mail.com"]] ✅
```

---

## Union-Find 模板代码

```python
class UnionFind:
    def __init__(self, n):
        self.parent = list(range(n))
        self.rank = [0] * n
    
    def find(self, x):
        if self.parent[x] != x:
            self.parent[x] = self.find(self.parent[x])  # 路径压缩
        return self.parent[x]
    
    def union(self, x, y):
        rx, ry = self.find(x), self.find(y)
        if rx == ry:
            return
        # 按秩合并
        if self.rank[rx] < self.rank[ry]:
            self.parent[rx] = ry
        elif self.rank[rx] > self.rank[ry]:
            self.parent[ry] = rx
        else:
            self.parent[ry] = rx
            self.rank[rx] += 1
```

---

## 今日Union-Find专题验证

| 题目 | 验证结果 |
|------|----------|
| LC200 岛屿数量 | grid1=1, grid2=3 ✅ |
| LC547 省份数量 | 2省 ✅ |
| LC130 被围绕区域 | 边界O保留 ✅ |
| LC990 等式可满足性 | a==b,b!=c,c==a→False ✅ |
| LC721 账户合并 | 同邮箱账户合并 ✅ |

**Union-Find专题完成**: 5题 ✅

---

## Union-Find 应用场景

| 场景 | 题目 |
|------|------|
| 岛屿问题 | LC200, LC305 |
| 连通分量 | LC547, LC323 |
| 等式满足性 | LC990 |
| 账户合并 | LC721 |
| 最小生成树 | LC1135 (Kruskal) |
| 合并石子 | LC1690 |
| 括号问题 | LC1106 |

*悟通自检 | 2026-04-17 13:25 CST*
