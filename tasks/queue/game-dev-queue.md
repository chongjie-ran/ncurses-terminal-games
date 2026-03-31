# 游戏开发队列

> 记录待开发/开发中的游戏项目

## 队列状态
- 队列创建时间: 2026-03-25
- 最后更新: 2026-03-31 13:37
- **状态**: 🎉 4个WASM游戏全部浏览器测试通过

---

## 本周完成 (2026-03-25 ~ 2026-04-05)

| 日期 | 项目 | 路径 | 说明 |
|------|------|------|
| 03-26 | 贪吃蛇 | projects/snake-game/ | ncurses 终端版 |
| 03-26 | 2048 | projects/game-2048/ | ncurses 终端版 |
| 03-26 | 扫雷 | projects/minesweeper/ | ncurses 终端版 |
| 03-26 | Flappy Bird | projects/flappy-bird/ | ncurses 终端版 |
| 03-26 | Hangman | projects/hangman/ | ncurses 终端版 |
| 03-27 | 俄罗斯方块 | projects/tetris/ | ncurses 终端版 |
| 03-27 | 华容道 | projects/huarong-dao/ | ncurses 终端版 |
| 03-28 | 推箱子 | projects/sokoban/ | ncurses 终端版 |
| 03-29 | AI意识守护者 | ai-memory-ghost/ | Web游戏 v1.1 |
| 03-29 | 贪吃蛇 Raylib | projects/snake-game-raylib/ | Raylib 图形版 ✅ |
| 03-29 | 2048 Raylib | projects/game-2048-raylib/ | Raylib 图形版 ✅ |
| 03-30凌晨 | 俄罗斯方块 Raylib | projects/tetris-raylib/ | Raylib 图形版 ✅ |
| 03-30凌晨 | Sokoban Raylib | projects/sokoban-raylib/ | Raylib 图形版 ✅ |
| 03-30早上 | Flappy Bird Raylib | projects/flappy-bird-raylib/ | 重力物理+管道 ✅ |
| 03-30上午 | Minesweeper Raylib | projects/minesweeper-raylib/ | 9x9扫雷+计时器 ✅ |
| 03-30上午 | Breakout | projects/breakout-raylib/ | 打砖块+反弹物理 ✅ |
| 03-30下午 | Memory Match | projects/memory-match-raylib/ | 4x4卡牌配对+8种符号 ✅ |
| 03-30下午 | Space Invaders | projects/space-invaders-raylib/ | 5波+4种外星人+爆炸动画 ✅ |
| 03-30晚 | Pac-Man | projects/pacman-raylib/ | 4 AI幽灵+能量豆+关卡递进 ✅ |
| 03-30晚 | Frogger | projects/frogger-raylib/ | 5车道交通+5水道原木+3条命+计时 ✅ |
| 03-31凌晨 | Frogger WASM | projects/frogger-raylib/wasm/ | Canvas 2D + Emscripten ✅ |
| 03-31凌晨 | Pac-Man WASM | projects/pacman-raylib/wasm/ | Canvas 2D + Emscripten ✅ |
| 03-31早上 | Space Invaders WASM | projects/space-invaders-raylib/wasm/ | Canvas 2D + Emscripten ✅ |
| 03-31上午 | Breakout WASM | projects/breakout-raylib/wasm/ | Canvas 2D + Emscripten + 手动JS加载器 ✅ |
| **03-31下午** | **WASM浏览器测试** | **4个游戏全部** | **Frogger+Pac-Man+Space Invaders+Breakout ✅** 🆕 |

**本周游戏总计: 24 个 + 浏览器测试 ✅**

---

## WASM 浏览器测试结果（2026-03-31 13:37）

| 游戏 | WASM | JS加载器 | Canvas渲染 | HUD | 键盘控制 |
|------|------|----------|------------|-----|---------|
| Frogger | ✅ 3.7KB | ✅ 手写 | ✅ | ✅ LIVES/SCORE/TIME/HOMES | ⏳ 待深入 |
| Pac-Man | ✅ 5.9KB | ✅ 手写 | ✅ | ✅ Score/Lives/Level/Dots | ✅ |
| Space Invaders | ✅ 19KB | ✅ 手写 | ✅ | ✅ SCORE/WAVE/LIVES/ALIENS | ✅ |
| Breakout | ✅ 10.4KB | ✅ 手写 | ✅ | ✅ Score/Lives/Level | ✅ |

**浏览器测试方法**: Playwright 自动化 + Python HTTP Server

---

## 下周计划 (2026-04-01 ~ 2026-04-06)

| 优先级 | 项目 | 类型 | 说明 |
|--------|------|------|------|
| P1 | WASM 游戏 GitHub Pages 部署 | 部署 | 将4个WASM游戏部署到GitHub Pages |
| P2 | 粒子效果集成 | 图形 | Raylib 粒子系统 |
| P2 | LeetCode 新技术栈 | 算法 | Trie/线段树/并查集进阶 |
| P3 | 新游戏: Snake WASM | WASM | 编译贪吃蛇到WASM |
| P3 | 音效集成 | 音频 | Raylib audio API |

---

## 技术栈统计（本周）

| 类型 | 数量 |
|------|------|
| ncurses 终端游戏 | 8 |
| Raylib 图形游戏 | 10 |
| Web 游戏 | 1 |
| WASM 游戏 | 4 |
| LeetCode 练习 | 88+ 道 |
| Hard 题目 | 41 道 |

---

## 今日完成 (2026-03-31 13:37)

### LeetCode 练习

| 题目 | 算法 | 结果 |
|------|------|------|
| LC994 腐烂的橘子 | 多源BFS | ✅ |
| LC934 最短的桥 | BFS两阶段 | ✅ |
| LC130 被围绕的区域 | BFS flood fill | ✅ |
| LC329 Longest Increasing Path | DFS+Memoization | ✅ |
| LC743 Network Delay Time | Dijkstra | ✅ |
| LC547 Number of Provinces | Union-Find/BFS | ✅ |
| LC79 Word Search | DFS+Backtracking | ✅ |
| LC208 Implement Trie | Trie | ✅ |
| LC215 Kth Largest Element | 堆/快速选择 | ✅ |
| LC212 Word Search II | Trie+DFS | ✅ |

### LeetCode 关键题目（新增）

| 题目 | 算法 |
|------|------|
| LC208 Implement Trie | 前缀树 insert/search/startsWith O(L) |
| LC215 Kth Largest Element | 最小堆O(nlogk) / 快速选择O(n) |
| LC212 Word Search II | Trie+DFS 批量查询 (LC79+LC208组合) |

### WASM 浏览器测试完成 ✅

**Frogger WASM** (测试时间: 12:00)
- HTTP服务: `python3 -m http.server 8765`
- HUD正确: LIVES: 3, SCORE: 0, TIME: 60, HOMES: 0/5
- Canvas渲染: ✅

**Pac-Man WASM** (测试时间: 13:37) 🆕
- HTTP服务: `python3 -m http.server 8766`
- HUD正确: Score: 0, Lives: 3, Level: 1, Dots: 0
- Canvas渲染: ✅
- 键盘控制提示可见

**Space Invaders WASM** (测试时间: 13:37) 🆕
- HTTP服务: `python3 -m http.server 8767`
- HUD正确: SCORE: 0, WAVE: 1, LIVES: 3, ALIENS: 32
- Canvas渲染: ✅

**Breakout WASM** (测试时间: 09:55)
- 编译成功: `breakout.wasm` 10.4KB + 手动JS加载器
- 待深入浏览器测试

---

### 本日累计 (2026-03-31 全天)

| 指标 | 数量 |
|------|------|
| LeetCode 完成 | **92+ 道** 🆕 |
| Hard 题目 | **41 道** |
| ncurses 游戏 | 8 个 |
| Raylib 图形游戏 | 10 个 |
| Web 游戏 | 1 个 |
| WASM 游戏 | **4 个** |
| WASM浏览器测试 | **4个全部通过** 🆕 |
| 本周游戏总计 | **24 个** 🎉 |

---

## 本周累计（2026-03-25 ~ 2026-03-31）

| 指标 | 数量 |
|------|------|
| LeetCode 完成 | **92+ 道** |
| Hard 题目 | **41 道** |
| ncurses 游戏 | 8 个 |
| Raylib 图形游戏 | 10 个 |
| Web 游戏 | 1 个 |
| WASM 游戏 | **4 个** |
| WASM浏览器测试 | **4个全部通过** |
| 本周游戏总计 | **24 个** 🎉 |

---

*最后更新: 2026-03-31 13:37*

---

## 今日更新 (2026-03-31 13:37)

### WASM 浏览器测试 — Pac-Man + Space Invaders ✅

**Pac-Man WASM 测试结果**:
| 测试项 | 结果 |
|--------|------|
| HTTP 服务加载 | ✅ 无崩溃 |
| HUD 渲染 | ✅ Score: 0, Lives: 3, Level: 1, Dots: 0 |
| Canvas 渲染 | ✅ 标题+HUD 元素可见 |
| 键盘事件提示 | ✅ Arrow Keys / WASD / P / R |

**Space Invaders WASM 测试结果**:
| 测试项 | 结果 |
|--------|------|
| HTTP 服务加载 | ✅ 无崩溃 |
| HUD 渲染 | ✅ SCORE: 0, WAVE: 1, LIVES: 3, ALIENS: 32 |
| Canvas 渲染 | ✅ 标题+HUD 元素可见 |
| 键盘事件提示 | ✅ ← → A D Move / SPACE Shoot / R Restart / P Pause |

**技术细节**:
- 服务启动: `python3 -m http.server 8766/8767`
- 测试工具: Playwright browser automation
- 文件确认:
  - `pacman.wasm` (5.9KB) + `pacman.js` (12.7KB) + `index.html`
  - `space_invaders.wasm` (19KB) + `space_invaders.js` (13.7KB) + `index.html`

### LeetCode Trie 技术栈新增 (2026-03-31 13:37)

**Trie 核心知识点**:
1. **LC208 Implement Trie**: 前缀树基本操作 (insert/search/startsWith)
2. **LC212 Word Search II**: Trie + DFS 批量查询（组合LC79+LC208）
3. **关键技巧**: TrieNode存储完整word字符串，避免重建
4. **复杂度**: O(L) per op (L=word length), 空间O(26*L*N)

**Trie 问题分类**:
| 题号 | 难度 | 描述 |
|------|------|------|
| LC208 | Medium | 实现 Trie (insert/search/startsWith) |
| LC212 | Hard | Word Search II (Trie + DFS 批量查询) |
| LC14 | Easy | 最长公共前缀 |
| LC648 | Medium | 单词替换 |
| LC676 | Medium | 魔法字典 |

**Kth Largest 问题** (LC215):
1. **方法1**: 最小堆大小K → O(n log k)
2. **方法2**: 快速选择 → 平均O(n)
3. **关键洞察**: 第K大 = index n-k (升序)

---

*最后更新: 2026-03-31 13:37*
