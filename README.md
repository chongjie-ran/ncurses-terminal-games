# 🎮 ncurses-terminal-games

A collection of classic terminal games built with **C++17** and **ncurses**.

![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-blue)
![C++](https://img.shields.io/badge/C%2B%2B-17-green)
![License](https://img.shields.io/badge/license-MIT-orange)

---

## 📋 Games

| Game | Description | Core Algorithm |
|------|-------------|----------------|
| 🐍 [Snake](./projects/snake-game/) | Classic snake eating food | deque + direction buffer |
| 🔢 [2048](./projects/game-2048/) | Slide and merge tiles | matrix rotation + merge |
| 💣 [Minesweeper](./projects/minesweeper/) | Clear mines without explosion | BFS flood fill + safe first-click |
| 🐦 [Flappy Bird](./projects/flappy-bird/) | Fly through pipes | gravity physics + AABB collision |
| 🔤 [Hangman](./projects/hangman/) | Guess the hidden word | set + random hint + ASCII art |

---

## 🚀 Quick Start

```bash
# Install ncurses (macOS)
brew install ncurses

# Linux
sudo apt install libncurses-dev

# Compile any game
cd projects/<game-name>
make

# Run
./<game-executable>
```

---

## 🛠️ Requirements

- C++17 compiler (clang++ / g++)
- ncurses library

---

## 📚 Code Practice

LeetCode algorithm practice solutions in `practice/`:

- `lc48_rotate_matrix.cpp` — Rotate n×n matrix 90° (transposition + mirror)
- `lc42_trap.cpp` — Trapping Rain Water (monotonic stack)
- `lc3_longest_substring.cpp` — Longest substring without repeating chars (sliding window)
- `lemon88_largest_rectangle.cpp` — Largest Rectangle in Histogram (monotonic stack)

---

## 📖 Learning Log

See `memory/developer-code-log.md` for detailed development notes, techniques learned, and problem-solving approaches.

---

*Developed as part of the 悟通 (WuTong) developer growth program*
