# 游戏开发队列

> 记录待开发/开发中的游戏项目

## 队列状态
- 队列创建时间: 2026-03-25
- 最后更新: 2026-03-31 08:00
- **状态**: 🎉 Frogger + Pac-Man + Space Invaders WASM 三连完成

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
| 03-31凌晨 | Frogger WASM | projects/frogger-raylib/wasm/ | Canvas 2D + Emscripten ✅ |
| 03-31凌晨 | Pac-Man WASM | projects/pacman-raylib/wasm/ | Canvas 2D + Emscripten ✅ |
| **03-31早上** | **Space Invaders WASM** | **projects/space-invaders-raylib/wasm/** | **Canvas 2D + Emscripten ✅** |

**本周游戏总计: 23 个 🎉**

---

## 下周计划 (2026-04-01 ~ 2026-04-06)

| 优先级 | 项目 | 类型 | 说明 |
|--------|------|------|------|
| P1 | Frogger WASM 浏览器测试 | WASM | 验证 Canvas 渲染正确性 |
| P1 | Breakout WASM | WASM | 编译 Breakout 到浏览器 |
| P2 | 粒子效果/音效集成 | 图形 | 粒子效果/音效集成 |
| P3 | WASM 游戏打包/发布 | 部署 | 将3个WASM游戏部署到GitHub Pages |

---

## 技术栈统计（本周）

| 类型 | 数量 |
|------|------|
| ncurses 终端游戏 | 8 |
| Raylib 图形游戏 | 10 |
| Web 游戏 | 1 |
| WASM 游戏 | 3 |
| LeetCode 练习 | 81+ 道 |
| Hard 题目 | 41 道 |

---

## 今日完成 (2026-03-31 08:00)

### LeetCode 练习

| 题目 | 算法 | 结果 |
|------|------|------|
| LC200 岛屿数量 | BFS flood fill | ✅ |
| LC207 课程表 | 拓扑排序 BFS | ✅ |
| LC133 克隆图 | BFS + 哈希表 | ✅ |

### LeetCode 关键题目（今日新增）

| 题目 | 算法 |
|------|------|
| LC200 Number of Islands | BFS flood fill（遇到1就展开标记） |
| LC207 Course Schedule | 拓扑排序Kahn算法 |
| LC133 Clone Graph | BFS + old→new映射 |

### 游戏开发：Space Invaders WASM ✅

**项目**: `projects/space-invaders-raylib/wasm/`
**WASM 编译**: ✅ 零错误通过（Emscripten 5.0.4）
**产物**: `space_invaders.js` (13.7KB) + `space_invaders.wasm` (19KB)

**技术实现**:
- `game.h/game.c` 纯C逻辑 → Emscripten 编译到 WASM
- `wasm_renderer.c`: 34个导出函数，无 raylib 依赖
- `index.html`: Canvas 2D 渲染 + 键盘控制（Arrow/WASD/Space/P/R）

**导出函数列表**:
```c
_init_game, _update_game, _get_game_state,
_get_score, _get_lives, _get_wave, _get_alive_count,
_get_player_x, _get_player_y,
_get_bullet_count, _get_bullet_x/y/active,
_get_enemy_bullet_count, _get_enemy_bullet_x/y/active,
_get_alien_count, _get_alien_x/y/alive/type/exploding,
_move_left, _move_right, _shoot, _reset_game,
_get_screen_w/h, _get_player_w/h, _get_alien_w/h/rows/cols
```

**Canvas 渲染设计**:
- 纯 Canvas 2D API 渲染（不依赖 raylib）
- 星空背景（60颗随机星星）
- 玩家飞船（绿色，带炮管）
- 外星人按行着色：红/橙/黄/绿，触角动画
- 玩家子弹绿色，敌人子弹红色
- HUD显示：分数、波次、生命、外星人数量
- 支持暂停(P)、重开(R)

---

### 本日累计 (2026-03-31 全天)

| 指标 | 数量 |
|------|------|
| LeetCode 完成 | **81+ 道** 🆕 |
| Hard 题目 | **41 道** 🆕 |
| Raylib 图形游戏 | 10 个 |
| WASM 游戏 | **3 个** 🆕 |
| 本日游戏总计 | **23 个** 🎉 |

---

*最后更新: 2026-03-31 08:00*
