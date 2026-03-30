# 游戏开发队列

> 记录待开发/开发中的游戏项目

## 队列状态
- 队列创建时间: 2026-03-25
- 最后更新: 2026-03-31 凌晨3:37
- **状态**: 🎉 本周队列全部清空，已开始 WASM 探索

---

## 本周完成 (2026-03-25 ~ 2026-04-05)

| 日期 | 项目 | 路径 | 说明 |
|------|------|------|------|
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
| **03-31凌晨** | **Frogger WASM** | **projects/frogger-raylib/wasm/** | **Canvas 2D + Emscripten ✅** |

**本周游戏总计: 21 个 🎉**

---

## 下周计划 (2026-04-01 ~ 2026-04-06)

| 优先级 | 项目 | 类型 | 说明 |
|--------|------|------|------|
| P1 | Frogger WASM 浏览器测试 | WASM | 验证 Canvas 渲染正确性 |
| P1 | Pac-Man WASM | WASM | 编译 Pac-Man 到 WebAssembly |
| P2 | Space Invaders WASM | WASM | 编译 Space Invaders 到浏览器 |
| P3 | 图形游戏增强 | 图形 | 粒子效果/音效集成 |

---

## 技术栈统计（本周）

| 类型 | 数量 |
|------|------|
| ncurses 终端游戏 | 8 |
| Raylib 图形游戏 | 10 |
| Web 游戏 | 1 |
| WASM 游戏 | 1 |
| LeetCode 练习 | 77+ 道 |
| Hard 题目 | 40 道 |

---

## 今日完成 (2026-03-31 03:37)

### LeetCode 练习

| 题目 | 算法 | 结果 |
|------|------|------|
| LC32 最长有效括号 | DP | ✅ |
| LC239 滑动窗口最大值 | 单调递减队列 | ✅ |

### LeetCode 关键题目（今日新增）

| 题目 | 算法 |
|------|------|
| LC32 Longest Valid Parentheses | DP（栈辅助） |
| LC239 Sliding Window Maximum | 单调递减队列 O(n) |

### 游戏开发：Frogger WASM ✅

**项目**: `projects/frogger-raylib/wasm/`
**Commit**: 待推送
**WASM 编译**: ✅ 零错误通过（Emscripten 5.0.4）
**产物**: `frogger.js` (12KB) + `frogger.wasm` (3.7KB) + `index.html`

**技术实现**:
- `game.h/game.c` 纯C逻辑 → Emscripten 编译到 WASM
- `wasm_renderer.c`: 25个导出函数，无 raylib 依赖
- `index.html`: Canvas 2D 渲染 + 键盘控制

**导出函数列表**:
```c
_init_game, _update_game, _get_game_state,
_get_frog_x, _get_frog_y, _get_frog_alive, _get_frog_lives,
_get_time_left, _get_homes_filled, _get_score,
_get_obs_count, _get_obs_x, _get_obs_y, _get_obs_active,
_get_log_count, _get_log_x, _get_log_y, _get_log_active,
_get_safe_zone, _get_invincible_timer,
_move_frog, _reset_frog,
_get_grid_h, _get_grid_w, _get_cell
```

---

### 本日累计 (2026-03-31 全天)

| 指标 | 数量 |
|------|------|
| LeetCode 完成 | **77+ 道** |
| Hard 题目 | **40 道** |
| Raylib 图形游戏 | 10 个 |
| WASM 游戏 | **1 个** 🆕 |
| 本日游戏总计 | **21 个** 🎉 |

---

*最后更新: 2026-03-31 03:37*
