"""测试固件"""
import sys
import pytest
import tempfile
import os
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent / "backend"))

@pytest.fixture
def temp_db():
    """临时数据库"""
    with tempfile.TemporaryDirectory() as tmpdir:
        db_path = Path(tmpdir) / "test.db"
        yield str(db_path)

@pytest.fixture
def sample_project_data():
    """样例项目数据"""
    return {
        "name": "test-repo",
        "repo_url": "https://github.com/test/repo.git",
        "branch": "main",
        "local_path": "/tmp/test-repo",
        "auth_type": "none",
        "auth_username": "",
        "auth_password": "secret123",
        "auth_ssh_key_path": "",
        "rules": {"max_size_mb": 100},
        "plugins": ["pre-commit"],
        "is_active": True
    }
