# 开发者代码日志

> 记录代码练习、游戏开发和经验沉淀
> 版本: v1.2 | 最后更新: 2026-03-31

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

### 单调栈问题分类

| 题目 | 核心技巧 |
|------|---------|
| LC84 直方图最大矩形 | 单调递增栈，哨兵清栈 |
| LC85 最大子矩阵 | 逐行→LC84 |
| LC907 子数组最小值之和 | 单调递增栈，贡献计算 |
| LC42 Trapping Rain Water | 单调递减栈，水量计算 |

---

## 三、Raylib 游戏工程结构

```
src/
├── main.c      # 入口/输入/循环（调用raylib）
├── game.h/c    # 纯C逻辑（无raylib依赖）← 可独立测试
├── draw.h/c    # Raylib渲染
```

**核心原则**: game.h/game.c 不引用 raylib.h，纯 C 逻辑可独立编译测试

---

## 四、本周技术栈突破（2026-03-25 ~ 2026-03-31）

| 技术 | 掌握度 | 代表题目 |
|------|--------|---------|
| LRU Cache 设计 | 熟练 | LC146 |
| 双堆设计中位数 | 熟练 | LC295 |
| Raylib 图形游戏 | 熟练 | 10个游戏 |
| Emscripten/WASM | 入门 | 编译测试成功 |
| 背包DP进阶 | 熟练 | LC416/LC474 |
| 字符串DP | 掌握 | LC87/LC97/LC188 |
| 单调栈（直方图） | 熟练 | LC84/LC85 |
| Sliding Window | 熟练 | LC76 |

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

### 单调栈解题模板（LC84/L C85）

**关键点**:
- 哨兵技术：`i=n` 时用 `height=0` 清空栈
- 宽度计算：`st.empty() ? i : i - st.top() - 1`
- 一次遍历 + 一次清栈 = 完整解法

---

## 六、游戏开发记录

### 本周完成 20 个游戏（2026-03-25 ~ 2026-03-30）

| 平台 | 数量 | 代表游戏 |
|------|------|---------|
| ncurses | 8 | 贪吃蛇/2048/扫雷/Flappy Bird/Hangman/俄罗斯方块/华容道/推箱子 |
| Raylib | 10 | 贪吃蛇/2048/俄罗斯方块/Sokoban/Flappy Bird/Minesweeper/Breakout/Memory Match/Space Invaders/Pac-Man/Frogger |
| Web | 1 | AI意识守护者 |

### WASM 探索进度（2026-03-31）

| 步骤 | 状态 |
|------|------|
| Emscripten SDK 安装 | ✅ (`~/emsdk`, 版本 5.0.4) |
| emcc 编译器可用 | ✅ |
| Hello WASM 编译测试 | ✅ (生成 .wasm + .js) |
| 编译 Raylib 游戏到 WASM | ⏳ 待下一步 |

---

## 七、GitHub 活跃度

- 本周 commit: 20+ 次
- 推送: 全部成功，无积压
- 新建 repo: `chongjie-ran/raylib-games`

---

*最后更新: 2026-03-31 01:55*
*版本: v1.2*
