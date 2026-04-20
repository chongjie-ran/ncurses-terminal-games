"""FastAPI API 端点测试"""
import sys
import pytest
import tempfile
import os
import json
from pathlib import Path
from unittest.mock import patch, MagicMock

sys.path.insert(0, str(Path(__file__).parent.parent / "backend"))

# 必须先设置环境变量，再导入app
import os as _os
os_db_path = None  # 将被 fixture 设置

# 捕获临时 DB 路径
_orig_init_db = None

def get_test_db_path(tmp_path):
    return str(tmp_path / "test.db")

class TestHealthEndpoint:
    """健康检查端点"""

    def test_health_ok(self, test_client):
        resp = test_client.get("/health")
        assert resp.status_code == 200
        data = resp.json()
        assert data["status"] == "ok"
        assert "db" in data


class TestProjectCRUD:
    """项目 CRUD 端点"""

    def test_create_project(self, test_client):
        resp = test_client.post("/projects", json={
            "name": "my-repo",
            "repo_url": "https://github.com/test/repo.git",
            "branch": "main",
            "local_path": "/tmp/my-repo",
            "auth_type": "none",
            "credentials": "",
        })
        assert resp.status_code == 200
        assert "id" in resp.json()
        assert resp.json()["message"] == "Created"

    def test_create_duplicate_path_conflict(self, test_client):
        payload = {
            "name": "repo-a",
            "repo_url": "https://github.com/test/a.git",
            "local_path": "/tmp/duplicate-repo",
            "auth_type": "none",
        }
        r1 = test_client.post("/projects", json=payload)
        assert r1.status_code == 200
        r2 = test_client.post("/projects", json=payload)
        assert r2.status_code == 409

    def test_create_missing_required_field(self, test_client):
        resp = test_client.post("/projects", json={
            "name": "test",
            # missing repo_url and local_path
        })
        assert resp.status_code == 422  # FastAPI validation error

    def test_list_projects(self, test_client):
        # 创建两个项目
        for i in range(2):
            test_client.post("/projects", json={
                "name": f"repo-{i}",
                "repo_url": f"https://github.com/test/{i}.git",
                "local_path": f"/tmp/repo-{i}",
                "auth_type": "none",
            })
        resp = test_client.get("/projects")
        assert resp.status_code == 200
        projects = resp.json()
        assert len(projects) == 2

    def test_get_project(self, test_client):
        create = test_client.post("/projects", json={
            "name": "get-test",
            "repo_url": "https://github.com/test/get.git",
            "local_path": "/tmp/get-test",
            "auth_type": "none",
        })
        pid = create.json()["id"]
        resp = test_client.get(f"/projects/{pid}")
        assert resp.status_code == 200
        data = resp.json()
        assert data["name"] == "get-test"

    def test_get_project_not_found(self, test_client):
        resp = test_client.get("/projects/99999")
        assert resp.status_code == 404

    def test_update_project(self, test_client):
        create = test_client.post("/projects", json={
            "name": "old-name",
            "repo_url": "https://github.com/test/old.git",
            "local_path": "/tmp/update-test",
            "auth_type": "none",
        })
        pid = create.json()["id"]
        resp = test_client.put(f"/projects/{pid}", json={
            "name": "new-name",
            "branch": "develop",
        })
        assert resp.status_code == 200
        assert resp.json()["message"] == "Updated"
        # 验证更新生效
        get = test_client.get(f"/projects/{pid}")
        data = get.json()
        assert data["name"] == "new-name"
        assert data["branch"] == "develop"

    def test_update_not_found(self, test_client):
        resp = test_client.put("/projects/99999", json={"name": "x"})
        assert resp.status_code == 404

    def test_delete_project(self, test_client):
        create = test_client.post("/projects", json={
            "name": "to-delete",
            "repo_url": "https://github.com/test/del.git",
            "local_path": "/tmp/del-test",
            "auth_type": "none",
        })
        pid = create.json()["id"]
        resp = test_client.delete(f"/projects/{pid}")
        assert resp.status_code == 200
        # 验证删除
        get = test_client.get(f"/projects/{pid}")
        assert get.status_code == 404

    def test_delete_not_found(self, test_client):
        resp = test_client.delete("/projects/99999")
        assert resp.status_code == 404

    def test_credentials_encrypted_in_response(self, test_client):
        """凭证在响应中应被解密（model层解密）"""
        test_client.post("/projects", json={
            "name": "cred-test",
            "repo_url": "https://github.com/test/cred.git",
            "local_path": "/tmp/cred-test",
            "auth_type": "token",
            "credentials": "super-secret-token",
        })
        resp = test_client.get("/projects")
        projects = resp.json()
        cred_proj = next(p for p in projects if p["name"] == "cred-test")
        # credentials 在解密后应该等于存入的值
        assert cred_proj["credentials"] == "super-secret-token"

    def test_rules_plugins_default_empty(self, test_client):
        """rules 和 plugins 默认空列表"""
        create = test_client.post("/projects", json={
            "name": "defaults-test",
            "repo_url": "https://github.com/test/def.git",
            "local_path": "/tmp/defaults-test",
            "auth_type": "none",
        })
        pid = create.json()["id"]
        resp = test_client.get(f"/projects/{pid}")
        data = resp.json()
        assert data["rules"] == []
        assert data["plugins"] == []


class TestPullEndpoint:
    """Pull 操作端点 - mock GitPuller"""

    @patch("git_puller.GitPuller")
    def test_pull_success(self, mock_gp_class, test_client):
        # 创建项目
        create = test_client.post("/projects", json={
            "name": "pull-test",
            "repo_url": "https://github.com/test/pull.git",
            "local_path": "/tmp/pull-test",
            "auth_type": "none",
        })
        pid = create.json()["id"]
        # Mock GitPuller.pull 返回成功
        mock_gp = MagicMock()
        mock_gp.pull.return_value = {
            "success": True,
            "message": "Pull success",
            "output": "",
            "duration": 0.5,
        }
        mock_gp_class.return_value = mock_gp
        resp = test_client.post(f"/projects/{pid}/pull")
        assert resp.status_code == 200
        data = resp.json()
        assert data["status"] == "success"
        mock_gp.pull.assert_called_once()

    @patch("git_puller.GitPuller")
    def test_pull_failure(self, mock_gp_class, test_client):
        create = test_client.post("/projects", json={
            "name": "pull-fail",
            "repo_url": "https://github.com/test/fail.git",
            "local_path": "/tmp/pull-fail",
            "auth_type": "none",
        })
        pid = create.json()["id"]
        mock_gp = MagicMock()
        mock_gp.pull.return_value = {
            "success": False,
            "message": "Fetch failed",
            "output": "",
            "duration": 0.1,
        }
        mock_gp_class.return_value = mock_gp
        resp = test_client.post(f"/projects/{pid}/pull")
        assert resp.status_code == 200
        assert resp.json()["status"] == "failed"

    @patch("git_puller.GitPuller")
    def test_pull_not_found(self, mock_gp_class, test_client):
        resp = test_client.post("/projects/99999/pull")
        assert resp.status_code == 404

    @patch("git_puller.GitPuller")
    def test_pull_updates_db_status(self, mock_gp_class, test_client):
        """pull 后 DB 中 status 字段应更新"""
        create = test_client.post("/projects", json={
            "name": "pull-status",
            "repo_url": "https://github.com/test/status.git",
            "local_path": "/tmp/pull-status",
            "auth_type": "none",
        })
        pid = create.json()["id"]
        mock_gp = MagicMock()
        mock_gp.pull.return_value = {"success": True, "message": "OK", "output": "", "duration": 0.3}
        mock_gp_class.return_value = mock_gp
        test_client.post(f"/projects/{pid}/pull")
        # 查询状态端点
        status = test_client.get(f"/projects/{pid}/status")
        assert status.status_code == 200


class TestStatusEndpoint:
    """Git Status 端点 - mock GitPuller"""

    @patch("git_puller.GitPuller")
    def test_get_status(self, mock_gp_class, test_client):
        create = test_client.post("/projects", json={
            "name": "status-test",
            "repo_url": "https://github.com/test/status.git",
            "local_path": "/tmp/status-test",
            "auth_type": "none",
        })
        pid = create.json()["id"]
        mock_gp = MagicMock()
        mock_gp.get_status.return_value = {
            "initialized": True,
            "branch": "main",
            "clean": True,
            "message": "nothing to commit",
            "last_commit": "abc123 test commit",
        }
        mock_gp_class.return_value = mock_gp
        resp = test_client.get(f"/projects/{pid}/status")
        assert resp.status_code == 200
        data = resp.json()
        assert data["git_status"]["branch"] == "main"
        assert data["git_status"]["clean"] is True

    @patch("git_puller.GitPuller")
    def test_get_status_not_found(self, mock_gp_class, test_client):
        resp = test_client.get("/projects/99999/status")
        assert resp.status_code == 404
