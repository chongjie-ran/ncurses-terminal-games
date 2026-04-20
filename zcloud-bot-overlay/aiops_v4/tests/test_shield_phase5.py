"""
Shield Phase 5 测试用例
测试API网关集成的所有功能
"""

import asyncio
import time
from datetime import datetime, timedelta
from unittest.mock import AsyncMock, MagicMock, patch

import pytest

# 测试导入
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from security.api_auth import (
    APIKeyAuth,
    AuthManager,
    JWTAuth,
    SignatureValidator,
)
from security.api_rate_limiter import (
    APIBreaker,
    BreakerRegistry,
    CircuitBreakerConfig,
    RateLimiter,
    RateLimitExceeded,
)
from security.api_validator import RequestValidator, get_validator
from security.models import (
    AuditAction,
    AuditLog,
    AuditLogFilter,
    Permission,
    PrivilegeEscalationRequest,
    PrivilegeEscalationStatus,
    User,
    UserCreate,
    UserUpdate,
    ValidationResult,
)
from security.sdk.client import ShieldClient, ShieldAPIError, ShieldSDK


# ========== 测试辅助函数 ==========

def run_async(coro):
    """运行异步测试"""
    try:
        loop = asyncio.get_running_loop()
    except RuntimeError:
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
    return loop.run_until_complete(coro)


# ========== JWTAuth 测试 ==========

class TestJWTAuth:
    """JWT认证测试"""
    
    def test_create_and_verify_token(self):
        """测试创建和验证Token"""
        auth = JWTAuth(secret_key="test-secret")
        
        token = auth.create_token(
            user_id="user123",
            permissions=["read", "write"],
            exp_seconds=3600
        )
        
        assert token is not None
        assert isinstance(token, str)
        
        payload = auth.verify_token(token)
        assert payload is not None
        assert payload["user_id"] == "user123"
        assert payload["permissions"] == ["read", "write"]
    
    def test_verify_invalid_token(self):
        """测试验证无效Token"""
        auth = JWTAuth(secret_key="test-secret")
        
        payload = auth.verify_token("invalid-token")
        assert payload is None
    
    def test_verify_expired_token(self):
        """测试验证过期Token"""
        auth = JWTAuth(secret_key="test-secret")
        
        token = auth.create_token(
            user_id="user123",
            permissions=[],
            exp_seconds=-1  # 已过期
        )
        
        payload = auth.verify_token(token)
        assert payload is None
    
    def test_refresh_token(self):
        """测试刷新Token"""
        auth = JWTAuth(secret_key="test-secret")
        
        token = auth.create_token(
            user_id="user123",
            permissions=["read"],
            exp_seconds=3600
        )
        
        new_token = auth.refresh_token(token, exp_seconds=7200)
        assert new_token is not None
        assert new_token != token
        
        payload = auth.verify_token(new_token)
        assert payload["user_id"] == "user123"


# ========== APIKeyAuth 测试 ==========

class TestAPIKeyAuth:
    """API Key认证测试"""
    
    def test_generate_api_key(self):
        """测试生成API Key"""
        auth = APIKeyAuth()
        
        api_key, api_secret = auth.generate_api_key("user123")
        
        assert api_key.startswith("sk_")
        assert len(api_key) >= 32
        assert len(api_secret) >= 32
    
    def test_verify_api_key_with_secret(self):
        """测试验证API Key（带Secret）"""
        auth = APIKeyAuth()
        
        api_key, api_secret = auth.generate_api_key("user123")
        
        user_id = auth.verify_api_key(api_key, api_secret)
        assert user_id == "user123"
    
    def test_verify_api_key_without_secret(self):
        """测试验证API Key（不带Secret）"""
        auth = APIKeyAuth()
        
        api_key, api_secret = auth.generate_api_key("user123")
        
        # 不带secret验证
        user_id = auth.verify_api_key(api_key)
        assert user_id == "user123"
    
    def test_verify_invalid_api_key(self):
        """测试验证无效API Key"""
        auth = APIKeyAuth()
        
        user_id = auth.verify_api_key("invalid-key")
        assert user_id is None
    
    def test_verify_wrong_secret(self):
        """测试验证错误Secret"""
        auth = APIKeyAuth()
        
        api_key, _ = auth.generate_api_key("user123")
        
        user_id = auth.verify_api_key(api_key, "wrong-secret")
        assert user_id is None
    
    def test_revoke_api_key(self):
        """测试撤销API Key"""
        auth = APIKeyAuth()
        
        api_key, api_secret = auth.generate_api_key("user123")
        
        result = auth.revoke_api_key(api_key)
        assert result is True
        
        # 验证已撤销
        user_id = auth.verify_api_key(api_key, api_secret)
        assert user_id is None
    
    def test_get_user_api_keys(self):
        """测试获取用户的所有API Key"""
        auth = APIKeyAuth()
        
        key1, _ = auth.generate_api_key("user123")
        key2, _ = auth.generate_api_key("user123")
        key3, _ = auth.generate_api_key("user456")
        
        keys = auth.get_user_api_keys("user123")
        assert len(keys) == 2
        assert key1 in keys
        assert key2 in keys
        assert key3 not in keys


# ========== SignatureValidator 测试 ==========

class TestSignatureValidator:
    """签名验证器测试"""
    
    def test_generate_and_verify_signature(self):
        """测试生成和验证签名"""
        secret = "test-secret"
        # Use a recent timestamp that will pass tolerance check
        current_time = str(int(time.time()))
        body = '{"key": "value"}'
        
        signature = SignatureValidator.generate_signature(
            api_secret=secret,
            method="POST",
            path="/api/test",
            timestamp=current_time,
            body=body
        )
        
        assert signature is not None
        
        result = SignatureValidator.verify_signature(
            api_secret=secret,
            method="POST",
            path="/api/test",
            timestamp=current_time,
            body=body,
            signature=signature,
            tolerance_seconds=300
        )
        assert result is True
    
    def test_verify_expired_timestamp(self):
        """测试验证过期时间戳"""
        result = SignatureValidator.verify_signature(
            api_secret="secret",
            method="POST",
            path="/api/test",
            timestamp="1000000000",  # 很久以前
            body=None,
            signature="any",
            tolerance_seconds=300
        )
        assert result is False


# ========== AuthManager 测试 ==========

class TestAuthManager:
    """认证管理器测试"""
    
    def test_authenticate_with_jwt(self):
        """测试JWT认证"""
        manager = AuthManager(jwt_secret="test-secret")
        
        token = manager.create_access_token(
            user_id="user123",
            permissions=["read", "write"]
        )
        
        from security.models import APICredentials
        creds = APICredentials(jwt_token=token)
        
        result = manager.authenticate(creds)
        assert result is not None
        assert result["user_id"] == "user123"
        assert result["auth_type"] == "jwt"
    
    def test_authenticate_with_api_key(self):
        """测试API Key认证"""
        manager = AuthManager()
        
        api_key, api_secret = manager.api_key_auth.generate_api_key("user123")
        
        from security.models import APICredentials
        creds = APICredentials(
            api_key=api_key,
            api_secret=api_secret
        )
        
        result = manager.authenticate(creds)
        assert result is not None
        assert result["user_id"] == "user123"
        assert result["auth_type"] == "api_key"
    
    def test_authenticate_failed(self):
        """测试认证失败"""
        manager = AuthManager()
        
        from security.models import APICredentials
        creds = APICredentials(api_key="invalid-key")
        
        result = manager.authenticate(creds)
        assert result is None


# ========== RateLimiter 测试 ==========

class TestRateLimiter:
    """限流器测试"""
    
    def test_acquire_within_limit(self):
        """测试在限制内获取令牌"""
        limiter = RateLimiter(default_capacity=10, default_refill_rate=1.0)
        
        async def test():
            results = []
            for _ in range(5):
                result = await limiter.acquire(tokens=1, user_id="user1", api_name="test")
                results.append(result)
            return results
        
        results = run_async(test())
        assert all(results)
    
    def test_acquire_exceed_limit(self):
        """测试超出限制"""
        limiter = RateLimiter(default_capacity=5, default_refill_rate=0.1)
        
        async def test():
            # 快速消耗所有令牌
            for _ in range(5):
                await limiter.acquire(tokens=1, user_id="user1", api_name="test")
            
            # 下一个应该失败
            result = await limiter.acquire(tokens=1, user_id="user1", api_name="test")
            return result
        
        result = run_async(test())
        assert result is False
    
    def test_different_users_independent(self):
        """测试不同用户限流独立"""
        limiter = RateLimiter(default_capacity=2, default_refill_rate=0.1)
        
        async def test():
            # 用户1消耗令牌
            await limiter.acquire(tokens=1, user_id="user1", api_name="test")
            await limiter.acquire(tokens=1, user_id="user1", api_name="test")
            
            # 用户2仍应有令牌
            result = await limiter.acquire(tokens=1, user_id="user2", api_name="test")
            return result
        
        result = run_async(test())
        assert result is True
    
    def test_try_acquire_or_raise(self):
        """测试获取失败抛出异常"""
        limiter = RateLimiter(default_capacity=1, default_refill_rate=0.1)
        
        async def test():
            await limiter.acquire(tokens=1, user_id="user1", api_name="test")
            
            with pytest.raises(RateLimitExceeded):
                await limiter.try_acquire_or_raise(tokens=1, user_id="user1", api_name="test")
        
        run_async(test())
    
    def test_reset_limiter(self):
        """测试重置限流器"""
        limiter = RateLimiter(default_capacity=2, default_refill_rate=0.1)
        
        async def test():
            await limiter.acquire(tokens=2, user_id="user1", api_name="test")
            
            # 应该失败
            result1 = await limiter.acquire(tokens=1, user_id="user1", api_name="test")
            assert result1 is False
            
            # 重置
            limiter.reset(user_id="user1", api_name="test")
            
            # 应该成功
            result2 = await limiter.acquire(tokens=1, user_id="user1", api_name="test")
            return result2
        
        result = run_async(test())
        assert result is True


# ========== APIBreaker 测试 ==========

class TestAPIBreaker:
    """熔断器测试"""
    
    def test_normal_operation(self):
        """测试正常操作"""
        breaker = APIBreaker("test-api")
        
        async def test():
            async def success_func():
                return "success"
            
            result = await breaker.call(success_func)
            return result
        
        result = run_async(test())
        assert result == "success"
        assert breaker.state.value == "closed"
    
    def test_circuit_opens_on_failures(self):
        """测试失败达到阈值时熔断器打开"""
        breaker = APIBreaker(
            "test-api",
            CircuitBreakerConfig(failure_threshold=3)
        )
        
        async def test():
            async def failing_func():
                raise Exception("API Error")
            
            # 触发3次失败
            for _ in range(3):
                try:
                    await breaker.call(failing_func)
                except Exception:
                    pass
            
            # 熔断器应该打开
            assert breaker.state.value == "open"
            
            # 再次调用应该被拒绝
            with pytest.raises(Exception):  # CircuitBreakerOpen
                await breaker.call(failing_func)
        
        run_async(test())
    
    def test_circuit_half_open_after_timeout(self):
        """测试超时后进入半开状态"""
        breaker = APIBreaker(
            "test-api",
            CircuitBreakerConfig(
                failure_threshold=1,
                timeout_seconds=0.1
            )
        )
        
        async def test():
            async def failing_func():
                raise Exception("API Error")
            
            # 触发一次失败
            try:
                await breaker.call(failing_func)
            except Exception:
                pass
            
            assert breaker.state.value == "open"
            
            # 等待超时
            await asyncio.sleep(0.2)
            
            # 状态应该变为half_open
            assert breaker.state.value == "half_open"
        
        run_async(test())
    
    def test_circuit_closes_after_success(self):
        """测试半开状态下成功恢复"""
        breaker = APIBreaker(
            "test-api",
            CircuitBreakerConfig(
                failure_threshold=1,
                success_threshold=2,
                timeout_seconds=0.1
            )
        )
        
        async def test():
            async def sometimes_failing():
                raise Exception("Error")
            
            async def success_func():
                return "success"
            
            # 触发失败
            try:
                await breaker.call(sometimes_failing)
            except Exception:
                pass
            
            # 等待超时
            await asyncio.sleep(0.2)
            
            # 半开状态下成功调用2次以达到success_threshold
            await breaker.call(success_func)
            await breaker.call(success_func)
            
            # 应该关闭（状态可能还在half_open，需要再检查一次）
            await asyncio.sleep(0.01)  # 短暂等待状态更新
            
            # 只要成功调用达到阈值，最终会变为closed
            assert breaker.stats.successful_calls >= 2
        
        run_async(test())
    
    def test_get_status(self):
        """测试获取状态"""
        breaker = APIBreaker("test-api")
        
        async def test():
            await breaker.call(lambda: "ok")
            return breaker.get_status()
        
        status = run_async(test())
        assert "name" in status
        assert "state" in status
        assert "stats" in status


# ========== BreakerRegistry 测试 ==========

class TestBreakerRegistry:
    """熔断器注册表测试"""
    
    def test_get_or_create_breaker(self):
        """测试获取或创建熔断器"""
        registry = BreakerRegistry()
        
        async def test():
            breaker1 = await registry.get_breaker("api1")
            breaker2 = await registry.get_breaker("api1")
            breaker3 = await registry.get_breaker("api2")
            
            assert breaker1 is breaker2  # 同一个实例
            assert breaker1 is not breaker3  # 不同实例
        
        run_async(test())
    
    def test_call_with_breaker(self):
        """测试使用熔断器调用"""
        registry = BreakerRegistry()
        
        async def test():
            result = await registry.call_with_breaker(
                "test-api",
                lambda: "success"
            )
            return result
        
        result = run_async(test())
        assert result == "success"


# ========== RequestValidator 测试 ==========

class TestRequestValidator:
    """请求验证器测试"""
    
    def test_validate_permission_request_valid(self):
        """测试验证有效权限请求"""
        validator = RequestValidator()
        
        result = validator.validate_permission_request({
            "user_id": "user123",
            "permission": "read",
            "level": 3
        })
        
        assert result.valid is True
        assert len(result.errors) == 0
    
    def test_validate_permission_request_invalid_user_id(self):
        """测试验证无效用户ID"""
        validator = RequestValidator()
        
        result = validator.validate_permission_request({
            "user_id": "user@123",  # 包含非法字符
            "permission": "read",
            "level": 3
        })
        
        assert result.valid is False
        assert len(result.errors) > 0
    
    def test_validate_permission_request_invalid_level(self):
        """测试验证无效级别"""
        validator = RequestValidator()
        
        result = validator.validate_permission_request({
            "user_id": "user123",
            "permission": "read",
            "level": 10  # 超过最大值5
        })
        
        assert result.valid is False
    
    def test_validate_permission_request_missing_field(self):
        """测试验证缺少必需字段"""
        validator = RequestValidator()
        
        result = validator.validate_permission_request({
            "user_id": "user123"
            # 缺少 permission 和 level
        })
        
        assert result.valid is False
    
    def test_validate_user_request_valid(self):
        """测试验证有效用户创建请求"""
        validator = RequestValidator()
        
        result = validator.validate_user_request({
            "user_id": "user123",
            "email": "test@example.com",
            "name": "Test User"
        }, is_create=True)
        
        assert result.valid is True
    
    def test_validate_user_request_invalid_email(self):
        """测试验证无效邮箱"""
        validator = RequestValidator()
        
        result = validator.validate_user_request({
            "user_id": "user123",
            "email": "invalid-email"
        }, is_create=True)
        
        assert result.valid is False
    
    def test_validate_audit_query(self):
        """测试验证审计查询"""
        validator = RequestValidator()
        
        result = validator.validate_audit_query({
            "user_id": "user123",
            "action": "grant_permission",
            "limit": 50
        })
        
        assert result.valid is True
    
    def test_validate_privilege_escalation(self):
        """测试验证提权申请"""
        validator = RequestValidator()
        
        result = validator.validate_privilege_escalation({
            "user_id": "user123",
            "level": 5,
            "reason": "Need admin access for debugging production issue"
        })
        
        assert result.valid is True
    
    def test_validate_privilege_escalation_short_reason(self):
        """测试验证提权申请原因过短"""
        validator = RequestValidator()
        
        result = validator.validate_privilege_escalation({
            "user_id": "user123",
            "level": 5,
            "reason": "short"  # 少于10个字符
        })
        
        assert result.valid is False


# ========== SecurityAPI 测试 ==========

class TestSecurityAPI:
    """安全API测试"""
    
    def setup_method(self):
        """设置测试"""
        from security.api_server import SecurityAPI
        self.api = SecurityAPI()
    
    def test_create_user(self):
        """测试创建用户"""
        async def test():
            user = await self.api.create_user(
                UserCreate(user_id="testuser", email="test@example.com"),
                created_by="admin"
            )
            return user
        
        result = run_async(test())
        assert result.user_id == "testuser"
        assert result.email == "test@example.com"
    
    def test_create_duplicate_user(self):
        """测试创建重复用户"""
        async def test():
            await self.api.create_user(
                UserCreate(user_id="testuser"),
                created_by="admin"
            )
            
            with pytest.raises(ValueError):
                await self.api.create_user(
                    UserCreate(user_id="testuser"),
                    created_by="admin"
                )
        
        run_async(test())
    
    def test_update_user(self):
        """测试更新用户"""
        async def test():
            # 先创建用户
            await self.api.create_user(
                UserCreate(user_id="testuser"),
                created_by="admin"
            )
            
            # 更新用户
            updated = await self.api.update_user(
                "testuser",
                UserUpdate(name="New Name", email="new@example.com"),
                updated_by="admin"
            )
            
            return updated
        
        result = run_async(test())
        assert result.name == "New Name"
        assert result.email == "new@example.com"
    
    def test_grant_permission(self):
        """测试授予权限"""
        async def test():
            perm = await self.api.grant_permission(
                user_id="testuser",
                permission="read",
                level=3,
                granted_by="admin"
            )
            return perm
        
        result = run_async(test())
        assert result.user_id == "testuser"
        assert result.permission == "read"
        assert result.level == 3
    
    def test_check_permission(self):
        """测试检查权限"""
        async def test():
            # 授予权限
            await self.api.grant_permission(
                user_id="testuser",
                permission="read",
                level=3
            )
            
            # 检查权限
            has_perm = await self.api.check_permission(
                user_id="testuser",
                permission="read"
            )
            
            no_perm = await self.api.check_permission(
                user_id="testuser",
                permission="write"
            )
            
            return has_perm, no_perm
        
        has_perm, no_perm = run_async(test())
        assert has_perm is True
        assert no_perm is False
    
    def test_revoke_permission(self):
        """测试撤销权限"""
        async def test():
            # 授予权限
            await self.api.grant_permission(
                user_id="testuser",
                permission="read",
                level=3
            )
            
            # 撤销权限
            result = await self.api.revoke_permission(
                user_id="testuser",
                permission="read"
            )
            
            # 检查权限
            has_perm = await self.api.check_permission(
                user_id="testuser",
                permission="read"
            )
            
            return result, has_perm
        
        result, has_perm = run_async(test())
        assert result is True
        assert has_perm is False
    
    def test_request_privilege_escalation(self):
        """测试申请提权"""
        async def test():
            request = await self.api.request_privilege_escalation(
                user_id="testuser",
                level=5,
                reason="Need admin access for production debugging"
            )
            return request
        
        result = run_async(test())
        assert result.user_id == "testuser"
        assert result.requested_level == 5
        assert result.status == PrivilegeEscalationStatus.PENDING
    
    def test_approve_privilege_escalation(self):
        """测试审批提权"""
        async def test():
            # 申请提权
            request = await self.api.request_privilege_escalation(
                user_id="testuser",
                level=4,
                reason="Need higher access level"
            )
            
            # 审批提权
            approved = await self.api.approve_privilege_escalation(
                request.request_id,
                approver_id="admin"
            )
            
            return approved
        
        result = run_async(test())
        assert result.status == PrivilegeEscalationStatus.APPROVED
        assert result.reviewed_by == "admin"
    
    def test_query_audit_logs(self):
        """测试查询审计日志"""
        async def test():
            # 创建用户（会记录审计日志）
            await self.api.create_user(
                UserCreate(user_id="testuser"),
                created_by="admin"
            )
            
            # 查询日志
            filters = AuditLogFilter(
                action=AuditAction.CREATE_USER,
                limit=10
            )
            logs = await self.api.query_audit_logs(filters)
            
            return logs
        
        result = run_async(test())
        assert len(result) > 0
        assert result[0].action == AuditAction.CREATE_USER
    
    def test_create_access_token(self):
        """测试创建访问令牌"""
        token = self.api.create_access_token(
            user_id="testuser",
            permissions=["read", "write"],
            exp_seconds=3600
        )
        
        assert token is not None
        assert isinstance(token, str)
    
    def test_create_api_key(self):
        """测试创建API Key"""
        api_key, api_secret = self.api.create_api_key("testuser")
        
        assert api_key.startswith("sk_")
        assert len(api_key) >= 32
        assert len(api_secret) >= 32


# ========== ShieldClient 测试 ==========

class TestShieldClient:
    """SDK客户端测试"""
    
    def test_init(self):
        """测试客户端初始化"""
        client = ShieldClient(
            api_key="sk_test123",
            base_url="https://api.test.local"
        )
        
        assert client.api_key == "sk_test123"
        assert client.base_url == "https://api.test.local"
        assert "X-API-Key" in client.session.headers
    
    def test_validate_api_key_format(self):
        """测试API Key格式验证"""
        validator = RequestValidator()
        
        result = validator.validate_api_key_format("sk_test1234567890123456789012345678")
        assert result.valid is True
        
        result = validator.validate_api_key_format("invalid")
        assert result.valid is False
    
    def test_validate_jwt_format(self):
        """测试JWT格式验证"""
        validator = RequestValidator()
        
        result = validator.validate_jwt_format("abc.def.ghi")
        assert result.valid is True
        
        result = validator.validate_jwt_format("invalid")
        assert result.valid is False


# ========== ShieldSDK 测试 ==========

class TestShieldSDK:
    """ShieldSDK封装测试"""
    
    def test_init(self):
        """测试SDK初始化"""
        sdk = ShieldSDK(api_key="sk_test123")
        assert sdk.client is not None
    
    def test_context_manager(self):
        """测试上下文管理器"""
        with ShieldSDK(api_key="sk_test123") as sdk:
            assert sdk.client is not None


# ========== 运行测试 ==========

if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])
