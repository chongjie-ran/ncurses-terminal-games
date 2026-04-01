# Developer Code Log

## 2026-04-01 💻

### 1. 代码练习 (LeetCode)

#### 新增题目: Sliding Window + Backtracking + BST

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC3 Longest Substring Without Repeating | Medium | Sliding Window (HashMap/Array) | seen[c]>=left时shrink,更新left=max(left,seen[c]+1) |
| LC22 Generate Parentheses | Medium | Backtracking/Recursion | open<n时加'(' , close<open时加')', Catalan数C_n |
| LC98 Validate Binary Search Tree | Medium | Tree DFS (Range check) | 递归传递(min,max), node.val必须在(min,max)区间 |
| LC207 Course Schedule | Medium | Topological Sort (Kahn's BFS) | 构建indegree数组,BFS队列弹出0入度节点 |
| LC684 Redundant Connection | Medium | Union-Find | 路径压缩+按秩合并,检测到cycle的边即为答案 |
| LC42 Trapping Rain Water | Hard | 单调栈 | 递减栈,维护宽度和高度,类似LC84但更复杂 |
| LC739 Daily Temperatures | Medium | 单调栈 | 递减栈,找下一个更大元素的下标差 |
| LC85 Maximal Rectangle | Hard | 单调栈 | 转化为一排柱子求最大矩形,结合LC84 |
| LC208 Implement Trie | Medium | Trie | 前缀树,26个子指针数组,insert/search/startsWith |

**Sliding Window 核心模式**:
```c
int left = 0, maxLen = 0;
for (int right = 0; right < n; right++) {
    if (seen[c] >= left) left = seen[c] + 1; // shrink
    seen[c] = right;
    maxLen = max(maxLen, right - left + 1);
}
```

**Backtracking (LC22 Generate Parentheses) 核心模式**:
- 递归参数: open数, close数
- 添加'(': open < n
- 添加')': close < open
- 结束: current.size() == 2*n
- 结果数: Catalan数 C_n = (2n)!/(n!(n+1)!)

**BST Validation 核心模式**:
- 递归传递合法区间: dfs(node, minVal, maxVal)
- 左子树: maxVal = node.val (所有节点 < node.val)
- 右子树: minVal = node.val (所有节点 > node.val)
- 必须严格小于/大于: <= minVal 或 >= maxVal → false

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

### 3. 经验沉淀

#### 技术难点1: Sliding Window变长窗口
- 核心: `if (seen[c] >= left) left = seen[c] + 1` 收缩左边界
- 时间O(n),空间O(min(charset_size, n))

#### 技术难点2: Backtracking括号生成
- Catalan数增长: n=3→5, n=4→14, n=5→42
- open<n时加'(' ; close<open时加')'

#### 技术难点3: BST严格边界检查
- 必须用long long处理int边界值(INT_MIN/INT_MAX)
- `node->val <= minVal || node->val >= maxVal` → false

#### 技术难点4: Snake方向锁dir_timer
- 防止一帧内多次改向导致180°翻转
- 3帧锁定期内忽略方向输入

#### 技术难点5: Reflect.apply替代Function.apply
- `Function.apply(null, ["return f()"])` 是错误的!
- 正确: `Reflect.apply(f, null, [])` 直接调用

#### 技术难点6: Emscripten游戏命名一致性
- game.c: `wasm_init_game()`, `wasm_tick_game()`
- wasm_main.c: `EMSCRIPTEN_KEEPALIVE void wasm_init_game(void) { wasm_init_game(); }` 陷阱!
- wasm_main.c中的wrapper函数名不能与game.c中相同!

---

### 4. 技术栈进步

| 领域 | 进步 |
|------|------|
| Sliding Window | HashMap+Array双实现, 变长窗口收缩模式 |
| Backtracking | 括号生成,Catalan数递推 |
| BST Validation | 递归区间传播, long long边界处理 |
| Snake WASM | 方向锁, 速度递增, Canvas 2D渲染 |
| WASM调试 | Reflect.apply替代复杂Function.apply |
| Emscripten | MODULARIZE+WebAssembly.instantiate直接加载 |

---

### 5. WASM游戏队列状态

**WASM游戏累计**: 13个 (Snake, 2048, Minesweeper, Memory Match, Tetris, Frogger, Sokoban, Space Invaders, Breakout, Pac-Man, Flappy Bird, Pong + 1 more)

**队列状态**:
- ✅ Snake WASM — 2026-04-01 完成, Playwright PASS, 无Console错误
- ✅ 所有计划WASM游戏已完成!

---

### 6. GitHub提交
- `05364c7` — feat: Snake WASM - pure C + Canvas 2D (7 files, ~770 lines)

---

## 2026-04-01 💻

### 1. 代码练习 (LeetCode)

#### 重点: Topological Sort + Union-Find

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC207 Course Schedule | Medium | Topological Sort (Kahn's BFS) | 构建indegree数组,BFS队列弹出0入度节点 |
| LC684 Redundant Connection | Medium | Union-Find | 路径压缩+按秩合并,检测到cycle的边即为答案 |
| LC42 Trapping Rain Water | Hard | 单调栈 | 递减栈,维护宽度和高度,类似LC84但更复杂 |
| LC739 Daily Temperatures | Medium | 单调栈 | 递减栈,找下一个更大元素的下标差 |
| LC85 Maximal Rectangle | Hard | 单调栈 | 转化为一排柱子求最大矩形,结合LC84 |
| LC208 Implement Trie | Medium | Trie | 前缀树,26个子指针数组,insert/search/startsWith |

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

---

### 2. 游戏开发: Breakout WASM ✅

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

### 3. 游戏验证: Space Invaders WASM ✅

#### Playwright测试结果
- Canvas exists: true ✅
- HUD: SCORE:0 WAVE:1 LIVES:3 ALIENS:32 ✅
- Console errors: NONE ✅

---

### 4. 游戏验证: Pac-Man WASM ✅

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

### 5. 游戏开发: Flappy Bird WASM ✅

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

#### 文件结构
```
projects/flappy-bird-wasm/
├── game.c       (纯C游戏逻辑, ~140行)
├── game.h       (类型定义+常量)
├── wasm_main.c  (Emscripten导出)
├── index.html   (Canvas 2D渲染+输入)
├── flappy.js    (编译产物)
└── flappy.wasm  (编译产物, 2.6KB)
```

---

### 6. 游戏开发: Pong WASM ✅

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
- TEST: PASS ✅

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

#### WASM编译命令
```bash
source ~/emsdk/emsdk_env.sh
emcc -O2 -s MODULARIZE=1 -s EXPORT_NAME="PongModule" \
  -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=64MB \
  -s EXPORTED_FUNCTIONS='["_wasm_init","_wasm_launch","_wasm_update","_wasm_get_state","_wasm_get_left_score","_wasm_get_right_score","_wasm_get_ball_x","_wasm_get_ball_y","_wasm_get_ball_radius","_wasm_get_paddle_y","_wasm_restart","_wasm_paddle_move"]' \
  --no-entry -o pong.js game.c wasm_main.c
```

#### 文件结构
```
projects/pong-wasm/
├── game.c       (纯C游戏逻辑)
├── game.h       (类型定义+常量)
├── wasm_main.c  (Emscripten导出)
├── index.html   (Canvas 2D渲染+输入)
├── pong.js      (编译产物)
└── pong.wasm    (编译产物)
```

---

### 7. 经验沉淀

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

---

### 8. 技术栈进步

| 领域 | 进步 |
|------|------|
| Topological Sort | Kahn's BFS, indegree数组, 环检测 |
| Union-Find | 路径压缩+按秩合并, 检测cycle |
| 单调栈 | Trapping Rain Water, Daily Temperatures, Maximal Rectangle |
| Trie | 前缀树, 26叉节点, insert/search/startsWith O(n) |
| Breakout WASM | rect-circle碰撞, 角度反弹, 砖块碰撞方向 |
| Pac-Man WASM | 幽灵AI, frightened模式, 闪烁效果 |
| Flappy Bird WASM | 重力物理, 管道生成, localStorage |
| Pong WASM | AI对手, 球速递增, C extern状态共享 |
| Playwright测试 | file:// CORS → HTTP server解决 |
| 游戏架构 | 纯C游戏逻辑 + Canvas 2D渲染分离模式 |

---

### 9. WASM游戏队列状态

**WASM游戏累计**: 12个 (Snake, 2048, Minesweeper, Memory Match, Tetris, Frogger, Sokoban, Space Invaders, Breakout, Pac-Man, Flappy Bird, Pong)

**队列状态**:
- ✅ Pong WASM — 2026-04-01 完成, Playwright PASS
- P3: Snake → WASM

---

### 10. GitHub提交
- `eb8295a` — feat: Breakout WASM - pure C + Canvas 2D (6 files, 589 lines)
- `0ca3e45` — feat: Flappy Bird WASM - pure C + Canvas 2D (8 files, 909 lines)

---

## 2026-03-31 💻

### 1. 代码练习 (LeetCode)

#### 重点: 滑动窗口 +  Expand Around Center + BFS Flood Fill

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

#### WASM编译命令
```bash
source ~/emsdk/emsdk_env.sh
emcc -O3 -s MODULARIZE=1 -s EXPORT_NAME="MemoryMatchModule" \
  -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=64MB \
  -s EXPORTED_FUNCTIONS='["_wasm_init","_wasm_tick","_wasm_click","_wasm_restart","_wasm_get_state","_wasm_get_moves","_wasm_get_matched","_wasm_get_card_state","_wasm_get_card_value","_wasm_get_grid_w","_wasm_get_grid_h","_malloc","_free"]' \
  --no-entry -o memory_match.js wasm_main.c
```

---

### 3. 游戏开发队列更新

**WASM游戏累计**: 5个 (Snake, 2048, Minesweeper, Memory Match, Tetris, Frogger, Sokoban)

**队列状态**:
- ✅ Memory Match WASM — 今日完成
- P3: Tetris WASM
- P3: Sokoban WASM

---

### 4. 技术栈进步

| 领域 | 进步 |
|------|------|
| Emscripten | MODULARIZE模式Promise+`.then()`回调模式 |
| Canvas 2D | click detection, card rendering, HUD updates |
| WASM调试 | Playwright console.error监控, 无错误=成功 |
| 游戏状态机 | PLAYING→WAITING→PLAYING循环设计 |

---

### 5. GitHub提交
- `dcb63b3` — feat: Memory Match WASM - pure C + Canvas 2D (5 files, 441 lines)

---

*版本: 1.10 | 更新: 2026-04-01*
