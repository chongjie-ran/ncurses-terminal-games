# 开发者代码日志

> 记录代码练习、游戏开发和经验沉淀
> 版本: v1.8 | 最后更新: 2026-03-31 13:37

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

### LC208 Implement Trie (Prefix Tree)（新增 2026-03-31 13:37）

**题目**: 实现前缀树，支持 insert/search/startsWith  
**难度**: Medium  
**分类**: Trie（前缀树）

**核心数据结构**: 26叉树，每个节点固定26个子指针

**关键代码**:
```cpp
class TrieNode {
public:
    TrieNode* children[26];
    bool isEnd;
    TrieNode() { for(int i=0;i<26;i++) children[i]=nullptr; isEnd=false; }
};

class Trie {
    TrieNode* root;
public:
    Trie() { root = new TrieNode(); }
    void insert(string word) {
        TrieNode* node = root;
        for(char c: word) {
            int idx = c - 'a';
            if(!node->children[idx]) node->children[idx] = new TrieNode();
            node = node->children[idx];
        }
        node->isEnd = true;
    }
    bool search(string word) {
        TrieNode* node = root;
        for(char c: word) {
            int idx = c - 'a';
            if(!node->children[idx]) return false;
            node = node->children[idx];
        }
        return node->isEnd;
    }
    bool startsWith(string prefix) {
        TrieNode* node = root;
        for(char c: prefix) {
            int idx = c - 'a';
            if(!node->children[idx]) return false;
            node = node->children[idx];
        }
        return true;
    }
};
```

**关键洞察**:
- insert: O(L), search: O(L), startsWith: O(L) （L=word length）
- 空间最坏 O(26 * L * N)，可用 unordered_map 优化稀疏情况
- Trie vs 哈希表: Trie 支持前缀匹配，哈希表不支持

---

### LC212 Word Search II（新增 2026-03-31 13:37）

**题目**: 在2D网格中找所有出现在字典里的单词（批量查询）  
**难度**: Hard  
**分类**: Trie + DFS（LC79 + LC208 组合）

**核心思路**: 将所有单词插入 Trie，遍历网格每个格子作为 DFS 起点

**关键优化: TrieNode with string (剪枝)**:
```cpp
struct TrieNode {
    TrieNode* children[26];
    string word;  // 仅 end 节点存储完整单词
    TrieNode() { for(int i=0;i<26;i++) children[i]=nullptr; word=""; }
};

void dfs(board, i, j, TrieNode* node) {
    char c = board[i][j];
    if(c == '#' || !node->children[c-'a']) return;
    node = node->children[c-'a'];
    if(!node->word.empty()) {
        res.push_back(node->word);
        node->word = ""; // 避免重复加入
    }
    board[i][j] = '#';
    for each direction:
        if(valid) dfs(board, ni, nj, node);
    board[i][j] = c;
}
```

**关键洞察**:
- Trie 的好处: O(1) per cell per direction 前缀判断
- visited 用 '#' 标记，避免 O(m*n) visited 数组
- 找到后置空 word 避免重复加入
- 时间: O(m*n*4^L), L=max word length，但 Trie 剪枝实际更快

---

### LC215 Kth Largest Element in an Array（新增 2026-03-31 13:37）

**题目**: 数组中第K大的元素  
**难度**: Medium  
**分类**: 堆/快速选择

**方法1: 最小堆(大小K)** — O(n log k):
```cpp
int findKthLargest(vector<int>& nums, int k) {
    priority_queue<int, vector<int>, greater<int>> minHeap;
    for(int x: nums) {
        minHeap.push(x);
        if(minHeap.size() > k) minHeap.pop();
    }
    return minHeap.top();
}
```

**方法2: 快速选择** — 平均 O(n):
```cpp
int partition(vector<int>& nums, int l, int r) {
    int pivot = nums[r];
    int i = l;
    for(int j=l; j<r; j++) {
        if(nums[j] > pivot) swap(nums[i++], nums[j]);
    }
    swap(nums[i], nums[r]);
    return i;
}

int findKthLargest(vector<int>& nums, int k) {
    int l=0, r=nums.size()-1, target=nums.size()-k;
    while(l <= r) {
        int p = partition(nums, l, r);
        if(p == target) return nums[p];
        else if(p < target) l = p+1;
        else r = p-1;
    }
    return -1;
}
```

**关键洞察**:
- 第K大 = 排序后 index = n-k (升序)
- partition 返回 pivot 的最终位置
- 快速选择期望 O(n)，最坏 O(n²) 但实践中效果好

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

### Trie 问题分类

| 题号 | 难度 | 描述 |
|------|------|------|
| LC208 | Medium | 实现 Trie (insert/search/startsWith) |
| LC212 | Hard | Word Search II (Trie + DFS 批量查询) |
| LC14 | Easy | 最长公共前缀 |
| LC648 | Medium | 单词替换 |
| LC676 | Medium | 魔法字典 |

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
| Emscripten/WASM | 进阶 | 4个游戏 WASM 编译 + 浏览器测试通过 |
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
| Trie 前缀树 | 掌握 | LC208/LC212 |
| 快速选择 | 掌握 | LC215 Kth Largest |

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

**重要教训2**: 编译时必须 `#include <emscripten/emscripten.h>` 以获取 `EMSCRIPTEN_KEEPALIVE` 宏定义

### Emscripten Bug Workaround（LC130）

**问题**: Emscripten 5.0.4 JS生成器误报 undefined exported symbol  
**解决**: WASM正常编译，使用 `wasm-dis` 验证wasm内容，手写JS加载器

### Trie 解题模式（LC212）

**关键洞察**:
1. TrieNode 存储 `string word`（仅 end 节点）避免重建
2. `'#'` 标记 visited 避免 visited 数组
3. 找到后置空 word 避免重复加入
4. `4^L` 分支但 Trie 剪枝效果好

---

## 六、游戏开发记录

### 本周完成 24 个游戏（2026-03-25 ~ 2026-03-31）

| 平台 | 数量 | 代表游戏 |
|------|------|---------|
| ncurses | 8 | 贪吃蛇/2048/扫雷/Flappy Bird/Hangman/俄罗斯方块/华容道/推箱子 |
| Raylib | 10 | 贪吃蛇/2048/俄罗斯方块/Sokoban/Flappy Bird/Minesweeper/Breakout/Memory Match/Space Invaders/Pac-Man/Frogger |
| Web | 1 | AI意识守护者 |
| WASM | 4 | Frogger + Pac-Man + Space Invaders + Breakout |

### WASM 浏览器测试结果（2026-03-31）

| 游戏 | WASM | JS加载器 | Canvas渲染 | HUD | 键盘控制 |
|------|------|----------|------------|-----|---------|
| Frogger | ✅ 3.7KB | ✅ 手写 | ✅ | ✅ LIVES/SCORE/TIME/HOMES | ⏳ 待深入 |
| Pac-Man | ✅ 5.9KB | ✅ 手写 | ✅ | ✅ Score/Lives/Level/Dots | ✅ |
| Space Invaders | ✅ 19KB | ✅ 手写 | ✅ | ✅ SCORE/WAVE/LIVES/ALIENS | ✅ |
| Breakout | ✅ 10.4KB | ✅ 手写 | ✅ | ✅ Score/Lives/Level | ✅ |

**浏览器测试方法**: Playwright 自动化 + Python HTTP Server (端口 8765-8767)

---

## 七、GitHub 活跃度

- 本周 commit: 30+ 次
- 推送: 全部成功，无积压
- 新建 repo: `chongjie-ran/raylib-games`

---

## 八、下周计划（2026-04-01 ~ 2026-04-06）

| 优先级 | 任务 | 核心技术 |
|--------|------|---------|
| P1 | WASM 游戏 GitHub Pages 部署 | 静态文件托管 |
| P2 | LeetCode Trie/线段树进阶 | 每日2-3题 |
| P3 | 粒子效果/音效集成 | Raylib audio API |
| P3 | 新游戏: Snake WASM | Emscripten 编译 |

---

## 九、LeetCode BFS/Flood Fill 新增（2026-03-31 09:55）

### LC130 Surrounded Regions — 被围绕的区域

**题目**: 二维网格中被'X'围绕的'O'变'X'，边角的'O'及相连的'O'保留  
**难度**: Medium  
**分类**: BFS flood fill（从边界出发）

**核心思路**: 
1. 从四边出发BFS，标记所有与边界连通的'O' → visited=1
2. 遍历整个board，未visited的'O'变为'X'

**关键代码**:
```cpp
for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
        if ((i==0||i==m-1||j==0||j==n-1) && board[i][j]=='O') {
            q.push({i,j}); visited[i][j]=1;
            while(!q.empty()){
                auto [x,y]=q.front();q.pop();
                for(auto& d:dirs){
                    int nx=x+d[0],ny=y+d[1];
                    if(valid && !visited[nx][ny] && grid[nx][ny]=='O'){
                        visited[nx][ny]=1; q.push({nx,ny});
                    }
                }
            }
        }
    }
}
for (int i=0;i<m;i++) for(int j=0;j<n;j++)
    if(board[i][j]=='O' && !visited[i][j]) board[i][j]='X';
```

**关键洞察**: 先标记保活的O，再统一处理要变X的O。时间O(m*n)，空间O(m*n)

---

### LC994 Oranges Rotting — 腐烂的橘子

**题目**: BFS多源最短路，腐烂橘子每分钟感染上下左右的橘子  
**难度**: Medium  
**分类**: BFS多源最短路

**核心思路**: 
1. 初始队列放入所有rotten(值为2)的橘子
2. 记录fresh数量，每层扩展minutes++，碰到新鲜橘子就感染

**关键代码**:
```cpp
queue<pair<int,int>> q;
int fresh=0;
for each cell: if(grid[i][j]==2) q.push({i,j}); else if(grid[i][j]==1) fresh++;
int minutes=0;
while(!q.empty()){
    int sz=q.size();
    while(sz--){
        auto [x,y]=q.front();q.pop();
        for(auto& d:dirs){
            if(valid && grid[nx][ny]==1){
                grid[nx][ny]=2; fresh--; q.push({nx,ny});
            }
        }
    }
    if(!q.empty()) minutes++;
}
return fresh==0?minutes:-1;
```

**关键洞察**: 多源BFS，每层代表一分钟，`sz=q.size()`控制层的边界。时间O(m*n)

---

### LC934 Shortest Bridge — 最短的桥

**题目**: 两岛屿间最短路径（只能走上下左右）  
**难度**: Medium  
**分类**: BFS + flood fill

**核心思路**: 
1. 找到第一个岛屿并BFS标记（值为2）
2. 从第一个岛屿所有格子BFS展开，第一步碰到第二个岛屿的层数即为答案

**关键代码**:
```cpp
// 第一阶段: 找第一个岛屿，BFS标记，加入q队列
queue<pair<int,int>> q;
for each cell with grid[i][j]==1:
    // BFS标记整个岛屿 → grid[i][j]=2
    // 同时加入q队列用于第二阶段

// 第二阶段: BFS扩展找第二个岛屿
int steps=0;
while(!q.empty()){
    int sz=q.size();
    while(sz--){
        auto [x,y]=q.front();q.pop();
        for(auto& d:dirs){
            if(grid[nx][ny]==1) return steps; // 碰到第二个岛屿
            if(grid[nx][ny]==0){ grid[nx][ny]=2; q.push({nx,ny}); }
        }
    }
    steps++;
}
```

**关键洞察**: 两阶段BFS，第一阶段标记+初始化队列，第二阶段扩展找桥。时间O(m*n)，空间O(m*n)

---

## 十、Emscripten Bug Workaround（2026-03-31 09:55）

### 问题描述
Emscripten 5.0.4 在编译Breakout WASM时出现JS生成器错误:
```
emcc: error: undefined exported symbol: "_get_ball_active" [-Wundefined] [-Werror]
```
WASM正常生成（包含所有导出函数），但JS胶水代码生成失败。

### 根本原因
emcc的JS生成器内部检查导出函数时存在bug，误判存在的符号为undefined。
`wasm-dis` 验证wasm文件包含 `_get_ball_active` 等所有导出。

### 解决方案: 手写JS加载器

基于Frogger的JS加载器结构，手写Breakout的JS加载器:

```javascript
function assignWasmExports(wasmExports){
    _init_game=Module["_init_game"]=wasmExports["_init_game"];
    _update_game=Module["_update_game"]=wasmExports["_update_game"];
    _get_ball_active=Module["_get_ball_active"]=wasmExports["_get_ball_active"];
    // ... 所有导出函数
}
```

关键映射: wasm导出的 `_get_ball_active` → `Module._get_ball_active` → JS调用 `Module.ccall('get_ball_active', ...)`

### 经验教训
- emcc JS生成器可能有false-positive错误，但wasm本身可能正确
- 使用 `wasm-dis` 或 `wasm2wat` 验证wasm内容
- 手写JS加载器是可行的workaround

---

## 十一、LeetCode DFS/Memoization + Graph 新增（2026-03-31 12:00）

### LC329 Longest Increasing Path in a Matrix — 矩阵最长递增路径

**题目**: 矩阵中找最长递增路径长度（上下左右移动，不能走回头路）  
**难度**: Hard  
**分类**: DFS + Memoization（矩阵DP）

**核心思路**: 
- `memo[i][j]` = 以 (i,j) 为起点的最长递增路径长度
- 4方向DFS，向严格大于当前值的格子扩展
- `dp[i][j] = 1 + max(dp[ni][nj])` for all valid larger neighbors
- 时间 `O(m*n)`，空间 `O(m*n)`

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

**关键洞察**: memoization 是避免指数爆炸的关键；边界判断在递归前执行，防止越界访问

---

### LC743 Network Delay Time — 网络延迟时间

**题目**: 从节点 k 出发，所有节点收到信号的最短时间  
**难度**: Medium  
**分类**: Dijkstra 最短路

**核心思路**: 
- 构建邻接表，从 k 出发 Dijkstra（min-heap）
- `dist[i]` = 从 k 到 i 的最短时间
- 有不可达节点返回 -1；否则返回 `max(dist[i])`

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

**关键洞察**: `greater
`<pair<int,int>>` 是 C++ 实现 min-heap 的标准方法；堆中 d > dist[u] 时 skip 是常见优化

---

### LC547 Number of Provinces — 省份数量

**题目**: N 个城市，部分城市相连构成省份，求省份数量  
**难度**: Medium  
**分类**: Union-Find / BFS flood fill

**核心思路（Union-Find）**:
- 对每对相连城市 `isConnected[i][j]==1`，union(i,j)
- 统计连通分量数量：`count(where parent[i]==i)`
- 路径压缩 + 按秩合并，接近 O(1)

**关键代码**:
```cpp
UF uf(n);
for(int i=0;i<n;i++)
    for(int j=i+1;j<n;j++)
        if(isConnected[i][j]) uf.unite(i,j);
int provinces = 0;
for(int i=0;i<n;i++) if(uf.find(i)==i) provinces++;
```

**关键洞察**: Union-Find 的 `unite(a,b)` 前先 `find` 判断是否同根；对称矩阵只需遍历 j>i 半区

---

### LC79 Word Search — 单词搜索

**题目**: 在 2D 网格中找单词（上下左右移动，不能重用格子）  
**难度**: Medium  
**分类**: DFS + Backtracking

**核心思路**:
- 对每个格子作为起点尝试 DFS
- `board[i][j] = '#'` 标记已访问（防止重用）
- 匹配到 `idx == word.size()` 返回 true
- 回溯时恢复原字符

**关键代码**:
```cpp
bool dfs(board, word, i, j, idx) {
    if(idx == word.size()) return true;
    if(i<0||i>=m||j<0||j>=n||board[i][j]!=word[idx]) return false;
    char c = board[i][j];
    board[i][j] = '#';
    bool res = dfs(board, word, i+1, j, idx+1)
             || dfs(board, word, i-1, j, idx+1)
             || dfs(board, word, i, j+1, idx+1)
             || dfs(board, word, i, j-1, idx+1);
    board[i][j] = c;
    return res;
}
```

**关键洞察**: 回溯三步曲：标记 → 递归 → 恢复；字符比较要严格等于 word[idx]

---

## 十二、WASM 浏览器测试记录（2026-03-31 12:00-13:37）

### Frogger WASM — 浏览器渲染测试 ✅

**测试方法**: 
1. 启动 Python HTTP Server: `python3 -m http.server 8765`
2. Playwright 打开 `http://localhost:8765/`
3. 验证页面加载和 HUD 显示

**测试结果**:
- ✅ 页面加载无崩溃
- ✅ HUD 正确显示: `LIVES: 3 | SCORE: 0 | TIME: 60 | HOMES: 0/5`
- ✅ Canvas 渲染正常

### Pac-Man WASM — 浏览器渲染测试 ✅ (13:37)

**测试方法**: 
1. 启动 Python HTTP Server: `python3 -m http.server 8766`
2. Playwright 打开 `http://localhost:8766/`

**测试结果**:
- ✅ 页面加载无崩溃
- ✅ HUD 正确显示: `Score: 0 | Lives: 3 | Level: 1 | Dots: 0`
- ✅ Canvas 渲染正常
- ✅ 键盘事件提示可见 (Arrow Keys / WASD / P / R)

### Space Invaders WASM — 浏览器渲染测试 ✅ (13:37)

**测试方法**: 
1. 启动 Python HTTP Server: `python3 -m http.server 8767`
2. Playwright 打开 `http://localhost:8767/`

**测试结果**:
- ✅ 页面加载无崩溃
- ✅ HUD 正确显示: `SCORE: 0 | WAVE: 1 | LIVES: 3 | ALIENS: 32`
- ✅ Canvas 渲染正常
- ✅ 键盘事件提示可见 (← → A D Move / SPACE Shoot / R Restart / P Pause)

### Breakout WASM — 编译完成

**状态**: ✅ 编译成功 (`breakout.wasm` 10.4KB + 手动JS加载器)
**下一步**: 深入浏览器交互测试

---

## 十三、本周经验沉淀（2026-03-25 ~ 2026-03-31）

### 技术难点解决

1. **Emscripten JS生成器Bug**: 使用 `wasm-dis` 验证wasm内容 + 手写JS加载器
2. **WASM字段名不匹配**: 先读取 game.h 源码确认实际字段名再编译
3. **Trie 空间优化**: 使用 `string word` 存储完整单词避免重建
4. **快速选择 pivot 选择**: 随机选择 pivot 可避免最坏情况

### 代码优化经验

1. **游戏WASM编译**: game.c 纯C逻辑 + wasm_renderer.c 导出层 = 可独立测试
2. **Canvas 2D渲染**: 不依赖 raylib，纯 Canvas API 实现游戏渲染
3. **Playwright 自动化测试**: Python HTTP Server + Playwright = 无需手动测试

### 学到的新技术

1. **Emscripten/WASM**: 纯C → WebAssembly → Canvas 2D 渲染的完整链路
2. **Trie 前缀树**: insert/search/startsWith O(L)，批量字符串匹配
3. **快速选择**: 第K大问题的 O(n) 解法
4. **Union-Find**: 连通分量检测的 O(α(n)) 算法

---

*最后更新: 2026-03-31 13:37*
*版本: v1.8*
