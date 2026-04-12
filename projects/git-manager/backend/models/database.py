"""数据库模型"""
from sqlalchemy import create_engine, Column, Integer, String, Text, DateTime, Boolean
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.sql import func

Base = declarative_base()

class Project(Base):
    __tablename__ = "projects"

    id = Column(Integer, primary_key=True, autoincrement=True)
    name = Column(String(255), nullable=False, unique=True)
    repo_url = Column(String(1024), nullable=False)
    auth_type = Column(String(50), default="none")
    auth_username = Column(String(255), default="")
    auth_password_encrypted = Column(Text, default="")
    auth_ssh_key_path = Column(String(512), default="")
    branch = Column(String(255), default="main")
    local_path = Column(String(1024), nullable=False)
    rules = Column(Text, default="{}")
    plugins = Column(Text, default="[]")
    is_active = Column(Boolean, default=True)
    last_pull_at = Column(DateTime, nullable=True)
    last_commit = Column(String(64), nullable=True)
    status = Column(String(50), default="idle")
    created_at = Column(DateTime, default=func.now())
    updated_at = Column(DateTime, default=func.now(), onupdate=func.now())

class PullLog(Base):
    __tablename__ = "pull_logs"

    id = Column(Integer, primary_key=True, autoincrement=True)
    project_id = Column(Integer, nullable=False)
    status = Column(String(50), nullable=False)
    message = Column(Text)
    commit_before = Column(String(64), nullable=True)
    commit_after = Column(String(64), nullable=True)
    diff_count = Column(Integer, default=0)
    created_at = Column(DateTime, default=func.now())

def init_db(db_path="~/.openclaw/workspace-developer/projects/git-manager/data/git_manager.db"):
    path = Path(db_path).expanduser()
    path.parent.mkdir(parents=True, exist_ok=True)
    engine = create_engine(f"sqlite:///{path}")
    Base.metadata.create_all(engine)
    return engine

if __name__ == "__main__":
    engine = init_db()
    print(f"数据库初始化完成：{engine.url}")
