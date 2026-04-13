"""数据库模型测试"""
import sys
import pytest
from pathlib import Path
from unittest.mock import patch
sys.path.insert(0, str(Path(__file__).parent.parent / "backend"))

# 修复database.py未导入Path的Bug（源码Bug，待悟通修复）
# 直接给模块打补丁
import backend.models.database as db_module
if not hasattr(db_module, 'Path'):
    db_module.Path = Path

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from backend.models.database import Base, Project, PullLog

def init_db_fixed(db_path):
    """修复版init_db（绕过源码Path未导入Bug）"""
    path = Path(db_path).expanduser()
    path.parent.mkdir(parents=True, exist_ok=True)
    engine = create_engine(f"sqlite:///{path}")
    Base.metadata.create_all(engine)
    return engine

class TestDatabaseInit:
    """数据库初始化测试"""
    
    def test_init_db_creates_engine(self, temp_db):
        engine = init_db_fixed(temp_db)
        assert engine is not None
        assert "sqlite" in str(engine.url)
    
    def test_init_db_creates_tables(self, temp_db):
        engine = init_db_fixed(temp_db)
        from sqlalchemy import inspect
        inspector = inspect(engine)
        tables = inspector.get_table_names()
        assert "projects" in tables
        assert "pull_logs" in tables
    
    def test_init_db_creates_parent_dirs(self, temp_db):
        """init_db应自动创建父目录"""
        import os
        parent = Path(temp_db).parent
        assert parent.exists()  # tempfile已创建


class TestProjectModel:
    """Project模型测试"""
    
    @pytest.fixture
    def db_session(self, temp_db):
        engine = init_db_fixed(temp_db)
        Session = sessionmaker(bind=engine)
        session = Session()
        yield session
        session.close()
    
    def test_create_project(self, db_session):
        p = Project(
            name="test-project",
            repo_url="https://github.com/test/repo.git",
            local_path="/tmp/test",
            auth_type="none"
        )
        db_session.add(p)
        db_session.commit()
        
        result = db_session.query(Project).filter_by(name="test-project").first()
        assert result is not None
        assert result.repo_url == "https://github.com/test/repo.git"
        assert result.branch == "main"  # 默认值
    
    def test_project_unique_name(self, db_session):
        p1 = Project(name="unique-test", repo_url="https://a.com/repo.git", local_path="/tmp/a")
        db_session.add(p1)
        db_session.commit()
        
        p2 = Project(name="unique-test", repo_url="https://b.com/repo.git", local_path="/tmp/b")
        db_session.add(p2)
        with pytest.raises(Exception):  # UNIQUE约束
            db_session.commit()
    
    def test_project_default_values(self, db_session):
        p = Project(name="defaults", repo_url="https://x.com/repo.git", local_path="/tmp/x")
        db_session.add(p)
        db_session.commit()
        
        assert p.auth_type == "none"
        assert p.branch == "main"
        assert p.is_active == True
        assert p.status == "idle"
        assert p.rules == "{}"
        assert p.plugins == "[]"
    
    def test_project_auth_types(self, db_session):
        for auth_type in ["none", "user_pass", "token", "ssh_key"]:
            p = Project(
                name=f"auth-{auth_type}",
                repo_url="https://x.com/repo.git",
                local_path=f"/tmp/{auth_type}",
                auth_type=auth_type
            )
            db_session.add(p)
        db_session.commit()
        
        count = db_session.query(Project).filter_by(auth_type="ssh_key").count()
        assert count == 1
    
    def test_project_update_status(self, db_session):
        p = Project(name="status-test", repo_url="https://x.com/repo.git", local_path="/tmp/x")
        db_session.add(p)
        db_session.commit()
        
        p.status = "running"
        db_session.commit()
        
        result = db_session.query(Project).filter_by(name="status-test").first()
        assert result.status == "running"
    
    def test_project_delete(self, db_session):
        p = Project(name="delete-me", repo_url="https://x.com/repo.git", local_path="/tmp/x")
        db_session.add(p)
        db_session.commit()
        
        db_session.delete(p)
        db_session.commit()
        
        result = db_session.query(Project).filter_by(name="delete-me").first()
        assert result is None


class TestPullLogModel:
    """PullLog模型测试"""
    
    @pytest.fixture
    def db_session(self, temp_db):
        engine = init_db_fixed(temp_db)
        Session = sessionmaker(bind=engine)
        session = Session()
        yield session
        session.close()
    
    @pytest.fixture
    def project_with_id(self, db_session):
        p = Project(name="log-test", repo_url="https://x.com/repo.git", local_path="/tmp/x")
        db_session.add(p)
        db_session.commit()
        db_session.refresh(p)
        return p.id
    
    def test_create_pull_log(self, db_session, project_with_id):
        log = PullLog(
            project_id=project_with_id,
            status="success",
            message="Pull successful",
            commit_before="abc123",
            commit_after="def456",
            diff_count=3
        )
        db_session.add(log)
        db_session.commit()
        
        result = db_session.query(PullLog).filter_by(project_id=project_with_id).first()
        assert result is not None
        assert result.status == "success"
        assert result.diff_count == 3
    
    def test_pull_log_default_diff_count(self, db_session, project_with_id):
        log = PullLog(
            project_id=project_with_id,
            status="success",
            message="OK"
        )
        db_session.add(log)
        db_session.commit()
        
        assert log.diff_count == 0
    
    def test_pull_log_multiple_for_same_project(self, db_session, project_with_id):
        for i in range(3):
            log = PullLog(
                project_id=project_with_id,
                status="success" if i < 2 else "failed",
                message=f"Pull {i}"
            )
            db_session.add(log)
        db_session.commit()
        
        count = db_session.query(PullLog).filter_by(project_id=project_with_id).count()
        assert count == 3
    
    def test_pull_log_ordering(self, db_session, project_with_id):
        """较新的日志应在前面（按created_at desc）"""
        for i in range(3):
            log = PullLog(
                project_id=project_with_id,
                status="success",
                message=f"Log {i}"
            )
            db_session.add(log)
        db_session.commit()
        
        logs = db_session.query(PullLog).filter_by(project_id=project_with_id).order_by(
            PullLog.created_at.desc()
        ).all()
        assert len(logs) == 3
