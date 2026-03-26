# 🐍 Snake Game - Terminal Edition

使用 ncurses 编写的经典贪吃蛇游戏，运行在终端中。

## 编译

```bash
make
```

或者直接：

```bash
g++ -std=c++17 -o snake snake.cpp -lncurses
```

## 运行

```bash
./snake
```

## 操作说明

| 按键 | 功能 |
|------|------|
| W / ↑ | 向上移动 |
| S / ↓ | 向下移动 |
| A / ← | 向左移动 |
| D / → | 向右移动 |
| Space / P | 暂停/继续 |
| Enter | 开始游戏 |
| R | 重新开始（游戏结束后）|
| Q / ESC | 退出游戏 |

## 游戏特性

- ✅ 方向键 + WASD 双支持
- ✅ 撞墙/撞自己检测
- ✅ 分数系统（吃食物 +10 分）
- ✅ 速度递增（每 50 分加速一次）
- ✅ 暂停/恢复功能
- ✅ 开始画面、游戏结束画面

## 技术栈

- **语言**: C++17
- **图形库**: ncurses
- **随机数**: std::mt19937
- **数据结构**: std::deque（双向队列模拟蛇身）

## 项目结构

```
snake-game/
├── snake.cpp    # 游戏逻辑实现 + main
├── snake.hpp    # 类定义和接口
├── Makefile     # 构建脚本
└── README.md    # 本文件
```

## 游戏截图

```
===== SNAKE =====
Use WASD or Arrow Keys to move
SPACE/P to pause
Press ENTER to start
Q/ESC to quit
```

## 后续扩展方向

- [ ] 墙壁穿越模式
- [ ] 双向蛇模式
- [ ] 关卡设计（障碍物）
- [ ] 最高分记录（文件持久化）
- [ ] 颜色支持
- [ ] GUI 版本（SDL2 / Raylib）
