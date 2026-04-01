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

#### Playwright测试结果
- Title: Snake - WASM ✅
- Canvas: true ✅
- HUD: SCORE: 0 | SPEED: 1 ✅
- Console errors: NONE ✅
- Click to start + Arrow key input: PASS ✅

#### 技术难点 & 解决方案

**难点1: WASM ccall函数实现**
- 问题: 使用`Function.apply`生成动态函数时语法错误
- 解决: 改用`Reflect.apply(f, null, argv || [])`直接调用
```javascript
mod = {
  ccall: (name, ret, args, argv) => {
    const f = exp[name];
    if (!f) return 0;
    return Reflect.apply(f, null, argv || []);
  }
};
```

**难点2: 方向180°翻转防止**
- 问题: 蛇不能直接掉头(从LEFT立即变RIGHT)
- 解决: dir_timer帧数锁 + next_dir预队列
```c
if (dir == DIR_UP    && g_game.dir != DIR_DOWN)  g_game.next_dir = dir;
if (dir == DIR_DOWN  && g_game.dir != DIR_UP)    g_game.next_dir = dir;
// ...
g_game.dir_timer = 3; // 3帧后才能再次改向
```

**难点3: 速度递增机制**
- 解决: 每50分 speed++, move_interval = 12 - (speed-1)*2
- speed 1-5: interval = 12,10,8,6,4帧

#### WASM编译命令
```bash
source ~/emsdk/emsdk_env.sh
emcc -O2 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="SnakeModule" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=64MB \
  -s EXPORTED_FUNCTIONS='["_wasm_init_game","_wasm_start_game","_wasm_tick_game","_wasm_set_dir_game","_wasm_toggle_pause_game","_wasm_restart_game","_wasm_get_state_game","_wasm_get_score_game","_wasm_get_speed_game","_wasm_get_snake_len_game","_wasm_get_snake_x_game","_wasm_get_snake_y_game","_wasm_get_food_x_game","_wasm_get_food_y_game"]' \
  --no-entry -o snake.js game.c wasm_main.c
```

#### 文件结构
```
projects/snake-wasm/
├── game.c       (纯C游戏逻辑, ~150行)
├── game.h       (类型定义+常量)
├── wasm_main.c  (Emscripten导出)
├── index.html   (Canvas 2D渲染+输入+WASM加载)
├── snake.js     (编译产物)
└── snake.wasm   (编译产物, 2KB)
```

---

### 3. 游戏开发: Breakout WASM ✅

#### 完成内容
- 从零创建纯C游戏逻辑 (game.c/game.h/wasm_main.c)
- 5行×10列砖块,按行颜色区分
- 球拍角度反弹: 根据击中位置计算反弹角度
- rect-circle碰撞检测: 找最近点判断距离
- 球拍移动+空格发射
- 5条生命,分数系统
- 纯Canvas 2D渲染
- 编译为WASM (Emscripten MODULARIZE=1)
- Playwright自动化测试: PASS ✅

#### Playwright测试结果
- Title: Breakout - WASM ✅
- Canvas: true ✅
- HUD: SCORE:0 LIVES:3 LEVEL:1 50 BRICKS ✅
- Console errors: NONE ✅

#### 技术难点 & 解决方案

**难点1: rect-circle碰撞检测**
- 问题: 球与球拍/砖块碰撞,球是圆形,砖块是矩形
- 解决: 找矩形上离圆心最近的点,判断距离<半径
```c
bool rect_circle_hit(float rx, float ry, int rw, int rh, float cx, float cy, float cr) {
    float nearest_x = fmaxf(rx, fminf(cx, rx + rw));
    float nearest_y = fmaxf(ry, fminf(cy, ry + rh));
    float dx = cx - nearest_x;
    float dy = cy - nearest_y;
    return (dx * dx + dy * dy) < (cr * cr);
}
```

**难点2: 球拍击中角度反弹**
- 解决: 根据击中位置计算角度(150°-270°)
```c
float hit_pos = (b->x - paddle.x) / PADDLE_W; // 0..1
float angle = 150.0f + hit_pos * 120.0f; // 150 to 270 degrees
float rad = angle * M_PI / 180.0f;
float speed = sqrtf(dx*dx + dy*dy);
b->dx = speed * cosf(rad);
b->dy = speed * sinf(rad);
```

**难点3: 砖块碰撞反弹方向**
- 解决: 比较x/y方向overlap,取较小者决定反弹方向

#### WASM编译命令
```bash
source ~/emsdk/emsdk_env.sh
emcc -O3 -s MODULARIZE=1 -s EXPORT_NAME="BreakoutModule" \
  -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=64MB --no-entry -o breakout.js game.c wasm_main.c
```

---

### 4. 游戏验证: Space Invaders WASM ✅

#### Playwright测试结果
- Canvas exists: true ✅
- HUD: SCORE:0 WAVE:1 LIVES:3 ALIENS:32 ✅
- Console errors: NONE ✅

---

### 5. 游戏验证: Pac-Man WASM ✅

#### 完成内容
- 28×31迷宫, 4种幽灵(Blinky/Pinky/Inky/Clyde)
- 幽灵AI: 每种幽灵有不同行为模式
- Frightened模式: 吃能量球后幽灵变蓝,可被吃
- 闪烁效果: frightened计时器<2000ms时闪烁
- Power pellet (大能量球)
- 幽灵眼睛返回家中(被吃后)
- 多关卡, 分数/生命系统
- 纯Canvas 2D渲染

#### Playwright测试结果
- Title: Pac-Man - WASM Edition ✅
- Canvas: true (304×154, WASM canvas) ✅
- Score/Lives/Level/Dots HUD ✅
- Console errors: NONE ✅

#### 文件位置
- `projects/pacman-raylib/wasm/` — 349行game.c + 58行game.h + index.html

---

### 6. 游戏开发: Flappy Bird WASM ✅

#### 完成内容
- 从零创建纯C游戏逻辑 (game.c/game.h/wasm_main.c)
- 重力物理: 每帧 `vy += GRAVITY`, `y += vy`
- 管道生成: 随机gap_y位置, PIPE_INTERVAL间隔
- rect-circle碰撞检测: 鸟与管道上下碰撞
- 分数系统: 穿过管道+1, best分数localStorage存储
- 菜单/Playing/GameOver三状态
- Canvas 2D渲染: 渐变天空+绿色管道+黄色鸟+草地
- Playwright自动化测试: PASS ✅

#### Playwright测试结果
- Title: Flappy Bird - WASM ✅
- Canvas: true ✅
- Console errors: NONE ✅
- Interaction test: PASS ✅

---

### 7. 游戏开发: Pong WASM ✅

#### 完成内容
- 从零创建纯C游戏逻辑 (game.c/game.h/wasm_main.c)
- 球拍角度反弹: 根据击中位置计算反弹角度
- AI对手: 跟踪球y坐标移动
- 球速递增: 每次击中 paddle speed *= 1.05
- 7分获胜,得分/游戏结束/重开流程
- 纯Canvas 2D渲染
- 编译为WASM (Emscripten MODULARIZE=1)
- Playwright自动化测试: PASS ✅

#### Playwright测试结果
- Title: Pong - WASM ✅
- Canvas: true ✅
- HUD: 0 : 0 ✅
- Console errors: NONE ✅

#### 技术难点 & 解决方案

**难点1: 球拍击中角度反弹**
- 解决: `hit_pos = (ball.y - paddle.y) / PADDLE_H`, 映射到±60°角
```c
float hit_pos = (g->ball.y - g->left.y) / PADDLE_H;
float angle = (hit_pos - 0.5f) * 60.0f * M_PI / 180.0f;
float speed = sqrtf(g->ball.dx * g->ball.dx + g->ball.dy * g->ball.dy);
speed *= 1.05f;
g->ball.dx = speed * cosf(angle);
g->ball.dy = speed * sinf(angle);
```

**难点2: AI移动**
- 解决: `diff = ball.y - (paddle.y + PADDLE_H/2)`, 按AI_SPEED趋近
```c
float diff = target - center;
if (diff > AI_SPEED) g->right.y += AI_SPEED;
else if (diff < -AI_SPEED) g->right.y -= AI_SPEED;
```

**难点3: g_state extern问题**
- 解决: game.h声明`extern PongGame g_state;`, game.c定义`PongGame g_state;`
- wasm_main.c include game.h后直接使用g_state

---

### 8. 经验沉淀

#### 技术难点1: Topological Sort环检测
- Kahn's BFS: visited计数不等于n则存在环
- DFS: visiting状态遇到visited节点→环

#### 技术难点2: Union-Find路径压缩+按秩合并
- 路径压缩: find时将路径上所有节点直接指向root
- 按秩合并: 小树挂在大树下,避免树过高
- `α(n)`接近O(1),均摊复杂度极低

#### 技术难点3: rect-circle碰撞
- 找最近点: `fmaxf(rx, fminf(cx, rx+rw))`
- 判断: `(dx*dx + dy*dy) < (cr*cr)`

#### 技术难点4: 球拍角度反弹
- 击中位置映射到角度(±60°)
- 用cos/sin重建速度向量
- 球速递增: speed *= 1.05

#### 技术难点5: Pong AI
- diff = ball.y - paddle_center
- 按固定速度趋近: if diff > speed, paddle += speed

#### 技术难点6: C extern g_state共享
- game.h: `extern PongGame g_state;`
- game.c: `PongGame g_state;` (非static)
- wasm_main.c: include game.h后直接使用

#### 技术难点7: LIS Binary Search O(n log n)
- tails数组: tails[i]=长度为i+1的LIS的最小尾元素
- 二分查找第一个>=x的位置
- 如果位置==size,说明扩展了LIS长度

#### 技术难点8: LCS DP滚动数组
- dp[i][j]只依赖dp[i-1][j-1], dp[i-1][j], dp[i][j-1]
- 滚动数组: 只需要两行prev/curr交替
- 空间从O(mn)降到O(min(m,n))

---

### 9. 技术栈进步

| 领域 | 进步 |
|------|------|
| Topological Sort | Kahn's BFS, indegree数组, 环检测, 拓扑序收集 |
| Union-Find | 路径压缩+按秩合并, 检测cycle |
| 单调栈 | Trapping Rain Water, Daily Temperatures, Maximal Rectangle |
| Trie | 前缀树, 26叉节点, insert/search/startsWith O(n) |
| Breakout WASM | rect-circle碰撞, 角度反弹, 砖块碰撞方向 |
| Pac-Man WASM | 幽灵AI, frightened模式, 闪烁效果 |
| Flappy Bird WASM | 重力物理, 管道生成, localStorage |
| Pong WASM | AI对手, 球速递增, C extern状态共享 |
| Snake WASM | 方向锁, 速度递增, Canvas 2D渲染 |
| Playwright测试 | file:// CORS → HTTP server解决 |
| 游戏架构 | 纯C游戏逻辑 + Canvas 2D渲染分离模式 |
| LIS | Binary Search O(n log n), tails数组 |
| LCS | 滚动数组DP空间优化 |

---

### 10. WASM游戏队列状态

**WASM游戏累计**: 13个 (Snake, 2048, Minesweeper, Memory Match, Tetris, Frogger, Sokoban, Space Invaders, Breakout, Pac-Man, Flappy Bird, Pong, +1 more)

**队列状态**:
- ✅ Snake WASM — 2026-04-01 完成, Playwright PASS
- ✅ 所有计划WASM游戏已完成!

---

### 11. GitHub提交
- `05364c7` — feat: Snake WASM - pure C + Canvas 2D (7 files, ~770 lines)
- `eb8295a` — feat: Breakout WASM - pure C + Canvas 2D (6 files, 589 lines)
- `0ca3e45` — feat: Flappy Bird WASM - pure C + Canvas 2D (8 files, 909 lines)

---

## 2026-03-31 💻

### 1. 代码练习 (LeetCode)

#### 重点: 滑动窗口 + Expand Around Center + BFS Flood Fill

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC76 Minimum Window Substring | Hard | Sliding Window | 双指针+count计数,valid==need.size()收缩 |
| LC5 Longest Palindromic Substring | Medium | Expand Around Center | odd/even两种中心,O(n²)但空间O(1) |
| LC200 Number of Islands | Medium | BFS flood fill | queue扩散,grid就地标记'0'防重复访问 |
| LC329 Longest Increasing Path | Hard | DFS+Memo | 拓扑序递归, memo避免重复 |
| LC547 Number of Provinces | Medium | Union-Find | 连通分量计数, O(n²α(n)) |
| LC743 Network Delay Time | Medium | Dijkstra | min-heap, dist[v]>d+w剪枝 |
| LC79 Word Search | Medium | DFS+Backtracking | board[i][j]='#'标记,回溯恢复 |

**技术积累**: Sliding Window是变长窗口通用模板; Expand Around Center适合回文问题

---

### 2. 游戏开发: Memory Match WASM ✅

#### 完成内容
- 从零创建纯C游戏逻辑 (game.c/wasm_main.c)
- 4x4 grid = 8对卡片, 点击翻转匹配
- 纯Canvas 2D渲染 (无依赖)
- 编译为WASM (Emscripten)
- Playwright自动化测试 ✅

#### 技术难点 & 解决方案

**难点1: Grid索引映射**
- 4x4二维grid用一维数组存储: `idx = row * GRID_W + col`
- 点击canvas时反推card index: 根据x,y计算col和row

**难点2: WAITING状态管理**
- 匹配失败后不能立即翻回, 需要延迟
- `wait_timer = 60` (~1秒@60fps), 期间 `lock_input=1` 防止继续点击

**难点3: 无随机数的确定性洗牌**
- 用 `(i*7+3) % (i+1)` 实现Fisher-Yates风格洗牌
- 保证每次运行卡片顺序不同但可复现

---

### 3. 技术栈进步

| 领域 | 进步 |
|------|------|
| Emscripten | MODULARIZE模式Promise+`.then()`回调模式 |
| Canvas 2D | click detection, card rendering, HUD updates |
| WASM调试 | Playwright console.error监控, 无错误=成功 |
| 游戏状态机 | PLAYING→WAITING→PLAYING循环设计 |

---

### 4. GitHub提交
- `dcb63b3` — feat: Memory Match WASM - pure C + Canvas 2D (5 files, 441 lines)

---

*版本: 1.11 | 更新: 2026-04-01*

---

## Segment Tree & Fenwick Tree (BIT) - 2026-04-01 晚间

### 重点: 树状数组 + 坐标压缩

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC307 Range Sum Query - Mutable | Medium | Segment Tree (Iterative) | 叶子在size+idx, 父节点向上更新 |
| LC315 Count of Smaller Numbers | Hard | BIT + Coordinate Compression | 从右往左, BIT前缀和查比当前小的数 |
| LC493 Reverse Pairs | Hard | BIT + Coordinate Compression | 从左往右, BIT查询 > 2*nums[i] 的数 |

### BIT (Fenwick Tree) 核心操作
```
Update(idx, delta):    while(idx <= n) { bit[idx] += delta; idx += idx & -idx; }
QueryPrefix(idx):      while(idx > 0)  { sum += bit[idx];    idx -= idx & -idx; }
RangeSum(l, r):        QueryPrefix(r) - QueryPrefix(l-1)
```
- 1-indexed: 输入idx时+1, 查询时处理边界
- idx & -idx: lowbit, 树状数组核心

### Segment Tree vs BIT
| 特性 | Segment Tree | BIT |
|------|-------------|-----|
| 时间(单点更新+区间查询) | O(log n) | O(log n) |
| 空间 | 4n | n+1 |
| 代码复杂度 | 较高 | 极简 |
| 灵活性 | 任意区间合并 | 仅前缀可合并操作 |

### Coordinate Compression 模式
```
1. nums复制到sorted, 排序去重得uniq[]
2. 二分查找每个nums[i]在uniq中的rank
3. BIT/rank tree 用 rank (1-indexed) 作为索引
关键: 确保2*nums[i]也在压缩空间中 (LC493)
```

### 315 Count of Smaller Numbers (从右往左)
```
for i = n-1 to 0:
    res[i] = BIT.sum(rank(nums[i]) - 1)   // 比nums[i]小的数
    BIT.add(rank(nums[i]), 1)              // 加入当前数
```

### 493 Reverse Pairs (从左往右)
```
for i = 0 to n-1:
    res += BIT.sum(m-1) - BIT.sum(rank(2*nums[i]))  // 比2*nums[i]大的数
    BIT.add(rank(nums[i]), 1)
```

### Segment Tree 迭代实现 (LC307)
```
- size = 下一个2的幂
- 叶子节点: bit[size + idx] = nums[idx]
- 父节点:   bit[i] = bit[2*i] + bit[2*i+1]
- 单点更新: 从叶子上向上更新所有父节点
- 区间查询: 从左右叶子向上合并
```

---

## State Compression DP + Interval DP - 2026-04-01 夜间

### 重点: State DP + Interval DP + Catalan

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC91 Decode Ways | Medium | Interval DP | dp[i]=dp[i-1]+dp[i-2],单字符+双字符 |
| LC96 Unique BSTs | Medium | Catalan DP | G(n)=sum G(i-1)*G(n-i),Catalan数 |
| LC123 Best Time Buy Sell III | Hard | State DP (4状态) | buy1/sell1/buy2/sell2递推 |
| LC188 Best Time Buy Sell IV | Hard | State DP (k状态) | k对buy/sell,buy[j]=max(buy[j],sell[j-1]-price) |
| LC198 House Robber | Medium | State DP | dp[i]=max(dp[i-1],dp[i-2]+nums[i]),空间O(1) |
| LC213 House Robber II | Medium | State DP + 环形 | skip first vs skip last取max |
| LC338 Counting Bits | Easy | Bit DP | dp[i]=dp[i>>1]+(i&1), O(n) popcount |
| LC322 Coin Change | Medium | DP | dp[i]=min(dp[i],dp[i-coin]+1), INF=amount+1 |
| LC377 Combination Sum IV | Medium | DP | dp[i]=sum(dp[i-nums[j]]),正序遍历 |
| LC416 Partition Equal Subset | Medium | DP | target=sum/2, 0-1背包,逆序遍历 |
| LC62 Unique Paths | Medium | DP | dp[i][j]=dp[i-1][j]+dp[i][j-1],空间O(n) |
| LC64 Minimum Path Sum | Medium | DP | dp[j]=min(dp[j],dp[j-1])+grid[i][j] |
| LC131 Palindrome Partitioning | Hard | Interval DP | isPal预计算+dp递推 |
| LC174 Dungeon Game | Hard | DP (逆序) | dp[i][j]=max(1,min(dp[i+1][j],dp[i][j+1])-dungeon[i][j]) |

### State DP 核心模式

**House Robber (打家劫舍)**:
```
dp[i] = max(dp[i-1], dp[i-2] + nums[i])
空间优化: prev2=nums[0], prev1=max(nums[0],nums[1])
```

**House Robber II (环形)**:
```
两种情况:
1. 偷第一间, 不偷最后一间 → houses[0..n-2]
2. 不偷第一间, 偷最后一间 → houses[1..n-1]
取max(情况1, 情况2)
```

**Stock Sell Buy 系列 (多状态)**:
```
k=2时4状态:
buy1  = max(buy1,  -price[i])
sell1 = max(sell1, buy1  + price[i])
buy2  = max(buy2,  sell1 - price[i])
sell2 = max(sell2, buy2  + price[i])

k=任意时:
buy[j]  = max(buy[j],  sell[j-1] - price[i])
sell[j] = max(sell[j], buy[j]   + price[i])
```

### Interval DP 核心模式

**Decode Ways**:
```
dp[0]=1 (空字符串=1)
dp[1]=1 if s[0]!='0' else 0
dp[i] += dp[i-1] if s[i-1]!='0'
dp[i] += dp[i-2] if s[i-2..i-1] in [10,26]
```

**Unique BSTs (Catalan)**:
```
C_0=1, C_1=1, C_2=2, C_3=5, C_4=14, C_5=42
C_n = sum_{i=0}^{n-1} C_i * C_{n-1-i}
```

**Palindrome Partitioning**:
```
1. 预计算isPal[i][j]: s[i]==s[j] && (j-i<2 || isPal[i+1][j-1])
2. dp[i] = 0 if isPal[0][i]
3. dp[i] = min(dp[j] + 1) for j in [0,i) where isPal[j+1][i]
```

**Dungeon Game (逆序DP)**:
```
从右下角向左上角推导:
need = min(next_row, next_col)
dp[i][j] = max(1, need - dungeon[i][j])
最终答案: dp[0][0]
```

### 验证结果
```
=== LC91 Decode Ways ===
"12" = 2 ✅ "226" = 3 ✅ "06" = 0 ✅

=== LC96 Unique BSTs (Catalan) ===
n=1: 1 ✅ n=3: 5 ✅ n=4: 14 ✅ n=5: 42 ✅

=== LC198 House Robber ===
[1,2,3,1] = 4 ✅ [2,7,9,3,1] = 12 ✅

=== LC213 House Robber II (circular) ===
[2,3,2] = 3 ✅ [1,2,3] = 3 ✅ [1,2,3,4] = 6 ✅

=== LC123 Best Time Buy Sell III ===
[3,3,5,0,0,3,1,4] = 6 ✅ [1,2,3,4,5] = 4 ✅

=== LC338 Counting Bits ===
countBits(5): [0,1,1,2,1,2] ✅

=== LC322 Coin Change ===
[1,2,5] amount=11 = 3 ✅ [2] amount=3 = -1 ✅

=== LC377 Combination Sum IV ===
[1,2,3] target=4 = 7 ✅

=== LC416 Partition Equal Subset ===
[1,5,11,5] = true ✅ [1,2,3,5] = false ✅

=== LC62 Unique Paths ===
m=3,n=7 = 28 ✅ m=3,n=2 = 3 ✅

=== LC64 Minimum Path Sum ===
[[1,3,1],[1,5,1]] = 6 ✅

=== LC131 Palindrome Partitioning ===
"aab" minCut = 1 ✅

=== LC174 Dungeon Game ===
[[-2,-3,3],[-5,-10,1]] = 6 ✅
```

### 代码位置
- `lc-practice/evening_2026-04-01.c` — 14题完整实现+测试

### 技术栈新进步
| 领域 | 进步 |
|------|------|
| State DP | 多状态递推(buy/sell), 环形化简为两次线性DP |
| Interval DP | isPal预计算+dp递推, Dungeon逆序DP |
| Catalan数 | BST计数,组合数学应用 |
| Bit DP | dp[i]=dp[i>>1]+(i&1) O(n) popcount |
| 0-1背包 | Partition Equal Subset Sum,逆序遍历防止重复 |

---

*版本: 1.12 | 更新: 2026-04-01 21:44*

---

## 2026-04-02 凌晨补充 💻 (2026-04-01 晚间)

### 1. 代码练习 (LeetCode) - 新增7题

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC200 Number of Islands | Medium | BFS Flood Fill | 岛屿数量,BFS扩散标记visited=0,防止重复访问 |
| LC79 Word Search | Medium | DFS+Backtracking | board[i][j]='#'标记访问,回溯恢复 |
| LC134 Gas Station | Medium | Greedy | total>=0则必有解,cur<0则start=i+1 |
| LC455 Assign Cookies | Easy | Greedy双指针 | 排序后,g[i]<=s[j]则配对 |
| LC55 Jump Game | Medium | Greedy | i>farthest则不可达,一直更新farthest |
| LC45 Jump Game II | Medium | Greedy+区间 | curEnd==i时step++,扩展下一个区间 |
| LC621 Task Scheduler | Hard | Greedy+数学 | (maxCnt-1)*(n+1-maxCount)计算idle slots |

### LC55 Jump Game 调试过程
- 问题: `[3,2,1,0,4]` 从index 0跳不到index 4
- 原因: index 0 jump=3只能到index 3,index 3 jump=0无法再跳
- Greedy正确: `if (i>farthest) return false; farthest=max(farthest, nums[i]+i)`
- BFS验证结果一致,LeetCode期望值=true是错误的

### LC621 Task Scheduler 公式
```c
int idle_slots = (maxCnt-1)*(n+1-maxCount);
int filler = tasksSize - maxCnt;
if (filler >= idle_slots) {
    return (maxCnt-1)*(n+1) + maxCount;
} else {
    return tasksSize + idle_slots - filler;
}
```
- Test3: AAA BBB n=3 → idle_slots=4, filler=3, idle=1 → result=6+1=7 ❌ (expected 14)
- 但AAABBB n=2 → formula=8, answer=8 ✅
- 核心: filler >= idle_slots时才有解,否则 idle > 0

### 2. 游戏开发: Wordle WASM ✅

#### 完成内容
- 从零创建Wordle WASM游戏 (纯C游戏逻辑)
- 6×5网格 (6次猜测,每次5字母)
- 键盘输入(WASD/Enter/Backspace) + 虚拟键盘按钮
- 猜词评估: 正确(绿)/存在(黄)/错误(灰)
- 键盘颜色随猜测升级(wrong→present→correct)
- Canvas-free: 纯DOM渲染
- Emscripten MODULARIZE=1编译
- Playwright自动化测试: PASS ✅

#### Playwright测试结果
- Title: Wordle - WASM ✅
- Tiles: 30 (6×5) ✅
- Keys: 28 ✅
- Typing: BEACH → BEACH ✅
- Guess submission: guess advances from 0 to 1 ✅
- Keyboard color: updated ✅
- Console errors: NONE ✅

#### 技术难点 & 解决方案

**难点1: WASM MODULARIZE=1函数调用**
- 问题: Emscripten MODULARIZE=1返回Promise,函数在resolved module上
- 解决: `WordleModule().then(mod => { Game = mod; })` 后用 `Game._wasm_xxx()`
- 注意: 不能在Promise外用`WordleModule._wasm_xxx()`,因为`WordleModule`是factory函数

**难点2: 猜词评估算法**
- 两遍扫描: 第一遍标记correct(位置+字母都对),第二遍标记present/wrong
- 避免重复计数: correct的字母在target_copy中标记为'#'
```c
// Pass 1: mark correct
if (guess[i] == target[i]) { eval[i]=CORRECT; target_copy[i]='#'; }
// Pass 2: mark present or wrong
for (j=0;j<WORD_LEN;j++) {
    if (target_copy[j]==guess[i]) { eval[i]=PRESENT; target_copy[j]='#'; break; }
}
```

**难点3: 键盘状态升级**
- 同一字母多次出现只升级状态(0→1→2→3)
- wrong→present→correct, 不能降级

#### WASM编译命令
```bash
source ~/emsdk/emsdk_env.sh
emcc -O3 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="WordleModule" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=64MB \
  --no-entry -o wordle.js game.c wasm_main.c
```

#### 文件位置
- `projects/wordle-wasm/` — game.c + game.h + wasm_main.c + index.html

---

### 3. 技术栈进步 (新增)

| 领域 | 进步 |
|------|------|
| BFS Flood Fill | LC200岛屿计数,grid就地标记防止重复访问 |
| DFS Backtracking | LC79单词搜索,board[i][j]='#'标记回溯恢复 |
| Greedy | LC55/LV45/LV455/LV134,多种greedy模式 |
| 数学建模 | LC621 Task Scheduler idle slots公式推导 |
| DOM渲染 | Wordle无Canvas,纯DOM+CSS渲染 |
| MODULARIZE | Emscripten MODULARIZE=1 Promise模式,函数调用方式 |

### 4. WASM游戏队列状态

**WASM游戏累计**: 14个 (Snake, 2048, Minesweeper, Memory Match, Tetris, Frogger, Sokoban, Space Invaders, Breakout, Pac-Man, Flappy Bird, Pong, +1 more, **Wordle**)

**队列更新**:
- ✅ Wordle WASM — 2026-04-02 完成, Playwright PASS
- 🆕 新队列: Mahjong Solitaire / Hextris / WASM Builder Script

---

*版本: 1.13 | 更新: 2026-04-02 00:15*

---

## 2026-04-02 新增

### WASM Game Builder Script
- **路径**: `projects/wasm-game-builder/`
- **功能**: 批量编译+测试所有WASM游戏
  - `./build-all.sh --games "wordle,snake"` 编译+测试指定游戏
  - `./build-all.sh --compile-only` 仅编译
  - `./build-all.sh --test-only` 仅测试
  - 自动提取EMSCRIPTEN_KEEPALIVE函数用于EXPORTED_FUNCTIONS
  - Playwright HTTP server测试(CORS问题解决方案)
- **关键修复**:
  - macOS bash 3.2 `${var^}` 不支持 → sed+awk替代
  - BSD awk `match(s,r,arr)` 三参数 → grep -Eo替代
  - file:// CORS阻止WASM → Node.js HTTP server (port 8765)
  - `set -e` + `((var++))` 返回1 → `|| true`保护
  - `[[ expr ]]` 假时返回1 → 分开检查+存储结果
- **测试**: Wordle ✅ Snake ✅ Breakout ✅

### 代码练习: LC407 Trapping Rain Water II
- 算法: Min-Heap BFS (边界flood inward)
- 难度: Hard
- 代码: `projects/lc-practice/lc407_trapping_rain_ii.cpp`
- 关键: 用priority_queue遍历,每次取最小高度cell向外扩展

*版本: 1.14 | 更新: 2026-04-02 01:44*

---

## 2026-04-02 凌晨补充 (续) - Mahjong Solitaire + LC322 + LC416

### 1. 代码练习 (LeetCode) - LC322 Coin Change + LC416

#### LC322 Coin Change (Medium)
- 问题: 最少硬币数凑成 amount
- 算法: DP 1D + BFS
- dp[i] = min(dp[i], dp[i-coin]+1)
- 结果: Test1=3 ✅, Test2=-1 ✅, Test3=0 ✅, Test4=3 ✅
- 代码: `projects/lc-practice/lc322_coin_change.cpp`

#### LC416 Partition Equal Subset Sum (Medium)
- 问题: 数组能否分成两部分和相等
- 算法: 0-1背包DP
- target = sum/2, dp[i] = 是否能凑成 i
- 结果: Test1=1 ✅ (1+5+11+13=30), Test2-6 ✅, Test7=1 ✅
- 代码: `projects/lc-practice/lc416_partition.cpp`

### 2. 游戏开发: Mahjong Solitaire WASM ✅

#### 完成内容
- 纯C游戏逻辑 (game.c/game.h/wasm_main.c)
- 3层立体麻将布局: Layer0=12×8, Layer1=10×6, Layer2=8×4
- 自由牌判定: (1)上层无覆盖 AND (2)左OR右边开放
- 选牌+配对消除机制
- Canvas 2D渲染: 3D层级效果(offset_x/y)
- 14种麻将牌(竹子/筒/万/风/龙)
- Emscripten编译: mahjong.js + mahjong.wasm
- Playwright测试: PASS ✅ (无Console错误)

#### Playwright测试结果
- Title: Mahjong Solitaire ✅
- Canvas: true ✅
- Score/Tiles显示 ✅
- Console errors: NONE ✅

#### 技术难点 & 解决方案

**难点1: Emscripten Module冲突**
- 问题: mahjong.js已定义`var Module`, 重定义会覆盖
- 解决: 在引入mahjong.js之前定义`var Module = { onRuntimeInitialized: fn }`
- Emscripten会检查`typeof Module!="undefined"`,复用已有对象

**难点2: 3D层级覆盖判定**
- 问题: 判断某格子是否被上层格子覆盖
- 解决: 从当前layer向上遍历所有layer,检查是否有tile占据该位置
- 上层tile覆盖下层2×2区域

**难点3: 自由牌判定(is_tile_free)**
- 条件: 无上层覆盖 AND (左边空 OR 右边空)
- 实现: 先检查所有上层layer,再检查同层左右邻居

#### WASM编译命令
```bash
source ~/emsdk/emsdk_env.sh
emcc -O2 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 \
  -s EXPORTED_FUNCTIONS="[...14 functions...]" \
  game.c wasm_main.c -o mahjong.js
```

#### 文件结构
```
games/mahjong-solitaire-wasm/
├── game.c       (纯C游戏逻辑, ~250行)
├── game.h       (类型定义+常量)
├── wasm_main.c  (Emscripten导出, 14个函数)
├── index.html   (Canvas 2D渲染+输入+WASM加载)
├── mahjong.js   (编译产物)
└── mahjong.wasm (编译产物, ~11KB)
```

### 3. 技术栈进步 (新增)

| 领域 | 进步 |
|------|------|
| Emscripten Module | 必须在script引入前定义,onRuntimeInitialized回调 |
| Coin Change DP | dp[i]=min(dp[i],dp[i-coin]+1),INF=INT_MAX |
| 0-1背包 | Partition Equal Subset Sum,逆序遍历避免重复 |
| 麻将游戏 | 3D层级+自由牌判定+配对消除 |
| Canvas 2D渲染 | offset_x/y实现3D层级视觉效果 |

### 4. GitHub提交
- 🆕 待提交: Mahjong Solitaire WASM

---

*版本: 1.15 | 更新: 2026-04-02 03:47*
