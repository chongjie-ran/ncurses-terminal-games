# Developer Code Log

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
```c
float overlap_left = (bx + BW) - (b->x - BALL_R);
float overlap_right = (b->x + BALL_R) - bx;
float overlap_top = (by + BH) - (b->y - BALL_R);
float overlap_bottom = (b->y + BALL_R) - by;
if (min(overlap_x, overlap_y) == overlap_x) b->dx = -b->dx;
else b->dy = -b->dy;
```

#### WASM编译命令
```bash
source ~/emsdk/emsdk_env.sh
emcc -O3 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="BreakoutModule" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=64MB \
  --no-entry -o breakout.js game.c wasm_main.c
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

#### 技术难点 & 解决方案

**难点1: 重力物理**
- 解决: 每帧vy累加gravity, y累加vy,保证vy有正有负(上下)
```c
g->bird.vy += GRAVITY;
g->bird.y += g->bird.vy;
```

**难点2: 管道碰撞**
- 解决: rect-circle碰撞, top pipe从y=0到gap_y, bottom pipe从gap_y+PIPE_GAP到ground
```c
// Top pipe collision
if (circle_rect_hit(bird_x, bird_y, R, pipe_x, 0, PIPE_W, gap_y)) ...
// Bottom pipe collision
float bottom_top = gap_y + PIPE_GAP;
if (circle_rect_hit(bird_x, bird_y, R, pipe_x, bottom_top, PIPE_W, ground_y - bottom_top)) ...
```

**难点3: 管道生成时机**
- 解决: 检查最后一个管道x位置,当屏幕宽度-last_x >= PIPE_INTERVAL时生成
```c
Pipe *last = &g->pipes[g->pipe_count - 1];
if ((float)SCREEN_W - last->x >= PIPE_INTERVAL) spawn_pipe(g);
```

#### WASM编译命令
```bash
source ~/emsdk/emsdk_env.sh
emcc -O2 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="Module" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=64MB \
  -s EXPORTED_FUNCTIONS='["_wasm_init","_wasm_jump","_wasm_update","_wasm_get_state","_wasm_get_bird_x","_wasm_get_bird_y","_wasm_get_bird_vy","_wasm_get_bird_rot","_wasm_get_score","_wasm_get_best_score","_wasm_get_pipe_count","_wasm_get_pipe_x","_wasm_get_pipe_gap_y","_wasm_get_screen_w","_wasm_get_screen_h","_wasm_get_ground_h","_wasm_get_bird_radius","_wasm_get_pipe_width","_wasm_get_pipe_gap"]' \
  --no-entry -o flappy.js game.c wasm_main.c
```

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

### 6. 经验沉淀

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
- 击中位置映射到角度(150°-270°)
- 用cos/sin重建速度向量
- 确保dy始终向上(负值)

#### 技术难点5: Flappy Bird重力物理
- 每帧累加重力,速度有正有负(上下飞)
- 旋转角度与速度挂钩: `bird_rot = fmaxf(-30, fminf(90, vy * 6))`

---

### 7. 技术栈进步

| 领域 | 进步 |
|------|------|
| Topological Sort | Kahn's BFS, indegree数组, 环检测 |
| Union-Find | 路径压缩+按秩合并, 检测cycle |
| 单调栈 | Trapping Rain Water, Daily Temperatures, Maximal Rectangle |
| Breakout WASM | rect-circle碰撞, 角度反弹, 砖块碰撞方向 |
| Pac-Man WASM | 幽灵AI, frightened模式, 闪烁效果 |
| Flappy Bird WASM | 重力物理, 管道生成, localStorage |
| Playwright测试 | MODULARIZE=1 + global Module混用场景测试 |
| 游戏架构 | 纯C游戏逻辑 + Canvas 2D渲染分离模式 |

---

### 8. WASM游戏队列状态

**WASM游戏累计**: 11个 (Snake, 2048, Minesweeper, Memory Match, Tetris, Frogger, Sokoban, Space Invaders, Breakout, Pac-Man, Flappy Bird)

**队列状态**:
- ✅ Breakout WASM — 2026-04-01 完成, Playwright PASS
- ✅ Space Invaders WASM — 2026-04-01 验证, Playwright PASS
- ✅ Pac-Man WASM — 2026-04-01 验证, Playwright PASS
- ✅ Flappy Bird WASM — 2026-04-01 完成, Playwright PASS
- P3: Snake → WASM
- P3: Pong WASM

---

### 9. GitHub提交
- (Flappy Bird WASM commits pending)

---

## 2026-04-01 (续) 💻

### 代码练习 - 每日LeetCode

#### 新增题目记录

| 题目 | 难度 | 算法 | 核心洞察 |
|------|------|------|----------|
| LC42 Trapping Rain Water | Hard | 单调栈 | 递减栈,维护宽度和高度,类似LC84但更复杂 |
| LC739 Daily Temperatures | Medium | 单调栈 | 递减栈,找下一个更大元素的下标差 |
| LC85 Maximal Rectangle | Hard | 单调栈 | 转化为一排柱子求最大矩形,结合LC84 |

**单调栈 进阶模式 (LC42/LC85)**:
- LC42: 递减栈,当遇到更高高度时计算能trap的水量
  - `water += (min(height[left], height[right]) - height[top]) * width`
- LC85: 把每一行当成直方图底边,累计高度,求最大矩形

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

#### 关键代码片段
```c
// 点击处理
int wasm_click(int card_index) {
    if (lock_input) return 0;
    if (card_states[card_index] != STATE_HIDDEN) return 0;
    card_states[card_index] = STATE_REVEALED;
    selected[num_selected++] = card_index;
    if (num_selected == 2) {
        moves++;
        if (cards[a] == cards[b]) {
            card_states[a] = STATE_MATCHED;
            card_states[b] = STATE_MATCHED;
            matched_pairs++;
        } else {
            game_state = GAME_STATE_WAITING;
            lock_input = 1;
            wait_timer = 60; // ~1秒
        }
    }
    return 1;
}

// Canvas点击检测 (JS)
canvas.addEventListener('click', (e) => {
    const rect = canvas.getBoundingClientRect();
    const mx = (e.clientX - rect.left) * scaleX;
    const my = (e.clientY - rect.top) * scaleY;
    for (let i = 0; i < TOTAL; i++) {
        const pos = getCellPos(i);
        if (mx >= pos.x && mx <= pos.x + CARD_SIZE &&
            my >= pos.y && my <= pos.y + CARD_SIZE) {
            Module._wasm_click(i);
            break;
        }
    }
});
```

#### WASM编译命令
```bash
source ~/emsdk/emsdk_env.sh
emcc -O3 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="MemoryMatchModule" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s TOTAL_MEMORY=64MB \
  -s EXPORTED_FUNCTIONS='["_wasm_init","_wasm_tick","_wasm_click","_wasm_restart","_wasm_get_state","_wasm_get_moves","_wasm_get_matched","_wasm_get_card_state","_wasm_get_card_value","_wasm_get_grid_w","_wasm_get_grid_h","_malloc","_free"]' \
  --no-entry -o memory_match.js wasm_main.c
```

---

### 3. 游戏开发队列更新

**WASM游戏累计**: 5个 (Snake, 2048, Minesweeper, Frogger, Memory Match)

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

*版本: 1.8 | 更新: 2026-04-01*
