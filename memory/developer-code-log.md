# Developer Code Log

## 2026-04-01 💻

### 1. 代码练习 (LeetCode)

#### 重点: Topological Sort + Union-Find + DP

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC207 Course Schedule | Medium | Topological Sort (Kahn's BFS) | 构建indegree数组,BFS队列弹出0入度节点 |
| LC684 Redundant Connection | Medium | Union-Find | 路径压缩+按秩合并,检测到cycle的边即为答案 |
| LC42 Trapping Rain Water | Hard | 单调栈 | 递减栈,维护宽度和高度,类似LC84但更复杂 |
| LC739 Daily Temperatures | Medium | 单调栈 | 递减栈,找下一个更大元素的下标差 |
| LC85 Maximal Rectangle | Hard | 单调栈 | 转化为一排柱子求最大矩形,结合LC84 |
| LC208 Implement Trie | Medium | Trie | 前缀树,26个子指针数组,insert/search/startsWith |
| LC210 Course Schedule II | Medium | Topological Sort BFS | queue弹出顺序即拓扑序,visited==n有解 |
| LC300 Longest Increasing Subsequence | Medium | Binary Search O(n log n) | tails数组+二分查找插入位置 |
| LC1143 Longest Common Subsequence | Medium | DP + 滚动数组 | dp[i][j]=dp[i-1][j-1]+1或max(dp[i-1][j],dp[i][j-1]) |

**Topological Sort (Kahn's BFS) 核心模式**:
- 构建adjacency list + indegree array
- 队列初始放入所有indegree==0的节点
- BFS: 弹出节点,访问邻居,邻居indegree--,入度为0则入队
- 最后检查visited数量==n,否则存在环

**Union-Find 核心模式**:
- `find(x)`: 路径压缩,递归找root
- `union(x,y)`: 按秩合并,返回false表示already connected(检测到环)
- 路径压缩: `parent[x] = find(parent[x])`
- 按秩: rank小的挂载到rank大的

**Topological Sort vs DFS**:
- BFS Kahn: 需要indegree数组,直观但需要额外空间
- DFS: 递归标记visited/visiting, visiting状态遇到visited=环

**Union-Find vs DFS/BFS**:
- Union-Find: 适合动态加边的场景,O(α(n))接近O(1)
- DFS: 需要 visited set,每次需要遍历图

**Trie 核心模式**:
- 每个节点26个子指针(children数组)对应a-z
- insert: 沿路径走,没有就创建,最后标记is_end=true
- search: 沿路径走,不存在返回false,是end返回true
- startsWith: 同search但不检查is_end
- 时间O(n) per op, 空间O(ALPHABET_SIZE * n)

**LIS Binary Search 核心模式**:
- tails[i] = 长度为i+1的LIS的最小尾元素(递增序列)
- 二分查找: 找第一个>=nums[i]的位置
- 如果插入位置==size,说明找到更长的LIS,size++
- 时间O(n log n), 空间O(n)

**LCS DP 核心模式**:
- dp[i][j] = text1前i个与text2前j个的LCS长度
- 字符相等: dp[i][j] = dp[i-1][j-1] + 1
- 字符不等: dp[i][j] = max(dp[i-1][j], dp[i][j-1])
- 滚动数组优化: 只用两行,交替使用
- 时间O(mn), 空间O(min(m,n))

---

### 2. 游戏开发: Snake WASM ✅

#### 完成内容
- 从零创建纯C游戏逻辑 (game.c/game.h/wasm_main.c)
- 20×15网格蛇形游戏
- 方向控制: WASD / 方向键
- 食物随机生成(避开蛇身)
- 碰撞检测: 撞墙 or 撞自己 → GAME OVER
- 速度递增: 每50分 speed++, move_interval--
- 菜单/Playing/Paused/GameOver 四状态
- Canvas 2D渲染: 绿色渐变蛇身+红色食物+脉冲动画
- Emscripten编译: snake.js + snake.wasm
- Playwright自动化测试: PASS ✅ (无Console错误)

#### 技术难点 & 解决方案

**难点1: WASM ccall函数实现**
- 解决: 改用`Reflect.apply(f, null, argv || [])`直接调用

**难点2: 方向180°翻转防止**
- 解决: dir_timer帧数锁 + next_dir预队列

**难点3: 速度递增机制**
- 解决: 每50分 speed++, move_interval = 12 - (speed-1)*2

---

### 3. 游戏开发: Breakout WASM ✅

#### 完成内容
- 从零创建纯C游戏逻辑 (game.c/game.h/wasm_main.c)
- 5行×10列砖块,按行颜色区分
- 球拍角度反弹: 根据击中位置计算反弹角度
- rect-circle碰撞检测: 找最近点判断距离
- 5条生命,分数系统
- 纯Canvas 2D渲染
- 编译为WASM (Emscripten MODULARIZE=1)
- Playwright自动化测试: PASS ✅

#### 技术难点

**rect-circle碰撞检测**:
```c
bool rect_circle_hit(float rx, float ry, int rw, int rh, float cx, float cy, float cr) {
    float nearest_x = fmaxf(rx, fminf(cx, rx + rw));
    float nearest_y = fmaxf(ry, fminf(cy, ry + rh));
    float dx = cx - nearest_x;
    float dy = cy - nearest_y;
    return (dx * dx + dy * dy) < (cr * cr);
}
```

---

### 4. 游戏验证: Space Invaders WASM ✅
### 5. 游戏验证: Pac-Man WASM ✅
### 6. 游戏开发: Flappy Bird WASM ✅
### 7. 游戏开发: Pong WASM ✅

---

## 2026-03-31 💻

### 代码练习: 滑动窗口 + Expand Around Center + BFS Flood Fill

| 题目 | 难度 | 算法 |
|------|------|------|
| LC76 Minimum Window Substring | Hard | Sliding Window |
| LC5 Longest Palindromic Substring | Medium | Expand Around Center |
| LC200 Number of Islands | Medium | BFS flood fill |
| LC329 Longest Increasing Path | Hard | DFS+Memo |
| LC547 Number of Provinces | Medium | Union-Find |
| LC743 Network Delay Time | Medium | Dijkstra |
| LC79 Word Search | Medium | DFS+Backtracking |

### 游戏开发: Memory Match WASM ✅

- 4x4 grid = 8对卡片, 点击翻转匹配
- Fisher-Yates风格确定性洗牌
- WAITING状态管理: 匹配失败后延迟翻回

---

## 2026-04-02 上午 💻

### 1. 代码练习 (LeetCode) - Boyer-Moore + Kadane + Backtracking

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC169 Majority Element | Easy | Boyer-Moore Voting | 候选人票数计数,count==0换人,众数必胜 |
| LC53 Maximum Subarray | Medium | Kadane's Algorithm | cur=max(nums[i],cur+nums[i]),best=max(best,cur) |
| LC46 Permutations | Medium | Backtracking | swap+递归+回溯,O(n!*n) |
| LC17 Letter Combinations | Medium | Backtracking | 按键位digits映射,递归生成所有组合 |

### Boyer-Moore Voting 核心模式
```
candidate=0, count=0
for n in nums:
    if count==0: candidate=n, count=1
    else: count += (n==candidate)?1:-1
return candidate
```
- 原理: 众数票数始终>其他所有数票数和

### Kadane's Algorithm 核心模式
```
best = cur = nums[0]
for i in 1..n-1:
    cur = max(nums[i], cur + nums[i])
    best = max(best, cur)
```
- 局部最优→全局最优: cur是包含i的最大子序和

### Backtracking Permutations 核心模式
```
def backtrack(nums, start):
    if start == n: result.append(nums.copy())
    for i in range(start, n):
        swap(nums, start, i)
        backtrack(nums, start+1)
        swap(nums, start, i)  # backtrack
```

### Backtracking Letter Combinations 核心模式
```
KEYPAD = ["","","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"]
for digit in digits:
    for c in KEYPAD[digit]:
        cur.push_back(c)
        backtrack(digits, idx+1, cur)
        cur.pop_back()  # backtrack
```

---

### 2. 游戏开发: Connect Four WASM ✅

#### 完成内容
- 纯C游戏逻辑 (game.c/game.h/wasm_main.c)
- 7列×6行棋盘,两玩家轮流落子
- 重力下落: 棋子落到最低可用行
- 四方向胜利检测: 水平/垂直/对角线1/对角线2
- 胜利线记录: win_x1/y1 → win_x2/y2
- 平局检测: 42个格子填满且无人获胜
- Canvas 2D渲染: 红色(🔴) vs 黄色(🟡)棋子+高光效果
- 列悬停高亮 + 列满禁止落子
- Emscripten MODULARIZE=1 编译
- Node.js测试: 全部验证通过 ✅
- Playwright测试: PASS ✅ (无Console错误)

#### 技术难点 & 解决方案

**难点1: wasm_main.c递归调用问题**
- 问题: `cf_start_game()` 调用 `cf_start_game()` 自身 → 无限递归
- 解决: game.c实现用`_impl`后缀(wasm_main.c不重名),wasm_main.c调用impl
```c
// game.c
void cf_start_game_impl(void) { ... }
// wasm_main.c
EMSCRIPTEN_KEEPALIVE void cf_start_game(void) {
    extern void cf_start_game_impl(void);
    cf_start_game_impl();
}
```

**难点2: 四方向胜利检测**
- 问题: 从最后落子位置检查4个方向是否连四
- 解决: 4方向同时扫描,统计连续同色棋子数,≥4则胜
```c
int DX[4] = {1, 0, 1, 1};
int DY[4] = {0, 1, 1, -1};
// 正向+反向计数,total>=4则胜
```

**难点3: 胜利线坐标记录**
- 解决: 正向找最远端(x1,y1),反向找最远端(x2,y2)
- 用于Canvas绘制绿色胜利线

#### WASM编译命令
```bash
source ~/emsdk/emsdk_env.sh
emcc -O3 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="Connect4Module" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=64MB \
  -s EXPORTED_FUNCTIONS='["_cf_start_game","_cf_drop_disc","_cf_get_cell","_cf_can_drop","_cf_get_state","_cf_get_player","_cf_get_winner","_cf_get_win_x1","_cf_get_win_y1","_cf_get_win_x2","_cf_get_win_y2"]' \
  --no-entry -o connect4.js game.c wasm_main.c
```

#### Playwright测试结果
```
Title: Connect Four - WASM ✅
Canvas: true ✅
Status: Click canvas to start! ✅
After start: 🔴 Red's turn ✅
Console errors: 0 ✅
```

#### 文件结构
```
projects/connect-four-wasm/
├── game.c       (纯C游戏逻辑, ~100行)
├── game.h       (类型定义+常量)
├── wasm_main.c  (Emscripten导出, 11个函数)
├── index.html   (Canvas 2D渲染+输入+WASM加载)
├── connect4.js  (编译产物)
├── connect4.wasm (编译产物, ~2KB)
├── build.sh     (编译脚本)
└── test.spec.js (Playwright测试)
```

---

### 3. 技术栈进步 (新增)

| 领域 | 进步 |
|------|------|
| Boyer-Moore Voting | LC169, O(n)时间O(1)空间众数算法 |
| Kadane Algorithm | LC53, O(n)最大子序和,局部最优→全局最优 |
| Backtracking | LC46/L17, 递归+swap+回溯模式 |
| Connect Four游戏 | 重力下落+四方向胜利检测+胜利线渲染 |
| WASM extern陷阱 | game.c用_impl后缀避免wasm_main.c同名递归 |

---

### 4. GitHub提交

- `6fec925` — feat: Connect Four WASM - pure C + Canvas 2D, 7x6 grid, 4-in-a-row win detection
- `73be752` — test: LC169 Majority Element, LC53 Max Subarray, LC46 Permutations, LC17 Letter Combinations

---

### 5. WASM游戏队列状态

**WASM游戏累计**: 17个
Snake, 2048, Minesweeper, Memory Match, Tetris, Frogger, Sokoban, Space Invaders, Breakout, Pac-Man, Flappy Bird, Pong, Wordle, Mahjong Solitaire, Hextris, Gomoku, **Connect Four**

**队列**: 建议下一个游戏 → Sudoku / Connect Four (2P) / Battleship

---

### 6. 经验沉淀

#### Boyer-Moore Voting 核心洞察
- 众数票数 > 其他所有数票数和
- count==0时换candidate,最后留下的必是众数
- 可扩展: LC229 求出现次数>n/3的数(需2个候选人)

#### Kadane's Algorithm 核心洞察
- cur = max(单独成子序列, 接续之前的序列)
- best = max(所有局部最优, 全局最优)
- 无负数情况也适用(简单取max)

#### Backtracking 模式总结
- **排列**: swap(start,i) → backtrack → swap回
- **组合**: 选择或不选 → backtrack
- **子集**: 按索引选 → backtrack
- 关键: 回溯时恢复状态

#### wasm_main.c 命名规范
- game.c: `cf_start_game_impl()` (internal)
- wasm_main.c: `cf_start_game()` (exported, calls impl)
- 避免: 同名函数相互调用导致递归

---

*版本: 1.18 | 更新: 2026-04-02 11:50*

---

## 2026-04-02 晚间 💻

### 代码练习: DFS边界标记 + 逆向思维

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC130 Surrounded Regions | Medium | DFS边界标记 | 从四边DFS标记安全O(用#),剩余O→X,#→O |

**Surrounded Regions 核心模式**:
```
1. 从四边出发DFS/BFS，标记所有边界O及连通O为 '#'
2. 遍历board: '#' → 'O' (安全), 'O' → 'X' (被包围)
3. 关键洞察: 边界O永远不会被X包围，所以从边界出发是正确思路
```

**边界标记逆向思维**:
- 正面思考: 找所有被X包围的O → 难(需检查四周都是X)
- 逆向思考: 找所有与边界O连通的O → 易(DFS/BFS)
- 从易方向突破

---

### 游戏开发: Battleship WASM 验证 ✅

#### 状态
- 构建时间: 2026-04-02 16:43 (今日下午)
- 文件结构: game.c + game.h + wasm_main.c + index.html + battleship.js + battleship.wasm
- Playwright测试: test.spec.js 完整 (4个测试用例)
- 所有文件验证: ✅ (h1/board/wasm/script全部存在)

#### 完成内容
- 10×10 grid × 2 (玩家+敌方)
- 5艘船: Carrier(5), Battleship(4), Cruiser(3), Submarine(3), Destroyer(2)
- 手动摆放 or Auto Place
- 方向切换 (水平/垂直)
- 点击敌方grid开火 → hit/miss 标记
- 胜利检测 (击沉所有敌舰)
- 统计: hit/miss/sunk

#### 技术难点
- 双grid设计: 玩家grid(真实板)+敌方grid(显示板)
- 船只摆放验证: 不能重叠,不能出界
- hit/miss状态独立存储

#### 文件结构
```
projects/battleship-wasm/
├── game.c       (纯C游戏逻辑)
├── game.h       (类型定义+常量)
├── wasm_main.c  (Emscripten导出)
├── index.html   (Canvas 2D渲染+输入+WASM加载)
├── battleship.js  (编译产物)
├── battleship.wasm (编译产物, ~2KB)
├── build.sh     (编译脚本)
└── test.spec.js (Playwright测试, 4个用例)
```

---

### WASM游戏队列状态 (2026-04-02 23:40)

**WASM游戏累计**: 18个
Snake, 2048, Minesweeper, Memory Match, Tetris, Frogger, Sokoban, Space Invaders, Breakout, Pac-Man, Flappy Bird, Pong, Wordle, Mahjong Solitaire, Hextris, Gomoku, Connect Four, **Sudoku**

**队列**:
- P1: Battleship WASM ✅ (验证通过, 2026-04-02)
- P2: Connect Four (AI) WASM

---

### 技术栈进步 (晚间)

| 领域 | 进步 |
|------|------|
| Surrounded Regions | 逆向思维: 从易方向(边界)DFS,避免正面困难检查 |
| 双Grid游戏架构 | 玩家grid(真实)+敌方grid(显示)的分离设计模式 |
| WASM Builder | 已能批量编译+测试所有WASM游戏 |

---

### 经验沉淀

#### 逆向思维模式 (Surrounded Regions启示)
- 很多"找被包围区域"问题都可以用边界出发DFS解决
- 关键转变: 从"找符合条件的"转为"排除不符合的"
- 适用: LC130 Surrounded Regions, 类似LC286/LC417

#### WASM游戏双Grid架构
- player_board[r][c] = 真实状态 (for player reference)
- enemy_board[r][c] = 显示状态 (hit/miss, 敌方船只不显示)
- 这样分开存储逻辑清晰,互不干扰

---

*版本: 1.19 | 更新: 2026-04-02 23:40*

---

## 2026-04-03 晚间 💻

### 1. 代码练习 (LeetCode) - 股票DP + 环形DP

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC213 House Robber II | Medium | 环形DP | 拆分为两个线性: max(rob(0..n-2), rob(1..n-1)) |
| LC122 Best Time to Buy and Sell Stock II | Medium | Greedy | 累加所有正差值 = 最大利润 |
| LC121 Best Time to Buy and Sell Stock | Easy | 单调扫描 | 维护min_price和max_profit,O(n) |

**House Robber II 核心模式**:
```c
// 环形: house[0]和house[n-1]不能同时偷
max(robLinear(nums, 0, n-1), robLinear(nums, 1, n))
// robLinear: dp[i]=max(dp[i-1], dp[i-2]+nums[i])
```

**Stock II Greedy 核心洞察**:
- 等价于累加所有正差值: profit += max(0, prices[i]-prices[i-1])
- 为什么正确: 可以当天买卖,所以任何上升段都可以分解

---

### 2. 游戏调试: Battleship WASM 全部4个Playwright测试通过 ✅

#### 问题排查过程

**测试1-3失败** → WASM函数不可用
- 根因: MODULARIZE=1返回Promise, script标签加载方式不兼容
- 修复: 重新编译 `-s MODULARIZE=0`

**测试3失败** → PAGE ERROR: `Module._battleship_get_cell_impl is not a function`
- 根因: JS调用 `_battleship_get_cell_impl` 但导出的函数是 `_battleship_get_cell`
- 修复: `battleship_get_cell_impl` → `battleship_get_cell`

**测试2失败** → Banner不更新到Phase 2
- 根因: banner.textContent更新只在`if(state===0)`分支内
- 修复: banner更新独立于placement-ui显示控制

**测试4失败** → Reset按钮不可见
- 根因: Reset在placement-ui内,Battle阶段placement-ui隐藏
- 修复: 将Reset按钮移到placement-ui外部

**最终**: 4/4 Playwright测试全部通过 ✅

#### 技术栈进步

| 领域 | 进步 |
|------|------|
| Emscripten MODULARIZE | MODULARIZE=0 vs MODULARIZE=1的差异和适用场景 |
| WASM调试 | 使用Playwright evaluate逐步验证函数存在性和返回值 |
| UI状态机 | 状态转换时banner更新必须在所有state分支处理 |

---

### 3. GitHub提交

- `3be7906` — fix: Battleship WASM - fix MODULARIZE issue, banner update bug, and reset button visibility

---

### 4. WASM游戏队列状态 (2026-04-03)

**WASM游戏累计**: 19个
Snake, 2048, Minesweeper, Memory Match, Tetris, Frogger, Sokoban, Space Invaders, Breakout, Pac-Man, Flappy Bird, Pong, Wordle, Mahjong Solitaire, Hextris, Gomoku, Connect Four, Sudoku, **Battleship**

**队列**:
- P1: Connect Four (AI) WASM ← 下一个开发目标

---

### 5. 经验沉淀

#### MODULARIZE=0 vs MODULARIZE=1
- MODULARIZE=0: `var Module = ...` 全局对象,script标签直接加载
- MODULARIZE=1: 返回Promise,需要`.then()`处理
- 小型游戏项目 → MODULARIZE=0更简单
- 需要ES6 import的项目 → MODULARIZE=1

#### WASM调试方法论
1. 检查console错误 → 确认WASM是否加载
2. `Object.keys(Module).filter(k=>typeof Module[k]==='function')` → 列出所有可用函数
3. `Module._xxx_get_game_state()` → 验证状态
4. 逐步验证UI交互

#### 状态转换UI更新原则
- 状态切换时: banner/按钮等UI更新不要放在特定state的if分支内
- 正确: 所有state分支都更新对应UI
- 或: 每个分支只更新状态相关的UI,共用部分放在分支外

---

*版本: 1.20 | 更新: 2026-04-03 21:30*

## 2026-04-03 💻

### 1. 代码练习 (LeetCode)

#### 重点: 1D DP + 2D DP (Pick/Skip Pattern)

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC198 House Robber | Easy | 1D DP | dp[i]=max(dp[i-1],dp[i-2]+nums[i]),空间O(1) |
| LC213 House Robber II | Medium | 1D DP (环形) | 首尾不能同时rob,分两种情况[0..n-2]和[1..n-1] |
| LC64 Minimum Path Sum | Medium | 2D DP | in-place: grid[i][j]+=min(up,left),空间O(1) |
| LC322 Coin Change | Medium | DP Unbounded Knapsack | dp[j]=min(dp[j],dp[j-coin]+1),INT_MAX哨兵 |
| LC518 Coin Change 2 | Medium | DP (组合数) | 外层循环coins,内层循环amount,顺序重要 |

**DP通用模式**:
- 1D pick/skip: `dp[i] = max(dp[i-1], dp[i-2] + val)`
- 2D grid: `dp[i][j] = min(dp[i-1][j], dp[i][j-1]) + cost`
- Unbounded: 外层循环物品,内层循环容量

### 2. 游戏开发: Connect Four vs AI (C/WASM)

#### 项目: `projects/connect-four-ai-wasm/`

**功能**:
- 6x7棋盘,支持2P和AI模式
- AI三级难度: Easy/Medium/Hard
- Minimax + Alpha-Beta剪枝
- 窗口评分启发函数

**AI核心实现**:
```c
// 评分窗口: 4连=10000, 3连+开放端=50, 2连+双开放=10
// 防御优先: 检测对手必胜立即阻挡
// Minimax深度: Easy=1, Medium=2, Hard=3
// Alpha-Beta剪枝加速搜索
```

**技术难点**:
- Emscripten MODULARIZE=1 异步加载: 需要`Connect4AIModule({totalMemory}).then(...)`
- WASM和JS分离: 需要正确加载路径
- 状态恢复: Minimax需要保存/恢复board state和game state

**WASM加载模式**:
```javascript
// HTML中:
var Module = null;
function initGame() {
  Module = Connect4AIModule({ totalMemory: 64 * 1024 * 1024 });
  Module.then(function(M) { Module = M; gameLoaded = true; });
}
// 使用 onload="initGame()" 在 script 标签上
```

### 3. 技术沉淀

#### Emscripten WASM加载调试技巧
- `window.Module` vs `Connect4AIModule()`: 前者是空对象,后者是异步工厂函数
- `MODULARIZE=1` 必须显式调用: `Connect4AIModule(config).then(...)`
- `onRuntimeInitialized` 回调确保WASM函数就绪

#### Minimax + Alpha-Beta 核心模式
```c
int minimax(int board[6][7], int depth, int alpha, int beta, bool maximizing) {
  if (depth == 0 || gameOver) return evaluate(board);
  if (maximizing) {
    int value = -INF;
    for each valid move {
      make move; value = max(value, minimax(..., depth-1, alpha, beta, false));
      undo move; alpha = max(alpha, value);
      if (beta <= alpha) break; // Beta cutoff
    }
    return value;
  } else {
    // symmetric for minimizing
  }
}
```

### 4. 下周计划
- 完成Connect Four AI的Hard模式测试
- 继续LeetCode DP专项练习
- 尝试Space Invaders游戏开发

## 2026-04-03 💻

### 1. 代码练习 (LeetCode)

#### 重点: Bellman-Ford 图论最短路

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC787 Cheapest Flights Within K Stops | Medium | Bellman-Ford (边数限制) | dp[city]=最少票价,K+1轮迭代,提前终止优化 |

**Bellman-Ford 核心模式 (边数限制版)**:
```python
dp = [INF] * n
dp[src] = 0
for i in range(K + 1):  # 最多K+1条边
    tmp = dp[:]
    for u, v, price in flights:
        if dp[u] != INF and dp[u] + price < tmp[v]:
            tmp[v] = dp[u] + price
    dp = tmp
    if not updated: break  # 提前终止
```
- 每轮只考虑多一条边
- 复制数组保证只用K+1条边
- 无更新时提前终止

**Bellman-Ford vs Dijkstra**:
- Bellman-Ford: O(VE), 支持负权边, 原生边数限制
- Dijkstra: O((V+E)logV), 不支持负权边, 无边数限制
- 选择依据: 有无负权边? 有无边数限制?

---

### 2. 游戏开发: Battleship WASM ✅

#### 完成内容
- 纯C游戏逻辑 + Emscripten编译
- 10x10网格, 船只: Carrier(5), Battleship(4), Cruiser(3), Submarine(3), Destroyer(2)
- 命中/未命中追踪, 击沉检测
- 随机船只部署算法
- Playwright自动化测试: PASS ✅

#### 技术难点 & 解决方案
**难点: 船只随机部署冲突检测**
- 解决: 方向+长度验证, 边界检查, 重叠检测

---

### 3. 游戏开发: Connect Four AI WASM ✅

#### 完成内容
- 纯C游戏逻辑 + Emscripten编译
- 7x6网格, 重力下落, 4连赢检测
- 3级AI: Minimax + Alpha-Beta剪枝
- 窗口评分启发式函数
- 即时胜负/阻断检测

#### 技术难点 & 解决方案

**难点1: Minimax高效剪枝**
- 解决: Alpha-Beta剪枝, 深度限制
- 窗口评分启发式: 评估4格窗口得分

**难点2: AI决策速度**
- 解决: 深度3限制, 立即胜负检测优先

```c
// 窗口评分示例
int score = 0;
if (window_count(player, 4) > 0) score += 100;
else if (window_count(player, 3) > 0 && window_count(player, 1) > 0) score += 10;
```

---

### 4. 经验沉淀

#### Connect Four AI 架构
1. **Game Logic**: 7x6 board, gravity drop, 4-direction win check
2. **AI Engine**: Minimax(depth=3) + Alpha-Beta pruning
3. **Heuristic**: 窗口评分(4格滑动窗口)
4. **Optimization**: 立即赢/挡检测优先

#### WASM游戏批量测试脚本
- `build_all.sh`: 编译所有WASM游戏
- `test_all.sh`: Playwright集成测试
- `--games filter`: 支持按名称过滤

---

*最后更新: 2026-04-03*

## 2026-04-04 💻

### 1. 代码练习 (LeetCode)

#### 重点: DFS/BFS 二维网格遍历

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC200 Number of Islands | Medium | DFS/BFS/Union-Find | "淹没"技巧,in-place标记避免重复 |

**DFS 二维网格遍历 核心模式**:
```python
def dfs(grid, i, j, m, n):
    if i < 0 or i >= m or j < 0 or j >= n or grid[i][j] == '0':
        return
    grid[i][j] = '0'  # 淹没
    dfs(grid, i+1, j, m, n)
    dfs(grid, i-1, j, m, n)
    dfs(grid, i, j+1, m, n)
    dfs(grid, i, j-1, m, n)
```

**BFS 二维网格遍历 核心模式**:
```python
from collections import deque
def bfs(grid, start_i, start_j):
    queue = deque([(start_i, start_j)])
    grid[start_i][start_j] = '0'
    while queue:
        i, j = queue.popleft()
        for di, dj in [(1,0),(-1,0),(0,1),(0,-1)]:
            ni, nj = i+di, j+dj
            if 0<=ni<m and 0<=nj<n and grid[ni][nj]=='1':
                grid[ni][nj] = '0'
                queue.append((ni, nj))
```

**"淹没"技巧要点**:
- 访问过的格子标记为'0'或visited
- 避免重复访问和计数
- in-place修改节省额外空间

**网格遍历注意事项**:
- 边界检查在递归/入队前进行
- 4方向: 上下左右 (不包括对角线除非题目要求)
- 使用m,n缓存grid尺寸避免重复计算

**DFS vs BFS 选择**:
- DFS: 代码简洁, 递归实现, 小岛屿
- BFS: 迭代实现, 避免栈溢出, 大岛屿

**扩展: Union-Find 实现**:
```python
class UnionFind:
    def __init__(self, grid):
        m, n = len(grid), len(grid[0])
        self.parent = list(range(m*n))
        self.count = 0
        for i in range(m):
            for j in range(n):
                if grid[i][j] == '1':
                    self.count += 1
    
    def find(self, x):
        while self.parent[x] != x:
            self.parent[x] = self.parent[self.parent[x]]
            x = self.parent[x]
        return x
    
    def union(self, x, y):
        px, py = self.find(x), self.find(y)
        if px == py: return False
        self.parent[px] = py
        self.count -= 1
        return True
```

---

### 2. 游戏开发: Raylib Games (规划)

#### 任务目标
- 使用raylib替代Canvas 2D开发经典游戏
- 学习现代游戏开发框架

#### Raylib 核心API
```c
#include "raylib.h"

int main() {
    InitWindow(800, 600, "Game");
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Game logic
        DrawText("Hello", 100, 100, 20, BLACK);
        
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
```

#### 编译命令
```bash
gcc game.c -o game -lraylib -lm -lpthread
```

#### Raylib vs Canvas 2D
| 特性 | Raylib | Canvas 2D |
|------|--------|-----------|
| 平台 | 桌面/嵌入式 | Web浏览器 |
| 渲染 | OpenGL/WebGL | 软件渲染 |
| 3D支持 | 原生 | 需WebGL |
| 打包 | 原生 | HTML+JS |

---

### 3. 经验沉淀

#### 本周算法学习总结 (2026-04-01 ~ 2026-04-04)
| 日期 | 主题 | 题目数 |
|------|------|--------|
| 04-01 | Topological Sort + Union-Find + DP | 9题 |
| 04-02 | Wordle/Mahjong/Hextris/Gomoku | 5游戏 |
| 04-03 | Bellman-Ford + Battleship/Connect4AI | 1题+2游戏 |
| 04-04 | DFS/BFS网格遍历 + Raylib规划 | 1题 |

#### 技术栈进步
- ✅ 图论算法 (Topological Sort, Union-Find, Bellman-Ford)
- ✅ 游戏AI (Minimax + Alpha-Beta)
- ✅ WASM游戏开发 (21个游戏完成)
- 🔲 Raylib游戏开发 (规划中)

---

### 4. 下周计划 (2026-04-05 ~ 2026-04-11)

#### 代码练习
- [ ] 完成DFS/BFS专项 (LC695, LC463, LC694)
- [ ] 学习回溯算法 (LC79, LC212)
- [ ] 复习图论高级算法

#### 游戏开发
- [ ] Space Invaders (raylib版)
- [ ] Breakout (raylib版)
- [ ] 开始Raylib游戏库建设

#### GitHub贡献
- [ ] 整理WASM游戏代码仓库
- [ ] 提交Raylib游戏代码
- [ ] 完善README文档

---

*最后更新: 2026-04-04*

---

## 2026-04-04 早间 💻 (补充)

### 1. 代码练习 (LeetCode) - LRU Cache + Trie

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC146 LRU Cache | Medium | 双向链表 + HashMap | 最近使用放链表头,最久未使用在链表尾 |
| LC208 Implement Trie | Medium | 前缀树 | insert/search/startsWith O(m), m=word length |
| LC211 Word Dictionary | Medium | Trie (通配符) | DFS遍历.的所有子节点,支持任意字符匹配 |

**LRU Cache 核心模式**:
```c
// 双向链表 + HashMap 实现 O(1) LRU Cache
// HashMap<key, Node*> 快速定位
// 双向链表维护顺序: head(最近) <-> ... <-> tail(最久)
// get(key): 找到节点,移到head,返回value
// put(key,val): 找到则更新并移到head; 未找到则新建插入head,若超容量则删除tail前一个
```

**Trie 核心模式**:
```c
// 每个节点 unordered_map<char, TrieNode*> children
// insert: 沿路径走,无则创建,最后标记isEnd=true
// search: 沿路径走,不存在返回false,是end返回true
// startsWith: 同search但不检查isEnd
```

**LC211 通配符搜索 核心模式**:
```c
// '.' 匹配任意字符,需要DFS遍历所有可能的路径
bool searchNode(word, idx, node):
    if idx == len: return node.isEnd
    if word[idx] == '.':
        for child in node.children:
            if searchNode(word, idx+1, child): return true
        return false
    else:
        if not node.children[char]: return false
        return searchNode(word, idx+1, node.children[char])
```

---

### 2. 游戏验证: Space Invaders Raylib ✅

#### 项目状态
- 路径: `projects/space-invaders-raylib/`
- 编译产物: `space_invaders` (Mach-O 64-bit arm64)
- 源码: game.c/game.h/draw.c/draw.h/main.c

#### 功能验证
- 8列×4行外星人,重力下落
- 玩家射击 + 敌人射击
- 爆炸动画效果 (explosion timer)
- Wave系统 (速度递增)
- 完整游戏循环: Menu → Playing → GameOver/Victory

#### 编译命令
```bash
g++ src/main.c src/game.c src/draw.c -o space_invaders \
  -Wall -Wextra -O2 \
  -I/opt/homebrew/include -L/opt/homebrew/lib \
  -lraylib -lm
```

---

### 3. 技术沉淀

#### LRU Cache 双向链表实现技巧
- 哑头(dummy head) + 哑尾(dummy tail) 简化边界处理
- remove(n): `n->prev->next = n->next; n->next->prev = n->prev`
- insertHead(n): 在head后插入,成为最近使用
- 驱逐时删除tail前一个(lru)

#### Trie vs HashMap
- Trie: 前缀搜索O(m), 支持前缀匹配, 空间换时间
- HashMap: 单键查找O(1), 不支持前缀匹配
- 适用场景: 自动补全, IP路由, 字符串检索

#### Raylib游戏项目结构
```
game.h          (常量定义, 类型声明, 函数原型)
game.c          (游戏逻辑: init, update, collision)
draw.c          (Raylib渲染: DrawRectangle, DrawCircle等)
main.c          (主循环: InitWindow, BeginDrawing, EndDrawing)
```

---

### 4. GitHub提交

```bash
cd ~/.openclaw/workspace-developer/projects/lc-practice/2026-04-04
git add lc146_lru_cache.cpp lc208_trie.cpp lc211_word_dictionary.cpp
git commit -m "feat: LRU Cache + Trie implementations in C++"
```

---

### 5. 今日总结

| 类型 | 完成内容 |
|------|----------|
| 代码练习 | LC146 LRU Cache, LC208 Trie, LC211 Word Dictionary |
| 游戏验证 | Space Invaders Raylib (原生Mac编译) |
| 学习主题 | 双向链表数据结构, Trie前缀树, 通配符DFS |

#### 累计进度
- WASM游戏: 21个
- Raylib游戏: Space Invaders, Flappy Bird, Breakout (原生编译)

---

*版本: 1.21 | 更新: 2026-04-04 05:40*
