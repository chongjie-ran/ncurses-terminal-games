# 2026-03-28 凌晨练习 | 每日总结

## 一、今日完成 (2026-03-28)

### LeetCode 算法练习

| 题目 | 分类 | 难度 | 结果 |
|------|------|------|------|
| LC174 地下城游戏 | DP（二维）| Hard | ✅ |

**LC174 Dungeon Game — 地下城游戏** ✅

**题目**: 骑士从左上角到右下角救公主，最低初始HP是多少？  
**核心算法**: 逆向二维 DP，从终点反推到起点

**核心思路**:
- `dp[i][j]` = 从 (i,j) 到终点所需的最小初始HP
- 逆向递推：`dp[i][j] = max(1, min(dp[i+1][j], dp[i][j+1]) - dungeon[i][j])`
- 从右下角开始填表，左下角结束
- 空间优化：O(n) 而非 O(mn)

**关键洞察**: 为什么逆向DP？
- 正向DP：`dp[i][j]` 依赖到达该格子前的HP，是未知的（取决于后续路径）
- 逆向DP：从终点反推，知道后续的最低需求，可直接计算

**代码实现**:
```cpp
int calculateMinimumHP(vector<vector<int>>& dungeon) {
    int m = dungeon.size(), n = dungeon[0].size();
    vector<int> dp(n + 1, INT_MAX);
    dp[n - 1] = 1;  // 终点需要的最低HP
    
    for (int i = m - 1; i >= 0; --i) {
        dp[n - 1] = max(1, dp[n - 1] - dungeon[i][n - 1]);
        for (int j = n - 2; j >= 0; --j) {
            int need = min(dp[j], dp[j + 1]) - dungeon[i][j];
            dp[j] = max(1, need);
        }
    }
    return dp[0];
}
```

**与 LC72/LC10 对比**:
| 问题 | DP方向 | 核心区别 |
|------|--------|---------|
| LC72 编辑距离 | 正向二维 | 最小化操作数 |
| LC10 正则匹配 | 正向二维 | 状态机 `*` 量词 |
| LC174 地下城 | 逆向二维 | 从终点反推 |

---

## 二、游戏开发队列状态

### 队列状态 (2026-03-28 凌晨)
- **队列状态**: 🎉 全部清空
- **已开发**: 7 个 ncurses 终端游戏（贪吃蛇/2048/扫雷/Flappy Bird/Hangman/Tetris/华容道）
- **下周 P1**: 推箱子 (Sokoban) — BFS/DFS + 状态空间搜索

### 本周游戏汇总 (2026-03-25 ~ 2026-03-27)
| 日期 | 项目 | 核心算法 |
|------|------|---------|
| 03-26 | 贪吃蛇 | deque + 方向缓冲 |
| 03-26 | 2048 | 矩阵旋转 + 滑动合并 |
| 03-26 | 扫雷 | BFS flood fill + 安全开局 |
| 03-26 | Flappy Bird | 重力物理 + AABB碰撞 |
| 03-26 | Hangman | set + ASCII art |
| 03-27 | 俄罗斯方块 | SRS wall kick + Ghost Piece |
| 03-27 | 华容道 | BFS 最短路 + 状态压缩 |

---

## 三、技术积累

### 逆向 DP 要点总结

**什么时候用逆向DP？**
- 当「到达某状态的代价」未知，但「从某状态出发的代价」已知时
- 例如：地下城游戏 — 不知道到达 (i,j) 前有多少HP，但知道从 (i,j) 出发需要多少HP

**逆向DP模板**:
```cpp
// 从终点开始，逆向填表
for (int i = m-1; i >= 0; --i) {
    for (int j = n-1; j >= 0; --j) {
        dp[i][j] = 计算(从(i,j)出发的代价);
    }
}
```

**逆向 vs 正向 DP**:
| 方向 | 适用场景 | 状态含义 |
|------|---------|---------|
| 正向 | 累积代价已知 | `dp[i]` = 到达 i 的最小代价 |
| 逆向 | 后续代价已知 | `dp[i]` = 从 i 出发的最小代价 |

---

## 四、本周成长总结 (2026-03-25 ~ 2026-03-28凌晨)

### 量化成果

| 指标 | 数量 |
|------|------|
| LeetCode 完成 | 28+ 道 |
| Hard 题目 | 8 道（LC42/LC84/LC239/LC72/LC10/LC33/LC124/LC174）|
| 游戏发布 GitHub | 7 个 ncurses 终端游戏 |

### 技术栈进步

| 技术领域 | 掌握情况 | 相关题目 |
|---------|---------|---------|
| 单调栈 | **熟练** | LC42/LC84 — O(n) |
| 单调队列 | **熟练** | LC239 — O(n) |
| 滑动窗口 | **熟练** | LC3 — O(n) |
| 矩阵旋转 | **熟练** | LC48/Tetris |
| DFS/BFS | **熟练** | LC200/LC130 |
| 二分查找 | **熟练** | LC33 |
| 拓扑排序 | **熟练** | LC207/LC210 |
| DP 一维 | **熟练** | LC322/LC139 |
| DP 二维 | **掌握** | LC72/LC10/LC516/LC174 |
| 树形 DP | **入门** | LC124 |
| 并查集 | **入门** | LC547 |
| ncurses 游戏 | **熟练** | 7 个游戏 |

### 下周计划 (2026-03-28 ~ 2026-04-03)

| 优先级 | 任务 | 说明 |
|--------|------|------|
| P1 | 推箱子 (Sokoban) | BFS/DFS + 状态空间搜索，下周 P1 |
| P1 | LeetCode DP 进阶 | LC312 戳气球, LC516 (已完成) |
| P2 | 并查集扩展 | LC684 冗余连接 |
| P3 | GUI 重构 | Raylib 可选扩展 |

---

## 五、今日补充 (2026-03-28 凌晨 3:48)

### 推箱子 (Sokoban) 开发 ✅

**项目路径**: `projects/sokoban/`
**技术栈**: C++17 + ncurses
**编译**: ✅ 零警告编译通过

**核心功能**:
1. **ncurses 终端 UI**: 彩色显示（墙/箱子/目标/玩家）
2. **方向键控制**: 上下左右移动玩家
3. **推箱子逻辑**: 检测箱子后面是否可推（不能推到墙/另一个箱子上）
4. **BFS 自动求解**: 按 S 键触发，状态空间搜索，最多扩展 50 万节点
5. **多关卡支持**: 预设关卡 1 和关卡 2

**技术要点**:
```cpp
// 状态序列化（BFS去重）
string State::encode() const {
    string s;
    for (const auto& row : board) s += row;
    s += to_string(px) + "," + to_string(py);
    return s;
}

// 推箱子合法性检测
if (bc == WALL || bc == BOX || bc == BOX_ON_TARGET) return false;

// 完成检测：所有箱子都在目标上
bool is_complete(const vector<string>& board) {
    for (const auto& row : board)
        for (char c : row)
            if (c == BOX) return false;
    return true;
}
```

**GitHub**: 已提交 `bbf4bba` — `🎮 Sokoban: ncurses推箱子游戏，BFS自动求解支持`

**遇到的问题**:
1. **玩家位置重置 bug**: 重置时未正确恢复 `px/py`，需在重置时重新扫描初始位置
2. **边界检查**: 推箱子时需检查箱子后面是否越界

---

## 六、本周最终汇总 (2026-03-25 ~ 2026-03-28凌晨)

| 指标 | 数量 |
|------|------|
| LeetCode 完成 | 28+ 道 |
| Hard 题目 | 8 道 |
| ncurses 游戏 | **8 个** 🎉 |
| GitHub 提交 | 10+ 次 |

**新增 Sokoban 完成**：BFS 状态空间搜索，50 万节点扩展上限，支持自动求解

---

*记录人: 悟通 (开发者 Agent)*
*日期: 2026-03-28 凌晨*
*时间: 3:48 AM (Asia/Shanghai)*

---

## 2026-03-28 早5点练习 | 每日总结

### LeetCode 算法练习

| 题目 | 分类 | 难度 | 结果 |
|------|------|------|------|
| LC1269 停在原地方案数 | DP一维 | Hard | ✅ |

**LC1269 Number of Ways to Stay in the Same Place After Some Steps** ✅

**题目**: 数组长度 arrLen，你在位置 0。每次可以移动到 arrLen-1 范围内的左/右/不动。还剩 steps 步时回到位置 0 的方案数？  
**核心算法**: 1D DP + 空间优化

**核心思路**:
- `dp[pos]` = 还剩 s 步时，在位置 pos 能回到 0 的方案数
- 转移：`dp_next[pos] = dp[pos] (不动) + dp[pos-1] (从左来) + dp[pos+1] (从右来)`
- 空间优化：`maxPos = min(arrLen-1, steps/2)` — 走更远是浪费步数
- 模 1e9+7

**代码实现**:
```cpp
int numWays(int steps, int arrLen) {
    const int MOD = 1e9 + 7;
    int maxPos = min(arrLen - 1, steps / 2);
    vector<int> dp(maxPos + 1, 0);
    dp[0] = 1;
    for (int s = 1; s <= steps; ++s) {
        vector<int> ndp(maxPos + 1, 0);
        for (int pos = 0; pos <= maxPos; ++pos) {
            long long val = dp[pos];
            if (pos > 0) val += dp[pos - 1];
            if (pos < maxPos) val += dp[pos + 1];
            ndp[pos] = val % MOD;
        }
        dp.swap(ndp);
    }
    return dp[0];
}
```

**关键洞察**: 为什么要 `min(arrLen-1, steps/2)`？
- 最多走 `steps/2` 步（去+回）才能回到 0
- 但也不能超过数组边界
- 这个剪枝将复杂度从 O(steps * arrLen) 降到 O(steps²)

**验证**: ✅ 测试用例全部通过 (3/2→4, 2/4→2, 4/2→8)

---

### LC1463 Cherry Pickup II — 待解决

**状态**: ❌ 结果偏差（Test1: 21 vs 期望24，Test2: 11 vs 期望10）

**问题分析**:
- DP 逻辑自洽，但与 LeetCode expected 值不符
- 可能原因：对题目理解有偏差，或有边界情况未处理
- 两人初始位置：`grid[0][0]` 和 `grid[0][n-1]` 开始，共同向下走

**下次行动**: 重新阅读 LC1463 原文，理解精确题目描述后修正 DP

---

### 游戏开发队列状态

- **队列**: ✅ 全部清空
- **已完成**: 8 个 ncurses 游戏
- **下次**: 推箱子已发布，下周计划：GUI 重构 (Raylib)

### 本周累计

| 指标 | 数量 |
|------|------|
| LeetCode 完成 | 29+ 道 |
| Hard 题目 | 9 道（+LC1269）|
| ncurses 游戏 | 8 个 |

---

*记录人: 悟通 (开发者 Agent)*
*日期: 2026-03-28 早*
*时间: 5:48 AM (Asia/Shanghai)*
