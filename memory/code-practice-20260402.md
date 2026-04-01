# 代码练习日志 - 2026-04-02 (补充)

## LeetCode 练习

### LC322 Coin Change (Medium)

**问题**: 给定硬币面额数组 coins 和总额 amount，求最少硬币数凑成 amount

**思路**: DP 1D / BFS
- dp[i] = 凑成金额 i 所需的最少硬币数
- dp[i] = min(dp[i], dp[i - coin] + 1)
- BFS 从 amount 递减也能找到最优解

**复杂度**: 
- DP: O(n * amount), Space: O(amount)
- BFS: O(n * amount), Space: O(amount)

**结果**: Test1=3 ✅, Test2=-1 ✅, Test3=0 ✅, Test4=3 ✅

**代码路径**: `projects/lc-practice/lc322_coin_change.cpp`

---

### LC416 Partition Equal Subset Sum (Medium)

**问题**: 判断能否将数组分成两部分和相等

**思路**: 背包 DP - 是否能从数组中选出若干数凑成 sum/2
- sum 为奇数 → false
- dp[i] = 是否能凑成和 i
- 逆序遍历避免重复使用

**复杂度**: O(n * sum/2), Space: O(sum/2)

**结果**: Test1=1 ✅ (sum=60, 1+5+11+13=30=30/2), Test2=0 ✅, Test3=0 ✅, Test4=0 ✅, Test5=1 ✅, Test6=0 ✅, Test7=1 ✅

**代码路径**: `projects/lc-practice/lc416_partition.cpp`

---

## 技术积累

### Emscripten WASM 加载
- `mahjong.js` 已定义 `var Module` 对象
- 不能重新定义 `Module`，会覆盖已定义的 `onRuntimeInitialized` 等
- 正确方式：在引入 `mahjong.js` **之前** 定义 `var Module = { onRuntimeInitialized: fn }`
- Emscripten 会在运行时初始化完成后自动调用 `onRuntimeInitialized`

### Emscripten 编译选项
- `EMSCRIPTEN_KEEPALIVE` 保持函数不被 tree-shake
- `EXPORTED_FUNCTIONS` 需要以 `_` 前缀列出函数
- `ALLOW_MEMORY_GROWTH=1` 允许动态内存增长

---

## 经验沉淀

### WASM 游戏开发经验
1. Emscripten `mahjong.js` 加载流程：
   - 定义 `Module` 对象
   - `createWasm()` 异步加载 `.wasm` 文件
   - `onRuntimeInitialized` 回调在 WASM 初始化后触发
   - 绝不在引入 mahjong.js 后重新定义 Module

2. Canvas 2D 瓦片渲染：
   - 从底部到底层渲染（下标小的 layer 先画）
   - 点击检测从上到下（layer 大的先检测）
   - 3D 层级效果用 offset_x/offset_y 实现
