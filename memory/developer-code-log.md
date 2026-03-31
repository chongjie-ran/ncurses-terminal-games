# 开发者代码日志

> 记录代码练习、游戏开发和经验沉淀
> 版本: v1.5 | 最后更新: 2026-03-31 08:00

---

## 一、LeetCode 练习记录

### LC84 Largest Rectangle in Histogram — 最大矩形

**题目**: 给定 n 个连续柱子的高度数组，找最大矩形面积  
**难度**: Hard  
**分类**: Monotonic Stack

**核心思路**: 单调递增栈，左边界+右边界各遍历一次

**关键代码**:
```cpp
int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    stack<int> st; // 存索引，单调递增
    int maxArea = 0;
    for (int i = 0; i <= n; i++) {
        int curH = (i == n) ? 0 : heights[i];
        while (!st.empty() && heights[st.top()] > curH) {
            int h = heights[st.top()];
            st.pop();
            int width = st.empty() ? i : (i - st.top() - 1);
            maxArea = max(maxArea, h * width);
        }
        st.push(i);
    }
    return maxArea;
}
```

**关键洞察**:
- 右边界用哨兵 `i=n`（高度0）清空栈
- 弹栈时计算：`width = st.empty() ? i : (i - st.top() - 1)`
- 时间: `O(n)`, 空间: `O(n)`

---

### LC85 Maximal Rectangle — 最大子矩形

**题目**: 二进制矩阵中找全是1的最大矩形  
**难度**: Hard  
**分类**: Monotonic Stack → LC84

**核心思路**: 逐行处理，heights[j] = 第j列连续1的当前高度，每行调用 LC84

**关键代码**:
```cpp
for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
        if (matrix[i][j] == '1') heights[j] += 1;
        else heights[j] = 0;
    }
    maxArea = max(maxArea, largestRectangleInHistogram(heights));
}
```

**关键洞察**:
- 2D → 多行 LC84 的转化是核心
- heights 数组复用，空间 `O(n)`
- 时间: `O(m*n)`

---

### LC76 Minimum Window Substring — 最小覆盖子串

**题目**: s 中找包含 t 所有字符的最小子串  
**难度**: Hard  
**分类**: Sliding Window + Hash

**核心思路**: 双指针，`formed` 计数满足 need 条件的字符种类数

**关键代码**:
```cpp
vector<int> need(256,0), have(256,0);
int required = 0, formed = 0;
int left = 0, right = 0;
while (right < s.size()) {
    char c = s[right++];
    if (need[c] > 0) {
        have[c]++;
        if (have[c] == need[c]) formed++;
    }
    while (formed == required) {
        // 更新最小窗口
        if (right - left < minLen) { minLen = right - left; minL = left; }
        char d = s[left++];
        if (need[d] > 0) {
            if (have[d] == need[d]) formed--;
            have[d]--;
        }
    }
}
```

**关键洞察**:
- `formed == required` 是窗口有效的判断条件
- `right` 是左开右闭的扩展端，`left` 收缩端
- 固定256数组，空间 `O(1)`

---

### LC146 LRU Cache — LRU 缓存

**题目**: 实现 O(1) get 和 put 的 LRU（最近最少使用）缓存  
**难度**: Hard  
**分类**: 设计 + 数据结构

**核心数据结构**: 哈希表 + 双向链表

**关键代码**:
```cpp
unordered_map<int, list<pair<int,int>>::iterator> mp;

int get(int key) {
    auto it = mp.find(key);
    if (it == mp.end()) return -1;
    lst.splice(lst.begin(), lst, it->second);  // O(1) 移到最前
    return it->second->second;
}
```

**关键洞察**:
- `splice(pos, lst, it)`: 将 it 从 lst 移动到 pos 位置，O(1)
- `unordered_map<key, list::iterator>` 实现 O(1) 随机访问
- 时间复杂度: O(1) get + O(1) put

---

### LC295 Find Median from Data Stream — 数据流中位数

**题目**: 数据流动态加入，随时查询中位数  
**难度**: Hard  
**分类**: 设计 + 堆

**核心数据结构**: 双堆（max-heap + min-heap）

**关键代码**:
```cpp
priority_queue<int> maxHeap;                                       // lower half (max-heap)
priority_queue<int, vector<int>, greater<int>> minHeap;             // upper half (min-heap)

void addNum(int num) {
    if (maxHeap.empty() || num <= maxHeap.top()) maxHeap.push(num);
    else minHeap.push(num);
    if (maxHeap.size() > minHeap.size() + 1) { minHeap.push(maxHeap.top()); maxHeap.pop(); }
    else if (maxHeap.size() < minHeap.size()) { maxHeap.push(minHeap.top()); minHeap.pop(); }
}
```

**关键洞察**:
- 不变量: `maxHeap.size() == minHeap.size()` 或 `+1`
- 直接用 `priority_queue<int>` 存正数，避免负数转换混淆

---

### LC239 Sliding Window Maximum — 滑动窗口最大值

**题目**: 数组 nums 中所有滑动窗口大小 k 的最大值  
**难度**: Hard  
**分类**: Sliding Window + Monotonic Deque

**核心思路**: 单调递减双端队列，队首永远是当前窗口最大值，O(n) 总体

**关键代码**:
```cpp
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> dq; // 存索引，值单调递减
    vector<int> ans;
    for (int i = 0; i < nums.size(); i++) {
        while (!dq.empty() && nums[dq.back()] <= nums[i])
            dq.pop_back();
        dq.push_back(i);
        if (dq.front() <= i - k) dq.pop_front();
        if (i >= k - 1) ans.push_back(nums[dq.front()]);
    }
    return ans;
}
```

**关键洞察**:
- deque 存索引而非值，索引可判断窗口范围
- 队尾pop: 移除小于当前元素的值（永远不会成为最大值）
- 队首pop: 移除超出窗口范围的旧元素
- 总时间: `O(n)`，每个元素最多入队出队各一次

---

### LC32 Longest Valid Parentheses — 最长有效括号

**题目**: s 中最长有效括号子串长度  
**难度**: Hard  
**分类**: 字符串 DP / 栈

**核心思路**: `dp[i]` = 以 i 结尾的最长有效括号长度

**关键代码**:
```cpp
for (int i = 1; i < n; i++) {
    if (s[i] == ')') {
        if (s[i-1] == '(') {
            dp[i] = (i >= 2 ? dp[i-2] : 0) + 2;
        } else {
            int j = i - dp[i-1] - 1;
            if (j >= 0 && s[j] == '(') {
                dp[i] = dp[i-1] + 2 + (j >= 1 ? dp[j-1] : 0);
            }
        }
        ans = max(ans, dp[i]);
    }
}
```

**关键洞察**:
- `s[i-1]=='('` → 凑成 "()" 对
- `s[i-1]==')'` → 向外扩展：`j = i - dp[i-1] - 1`，检查 s[j] 是否为 '('
- `dp[i-1] + 2` 加上当前匹配对，再加上 `dp[j-1]` 处理嵌套

---

### LC312 Burst Balloons — 戳气球（新增 2026-03-31 05:40）

**题目**: 戳破气球获得金币，戳破 i 获得 `nums[left]*nums[i]*nums[right]`，最大金币  
**难度**: Hard  
**分类**: 区间DP

**核心思路**: 添加虚拟气球（值为1）后，`dp[i][j]` = 开区间(i,j)内戳破所有气球的最大金币，枚举最后戳的k

**关键代码**:
```cpp
int maxCoins(vector<int>& nums) {
    int n = nums.size();
    vector<int> a(n + 2, 1);
    for (int i = 0; i < n; i++) a[i + 1] = nums[i];
    n = a.size();

    vector<vector<int>> dp(n, vector<int>(n, 0));
    for (int len = 3; len <= n; len++) {
        for (int i = 0; i + len <= n; i++) {
            int j = i + len - 1;
            for (int k = i + 1; k < j; k++) {
                dp[i][j] = max(dp[i][j],
                    dp[i][k] + dp[k][j] + a[i] * a[k] * a[j]);
            }
        }
    }
    return dp[0][n - 1];
}
```

**关键洞察**:
- 关键洞察：最后戳破的气球k决定左右分割，虚拟气球1处理边界
- `dp[i][j]` 定义为开区间(i,j)的最大值，`i+1 < k < j-1`
- 时间: `O(n^3)`, 空间: `O(n^2)`
- 经典测试用例: `[3,1,5,8]` → 167 ✅

---

### LC200 Number of Islands — 岛屿数量（新增 2026-03-31 08:00）

**题目**: 二进制网格中找岛屿数量（1=陆地，0=水）  
**难度**: Medium  
**分类**: BFS/DFS flood fill

**核心思路**: 遍历网格，遇到'1'就BFS展开，同时标记为'0'

**关键代码**:
```cpp
int numIslands(vector<vector<char>>& grid) {
    int m=grid.size(), n=grid[0].size(), count=0;
    vector<vector<int>> dirs = {{1,0},{-1,0},{0,1},{0,-1}};
    for (int i=0;i<m;i++) for (int j=0;j<n;j++) {
        if (grid[i][j]=='1') {
            count++;
            queue<pair<int,int>> q; q.push({i,j}); grid[i][j]='0';
            while(!q.empty()) {
                auto [x,y]=q.front(); q.pop();
                for(auto& d:dirs) {
                    int nx=x+d[0], ny=y+d[1];
                    if(nx>=0&&nx<m&&ny>=0&&ny<n&&grid[nx][ny]=='1'){
                        grid[nx][ny]='0'; q.push({nx,ny});
                    }
                }
            }
        }
    }
    return count;
}
```

**关键洞察**:
- 网格BFS，4方向展开
- 边BFS边标记为'0'，避免重复访问
- 时间: `O(m*n)`，空间: `O(min(m,n))`

---

### LC207 Course Schedule — 课程表（新增 2026-03-31 08:00）

**题目**: 判断是否能完成所有课程（先修课依赖）  
**难度**: Medium  
**分类**: 拓扑排序（BFS Kahn算法）

**核心思路**: indegree==0的节点先处理，有环则visited<numCourses

**关键代码**:
```cpp
bool canFinish(int numCourses, vector<vector<int>>& prereq) {
    vector<vector<int>> adj(numCourses);
    vector<int> indegree(numCourses,0);
    for(auto& p:prereq){ adj[p[1]].push_back(p[0]); indegree[p[0]]++; }
    queue<int> q; for(int i=0;i<numCourses;i++) if(indegree[i]==0) q.push(i);
    int visited=0;
    while(!q.empty()){
        int u=q.front();q.pop();visited++;
        for(int v:adj[u]){ indegree[v]--; if(indegree[v]==0) q.push(v); }
    }
    return visited==numCourses; // 有环则visited<numCourses
}
```

**关键洞察**:
- `indegree` = 入度，有入度的课必须先修完才能学
- BFS从入度0的节点开始（没有先修课的课）
- visited==numCourses 说明所有课都能修完（有向无环图）
- 有环时环内节点入度永远不为0

---

### LC133 Clone Graph — 克隆图（新增 2026-03-31 08:00）

**题目**: 克隆无向图，返回深拷贝节点  
**难度**: Medium  
**分类**: BFS/DFS + 哈希表

**核心思路**: `old→new` 映射，边遍历边构建新图

**关键代码**:
```cpp
Node* cloneGraph(Node* node) {
    if(!node) return nullptr;
    unordered_map<Node*,Node*> mp;
    queue<Node*> q; q.push(node); mp[node]=new Node(node->val);
    while(!q.empty()){
        Node* cur=q.front();q.pop();
        for(Node* n:cur->neighbors){
            if(mp.find(n)==mp.end()){ mp[n]=new Node(n->val); q.push(n); }
            mp[cur]->neighbors.push_back(mp[n]);
        }
    }
    return mp[node];
}
```

**关键洞察**:
- `unordered_map<Node*,Node*>` 建立 old→new 映射
- BFS遍历原图，新节点按需创建（lazy creation）
- 边遍历边构建新图的邻居连接

---

## 二、设计问题通用模板

### LRU/LFU/FIFO 等缓存设计

| 缓存类型 | 数据结构 | 操作 | 时间 |
|---------|---------|------|------|
| LRU | 双向链表 + 哈希表 | splice移动 + map查找 | O(1) |
| LFU | 双向链表 + 哈希表 + 频率桶 | 频率递增 + 迁移 | O(1) |
| FIFO | 队列 + 哈希表 | 入队/出队 + map | O(1) |

### 双堆中位数/第K大/数据流

| 问题 | 数据结构 | 操作 |
|------|---------|------|
| 中位数 | maxHeap + minHeap | add O(log n), find O(1) |
| 第K大 | minHeap(大小K) | add O(log K), top O(1) |
| 第K小 | maxHeap(大小K) | add O(log K), top O(1) |
| 滑动窗口中位数 | 双堆 + 延迟删除 | O(log n) per move |

### 区间DP问题分类

| 题目 | 核心技巧 |
|------|---------|
| LC312 戳气球 | dp[i][j]=枚举最后戳破的k，虚拟气球 |
| LC364 Reverse Linked List II | 区间翻转 |
| LC87 扰乱字符串 | 区间划分+字符统计 |

### 单调栈问题分类

| 题目 | 核心技巧 |
|------|---------|
| LC84 直方图最大矩形 | 单调递增栈，哨兵清栈 |
| LC85 最大子矩阵 | 逐行→LC84 |
| LC907 子数组最小值之和 | 单调递增栈，贡献计算 |
| LC42 接雨水 | 单调递减栈，水量计算 |
| LC32 最长有效括号 | DP，嵌套展开 |

### 单调队列问题分类

| 题目 | 核心技巧 |
|------|---------|
| LC239 滑动窗口最大值 | 单调递减 deque，存索引，O(n) |
| LC862 最短子数组和≥K | 单调递增 deque + 前缀和 |
| LC975 奇偶跳 | 单调递减/递增 deque，栈思想 |

### 图问题分类

| 题目 | 核心技巧 |
|------|---------|
| LC200 岛屿数量 | BFS flood fill，边访问边标记 |
| LC207 课程表 | 拓扑排序Kahn，indegree==0的BFS |
| LC133 克隆图 | BFS + old→new映射 |

---

## 三、Raylib 游戏工程结构

```
src/
├── main.c      # 入口/输入/循环（调用raylib）
├── game.h/c    # 纯C逻辑（无raylib依赖）← 可独立测试
├── draw.h/c    # Raylib渲染
```

**核心原则**: game.h/game.c 不引用 raylib.h，纯 C 逻辑可独立编译测试

**WASM 扩展**:
```
src/game.h/c     # 纯C逻辑（无依赖）
wasm/wasm_renderer.c  # Emscripten导出函数
wasm/index.html  # Canvas 2D渲染 + JS输入
    ↓ Emscripten 编译
*.wasm + *.js
```

---

## 四、本周技术栈突破（2026-03-25 ~ 2026-03-31）

| 技术 | 掌握度 | 代表题目/项目 |
|------|--------|---------|
| LRU Cache 设计 | 熟练 | LC146 |
| 双堆设计中位数 | 熟练 | LC295 |
| Raylib 图形游戏 | 熟练 | 10个游戏 |
| Emscripten/WASM | 进阶 | Frogger + Pac-Man + Space Invaders WASM 编译成功 |
| 区间DP | 进阶 | LC312 Burst Balloons |
| 背包DP进阶 | 熟练 | LC416/LC474 |
| 字符串DP | 掌握 | LC87/LC97/LC188 |
| 单调栈（直方图） | 熟练 | LC84/LC85 |
| Sliding Window | 熟练 | LC76 |
| 单调队列 | 熟练 | LC239 |
| 有效括号DP | 掌握 | LC32 |
| 图BFS flood fill | 掌握 | LC200 Number of Islands |
| 拓扑排序 | 掌握 | LC207 Course Schedule |
| 图克隆 | 掌握 | LC133 Clone Graph |

---

## 五、本周经验总结

### 调试方法论（LC295 双堆 Bug 修复）

**问题**: 初期使用 `maxHeap.push(-num)` 实现 max-heap，在 rebalance 时混淆正负  
**症状**: [1,2,3] 的中位数输出 1 而非 2  

**修复过程**:
1. 追踪 rebalance 过程：`maxHeap=[1,2], minHeap=[3]`，中位数应为 2
2. 发现 `maxHeap.top() = -1`（存储 -1 和 -2），值为 1 而非 2
3. 根本原因：存储负数后，max-heap 的 top 是最大的负数（最小的绝对值）
4. 修复：直接用 `priority_queue<int>` 存储正数，无需取负

**教训**: 优先使用标准库默认行为，避免手动的正负转换导致混淆

### 单调栈解题模板（LC84/LC85）

**关键点**:
- 哨兵技术：`i=n` 时用 `height=0` 清空栈
- 宽度计算：`st.empty() ? i : i - st.top() - 1`
- 一次遍历 + 一次清栈 = 完整解法

### Emscripten WASM 编译经验

**问题**: 编译时成员名称不匹配（game struct 字段名与 wasm_renderer.c 不一致）  
**修复**: 先完整读取 game.h，了解实际字段名（`obsCount` 而非 `numVehicles`，`isAlive` 而非 `alive`，`timeLeft` 而非 `timeLimit`）

**关键教训**: 读取源码确认字段名，不要凭记忆猜测

**重要教训2**: 编译时必须 `#include <emscripten/emscripten.h>` 以获取 `EMSCRIPTEN_KEEPALIVE` 宏定义，否则编译器报错 "variable has incomplete type 'void'"

### 区间DP模板（LC312）

**关键洞察**: 先添加虚拟气球（值为1）到数组两端，将边界情况统一处理

**通用模板**:
```cpp
// 1. 扩展数组（添加哨兵）
// 2. dp[i][j] = 开区间(i,j)的最优值
// 3. 枚举最后处理的元素k
// 4. dp[i][j] = max(dp[i][k] + dp[k][j] + cost(i,k,j))
```

### 图BFS flood fill（LC200）

**关键洞察**: 边访问边标记是避免重复访问的关键

**通用模板**:
```cpp
// 遍历网格每个格子
// 遇到目标值 → count++ → BFS展开 → 展开时标记为已访问
// 时间 O(m*n)，空间 O(min(m,n))
```

---

## 六、游戏开发记录

### 本周完成 23 个游戏（2026-03-25 ~ 2026-03-31）

| 平台 | 数量 | 代表游戏 |
|------|------|---------|
| ncurses | 8 | 贪吃蛇/2048/扫雷/Flappy Bird/Hangman/俄罗斯方块/华容道/推箱子 |
| Raylib | 10 | 贪吃蛇/2048/俄罗斯方块/Sokoban/Flappy Bird/Minesweeper/Breakout/Memory Match/Space Invaders/Pac-Man/Frogger |
| Web | 1 | AI意识守护者 |
| WASM | 3 | Frogger + Pac-Man + Space Invaders 🆕 |

### WASM 探索进度（2026-03-31）

| 步骤 | 状态 |
|------|------|
| Emscripten SDK 安装 | ✅ (`~/emsdk`, 版本 5.0.4) |
| emcc 编译器可用 | ✅ |
| Hello WASM 编译测试 | ✅ (生成 .wasm + .js) |
| Frogger 纯C逻辑编译 | ✅ (`frogger.wasm` 3.7KB + `frogger.js` 12KB) |
| Pac-Man WASM 编译 | ✅ (`pacman.wasm` 5.9KB + `pacman.js` 12.7KB) |
| Space Invaders WASM 编译 | ✅ (`space_invaders.wasm` 19KB + `space_invaders.js` 13.7KB) |
| Canvas 2D 渲染器 | ✅ (3个游戏 `index.html` 完整实现) |
| 键盘输入控制 | ✅ (Arrow Keys / WASD / Space / P / R) |
| 浏览器测试 | ⏳ 待测试（3个游戏均待浏览器验证） |

**Space Invaders WASM 导出函数** (34个):
```c
_init_game, _update_game, _get_game_state,
_get_score, _get_lives, _get_wave, _get_alive_count,
_get_player_x, _get_player_y,
_get_bullet_count, _get_bullet_x/y/active,
_get_enemy_bullet_count, _get_enemy_bullet_x/y/active,
_get_alien_count, _get_alien_x/y/alive/type/exploding,
_move_left, _move_right, _shoot, _reset_game,
_get_screen_w/h, _get_player_w/h, _get_alien_w/h/rows/cols
```

**Space Invaders Canvas 渲染亮点**:
- 星空背景（60颗随机星星，透明度各异）
- 玩家飞船（绿色带炮管）
- 外星人4色（红/橙/黄/绿），触角交替动画
- 玩家子弹绿色，敌人子弹红色
- HUD：分数、波次、生命、外星人数量
- 支持暂停(P)、重开(R)

---

## 七、GitHub 活跃度

- 本周 commit: 30+ 次
- 推送: 全部成功，无积压
- 新建 repo: `chongjie-ran/raylib-games`

---

## 八、下周计划（2026-04-01 ~ 2026-04-06）

| 优先级 | 任务 | 核心技术 |
|--------|------|---------|
| P1 | Frogger WASM 浏览器测试 | Canvas渲染 + JS/WASM交互 |
| P1 | Breakout WASM 编译 | 编译 Breakout 到 WebAssembly |
| P2 | LeetCode 图论+BFS/DFS | 每日2-3题 |
| P3 | WASM 游戏部署到 GitHub Pages | 静态文件托管 |
| P3 | 粒子效果/音效集成 | Raylib audio API |

---

*最后更新: 2026-03-31 08:00*
*版本: v1.5*
