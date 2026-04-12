"""CLI 命令行工具"""
import sys
import argparse
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent))

from backend.models.database import init_db, Project, PullLog
from backend.schemas.schemas import ProjectCreate
from backend.core import Encryptor
from sqlalchemy.orm import Session
from datetime import datetime

def get_db():
    engine = init_db()
    from sqlalchemy.orm import sessionmaker
    SessionLocal = sessionmaker(bind=engine)
    return SessionLocal()

def cmd_list(args):
    db = get_db()
    projects = db.query(Project).all()
    print(f"{'='*80}")
    print(f"{"ID":<5} {"名称":<20} {"仓库":<40} {"状态":<10}")
    print(f"{'-'*80}")
    for p in projects:
        print(f"{p.id:<5} {p.name:<20} {p.repo_url[:38]:<40} {p.status:<10}")
    print(f"{'='*80}")
    print(f"共 {len(projects)} 个项目")

def cmd_create(args):
    db = get_db()
    existing = db.query(Project).filter(Project.name == args.name).first()
    if existing:
        print(f"错误：项目 {args.name} 已存在")
        return 1
    
    encrypted_password = Encryptor.encrypt(args.password or "")
    project = Project(
        name=args.name,
        repo_url=args.url,
        branch=args.branch,
        local_path=args.path,
        auth_type=args.auth_type,
        auth_username=args.username or "",
        auth_password_encrypted=encrypted_password,
        auth_ssh_key_path=args.ssh_key or ""
    )
    db.add(project)
    db.commit()
    print(f"成功：项目 {args.name} 创建成功 (ID: {project.id})")
    return 0

def cmd_delete(args):
    db = get_db()
    project = db.query(Project).filter(Project.id == args.id).first()
    if not project:
        print(f"错误：项目 ID {args.id} 不存在")
        return 1
    db.delete(project)
    db.commit()
    print(f"成功：项目 {project.name} 已删除")
    return 0

def cmd_pull(args):
    from backend.api.routes import pull_project
    db = get_db()
    project = db.query(Project).filter(Project.id == args.id).first()
    if not project:
        print(f"错误：项目 ID {args.id} 不存在")
        return 1
    print(f"正在拉取项目：{project.name}")
    result = pull_project(project.id, db)
    if result.success:
        print(f"成功：{result.message}")
    else:
        print(f"失败：{result.message}")
    return 0 if result.success else 1

def cmd_logs(args):
    db = get_db()
    logs = db.query(PullLog).filter(PullLog.project_id == args.id).order_by(
        PullLog.created_at.desc()
    ).limit(args.limit).all()
    print(f"{'='*80}")
    print(f"项目 {args.id} 的拉取日志：")
    print(f"{'-'*80}")
    for log in logs:
        status_icon = "✅" if log.status == "success" else "❌"
        print(f"{status_icon} [{log.created_at}] {log.message}")
    print(f"{'='*80}")

def main():
    parser = argparse.ArgumentParser(description="Git 项目管理 CLI")
    subparsers = parser.add_subparsers(dest="command", help="子命令")
    
    # list
    subparsers.add_parser("list", help="列出所有项目")
    
    # create
    create_parser = subparsers.add_parser("create", help="创建项目")
    create_parser.add_argument("--name", required=True, help="项目名称")
    create_parser.add_argument("--url", required=True, help="Git 仓库地址")
    create_parser.add_argument("--path", required=True, help="本地路径")
    create_parser.add_argument("--branch", default="main", help="分支名")
    create_parser.add_argument("--auth-type", default="none", choices=["none","user_pass","token","ssh_key"])
    create_parser.add_argument("--username", default="", help="用户名")
    create_parser.add_argument("--password", default="", help="密码/Token")
    create_parser.add_argument("--ssh-key", default="", help="SSH 私钥路径")
    
    # delete
    delete_parser = subparsers.add_parser("delete", help="删除项目")
    delete_parser.add_argument("--id", type=int, required=True, help="项目 ID")
    
    # pull
    pull_parser = subparsers.add_parser("pull", help="拉取项目")
    pull_parser.add_argument("--id", type=int, required=True, help="项目 ID")
    
    # logs
    logs_parser = subparsers.add_parser("logs", help="查看拉取日志")
    logs_parser.add_argument("--id", type=int, required=True, help="项目 ID")
    logs_parser.add_argument("--limit", type=int, default=20, help="日志数量")
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 0
    
    commands = {"list": cmd_list, "create": cmd_create, "delete": cmd_delete, "pull": cmd_pull, "logs": cmd_logs}
    return commands.get(args.command, lambda a: 1)(args)

if __name__ == "__main__":
    sys.exit(main())
