# 代码练习日志 - 2026-04-02

## 上午练习

### LC322 Coin Change (Medium)
**问题**: 给定硬币面额数组 coins 和总额 amount，求最少硬币数凑成 amount
**思路**: DP 1D - dp[i] = 凑成金额 i 所需的最少硬币数, dp[i] = min(dp[i], dp[i - coin] + 1)
**复杂度**: O(n * amount) Time, O(amount) Space
**结果**: Test1=3 ✅, Test2=-1 ✅, Test3=0 ✅, Test4=3 ✅
**代码路径**: `projects/lc-practice/lc322_coin_change.cpp`

### LC416 Partition Equal Subset Sum (Medium)
**问题**: 判断能否将数组分成两部分和相等
**思路**: 背包 DP - sum为奇数→false; dp[i]=是否能凑成和i; 逆序遍历避免重复使用
**复杂度**: O(n * sum/2), Space: O(sum/2)
**结果**: Test1=1 ✅, Test2=0 ✅, Test3=0 ✅, Test4=0 ✅, Test5=1 ✅, Test6=0 ✅, Test7=1 ✅
**代码路径**: `projects/lc-practice/lc416_partition.cpp`

---

## 下午练习 (继续)

### LC130 Surrounded Regions (Medium)
**问题**: 将被X完全包围的O区域变成X（边界O及其连通O保持不变）
**思路**: DFS/BFS从四边出发标记所有与边界连通的O（用#标记），剩余O都是被X包围的，改为X，最后恢复#→O
**核心洞察**: 只需从边界出发DFS，难点是理解"#作为临时标记→最终还原为O"的翻转逻辑
**复杂度**: O(n*m) Time, O(n*m) Space (recursion stack)
**结果**: Test1=✅, Test2=✅, Test3=✅, Test4=✅ (ALL PASS)
**代码路径**: `projects/lc-practice/lc130_surrounded_regions.cpp`

### 技术积累 (Emscripten WASM)

#### JS加载已定义的Module
- 问题: `mahjong.js` 已定义 `var Module` 对象，重新定义会覆盖
- 解决: 在引入 `mahjong.js` **之前** 定义 `var Module = { onRuntimeInitialized: fn }`
- Emscripten 会在运行时初始化完成后自动调用 `onRuntimeInitialized`

#### WASM Builder脚本
- 使用 grep+sed 提取 `EMSCRIPTEN_KEEPALIVE` 标记的函数
- JSON格式 `EXPORTED_FUNCTIONS`
- HTTP server 解决 CORS 问题

#### Emscripten 编译选项
- `EMSCRIPTEN_KEEPALIVE` 保持函数不被 tree-shake
- `EXPORTED_FUNCTIONS` 需要以 `_` 前缀列出函数
- `ALLOW_MEMORY_GROWTH=1` 允许动态内存增长
- `MODULARIZE=1` 导出函数为 `Module._wasm_xxx()`
- 关键: GameModule 必须在 game.js 加载之后再加载

---

## 今日汇总 (2026-04-02)

| 题目 | 难度 | 算法 | 结果 |
|------|------|------|------|
| LC322 Coin Change | Medium | 动态规划 | ✅ |
| LC416 Partition Equal Subset Sum | Medium | 背包DP | ✅ |
| LC169 Majority Element | Easy | Boyer-Moore | ✅ (上午) |
| LC53 Maximum Subarray | Medium | Kadane | ✅ (上午) |
| LC46 Permutations | Medium | Backtracking | ✅ (上午) |
| LC17 Letter Combinations | Medium | Backtracking | ✅ (上午) |
| LC130 Surrounded Regions | Medium | DFS边界标记 | ✅ (晚) |

**今日总计**: 7题 (3 Easy/Medium + 4 上午 Medium)

---

## 经验沉淀

### Surrounded Regions 核心模式 (LC130/LC200/LC463 关联)
```
1. 从四边出发DFS/BFS，标记所有边界O及其连通区域为 '#'
2. 剩余所有O（未标记）都是被X包围的 → 改为X
3. 恢复所有 '#' → O
```
- 关键洞察: 边界O不可能被捕获，所以从边界出发DFS是正确思路
- 类似: LC200岛屿数量(4方向BFS/DFS), LC463岛屿周长

### Emscripten MODULARIZE 模式
- `emcc -s MODULARIZE=1 -s EXPORT_NAME="GameModule"`
- `var gameModule = GameModule({ canvas: ... })`
- 导出函数: `gameModule._wasm_funcname(arg1, arg2)`
- `onRuntimeInitialized`: WASM完全加载后回调

---

*版本: 1.1 | 更新: 2026-04-02 23:40*
