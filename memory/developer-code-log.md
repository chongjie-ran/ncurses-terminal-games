# Developer Code Log

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

*版本: 1.2 | 更新: 2026-03-31*
