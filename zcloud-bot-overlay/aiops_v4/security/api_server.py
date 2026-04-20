"""
RESTful API服务器
提供安全模块的REST API接口
"""

from __future__ import annotations

import asyncio
import json
import uuid
from datetime import datetime
from typing import Any, Optional

from .api_auth import APIKeyAuth, AuthManager, JWTAuth
from .api_rate_limiter import (
    APIBreaker,
    BreakerRegistry,
    CircuitBreakerConfig,
    RateLimiter,
)
from .api_validator import RequestValidator, get_validator
from .models import (
    AuditAction,
    AuditLog,
    AuditLogFilter,
    Permission,
    PrivilegeEscalationRequest,
    PrivilegeEscalationStatus,
    User,
    UserCreate,
    UserUpdate,
)


class SecurityAPI:
    """安全模块REST API"""
    
    def __init__(
        self,
        jwt_secret: str = "default-jwt-secret",
        rate_limit_capacity: int = 100,
        rate_limit_refill_rate: float = 10.0
    ):
        """
        初始化API
        
        Args:
            jwt_secret: JWT密钥
            rate_limit_capacity: 默认限流容量
            rate_limit_refill_rate: 默认限流补充速率
        """
        self.auth_manager = AuthManager(jwt_secret)
        self.api_key_auth = APIKeyAuth()
        self.rate_limiter = RateLimiter(rate_limit_capacity, rate_limit_refill_rate)
        self.breaker_registry = BreakerRegistry()
        self.validator = get_validator()
        
        # 数据存储（内存）
        self._users: dict[str, User] = {}
        self._permissions: dict[str, dict[str, Permission]] = {}  # user_id -> {permission -> Permission}
        self._audit_logs: list[AuditLog] = []
        self._escalation_requests: dict[str, PrivilegeEscalationRequest] = {}
    
    # ========== 辅助方法 ==========
    
    def _generate_id(self) -> str:
        """生成唯一ID"""
        return str(uuid.uuid4())
    
    def _get_user_permissions(self, user_id: str) -> dict[str, Permission]:
        """获取用户的所有权限"""
        if user_id not in self._permissions:
            self._permissions[user_id] = {}
        return self._permissions[user_id]
    
    async def _check_rate_limit(
        self,
        user_id: Optional[str],
        api_name: str
    ) -> bool:
        """检查限流"""
        return await self.rate_limiter.acquire(
            tokens=1,
            user_id=user_id,
            api_name=api_name
        )
    
    def _log_audit(
        self,
        action: AuditAction,
        user_id: Optional[str] = None,
        resource: Optional[str] = None,
        details: Optional[dict] = None,
        status: str = "success",
        ip_address: Optional[str] = None
    ) -> None:
        """记录审计日志"""
        log = AuditLog(
            log_id=self._generate_id(),
            user_id=user_id,
            action=action,
            resource=resource,
            details=details or {},
            ip_address=ip_address,
            timestamp=datetime.utcnow(),
            status=status
        )
        self._audit_logs.append(log)
    
    async def _call_with_breaker(
        self,
        api_name: str,
        func,
        *args,
        **kwargs
    ):
        """使用熔断器调用"""
        return await self.breaker_registry.call_with_breaker(
            api_name,
            func,
            *args,
            config=CircuitBreakerConfig(
                failure_threshold=5,
                success_threshold=2,
                timeout_seconds=30.0
            ),
            **kwargs
        )
    
    # ========== 权限管理API ==========
    
    async def grant_permission(
        self,
        user_id: str,
        permission: str,
        level: int,
        granted_by: Optional[str] = None,
        ip_address: Optional[str] = None
    ) -> Permission:
        """
        授予权限
        
        Args:
            user_id: 用户ID
            permission: 权限类型
            level: 权限级别
            granted_by: 授权者ID
            ip_address: IP地址
            
        Returns:
            Permission对象
        """
        # 限流检查
        await self._check_rate_limit(user_id, "grant_permission")
        
        # 验证请求
        validation = self.validator.validate_permission_request({
            "user_id": user_id,
            "permission": permission,
            "level": level
        })
        if not validation.valid:
            self._log_audit(
                AuditAction.GRANT_PERMISSION,
                granted_by,
                f"user:{user_id}",
                {"error": validation.errors},
                "failure",
                ip_address
            )
            raise ValueError(f"Validation failed: {validation.errors}")
        
        # 创建权限
        perm = Permission(
            user_id=user_id,
            permission=permission,
            level=level,
            granted_at=datetime.utcnow(),
            granted_by=granted_by
        )
        
        permissions = self._get_user_permissions(user_id)
        permissions[permission] = perm
        
        # 记录审计日志
        self._log_audit(
            AuditAction.GRANT_PERMISSION,
            granted_by,
            f"user:{user_id}",
            {"permission": permission, "level": level},
            "success",
            ip_address
        )
        
        return perm
    
    async def revoke_permission(
        self,
        user_id: str,
        permission: str,
        revoked_by: Optional[str] = None,
        ip_address: Optional[str] = None
    ) -> bool:
        """
        撤销权限
        
        Args:
            user_id: 用户ID
            permission: 权限类型
            revoked_by: 撤销者ID
            ip_address: IP地址
            
        Returns:
            是否成功撤销
        """
        await self._check_rate_limit(user_id, "revoke_permission")
        
        permissions = self._get_user_permissions(user_id)
        if permission not in permissions:
            return False
        
        del permissions[permission]
        
        self._log_audit(
            AuditAction.REVOKE_PERMISSION,
            revoked_by,
            f"user:{user_id}",
            {"permission": permission},
            "success",
            ip_address
        )
        
        return True
    
    async def check_permission(
        self,
        user_id: str,
        permission: str,
        ip_address: Optional[str] = None
    ) -> bool:
        """
        检查权限
        
        Args:
            user_id: 用户ID
            permission: 权限类型
            ip_address: IP地址
            
        Returns:
            是否有权限
        """
        await self._check_rate_limit(user_id, "check_permission")
        
        permissions = self._get_user_permissions(user_id)
        has_permission = permission in permissions
        
        self._log_audit(
            AuditAction.CHECK_PERMISSION,
            user_id,
            f"permission:{permission}",
            {"has_permission": has_permission},
            "success",
            ip_address
        )
        
        return has_permission
    
    # ========== 提权申请API ==========
    
    async def request_privilege_escalation(
        self,
        user_id: str,
        level: int,
        reason: str,
        ip_address: Optional[str] = None
    ) -> PrivilegeEscalationRequest:
        """
        申请提权
        
        Args:
            user_id: 用户ID
            level: 请求的级别
            reason: 申请原因
            ip_address: IP地址
            
        Returns:
            提权申请对象
        """
        await self._check_rate_limit(user_id, "request_escalation")
        
        # 验证请求
        validation = self.validator.validate_privilege_escalation({
            "user_id": user_id,
            "level": level,
            "reason": reason
        })
        if not validation.valid:
            raise ValueError(f"Validation failed: {validation.errors}")
        
        # 获取当前级别
        current_level = 1
        permissions = self._get_user_permissions(user_id)
        if permissions:
            current_level = max(p.level for p in permissions.values())
        
        request = PrivilegeEscalationRequest(
            request_id=self._generate_id(),
            user_id=user_id,
            current_level=current_level,
            requested_level=level,
            reason=reason,
            status=PrivilegeEscalationStatus.PENDING,
            created_at=datetime.utcnow()
        )
        
        self._escalation_requests[request.request_id] = request
        
        self._log_audit(
            AuditAction.REQUEST_ESCALATION,
            user_id,
            f"request:{request.request_id}",
            {"requested_level": level, "reason": reason},
            "success",
            ip_address
        )
        
        return request
    
    async def approve_privilege_escalation(
        self,
        request_id: str,
        approver_id: str,
        ip_address: Optional[str] = None
    ) -> PrivilegeEscalationRequest:
        """
        审批提权
        
        Args:
            request_id: 申请ID
            approver_id: 审批者ID
            ip_address: IP地址
            
        Returns:
            更新后的申请对象
        """
        await self._check_rate_limit(approver_id, "approve_escalation")
        
        if request_id not in self._escalation_requests:
            raise ValueError(f"Request not found: {request_id}")
        
        request = self._escalation_requests[request_id]
        
        if request.status != PrivilegeEscalationStatus.PENDING:
            raise ValueError(f"Request is not pending: {request.status}")
        
        # 更新申请状态
        request.status = PrivilegeEscalationStatus.APPROVED
        request.reviewed_by = approver_id
        request.reviewed_at = datetime.utcnow()
        
        # 授予相应权限
        await self.grant_permission(
            request.user_id,
            "admin",
            request.requested_level,
            approver_id,
            ip_address
        )
        
        self._log_audit(
            AuditAction.APPROVE_ESCALATION,
            approver_id,
            f"request:{request_id}",
            {"approved_level": request.requested_level},
            "success",
            ip_address
        )
        
        return request
    
    async def reject_privilege_escalation(
        self,
        request_id: str,
        rejector_id: str,
        reason: Optional[str] = None,
        ip_address: Optional[str] = None
    ) -> PrivilegeEscalationRequest:
        """
        拒绝提权
        
        Args:
            request_id: 申请ID
            rejector_id: 拒绝者ID
            reason: 拒绝原因
            ip_address: IP地址
            
        Returns:
            更新后的申请对象
        """
        await self._check_rate_limit(rejector_id, "reject_escalation")
        
        if request_id not in self._escalation_requests:
            raise ValueError(f"Request not found: {request_id}")
        
        request = self._escalation_requests[request_id]
        request.status = PrivilegeEscalationStatus.REJECTED
        request.reviewed_by = rejector_id
        request.reviewed_at = datetime.utcnow()
        
        self._log_audit(
            AuditAction.REJECT_ESCALATION,
            rejector_id,
            f"request:{request_id}",
            {"reason": reason},
            "success",
            ip_address
        )
        
        return request
    
    # ========== 审计日志API ==========
    
    async def query_audit_logs(
        self,
        filters: AuditLogFilter,
        ip_address: Optional[str] = None
    ) -> list[AuditLog]:
        """
        查询审计日志
        
        Args:
            filters: 查询过滤器
            ip_address: IP地址
            
        Returns:
            审计日志列表
        """
        await self._check_rate_limit(None, "query_audit_logs")
        
        results = self._audit_logs
        
        # 应用过滤器
        if filters.user_id:
            results = [log for log in results if log.user_id == filters.user_id]
        if filters.action:
            results = [log for log in results if log.action == filters.action]
        if filters.start_time:
            results = [log for log in results if log.timestamp >= filters.start_time]
        if filters.end_time:
            results = [log for log in results if log.timestamp <= filters.end_time]
        if filters.resource:
            results = [log for log in results if log.resource == filters.resource]
        if filters.status:
            results = [log for log in results if log.status == filters.status]
        
        # 排序和分页
        results = sorted(results, key=lambda x: x.timestamp, reverse=True)
        results = results[filters.offset:filters.offset + filters.limit]
        
        return results
    
    async def export_audit_logs(
        self,
        filters: AuditLogFilter,
        format: str = "json",
        ip_address: Optional[str] = None
    ) -> bytes:
        """
        导出审计日志
        
        Args:
            filters: 查询过滤器
            format: 导出格式（json/csv）
            ip_address: IP地址
            
        Returns:
            导出数据
        """
        logs = await self.query_audit_logs(filters, ip_address)
        
        if format == "json":
            return json.dumps([log.model_dump() for log in logs], default=str).encode()
        elif format == "csv":
            lines = ["log_id,user_id,action,resource,status,timestamp"]
            for log in logs:
                lines.append(
                    f"{log.log_id},{log.user_id or ''},{log.action.value},"
                    f"{log.resource or ''},{log.status},{log.timestamp.isoformat()}"
                )
            return "\n".join(lines).encode()
        else:
            raise ValueError(f"Unsupported format: {format}")
    
    # ========== 用户管理API ==========
    
    async def create_user(
        self,
        user: UserCreate,
        created_by: Optional[str] = None,
        ip_address: Optional[str] = None
    ) -> User:
        """
        创建用户
        
        Args:
            user: 用户创建请求
            created_by: 创建者ID
            ip_address: IP地址
            
        Returns:
            创建的用户对象
        """
        await self._check_rate_limit(None, "create_user")
        
        # 验证请求
        validation = self.validator.validate_user_request(user.model_dump(), is_create=True)
        if not validation.valid:
            raise ValueError(f"Validation failed: {validation.errors}")
        
        if user.user_id in self._users:
            raise ValueError(f"User already exists: {user.user_id}")
        
        new_user = User(
            user_id=user.user_id,
            email=user.email,
            name=user.name,
            enabled=user.enabled,
            created_at=datetime.utcnow(),
            updated_at=datetime.utcnow()
        )
        
        self._users[user.user_id] = new_user
        
        self._log_audit(
            AuditAction.CREATE_USER,
            created_by,
            f"user:{user.user_id}",
            {"email": user.email, "name": user.name},
            "success",
            ip_address
        )
        
        return new_user
    
    async def update_user(
        self,
        user_id: str,
        user: UserUpdate,
        updated_by: Optional[str] = None,
        ip_address: Optional[str] = None
    ) -> User:
        """
        更新用户
        
        Args:
            user_id: 用户ID
            user: 用户更新请求
            updated_by: 更新者ID
            ip_address: IP地址
            
        Returns:
            更新后的用户对象
        """
        await self._check_rate_limit(user_id, "update_user")
        
        if user_id not in self._users:
            raise ValueError(f"User not found: {user_id}")
        
        existing_user = self._users[user_id]
        
        # 更新字段
        update_data = user.model_dump(exclude_unset=True)
        for field, value in update_data.items():
            setattr(existing_user, field, value)
        
        existing_user.updated_at = datetime.utcnow()
        
        self._log_audit(
            AuditAction.UPDATE_USER,
            updated_by,
            f"user:{user_id}",
            update_data,
            "success",
            ip_address
        )
        
        return existing_user
    
    async def get_user(
        self,
        user_id: str,
        ip_address: Optional[str] = None
    ) -> Optional[User]:
        """
        获取用户
        
        Args:
            user_id: 用户ID
            ip_address: IP地址
            
        Returns:
            用户对象，不存在则返回None
        """
        return self._users.get(user_id)
    
    async def delete_user(
        self,
        user_id: str,
        deleted_by: Optional[str] = None,
        ip_address: Optional[str] = None
    ) -> bool:
        """
        删除用户
        
        Args:
            user_id: 用户ID
            deleted_by: 删除者ID
            ip_address: IP地址
            
        Returns:
            是否成功删除
        """
        await self._check_rate_limit(user_id, "delete_user")
        
        if user_id not in self._users:
            return False
        
        del self._users[user_id]
        
        # 删除相关权限
        if user_id in self._permissions:
            del self._permissions[user_id]
        
        self._log_audit(
            AuditAction.DELETE_USER,
            deleted_by,
            f"user:{user_id}",
            {},
            "success",
            ip_address
        )
        
        return True
    
    # ========== 认证API ==========
    
    def create_access_token(
        self,
        user_id: str,
        permissions: list[str],
        exp_seconds: int = 3600
    ) -> str:
        """
        创建访问令牌
        
        Args:
            user_id: 用户ID
            permissions: 权限列表
            exp_seconds: 过期时间（秒）
            
        Returns:
            JWT Token
        """
        return self.auth_manager.create_access_token(
            user_id,
            permissions,
            exp_seconds
        )
    
    def create_api_key(
        self,
        user_id: str,
        api_secret: Optional[str] = None
    ) -> tuple[str, str]:
        """
        创建API Key
        
        Args:
            user_id: 用户ID
            api_secret: API Secret（可选）
            
        Returns:
            (api_key, api_secret)
        """
        return self.api_key_auth.generate_api_key(user_id, api_secret)
    
    # ========== 状态API ==========
    
    def get_rate_limit_status(
        self,
        user_id: Optional[str] = None,
        api_name: str = "default"
    ) -> dict:
        """获取限流状态"""
        return self.rate_limiter.get_status(user_id, api_name)
    
    def get_breaker_status(self, name: Optional[str] = None) -> dict:
        """获取熔断器状态"""
        if name:
            breakers = self.breaker_registry.get_all_status()
            return breakers.get(name, {"error": "Breaker not found"})
        return self.breaker_registry.get_all_status()
    
    # ========== 重置API ==========
    
    def reset_rate_limiter(
        self,
        user_id: Optional[str] = None,
        api_name: Optional[str] = None
    ) -> None:
        """重置限流器"""
        self.rate_limiter.reset(user_id, api_name)
    
    def reset_breakers(self) -> None:
        """重置所有熔断器"""
        self.breaker_registry.reset_all()
