"""
安全模块数据模型
定义API使用的数据结构
"""

from __future__ import annotations

from datetime import datetime
from enum import Enum
from typing import Any, Optional

from pydantic import BaseModel, ConfigDict, Field


class PermissionLevel(str, Enum):
    """权限级别"""
    READ = "read"
    WRITE = "write"
    DELETE = "delete"
    ADMIN = "admin"


class PrivilegeEscalationStatus(str, Enum):
    """提权申请状态"""
    PENDING = "pending"
    APPROVED = "approved"
    REJECTED = "rejected"


class AuditAction(str, Enum):
    """审计动作类型"""
    GRANT_PERMISSION = "grant_permission"
    REVOKE_PERMISSION = "revoke_permission"
    CHECK_PERMISSION = "check_permission"
    REQUEST_ESCALATION = "request_escalation"
    APPROVE_ESCALATION = "approve_escalation"
    REJECT_ESCALATION = "reject_escalation"
    CREATE_USER = "create_user"
    UPDATE_USER = "update_user"
    DELETE_USER = "delete_user"
    LOGIN = "login"
    LOGOUT = "logout"


class UserBase(BaseModel):
    """用户基础模型"""
    user_id: str = Field(..., pattern=r"^[a-zA-Z0-9_]+$", max_length=64)
    email: Optional[str] = None
    name: Optional[str] = None
    enabled: bool = True


class UserCreate(UserBase):
    """创建用户请求"""
    pass


class UserUpdate(BaseModel):
    """更新用户请求"""
    email: Optional[str] = None
    name: Optional[str] = None
    enabled: Optional[bool] = None


class User(UserBase):
    """用户响应模型"""
    model_config = ConfigDict(extra="allow")
    
    created_at: datetime = Field(default_factory=datetime.utcnow)
    updated_at: datetime = Field(default_factory=datetime.utcnow)


class Permission(BaseModel):
    """权限模型"""
    user_id: str
    permission: str
    level: int = Field(..., ge=1, le=5)
    granted_at: datetime = Field(default_factory=datetime.utcnow)
    granted_by: Optional[str] = None


class PrivilegeEscalationRequest(BaseModel):
    """提权申请模型"""
    request_id: str
    user_id: str
    current_level: int
    requested_level: int
    reason: str
    status: PrivilegeEscalationStatus = PrivilegeEscalationStatus.PENDING
    created_at: datetime = Field(default_factory=datetime.utcnow)
    reviewed_by: Optional[str] = None
    reviewed_at: Optional[datetime] = None


class AuditLog(BaseModel):
    """审计日志模型"""
    model_config = ConfigDict(extra="allow")
    
    log_id: str
    user_id: Optional[str] = None
    action: AuditAction
    resource: Optional[str] = None
    details: dict[str, Any] = Field(default_factory=dict)
    ip_address: Optional[str] = None
    timestamp: datetime = Field(default_factory=datetime.utcnow)
    status: str = "success"  # success, failure


class AuditLogFilter(BaseModel):
    """审计日志查询过滤器"""
    user_id: Optional[str] = None
    action: Optional[AuditAction] = None
    start_time: Optional[datetime] = None
    end_time: Optional[datetime] = None
    resource: Optional[str] = None
    status: Optional[str] = None
    limit: int = Field(default=100, le=1000)
    offset: int = Field(default=0, ge=0)


class ValidationResult(BaseModel):
    """验证结果模型"""
    valid: bool
    errors: list[str] = Field(default_factory=list)


class APICredentials(BaseModel):
    """API认证凭证"""
    api_key: Optional[str] = None
    api_secret: Optional[str] = None
    jwt_token: Optional[str] = None
    jwt_algorithm: str = "HS256"


class TokenPayload(BaseModel):
    """JWT Token载荷"""
    user_id: str
    permissions: list[str] = Field(default_factory=list)
    exp: int
    iat: Optional[int] = None
