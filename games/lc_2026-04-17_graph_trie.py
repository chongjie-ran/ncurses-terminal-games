"""
LC练习 - 图论 + Trie + 排序搜索专题
验证完成时间: 2026-04-17 11:17
"""

# ============ 图论 ============
# LC200 岛屿数量 (BFS/DFS)
def num_islands(grid):
    if not grid: return 0
    m, n = len(grid), len(grid[0])
    def dfs(i, j):
        if i<0 or i>=m or j<0 or j>=n or grid[i][j]=='0':
            return
        grid[i][j] = '0'
        for di, dj in [(1,0),(-1,0),(0,1),(0,-1)]:
            dfs(i+di, j+dj)
    count = 0
    for i in range(m):
        for j in range(n):
            if grid[i][j] == '1':
                count += 1
                dfs(i, j)
    return count

g1 = [
  ["1","1","1","1","0"],
  ["1","1","0","1","0"],
  ["1","1","0","0","0"],
  ["0","0","0","0","0"]
]
g2 = [
  ["1","1","0","0","0"],
  ["1","1","0","0","0"],
  ["0","0","1","0","0"],
  ["0","0","0","1","1"]
]
print(f"LC200 岛屿: g1={num_islands([row[:] for row in g1])}, g2={num_islands([row[:] for row in g2])}")

# LC207 课程表 (拓扑排序 - BFS Kahn)
from collections import deque, defaultdict
def canFinish(numCourses, prerequisites):
    g = defaultdict(list)
    indeg = [0]*numCourses
    for a,b in prerequisites:
        g[b].append(a); indeg[a]+=1
    q = deque([i for i in range(numCourses) if indeg[i]==0])
    visited = 0
    while q:
        u = q.popleft(); visited += 1
        for v in g[u]:
            indeg[v] -= 1
            if indeg[v]==0: q.append(v)
    return visited == numCourses

print(f"LC207 课程表: 2={canFinish(2,[[1,0]])}, 2={canFinish(2,[[1,0],[0,1]])}")

# LC417 太平洋大西洋水流 (多源DFS)
def pacificAtlantic(heights):
    if not heights: return []
    m, n = len(heights), len(heights[0])
    pac, atl = [[False]*n for _ in range(m)], [[False]*n for _ in range(m)]
    def dfs(i, j, visited):
        visited[i][j] = True
        for di,dj in [(1,0),(-1,0),(0,1),(0,-1)]:
            ni,nj=i+di,j+dj
            if 0<=ni<m and 0<=nj<n and not visited[ni][nj] and heights[ni][nj]>=heights[i][j]:
                dfs(ni,nj,visited)
    for i in range(m): dfs(i,0,pac), dfs(i,n-1,atl)
    for j in range(n): dfs(0,j,pac), dfs(m-1,j,atl)
    return [[i,j] for i in range(m) for j in range(n) if pac[i][j] and atl[i][j]]

h = [[1,2,2,3,5],[3,2,3,5,4],[6,5,4,4,5],[8,7,6,5,5]]
print(f"LC417 太平洋大西洋: {len(pacificAtlantic(h))}个格子")
for p in pacificAtlantic(h): print(f"  {p}")

# ============ Trie ============
# LC208 实现Trie
class TrieNode:
    def __init__(self):
        self.children = {}
        self.isEnd = False
class Trie:
    def __init__(self):
        self.root = TrieNode()
    def insert(self, word):
        node = self.root
        for c in word:
            if c not in node.children:
                node.children[c] = TrieNode()
            node = node.children[c]
        node.isEnd = True
    def search(self, word):
        node = self._prefix(word)
        return node is not None and node.isEnd
    def startsWith(self, prefix):
        return self._prefix(prefix) is not None
    def _prefix(self, prefix):
        node = self.root
        for c in prefix:
            if c not in node.children:
                return None
            node = node.children[c]
        return node

t = Trie()
t.insert("apple")
print(f"LC208 Trie: search(apple)={t.search('apple')}, search(app)={t.search('app')}, startsWith(app)={t.startsWith('app')}")

# LC79 单词搜索 (回溯)
def exist(board, word):
    m, n = len(board), len(board[0])
    def dfs(i, j, idx):
        if idx == len(word): return True
        if i<0 or i>=m or j<0 or j>=n or board[i][j]!=word[idx]: return False
        board[i][j] = '#'
        for di,dj in [(1,0),(-1,0),(0,1),(0,-1)]:
            if dfs(i+di, j+dj, idx+1): return True
        board[i][j] = word[idx]
        return False
    for i in range(m):
        for j in range(n):
            if dfs(i,j,0): return True
    return False

b = [['A','B','C','E'],['S','F','C','S'],['A','D','E','E']]
print(f"LC79 单词搜索: ABCCED={exist([row[:] for row in b],'ABCCED')}, SEE={exist([row[:] for row in b],'SEE')}, ABCB={exist([row[:] for row in b],'ABCB')}")

# ============ 排序与搜索 ============
# LC215 第K大元素
def findKthLargest(nums, k):
    return sorted(nums, reverse=True)[k-1]

print(f"LC215: 第2大={findKthLargest([3,2,1,5,6,4],2)}, 第4大={findKthLargest([3,2,1,5,6,4],4)}")

# LC347 前K高频 (桶排序)
from collections import Counter
def topKFrequent(nums, k):
    freq = Counter(nums)
    bucket = [[] for _ in range(len(nums)+1)]
    for num, cnt in freq.items():
        bucket[cnt].append(num)
    res = []
    for i in range(len(nums), 0, -1):
        res.extend(bucket[i])
        if len(res) >= k: break
    return res[:k]

print(f"LC347: {topKFrequent([1,1,1,2,2,3],2)}")
print(f"LC347: {topKFrequent([1],1)}")

print("\n✅ 图论+Trie+排序搜索 验证完成")
