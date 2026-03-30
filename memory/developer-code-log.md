# 开发者代码日志

> 记录代码练习、游戏开发和经验沉淀
> 版本: v1.0 | 最后更新: 2026-03-30

---

## 一、LeetCode 练习记录

### LC146 LRU Cache — LRU 缓存

**题目**: 实现 O(1) get 和 put 的 LRU（最近最少使用）缓存  
**难度**: Hard  
**分类**: 设计 + 数据结构

**核心数据结构**: 哈希表 + 双向链表

**关键代码**:
```cpp
list<pair<int,int>> lst;  // front=MRU, back=LRU
unordered_map<int, list<pair<int,int>>::iterator> mp;

int get(int key) {
    auto it = mp.find(key);
    if (it == mp.end()) return -1;
    lst.splice(lst.begin(), lst, it->second);  // O(1) 移到最前
    return it->second->second;
}

void put(int key, int value) {
    if (mp.count(key)) {
        mp[key]->second = value;
        lst.splice(lst.begin(), lst, mp[key]);
    } else {
        if (lst.size() == cap) { mp.erase(lst.back().first); lst.pop_back(); }
        lst.emplace_front(key, value);
        mp[key] = lst.begin();
    }
}
```

**关键洞察**:
- `splice(pos, lst, it)`: 将 it 从 lst 移动到 pos 位置，O(1)
- `unordered_map<key, list::iterator>` 实现 O(1) 随机访问
- 驱逐时：删除 `lst.back()` 和对应的 map 条目
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

    if (maxHeap.size() > minHeap.size() + 1) {
        minHeap.push(maxHeap.top()); maxHeap.pop();  // 移动最大lower到upper
    } else if (maxHeap.size() < minHeap.size()) {
        maxHeap.push(minHeap.top()); minHeap.pop();  // 移动最小upper到lower
    }
}

double findMedian() {
    if (maxHeap.size() > minHeap.size()) return (double)maxHeap.top();
    else return (double)(maxHeap.top() + minHeap.top()) / 2.0;
}
```

**关键洞察**:
- 不变量: `maxHeap.size() == minHeap.size()` 或 `maxHeap.size() == minHeap.size() + 1`
- 中位数公式: 奇数个 → maxHeap.top()，偶数个 → 平均值
- **调试Bug**: 使用负数存储 max-heap（`maxHeap.push(-num)`）在 rebalance 时容易混淆正负
- **正确方法**: 直接用 `priority_queue<int>` 存正数，`greater<int>` 存 min-heap

**验证**: ✅ [1,2]→1.5, [1,2,3]→2.0, [1,2,3,4]→2.5

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

## 四、本周技术栈突破（2026-03-25 ~ 2026-03-30）

| 技术 | 掌握度 | 代表题目 |
|------|--------|---------|
| LRU Cache 设计 | 熟练 | LC146 |
| 双堆设计中位数 | 熟练 | LC295 |
| Raylib 图形游戏 | 熟练 | 10个游戏 |
| Emscripten/WASM | 入门 | 编译测试成功 |
| 背包DP进阶 | 熟练 | LC416/LC474 |
| 字符串DP | 掌握 | LC87/LC97/LC188 |

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

---

## 六、游戏开发记录

### 本周完成 20 个游戏（2026-03-25 ~ 2026-03-30）

| 平台 | 数量 | 代表游戏 |
|------|------|---------|
| ncurses | 8 | 贪吃蛇/2048/扫雷/Flappy Bird/Hangman/俄罗斯方块/华容道/推箱子 |
| Raylib | 10 | 贪吃蛇/2048/俄罗斯方块/Sokoban/Flappy Bird/Minesweeper/Breakout/Memory Match/Space Invaders/Pac-Man/Frogger |
| Web | 1 | AI意识守护者 |

---

## 七、GitHub 活跃度

- 本周 commit: 20+ 次
- 推送: 全部成功，无积压
- 新建 repo: `chongjie-ran/raylib-games`

---

*最后更新: 2026-03-30 23:55*
*版本: v1.1*
