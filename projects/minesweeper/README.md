# 扫雷 - Terminal Version

终端版扫雷游戏，使用 ncurses 库实现。

## 编译

```bash
g++ -std=c++17 -Wall -Wextra -O2 -o minesweeper minesweeper.cpp -lncurses
```

## 运行

```bash
./minesweeper
```

## 操作说明

| 按键 | 功能 |
|------|------|
| ↑ / W | 向上移动光标 |
| ↓ / S | 向下移动光标 |
| ← / A | 向左移动光标 |
| → / D | 向右移动光标 |
| 空格 / Enter | 翻开格子 |
| F | 标记/取消标记地雷 |
| R | 重新开始 |
| Q | 退出 |

## 游戏规则

- 16×30 网格，共 99 个地雷
- 翻开数字格：显示周围8格的地雷数量
- 翻开空白格：自动展开连通的空白区域（BFS flood fill）
- 标记地雷：防止误踩
- 胜利条件：翻开所有非地雷格子
- 失败条件：踩到地雷

## 核心算法

### BFS Flood Fill（洪水填充）

空白格子展开是扫雷的核心算法：

```cpp
void Minesweeper::flood_fill(int r, int c) {
    std::vector<std::pair<int,int>> q;
    q.emplace_back(r, c);
    std::vector<std::vector<bool>> visited(ROWS, std::vector<bool>(COLS, false));

    size_t head = 0;
    while (head < q.size()) {
        auto [cr, cc] = q[head++];
        grid_[cr][cc].state = CellState::REVEALED;
        ++revealed_count_;

        // 数字格停止扩展
        if (grid_[cr][cc].value != CellValue::EMPTY) continue;

        // 空白格，向8个方向扩展
        for (int k = 0; k < 8; ++k) {
            int nr = cr + dr[k], nc = cc + dc[k];
            if (边界检查 && !visited[nr][nc] && !grid_[nr][nc].is_mine) {
                visited[nr][nc] = true;
                q.emplace_back(nr, nc);
            }
        }
    }
}
```

### 安全开局算法

第一次点击不放雷，提升游戏体验：

```cpp
void Minesweeper::place_mines(int exclude_r, int exclude_c) {
    // 排除点击位置周围3x3区域
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (abs(r - exclude_r) <= 1 && abs(c - exclude_c) <= 1) continue;
            candidates.emplace_back(r, c);
        }
    }
    std::shuffle(candidates.begin(), candidates.end(), rng_);
    // 从剩余格子中随机放置地雷
}
```

### 数字计算

每个格子计算周围8格地雷数量：

```cpp
for (int k = 0; k < 8; ++k) {
    int nr = r + dr[k], nc = c + dc[k];
    if (nr/在边界内 && grid_[nr][nc].is_mine) ++cnt;
}
```

## 技术栈

- C++17
- ncurses 终端UI库
- BFS  flood fill 算法
- 随机洗牌算法（std::shuffle）

## 扩展方向

- [ ] 计时器和最好成绩记录
- [ ] 难度选择（初级/中级/高级）
- [ ] GUI 版本 (Raylib)
- [ ] 微信小程序版

---

*开发日期: 2026-03-26*
