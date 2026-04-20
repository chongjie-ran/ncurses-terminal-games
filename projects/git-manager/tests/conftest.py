"""测试固件"""
import sys
import pytest
import tempfile
import os
import shutil
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
        "is_active": True,
    }


# ── FastAPI TestClient fixture ──────────────────────────────────────────────

@pytest.fixture
def tmp_path_factory_basedir(tmp_path_factory):
    """共享临时目录供 test_client 使用"""
    return tmp_path_factory.mktemp("git_manager_test")


@pytest.fixture
def test_client(tmp_path_factory_basedir):
    """
    FastAPI TestClient，连到临时 SQLite DB（每次测试独立）。
    通过 monkeypatch 覆盖 main.py 中的 DB_PATH。
    """
    from fastapi.testclient import TestClient
    import main as main_module

    # 每次创建新的临时 DB
    test_db = tmp_path_factory_basedir / "test.db"
    main_module.DB_PATH = test_db

    # 预先 import git_puller，让 patch("git_puller.GitPuller") 能找到模块
    scripts_dir = str(Path(__file__).parent.parent / "scripts")
    if scripts_dir not in sys.path:
        sys.path.insert(0, scripts_dir)
    import git_puller  # noqa: F401 - 注册到 sys.modules

    # 确保 init_db 使用新的 DB
    main_module.init_db()

    client = TestClient(main_module.app)
    yield client

    # 清理
    if test_db.exists():
        test_db.unlink()
