# 2048 Game - Terminal Version

终端版 2048 游戏，使用 ncurses 库实现。

## 编译

```bash
g++ -std=c++17 -Wall -Wextra -O2 -o game2048 game2048.cpp -lncurses
```

## 运行

```bash
./game2048
```

## 操作说明

| 按键 | 功能 |
|------|------|
| ↑ / W | 向上滑动 |
| ↓ / S | 向下滑动 |
| ← / A | 向左滑动 |
| → / D | 向右滑动 |
| R | 重新开始 |
| Q | 退出游戏 |

## 游戏规则

- 4×4 网格
- 通过滑动合并相同数字的方块
- 目标：合成 2048
- 每次滑动后随机生成 2 或 4（90% 概率为 2）
- 无法移动时游戏结束

## 核心算法

### 滑动合并（Slide & Merge）

对每一行/列，向滑动方向压缩相邻相同元素并合并：

```cpp
std::pair<bool, int> slide_row_left(std::array<int, 4>& row) {
    std::array<int, 4> new_row = {};
    int write_idx = 0;
    int gained = 0;
    bool moved = false;

    for (int i = 0; i < 4; ++i) {
        if (row[i] == 0) continue;
        
        if (write_idx > 0 && new_row[write_idx - 1] == row[i]) {
            // 合并：前一位置翻倍，得分累加
            new_row[write_idx - 1] *= 2;
            gained += new_row[write_idx - 1];
            write_idx--;  // 合并后不移动write_idx，保持连续
            moved = true;
        } else {
            new_row[write_idx++] = row[i];
            if (write_idx - 1 != i) moved = true;
        }
    }
    row = new_row;
    return {moved, gained};
}
```

### 关键技巧

1. **先合并再压缩**：相同元素只合并一次
2. **合并后不推进写指针**：合并后的元素不能立即再合并
3. **矩阵旋转复用**：上下左右四个方向通过旋转行/列实现
4. **游戏状态检测**：空位检测 + 相邻相等检测

### 数据结构

- 网格：`std::array<std::array<int, 4>, 4>`（4×4 矩阵）
- 随机数：`std::mt19937` + `std::uniform_int_distribution`
- 分数持久化：写入 `/tmp/2048_best.txt`

## 技术栈

- C++17
- ncurses 终端UI库
- OOP 设计（Game2048 类封装）

## 扩展方向

- [ ] GUI 版本 (Raylib)
- [ ] 动画效果
- [ ] 悔棋功能
- [ ] 微信/网页版

---

*开发日期: 2026-03-26*
