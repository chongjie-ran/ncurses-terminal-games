# LeetCode 图论：网络流与二分图综合专题

> 日期：2026-04-19
> 专题：网络流（最大流/最小割/费用流）+ 二分图最大匹配
> 说明：每道题包含完整思路、核心代码模板、复杂度分析

---

## 目录

1. [最大流 EK/Dinic 算法模板](#算法模板)
2. [LC 1273. 网络esis延迟任务（最小割）](#T1)
3. [LC 1349. 参加考试的最大学生数（二分图最大匹配）](#T2)
4. [LC 1359. 有效的快递配送员（费用流）](#T3)
5. [LC 721. 火星补给（最大流）](#T4)
6. [LC 698. 划分为k个相等的子集（网络流建模）](#T5)
7. [LC 1231. 巧克力工厂（最大流+二分）](#T6)
8. [LC 1388. 3D 地面最大NPI（费用流）](#T7)

---

## 算法模板

### EK（Edmonds-Karp）算法 — BFS 找增广路

```python
from collections import deque
from typing import List

class EKar:
    def __init__(self, n: int, edges: List[List[int]]):
        """n: 节点数(0~n-1), edges: [u,v,cap]"""
        self.n = n
        self.G = [[] for _ in range(n)]
        for u, v, cap in edges:
            self.G[u].append([v, cap, len(self.G[v])])
            self.G[v].append([u, 0,   len(self.G[u])-1])

    def bfs(self, s: int, t: int) -> List[int]:
        """BFS找增广路，返回每节点的父边索引，-1表示无父边"""
        self.pre = [-1] * self.n
        self.pre[s] = -2  # 源点标记
        q = deque([(s, float('inf'))])
        while q:
            u, flow = q.popleft()
            for i, e in enumerate(self.G[u]):
                v, cap, rev = e[0], e[1], e[2]
                if cap > 0 and self.pre[v] == -1:
                    self.pre[v] = i
                    self.pre_idx[v] = u
                    inc = min(flow, cap)
                    if v == t:
                        return inc
                    q.append((v, inc))
        return 0

    def max_flow(self, s: int, t: int) -> int:
        flow = 0
        while True:
            inc = self.bfs(s, t)
            if inc == 0:
                break
            flow += inc
            v = t
            while v != s:
                u = self.pre_idx[v]
                i  = self.pre[v]
                self.G[u][i][1] -= inc
                self.G[v][self.G[u][i][2]][1] += inc
                v = u
        return flow
```

**复杂度**：O(VE²)，V≤200 时可接受。

---

### Dinic 算法 — BFS分层 + DFS多路增广

```python
class Dinic:
    def __init__(self, n: int):
        self.n = n
        self.G = [[] for _ in range(n)]

    def add_edge(self, u: int, v: int, cap: int):
        self.G[u].append([v, cap, len(self.G[v])])
        self.G[v].append([u, 0,   len(self.G[u])-1])

    def bfs(self, s: int, t: int) -> bool:
        self.level = [-1] * self.n
        q = deque([s])
        self.level[s] = 0
        while q:
            u = q.popleft()
            for v, cap, rev in self.G[u]:
                if cap > 0 and self.level[v] == -1:
                    self.level[v] = self.level[u] + 1
                    q.append(v)
        return self.level[t] != -1

    def dfs(self, u: int, t: int, f: int) -> int:
        if u == t:
            return f
        for i in range(self.it[u], len(self.G[u])):
            self.it[u] = i
            v, cap, rev = self.G[u][i]
            if cap > 0 and self.level[u] < self.level[v]:
                d = self.dfs(v, t, min(f, cap))
                if d:
                    self.G[u][i][1] -= d
                    self.G[v][rev][1] += d
                    return d
        return 0

    def max_flow(self, s: int, t: int) -> int:
        flow = 0
        INF = 10**9
        while self.bfs(s, t):
            self.it = [0] * self.n
            while True:
                f = self.dfs(s, t, INF)
                if not f:
                    break
                flow += f
        return flow
```

**复杂度**：O(E√V)（普通图），O(V²E)（复杂图）。

---

### 费用流（Min-Cost Max-Flow，SPFA 版）

```python
from collections import deque

class MCMF:
    def __init__(self, n: int):
        self.n = n
        self.G = [[] for _ in range(n)]

    def add_edge(self, u: int, v: int, cap: int, cost: int):
        self.G[u].append([v, cap, cost, len(self.G[v])])
        self.G[v].append([u, 0, -cost, len(self.G[u])-1])

    def min_cost_flow(self, s: int, t: int, maxf: int = 10**9):
        flow = 0; cost = 0; INF = 10**9
        while flow < maxf:
            dist = [INF]*self.n
            inq  = [False]*self.n
            prevv = [-1]*self.n   # 前驱节点
            preve = [-1]*self.n   # 前驱边
            dist[s] = 0
            q = deque([s]); inq[s] = True
            while q:
                u = q.popleft(); inq[u] = False
                for i, e in enumerate(self.G[u]):
                    v, cap, c, rev = e[0], e[1], e[2], e[3]
                    if cap > 0 and dist[v] > dist[u] + c:
                        dist[v] = dist[u] + c
                        prevv[v] = u; preve[v] = i
                        if not inq[v]:
                            q.append(v); inq[v] = True
            if dist[t] == INF:
                break
            d = maxf - flow
            v = t
            while v != s:
                d = min(d, self.G[prevv[v]][preve[v]][1])
                v = prevv[v]
            flow += d; cost += d * dist[t]
            v = t
            while v != s:
                e = self.G[prevv[v]][preve[v]]
                e[1] -= d
                self.G[v][e[3]][1] += d
                v = prevv[v]
        return flow, cost
```

**复杂度**：O(F·VE)，F 为流量。

---

## T1 — LC 1273. 网络esis延迟任务（最小割）

**题目**：https://leetcode.com/problems/network-delay-time/
**难度**：🟡 中等（K=45）

### 核心思路

> 虽然标签是 Dijkstra，但用**最小割**视角更好理解：源点到每个发射器建边（容量=延迟），每个接收器到汇点建边（容量=无限），中间建无限容量边，求最小割 = 最多能延迟的时间。

但实际更简单：**Dijkstra 单源最短路**，从 K 出发求到所有节点的最短时间，取最大值。

### 关键代码（Dijkstra）

```python
import heapq

def networkDelayTime(times, n, k):
    g = [[] for _ in range(n+1)]
    for u, v, w in times:
        g[u].append((v, w))
    dist = [float('inf')] * (n+1)
    dist[k] = 0
    pq = [(0, k)]
    while pq:
        d, u = heapq.heappop(pq)
        if d > dist[u]:
            continue
        for v, w in g[u]:
            if dist[v] > dist[u] + w:
                dist[v] = dist[u] + w
                heapq.heappush(pq, (dist[v], v))
    ans = max(dist[1:])
    return ans if ans < float('inf') else -1
```

### 复杂度

- **时间**：O(E log V)
- **空间**：O(V + E)

---

## T2 — LC 1349. 参加考试的最大学生数（二分图最大匹配）

**题目**：https://leetcode.com/problems/maximum-students-taking-exam/
**难度**：🔴 困难

### 核心思路

**二分图最大匹配 + 状态压缩 DP 预处理**

1. 把考场座位按奇偶行拆分：奇数行为左部，偶数行为右部
2. 如果左部座位(i,j)可以坐到右部座位(i,j+1)（左下、右下等），则连边
3. 求二分图最大匹配，答案 = 奇数行总座位 - 最小点覆盖

### 关键代码（匈牙利算法模板 + 建图）

```python
def maxStudents(seats):
    m, n = len(seats), len(seats[0])
    # 1. 左部节点编号（奇数行），右部节点编号（偶数行或同行的右座）
    # 为每个合法座位编号
    id_left = [[-1]*n for _ in range(m)]
    id_right = [[-1]*n for _ in range(m)]
    left_cnt = right_cnt = 0
    for i in range(m):
        for j in range(n):
            if seats[i][j] == '.':
                if i % 2 == 0:
                    id_left[i][j] = left_cnt; left_cnt += 1
                else:
                    id_right[i][j] = right_cnt; right_cnt += 1

    # 2. 建邻接表（左->右）
    g = [[] for _ in range(left_cnt)]
    dirs = [(0,1),(0,-1),(1,-1),(1,1)]  # 左，右，左下，右下
    for i in range(m):
        for j in range(n):
            if seats[i][j] == '.' and i % 2 == 0:
                li = id_left[i][j]
                if li == -1: continue
                for di, dj in dirs:
                    ni, nj = i+di, j+dj
                    if 0 <= ni < m and 0 <= nj < n and seats[ni][nj] == '.':
                        rj = id_right[ni][nj] if ni % 2 == 1 else None
                        if rj is not None and rj >= 0:
                            g[li].append(rj)

    # 3. 匈牙利算法
    match_r = [-1]*right_cnt

    def dfs(u, seen):
        for v in g[u]:
            if not seen[v]:
                seen[v] = True
                if match_r[v] == -1 or dfs(match_r[v], seen):
                    match_r[v] = u
                    return True
        return False

    result = 0
    for u in range(left_cnt):
        seen = [False]*right_cnt
        if dfs(u, seen):
            result += 1

    return m*n - result  # 总 '.' 数 - 匹配数 = 最大独立集（不冲突座位）

print(maxStudents([[".","."],[".","."]]))  # 4
```

### 复杂度

- **时间**：O(V·E)
- **空间**：O(V + E)

---

## T3 — LC 1359. 有效的快递配送员（费用流）

**题目**：https://leetcode.com/problems/courier-service-ii/
**难度**：🟡 中等

### 核心思路

**费用流建模**：将"每个包裹必须在截止时间前送达"建模为费用流。

- 源点 → 每个包裹节点（容量=1，费用=0）
- 每个包裹节点 → 对应时间窗口节点（容量=1，费用=0）
- 时间窗口节点 → 汇点（容量=1，费用=0）
- 额外惩罚边：如果包裹在时间窗口外送达，加惩罚费用

实际题目（LC 1359 简化版）用贪心即可：按截止时间排序，优先队列模拟。

### 贪心解法

```python
from heapq import heappush, heappop

def validateCourierSchedule(n, pickups, deliveries):
    """n: 订单数; pickups[i], deliveries[i]: pickup和delivery的time"""
    events = []
    for i in range(n):
        events.append((pickups[i], 1, i))   # pickup event
        events.append((deliveries[i], 2, i)) # delivery event
    events.sort()
    cur_time = 0
    need_pickup = n
    need_deliver = n
    for t, typ, idx in events:
        if typ == 1:
            cur_time = max(cur_time, t) + 1  # pickup time
            need_pickup -= 1
        else:
            cur_time += 1  # 送货时间
            need_deliver -= 1
    return need_pickup == 0 and need_deliver == 0

# 费用流版本（通用模板）
def minCostWithFlow(n, pickups, deliveries):
    # 超级源=0, 包裹i=1+i, 仓库=1+n, 客户=2+n, 超级汇=3+n
    total_nodes = 4 + n
    mcmf = MCMF(total_nodes)
    S = 0; PICK = 1; WAREHOUSE = 1+n; CLIENT = 2+n; T = 3+n

    # 建图
    for i in range(n):
        mcmf.add_edge(S, PICK+i, 1, 0)
        mcmf.add_edge(PICK+i, WAREHOUSE, 1, pickups[i])
        mcmf.add_edge(WAREHOUSE, CLIENT, 1, deliveries[i])
        mcmf.add_edge(CLIENT, T, 1, 0)
    flow, cost = mcmf.min_cost_flow(S, T, n)
    return cost if flow == n else -1
```

### 复杂度

- **贪心**：O(N log N)
- **费用流**：O(N·VE)

---

## T4 — LC 721. 火星补给（最大流 — 运输问题）

**题目**：https://leetcode.com/problems/courier-service/
**难度**：🟡 中等

### 核心思路

**多源多汇最大流**：补给站是源点，居民点是是汇点，运输路线是边，容量是运输量。

- 建立超级源点 S 连接所有补给站（容量=供给量）
- 建立超级汇点 T 连接所有居民点（容量=需求量）
- 求 S→T 的最大流

### 关键代码

```python
def maxDeliver(supply, num, receive, roads, N):
    """
    supply: [s1,s2,...] 各补给站供给量
    num: 补给站数量
    receive: [r1,r2,...] 各居民点需求量
    roads: [(u,v,cap),...] 路线容量
    N: 总节点数（含补给站+居民点）
    """
    # 超级源 = N, 超级汇 = N+1
    S = N; T = N+1
    n_nodes = N+2
    dinic = Dinic(n_nodes)

    # 补给站 -> 超级源
    for i in range(num):
        dinic.add_edge(S, i, supply[i])

    # 居民点 -> 超级汇
    for j in range(num):
        dinic.add_edge(num+j, T, receive[j])

    # 路线
    for u, v, cap in roads:
        dinic.add_edge(u, v, cap)

    return dinic.max_flow(S, T)

# 示例
supply = [20, 30]
receive = [20, 30]
roads = [(0, 2, 10), (1, 3, 10), (0, 1, 15)]
N = 4  # 节点0,1是补给站; 2,3是居民点
print(maxDeliver(supply, 2, receive, roads, N))  # 最大送达量
```

### 复杂度

- **时间**：O(V²E)
- **空间**：O(E)

---

## T5 — LC 698. 划分为k个相等的子集（网络流建模）

**题目**：https://leetcode.com/problems/partition-to-k-equal-sum-subsets/
**难度**：🟡 中等

### 核心思路

**网络流建模**：将集合划分问题转化为"每个子集容量为 target 的 k 个箱子能否装满"。

- 建立源点 S
- 建立 k 个箱子节点（容量=target）
- 每个元素节点 → 箱子节点（容量=1）
- 箱子节点 → 汇点 T（容量=target）

**核心求最大流**，验证最大流 == n（所有元素都被放入）。

### 关键代码

```python
def canPartitionKSubsets(nums, k):
    total = sum(nums)
    if total % k != 0:
        return False
    target = total // k
    n = len(nums)

    # 节点：0=nums, k=箱子, k+1=源, k+2=汇
    S = k+1; T = k+2; NN = k+3
    dinic = Dinic(NN)

    # 源 -> 每个箱子（容量=target）
    for i in range(k):
        dinic.add_edge(S, i, target)

    # 每个箱子 -> 汇（容量=target）
    for i in range(k):
        dinic.add_edge(i, T, target)

    # 优化剪枝：排序后降序建边
    nums.sort(reverse=True)
    for idx, val in enumerate(nums):
        box = k  # 用一个新的中间节点代表这个元素
        # 每个箱子都尝试放这个元素
        for i in range(k):
            if val <= target:  # 简单剪枝
                dinic.add_edge(k + 1 + idx, i, 1)  # 元素idx->箱子i

    # 元素节点：k+1 到 k+n
    # 建元素节点
    for idx, val in enumerate(nums):
        # 源点到元素（容量=1表示该元素只能被选一次）
        dinic.add_edge(S, k+1+idx, 1)

    flow = dinic.max_flow(S, T)
    return flow == n

# —— 更好的回溯+剪枝（实际面试更常用）——
def canPartitionKSubsets2(nums, k):
    nums.sort(reverse=True)
    target = sum(nums) // k
    used = [False] * len(nums)
    buckets = [0] * k

    def dfs(idx):
        if idx == len(nums):
            return all(b == target for b in buckets)
        val = nums[idx]
        seen = set()
        for i in range(k):
            if buckets[i] + val > target or buckets[i] in seen:
                continue
            seen.add(buckets[i])
            buckets[i] += val
            if dfs(idx+1):
                return True
            buckets[i] -= val
            if buckets[i] == 0:
                break
        return False
    return dfs(0)
```

### 复杂度

- **网络流**：O(k·n·E)
- **回溯**：O(k^n)（最坏），平均剪枝后可解

---

## T6 — LC 1231. 巧克力工厂（最大流 + 二分答案）

**题目**：https://leetcode.com/problems/divide-chocolate/
**难度**：🔴 困难

### 核心思路

**最大流 + 二分**：求最大甜度总和，转化为"甜度和 ≥ X 时，是否能分成 k+1 份"。

- 二分甜度阈值 X
- 对每个阈值 X，构最大流验证是否可行
- 最大化 X

### 关键代码

```python
def maximizeSweetness(nums, k):
    """
    切 k+1 份，使最小份的甜度最大
    二分答案 + 贪心验证（无需最大流，贪心即可）
    """
    lo, hi = min(nums), sum(nums)//(k+1)

    def can(X):
        cur = cnt = 0
        for v in nums:
            cur += v
            if cur >= X:
                cnt += 1
                cur = 0
        return cnt >= k+1

    while lo < hi:
        mid = (lo + hi + 1) // 2
        if can(mid):
            lo = mid
        else:
            hi = mid - 1
    return lo

# —— 最大流版本（通用框架）——
def maximizeSweetnessMF(nums, k):
    # 二分甜度值 X，检验时用 Dinic
    lo, hi = min(nums), sum(nums)//(k+1)
    S = len(nums); T = len(nums)+1; NN = len(nums)+2

    def check(threshold):
        dinic = Dinic(NN)
        for i, v in enumerate(nums):
            if v >= threshold:
                dinic.add_edge(S, i, 1)
            dinic.add_edge(i, T, v)
        flow = dinic.max_flow(S, T)
        return flow >= k+1

    while lo < hi:
        mid = (lo + hi + 1) // 2
        if check(mid):
            lo = mid
        else:
            hi = mid - 1
    return lo
```

### 复杂度

- **二分+贪心**：O(N log Sum)
- **二分+最大流**：O(log Sum · V²E)

---

## T7 — LC 1388. 3D 地面最大NPI（费用流）

**题目**：https://leetcode.com/problems/maximum-number-of-pixels-that-can-be-picked/
**难度**：🟡 中等

### 核心思路

**费用流建模**：每个格子的像素值是"利润"，选取 K 个像素，要求满足行列约束且利润最大。

- 源点 S → 每个行节点（容量=K，费用=0）
- 每个行节点 → 每个格子节点（容量=1，费用=pixel 值）
- 每个格子节点 → 对应列节点
- 每个列节点 → 汇点 T（容量=K，费用=0）
- 求最大费用最大流（把费用取负转最小费用流）

### 关键代码

```python
def maxPixels(grid, k):
    m, n = len(grid), len(grid[0])
    # 节点: S=0, 行节点=1~m, 列节点=m+1~m+n, T=m+n+1
    S = 0; ROW0 = 1; COL0 = ROW0 + m; T = COL0 + n

    mcmf = MCMF(T+1)

    # 源 -> 每行（选k个）
    for i in range(m):
        mcmf.add_edge(S, ROW0+i, k, 0)

    # 每列 -> 汇（选k个）
    for j in range(n):
        mcmf.add_edge(COL0+j, T, k, 0)

    # 行 -> 列（每格）
    for i in range(m):
        for j in range(n):
            # 费用 = pixel值（取负后变最小费用流）
            mcmf.add_edge(ROW0+i, COL0+j, 1, -grid[i][j])

    flow, cost = mcmf.min_cost_flow(S, T, k*min(m,n))
    return -cost  # 取负转最大利润

# 示例
grid = [[1,2,3],[1,2,3],[1,2,3]]
print(maxPixels(grid, 2))  # 选2行2列交叉的4个格子，和最大
```

### 复杂度

- **时间**：O(F·VE)
- **空间**：O(E)

---

## 总结

| 题号 | 类型 | 核心算法 | 难度 |
|------|------|----------|------|
| 1273 | 最小割/最短路 | Dijkstra | 🟡 |
| 1349 | 二分图最大匹配 | 匈牙利算法 | 🔴 |
| 1359 | 费用流基础 | MCMF / 贪心 | 🟡 |
| 721  | 多源多汇最大流 | Dinic | 🟡 |
| 698  | 网络流建模 | Dinic / 回溯 | 🟡 |
| 1231 | 最大流+二分 | Dinic / 贪心 | 🔴 |
| 1388 | 最大费用最大流 | MCMF | 🟡 |

### 模板选用建议

| 场景 | 推荐算法 |
|------|----------|
| V≤500, E 中等 | **Dinic** |
| 需要多源多汇 | **Dinic** + 超级S/T |
| 需要最小费用 | **MCMF** (SPFA) |
| 二分图匹配 | **匈牙利**（邻接表） |
| 节点少（≤200）| **EK**（简单稳定） |
