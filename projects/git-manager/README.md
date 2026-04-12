# Git Project Manager

Git 项目管理系统 - 幂等并行仓库拉取 + FastAPI + React

## 功能特性

- **幂等拉取**: 已存在则更新，不存在则克隆
- **并发安全**: 基于文件锁的冲突保护
- **多种认证**: 无认证 / 用户名密码 / Token / SSH私钥
- **加密存储**: 认证信息使用 Fernet 对称加密
- **完整日志**: 每次拉取记录 commit 变更
- **RESTful API**: FastAPI 提供完整 CRUD + 执行接口
- **CLI 工具**: 命令行直接操作
- **Web UI**: React 前端，现代化界面

## 目录结构

```
git-manager/
├── scripts/
│   └── git_puller.py
├── backend/
│   ├── main.py
│   ├── cli.py
│   ├── api/routes.py
│   ├── models/database.py
│   ├── schemas/schemas.py
│   └── core/
│       ├── __init__.py
│       └── puller.py
├── frontend/
│   ├── src/
│   │   ├── App.tsx
│   │   ├── main.tsx
│   │   └── index.css
│   ├── package.json
│   └── vite.config.ts
└── requirements.txt
```

## 快速开始

### 安装依赖
```bash
pip install -r requirements.txt
```

### 启动后端
```bash
cd backend && python main.py
```

### 启动前端
```bash
cd frontend && npm install && npm run dev
```

## CLI 命令
```bash
python backend/cli.py list
python backend/cli.py create --name proj --url URL --path /local/path
python backend/cli.py pull --id 1
python backend/cli.py logs --id 1
```

## API 接口
| 方法 | 路径 | 说明 |
|------|------|------|
| GET | /api/projects | 列出项目 |
| POST | /api/projects | 创建项目 |
| PUT | /api/projects/{id} | 更新项目 |
| DELETE | /api/projects/{id} | 删除项目 |
| POST | /api/projects/{id}/pull | 拉取 |
| GET | /api/projects/{id}/logs | 日志 |
