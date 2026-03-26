# Hangman - Terminal Version

终端版猜单词游戏，使用 ncurses 库实现。

## 编译

```bash
g++ -std=c++17 -Wall -Wextra -O2 -o hangman hangman.cpp -lncurses
```

## 运行

```bash
./hangman
```

## 操作

| 按键 | 功能 |
|------|------|
| A-Z | 猜测字母 |
| 空格 | 游戏结束后重新开始 |
| Q | 退出 |

## 游戏规则

- 猜出隐藏的单词
- 每次猜错会画出 Hangman 的一部分
- 6 次猜错后游戏结束
- 单词会按类别分组（编程/科技/AI/硬件/游戏开发）

## 技术实现

- **ASCII 艺术**: 7 阶段 Hangman 图形
- **滑动窗口**: 字母状态追踪
- **随机提示**: 游戏开始时随机显示1-2个字母作为提示
- **分类单词库**: 5个类别，共45+个单词
- **ncurses UI**: 彩色终端渲染

## 项目结构

```
hangman.cpp  # 主程序
README.md    # 说明文档
```

## 单词库

| 类别 | 单词数 |
|------|--------|
| Programming | 5 |
| Technology | 10 |
| AI/ML | 9 |
| Computer Parts | 9 |
| Game Dev | 13 |
