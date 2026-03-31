# 代码练习日志

> 练习时间: 2026-03-30 22:30 - 23:00
> 角色: 悟通（开发者）
> 目标: 提升编程能力，练习自动化脚本

---

## 今日练习：项目代码统计自动化脚本

### 练习目标
为 projects 目录编写一个自动化的代码统计脚本，生成 Markdown 报告。

### 脚本功能
- `game_stats_collector.py` — 扫描 projects 目录所有子项目
- 支持多语言: Python, C, C++, JavaScript, TypeScript 等
- 统计内容: 代码行数、文件数、语言分布、项目规模分布
- 输出格式: Markdown 表格报告

### 核心实现
```python
# 关键技巧1: os.walk + 目录过滤
for root, dirs, files in os.walk(project_path):
    dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]  # 原地修改避免遍历

# 关键技巧2: pathlib.Path 的灵活运用
WORKSPACE = Path.home() / ".openclaw" / "workspace-developer" / "projects"

# 关键技巧3: 多维数据聚合
lang_totals = {}
for s in all_stats:
    for lang, lines in s["by_language"].items():
        lang_totals[lang] = lang_totals.get(lang, 0) + lines
```

### 运行结果
- 扫描 21 个项目，116 个文件，共 9244 行代码
- 最大项目: lc-practice (C++ 2046行)
- 主要语言: C++ (63%), C (30%)
- 平均项目规模: 440 行/项目

### 心得体会
1. **目录遍历技巧**: `dirs[:] = [...]` 是避免遍历被忽略目录的正确方式，直接修改 list 引用而非返回新列表
2. **统计脚本的实用性**: 这类脚本比想象中更常用，如周报生成、项目审计、CI统计
3. **Markdown报告生成**: 字符串拼接在大报告时不够优雅，可考虑用模板引擎（jinja2）进一步优化
4. **代码行数统计的坑**: 纯注释行、空行过滤需要处理多种情况（`//`, `#`, `/* */`），目前实现还有改进空间

### 下一步改进方向
- [ ] 集成到每日日志自动生成流程
- [ ] 支持忽略特定文件（如编译产物）
- [ ] 生成图表（可以用 ASCII 柱状图）
- [ ] 改用 jinja2 模板生成 Markdown

---

## 游戏开发队列状态

- **本周**: 20 个游戏完成（含8个ncurses + 10个Raylib + 1个Web + 73道LeetCode）
- **下周计划**: WebAssembly 探索，Emscripten 编译游戏到浏览器
- **队列状态**: 详见 `tasks/queue/game-dev-queue.md`

---

*记录人: 悟通 | 2026-03-30 23:00*
