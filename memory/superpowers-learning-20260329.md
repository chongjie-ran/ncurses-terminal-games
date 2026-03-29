# Superpowers开发方法论学习

> 日期：2026-03-29
> 角色：悟通（开发者）

## 一、核心概念

Superpowers是一套为AI coding agent设计的**结构化开发方法论**，核心理念：

> 不要一上来就写代码，而是：理解需求 → 设计方案 → 制定计划 → TDD实现 → 审查代码 → 收尾

## 二、5条核心原则

| 原则 | 含义 |
|------|------|
| 1. 设计在实现之先 | 不要跳到代码，先设计方案 |
| 2. 证据在声称之前 | 不要说"修好了"，要运行验证 |
| 3. 根因在修复之先 | 不要猜测，要调试找根因 |
| 4. 测试在代码之先 | TDD，不是后补测试 |
| 5. 审查在集成之先 | 问题早发现 |

## 三、TDD循环

```
RED（红）→ GREEN（绿）→ REFACTOR（重构）
  ↓           ↓            ↓
写失败测试    写最少代码    重构优化
```

## 四、完整开发流程

```
用户请求 → brainstorming(设计) → isolated-workspace(建分支)
→ writing-plans(写计划) → subagent-dev/executing-plans(执行)
→ verification(验证) → code-review(审查) → finishing-branch(收尾)
```

## 五、Skill套件（9个核心技能）

### 🚀 入门
- `superpowers-overview` - 总览入口
- `superpowers-brainstorming` - 需求探索+方案设计
- `superpowers-writing-plans` - 写实现计划

### 🔨 执行
- `superpowers-isolated-workspace` - 创建隔离分支
- `superpowers-subagent-dev` - 派发subagent执行
- `superpowers-parallel-agents` - 并行执行
- `superpowers-tdd` - TDD循环

### ✅ 质量保障
- `superpowers-verification` - 证据先行验证
- `superpowers-systematic-debugging` - 系统调试
- `openclaw-requesting-code-review` - 请求代码审查
- `openclaw-receiving-code-review` - 接收审查反馈
- `superpowers-finishing-branch` - 分支收尾

## 六、与AGENTS.md的关系

| 文件 | 职责 |
|------|------|
| AGENTS.md | 我是谁、我的职责、我的工作区 |
| Superpowers | 结构化开发流程、质量规范 |

**两者协同**：先了解我是谁，再用正确方法做事。

## 七、日常决策

- "要做X功能" → brainstorming → writing-plans → subagent-dev
- "要修bug" → systematic-debugging → tdd → verification
- "3个独立测试失败" → parallel-agents → 整合
- "代码写完准备提交" → verification → code-review → finishing-branch

## 八、悟通的行动指引

作为开发者（悟通），以后接到开发任务时：
1. 先用 `superpowers-brainstorming` 理解需求
2. 再用 `superpowers-writing-plans` 制定计划
3. 使用 `superpowers-tdd` 进行TDD实现
4. 用 `superpowers-verification` 验证
5. 用 `openclaw-requesting-code-review` 请求审查
