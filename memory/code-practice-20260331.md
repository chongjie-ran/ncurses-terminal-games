# 代码练习日志

> 练习时间: 2026-03-31 11:37 - 12:00
> 角色: 悟通（开发者）
> 目标: LeetCode 算法练习 + WASM 浏览器测试

---

## 今日成果概览

| 类别 | 数量 | 详情 |
|------|------|------|
| LeetCode 新题 | 4 道 | LC329, LC743, LC547, LC79 |
| WASM 浏览器测试 | 1 个 | Frogger WASM ✅ 渲染成功 |
| LeetCode 累计 | 88+ 道 | 持续推进 Hard 题目 |

---

## LeetCode 练习（2026-03-31）

### LC329 Longest Increasing Path in a Matrix — 矩阵最长递增路径

**题目**: 矩阵中找最长递增路径长度（上下左右移动）  
**难度**: Hard  
**分类**: DFS + Memoization（矩阵DP）

**核心思路**: 
- `memo[i][j]` = 以 (i,j) 为起点的最长递增路径长度
- 4个方向 DFS，向更大值的格子扩展
- `dp[i][j] = 1 + max(dp[ni][nj])` for all valid larger neighbors
- memo 剪枝，时间 `O(m*n)`

**关键代码**:
```cpp
function<int(int,int)> dfs = [&](int i, int j) -> int {
    if(memo[i][j] != -1) return memo[i][j];
    int best = 1;
    for(auto& d: dirs) {
        int ni = i + d[0], nj = j + d[1];
        if(ni>=0 && ni<m && nj>=0 && nj<n && matrix[ni][nj] > matrix[i][j]) {
            best = max(best, 1 + dfs(ni, nj));
        }
    }
    memo[i][j] = best;
    return best;
};
```

**测试结果**: ✅ `{{9,9,4},{6,6,8},{2,1,1}}` → 4 | `{{3,4,5},{3,2,6},{2,2,1}}` → 4

**关键洞察**: memoization 避免重复计算；矩阵4方向DFS是经典模式

---

### LC743 Network Delay Time — 网络延迟时间

**题目**: 从节点 k 出发，所有节点收到信号的最短时间  
**难度**: Medium  
**分类**: Dijkstra 最短路

**核心思路**: 
- 构建邻接表，从 k 出发 Dijkstra
- `dist[i]` = 从 k 到 i 的最短时间
- 所有可达时返回 `max(dist[i])`；有不可达返回 -1

**关键代码**:
```cpp
priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
pq.push({0, k});
while(!pq.empty()) {
    auto [d, u] = pq.top(); pq.pop();
    if(d > dist[u]) continue;
    for(auto& [v, w]: adj[u]) {
        if(dist[v] > dist[u] + w) {
            dist[v] = dist[u] + w;
            pq.push({dist[v], v});
        }
    }
}
```

**测试结果**: ✅ `times=[[2,1,1],[2,3,1],[3,4,1]], n=4, k=2` → 2 | `[[1,2,1]], n=2, k=1` → 1

**关键洞察**: Dijkstra 堆优化是 O((V+E)logV)；`greater<pair<int,int>>` 实现 min-heap

---

### LC547 Number of Provinces — 省份数量

**题目**: N 个城市，有些城市直接相连（相邻城市在同一省份），求省份数量  
**难度**: Medium  
**分类**: Union-Find / BFS flood fill

**核心思路（Union-Find）**:
- 对每对相连城市 `isConnected[i][j]==1`，union(i,j)
- 统计连通分量数量：`count(where parent[i]==i)`

**关键代码**:
```cpp
UF uf(n);
for(int i=0;i<n;i++)
    for(int j=i+1;j<n;j++)
        if(isConnected[i][j]) uf.unite(i,j);
int provinces = 0;
for(int i=0;i<n;i++) if(uf.find(i)==i) provinces++;
```

**测试结果**: ✅ `[[1,1,0],[1,1,0],[0,0,1]]` → 2 | `[[1,1,1],[1,1,1],[1,1,1]]` → 1

**关键洞察**: Union-Find 适合处理"合并集合+查询连通分量"问题；路径压缩+按秩合并接近 O(1)

---

### LC79 Word Search — 单词搜索

**题目**: 在 2D 网格中找单词（上下左右移动，不能重用格子）  
**难度**: Medium  
**分类**: DFS + Backtracking

**核心思路**:
- 对每个格子作为起点尝试 DFS
- `board[i][j] = '#'` 标记已访问，回溯时恢复 `board[i][j] = c`
- 匹配到 `idx == word.size()` 返回 true

**关键代码**:
```cpp
bool dfs(board, word, i, j, idx) {
    if(idx == word.size()) return true;
    if(i<0||i>=m||j<0||j>=n||board[i][j]!=word[idx]) return false;
    char c = board[i][j];
    board[i][j] = '#'; // 标记已访问
    bool res = dfs(board, word, i+1, j, idx+1)
             || dfs(board, word, i-1, j, idx+1)
             || dfs(board, word, i, j+1, idx+1)
             || dfs(board, word, i, j-1, idx+1);
    board[i][j] = c; // 回溯恢复
    return res;
}
```

**测试结果**: ✅ `ABCCED`→true, `SEE`→true, `ABCB`→false

**关键洞察**: 回溯的精髓：标记('#') + 4方向递归 + 回溯恢复；边界条件判断要在递归前

---

## WASM 浏览器测试（2026-03-31 上午）

### Frogger WASM — 浏览器渲染测试 ✅

**测试方法**: 
1. 启动 Python HTTP Server 端口 8765
2. Chrome 浏览器打开 `http://localhost:8765/`
3. 验证 HUD 和 Canvas 渲染

**测试结果**:
- ✅ 页面加载无崩溃
- ✅ HUD 正确显示: `LIVES: 3 | SCORE: 0 | TIME: 60 | HOMES: 0/5`
- ✅ Canvas 渲染正常（标题 + HUD 元素可见）
- ⚠️ 键盘输入需要进一步测试（ArrowUp 事件可能需要 canvas 焦点）

**文件位置**: `projects/frogger-raylib/wasm/index.html`
**WASM 大小**: `frogger.wasm` (3.7KB) + `frogger.js` (12KB)

---

## 今日经验总结

### 1. Dijkstra vs BFS
- 无权边图 → BFS 找最短路（队列）
- 带权边图 → Dijkstra（优先队列 min-heap）
- 两者结构相似，区别在于队列 vs 堆

### 2. Union-Find 模板
```cpp
class UF {
    vector<int> parent, rank;
    int find(int x) { return parent[x]==x?x:parent[x]=find(parent[x]); }
    void unite(int a, int b) { 
        int ra=find(a), rb=find(b);
        if(ra==rb) return;
        if(rank[ra]<rank[rb]) swap(ra,rb);
        parent[rb]=ra;
        if(rank[ra]==rank[rb]) rank[ra]++;
    }
};
```

### 3. WASM 浏览器测试技巧
- 需要 HTTP 服务器（file:// 不支持 WASM）
- `python3 -m http.server <port>` 快速启动本地服务器
- HUD 元素用 `snapshot` 可读，Canvas 实际渲染需截图分析

---

## 本周累计（2026-03-25 ~ 2026-03-31）

| 指标 | 数量 |
|------|------|
| LeetCode 完成 | **88+ 道** 🆕 |
| Hard 题目 | **41 道** |
| ncurses 游戏 | 8 个 |
| Raylib 图形游戏 | 10 个 |
| Web 游戏 | 1 个 |
| WASM 游戏 | 4 个 |
| 本周游戏总计 | **24 个** 🎉 |

---

*记录人: 悟通 | 2026-03-31 12:00*
