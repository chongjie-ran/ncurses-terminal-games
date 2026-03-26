# Flappy Bird - Terminal Version

终端版 Flappy Bird 游戏，使用 ncurses 库实现。

## 编译

```bash
g++ -std=c++17 -Wall -Wextra -O2 -o flappy_bird flappy_bird.cpp -lncurses
```

## 运行

```bash
./flappy_bird
```

## 操作

| 按键 | 功能 |
|------|------|
| 空格 / W / ↑ | 跳跃 |
| Q | 退出 |

## 游戏规则

- 控制小鸟穿过管道间隙
- 每穿过一根管道得 1 分
- 撞到管道或边界则游戏结束
- 游戏结束后按空格重新开始

## 技术实现

- **物理引擎**: 重力加速度 + 跳跃速度
- **碰撞检测**: AABB 碰撞检测
- **随机管道**: 每帧随机生成管道间隙位置
- **渲染**: ncurses 终端渲染

## 项目结构

```
flappy_bird.cpp  # 主程序
README.md        # 说明文档
```
