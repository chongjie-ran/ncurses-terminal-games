"""Schema验证测试"""
import sys
import pytest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent.parent / "backend"))

from backend.schemas.schemas import (
    ProjectCreate, ProjectUpdate, ProjectResponse,
    PullLogResponse, PullResult, ApiResponse, ProjectListResponse,
    AuthTypeEnum
)

class TestProjectCreate:
    """ProjectCreate Schema测试"""
    
    def test_valid_project(self, sample_project_data):
        p = ProjectCreate(**sample_project_data)
        assert p.name == "test-repo"
        assert p.auth_type == AuthTypeEnum.NONE
    
    def test_name_min_length(self, sample_project_data):
        sample_project_data["name"] = ""
        with pytest.raises(Exception):
            ProjectCreate(**sample_project_data)
    
    def test_name_max_length(self, sample_project_data):
        sample_project_data["name"] = "a" * 256
        with pytest.raises(Exception):
            ProjectCreate(**sample_project_data)
    
    def test_default_branch(self, sample_project_data):
        del sample_project_data["branch"]
        p = ProjectCreate(**sample_project_data)
        assert p.branch == "main"
    
    def test_default_auth_type(self, sample_project_data):
        del sample_project_data["auth_type"]
        p = ProjectCreate(**sample_project_data)
        assert p.auth_type == AuthTypeEnum.NONE
    
    def test_default_is_active(self, sample_project_data):
        del sample_project_data["is_active"]
        p = ProjectCreate(**sample_project_data)
        assert p.is_active == True
    
    def test_auth_type_user_pass(self, sample_project_data):
        sample_project_data["auth_type"] = AuthTypeEnum.USER_PASS
        sample_project_data["auth_username"] = "user1"
        p = ProjectCreate(**sample_project_data)
        assert p.auth_type == AuthTypeEnum.USER_PASS
        assert p.auth_username == "user1"
    
    def test_auth_type_token(self, sample_project_data):
        sample_project_data["auth_type"] = AuthTypeEnum.TOKEN
        p = ProjectCreate(**sample_project_data)
        assert p.auth_type == AuthTypeEnum.TOKEN
    
    def test_auth_type_ssh_key(self, sample_project_data):
        sample_project_data["auth_type"] = AuthTypeEnum.SSH_KEY
        sample_project_data["auth_ssh_key_path"] = "/home/user/.ssh/id_rsa"
        p = ProjectCreate(**sample_project_data)
        assert p.auth_type == AuthTypeEnum.SSH_KEY
    
    def test_rules_default_empty_dict(self, sample_project_data):
        del sample_project_data["rules"]
        p = ProjectCreate(**sample_project_data)
        assert p.rules == {}
    
    def test_plugins_default_empty_list(self, sample_project_data):
        del sample_project_data["plugins"]
        p = ProjectCreate(**sample_project_data)
        assert p.plugins == []
    
    def test_password_excluded_from_repr(self, sample_project_data):
        """auth_password不应出现在响应中（exclude=True）"""
        p = ProjectCreate(**sample_project_data)
        # 检查exclude配置
        cls_fields = type(p).model_fields
        assert cls_fields["auth_password"].exclude == True


class TestProjectUpdate:
    """ProjectUpdate Schema测试"""
    
    def test_all_fields_optional(self):
        u = ProjectUpdate()
        assert u.name is None
        assert u.repo_url is None
        assert u.branch is None
        assert u.is_active is None
    
    def test_partial_update_name_only(self):
        u = ProjectUpdate(name="new-name")
        assert u.name == "new-name"
        assert u.repo_url is None
    
    def test_exclude_unset_false_password(self):
        """未设置的password字段不应出现在dump中"""
        u = ProjectUpdate(name="new-name")
        data = u.model_dump(exclude_unset=True)
        assert "auth_password" not in data


class TestPullResult:
    """PullResult Schema测试"""
    
    def test_success_result(self):
        r = PullResult(
            success=True,
            message="Pull成功",
            repo_path="/tmp/repo",
            changed=True,
            commit_before="abc123",
            commit_after="def456",
            diff_count=5
        )
        assert r.success == True
        assert r.changed == True
        assert r.diff_count == 5
    
    def test_no_change_result(self):
        r = PullResult(
            success=True,
            message="已是最新",
            repo_path="/tmp/repo",
            changed=False,
            diff_count=0
        )
        assert r.changed == False
        assert r.diff_count == 0
    
    def test_failed_result(self):
        r = PullResult(
            success=False,
            message="合并冲突",
            repo_path="/tmp/repo",
            changed=False
        )
        assert r.success == False
    
    def test_default_diff_count(self):
        r = PullResult(success=True, message="OK", repo_path="/repo", changed=True)
        assert r.diff_count == 0


class TestApiResponse:
    """ApiResponse Schema测试"""
    
    def test_success_response_with_data(self):
        r = ApiResponse(success=True, message="OK", data={"id": 1})
        assert r.success == True
        assert r.data == {"id": 1}
    
    def test_success_response_without_data(self):
        r = ApiResponse(success=True, message="Deleted")
        assert r.data is None
    
    def test_failure_response(self):
        r = ApiResponse(success=False, message="Not found", data=None)
        assert r.success == False
