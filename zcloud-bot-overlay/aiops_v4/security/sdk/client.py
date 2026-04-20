"""
Shield Python SDK客户端
封装对Shield安全模块REST API的调用
"""

from __future__ import annotations

import json
import time
from typing import Any, Optional

import requests


class ShieldClient:
    """Shield Python SDK客户端"""
    
    def __init__(
        self,
        api_key: str,
        base_url: str = "https://api.shield.local",
        api_secret: Optional[str] = None,
        timeout: int = 30
    ):
        """
        初始化SDK客户端
        
        Args:
            api_key: API Key
            base_url: API基础URL
            api_secret: API Secret（用于签名）
            timeout: 请求超时时间（秒）
        """
        self.api_key = api_key
        self.api_secret = api_secret
        self.base_url = base_url.rstrip("/")
        self.timeout = timeout
        self.session = requests.Session()
        self.session.headers.update({
            "X-API-Key": api_key,
            "Content-Type": "application/json",
            "User-Agent": "Shield-Python-SDK/1.0.0"
        })
    
    def _make_request(
        self,
        method: str,
        endpoint: str,
        data: Optional[dict] = None,
        params: Optional[dict] = None
    ) -> dict:
        """
        发送HTTP请求
        
        Args:
            method: HTTP方法
            endpoint: API端点
            data: 请求数据
            params: 查询参数
            
        Returns:
            响应数据
        """
        url = f"{self.base_url}/{endpoint.lstrip('/')}"
        
        try:
            response = self.session.request(
                method=method,
                url=url,
                json=data,
                params=params,
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json() if response.content else {}
        except requests.exceptions.RequestException as e:
            raise ShieldAPIError(f"Request failed: {str(e)}") from e
    
    # ========== 权限管理 ==========
    
    def grant_permission(
        self,
        user_id: str,
        permission: str,
        level: int,
        granted_by: Optional[str] = None
    ) -> dict:
        """
        授予权限
        
        Args:
            user_id: 用户ID
            permission: 权限类型（read/write/delete/admin）
            level: 权限级别（1-5）
            granted_by: 授权者ID
            
        Returns:
            权限对象
        """
        return self._make_request(
            "POST",
            "/permissions/grant",
            data={
                "user_id": user_id,
                "permission": permission,
                "level": level,
                "granted_by": granted_by
            }
        )
    
    def revoke_permission(
        self,
        user_id: str,
        permission: str,
        revoked_by: Optional[str] = None
    ) -> dict:
        """
        撤销权限
        
        Args:
            user_id: 用户ID
            permission: 权限类型
            revoked_by: 撤销者ID
            
        Returns:
            结果对象
        """
        return self._make_request(
            "POST",
            "/permissions/revoke",
            data={
                "user_id": user_id,
                "permission": permission,
                "revoked_by": revoked_by
            }
        )
    
    def check_permission(
        self,
        user_id: str,
        permission: str
    ) -> bool:
        """
        检查权限
        
        Args:
            user_id: 用户ID
            permission: 权限类型
            
        Returns:
            是否有权限
        """
        response = self._make_request(
            "GET",
            "/permissions/check",
            params={"user_id": user_id, "permission": permission}
        )
        return response.get("has_permission", False)
    
    # ========== 提权申请 ==========
    
    def request_privilege_escalation(
        self,
        user_id: str,
        level: int,
        reason: str
    ) -> dict:
        """
        申请提权
        
        Args:
            user_id: 用户ID
            level: 请求的级别
            reason: 申请原因
            
        Returns:
            提权申请对象
        """
        return self._make_request(
            "POST",
            "/escalation/request",
            data={
                "user_id": user_id,
                "level": level,
                "reason": reason
            }
        )
    
    def approve_privilege_escalation(
        self,
        request_id: str,
        approver_id: str
    ) -> dict:
        """
        审批提权
        
        Args:
            request_id: 申请ID
            approver_id: 审批者ID
            
        Returns:
            更新后的申请对象
        """
        return self._make_request(
            "POST",
            "/escalation/approve",
            data={
                "request_id": request_id,
                "approver_id": approver_id
            }
        )
    
    def reject_privilege_escalation(
        self,
        request_id: str,
        rejector_id: str,
        reason: Optional[str] = None
    ) -> dict:
        """
        拒绝提权
        
        Args:
            request_id: 申请ID
            rejector_id: 拒绝者ID
            reason: 拒绝原因
            
        Returns:
            更新后的申请对象
        """
        return self._make_request(
            "POST",
            "/escalation/reject",
            data={
                "request_id": request_id,
                "rejector_id": rejector_id,
                "reason": reason
            }
        )
    
    # ========== 审计日志 ==========
    
    def query_logs(
        self,
        user_id: Optional[str] = None,
        start_time: Optional[str] = None,
        end_time: Optional[str] = None,
        action: Optional[str] = None,
        resource: Optional[str] = None,
        status: Optional[str] = None,
        limit: int = 100,
        offset: int = 0
    ) -> list[dict]:
        """
        查询审计日志
        
        Args:
            user_id: 用户ID
            start_time: 开始时间（ISO格式）
            end_time: 结束时间（ISO格式）
            action: 动作类型
            resource: 资源
            status: 状态（success/failure）
            limit: 返回数量限制
            offset: 偏移量
            
        Returns:
            审计日志列表
        """
        params = {
            "limit": limit,
            "offset": offset
        }
        if user_id:
            params["user_id"] = user_id
        if start_time:
            params["start_time"] = start_time
        if end_time:
            params["end_time"] = end_time
        if action:
            params["action"] = action
        if resource:
            params["resource"] = resource
        if status:
            params["status"] = status
        
        response = self._make_request("GET", "/audit/logs", params=params)
        return response.get("logs", [])
    
    def export_logs(
        self,
        start_time: str,
        end_time: str,
        format: str = "json",
        **kwargs
    ) -> bytes:
        """
        导出审计日志
        
        Args:
            start_time: 开始时间
            end_time: 结束时间
            format: 导出格式（json/csv）
            **kwargs: 其他查询参数
            
        Returns:
            导出数据（字节）
        """
        url = f"{self.base_url}/audit/export"
        params = {
            "start_time": start_time,
            "end_time": end_time,
            "format": format,
            **kwargs
        }
        
        try:
            response = self.session.get(
                url,
                params=params,
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.content
        except requests.exceptions.RequestException as e:
            raise ShieldAPIError(f"Export failed: {str(e)}") from e
    
    # ========== 用户管理 ==========
    
    def create_user(
        self,
        user_id: str,
        email: Optional[str] = None,
        name: Optional[str] = None,
        enabled: bool = True,
        created_by: Optional[str] = None
    ) -> dict:
        """
        创建用户
        
        Args:
            user_id: 用户ID
            email: 邮箱
            name: 姓名
            enabled: 是否启用
            created_by: 创建者ID
            
        Returns:
            用户对象
        """
        return self._make_request(
            "POST",
            "/users",
            data={
                "user_id": user_id,
                "email": email,
                "name": name,
                "enabled": enabled,
                "created_by": created_by
            }
        )
    
    def update_user(
        self,
        user_id: str,
        email: Optional[str] = None,
        name: Optional[str] = None,
        enabled: Optional[bool] = None,
        updated_by: Optional[str] = None
    ) -> dict:
        """
        更新用户
        
        Args:
            user_id: 用户ID
            email: 邮箱
            name: 姓名
            enabled: 是否启用
            updated_by: 更新者ID
            
        Returns:
            更新后的用户对象
        """
        data = {"user_id": user_id}
        if email is not None:
            data["email"] = email
        if name is not None:
            data["name"] = name
        if enabled is not None:
            data["enabled"] = enabled
        if updated_by is not None:
            data["updated_by"] = updated_by
        
        return self._make_request("PUT", f"/users/{user_id}", data=data)
    
    def get_user(self, user_id: str) -> Optional[dict]:
        """
        获取用户
        
        Args:
            user_id: 用户ID
            
        Returns:
            用户对象
        """
        return self._make_request("GET", f"/users/{user_id}")
    
    def delete_user(
        self,
        user_id: str,
        deleted_by: Optional[str] = None
    ) -> bool:
        """
        删除用户
        
        Args:
            user_id: 用户ID
            deleted_by: 删除者ID
            
        Returns:
            是否成功
        """
        response = self._make_request(
            "DELETE",
            f"/users/{user_id}",
            data={"deleted_by": deleted_by} if deleted_by else {}
        )
        return response.get("success", False)
    
    # ========== 认证 ==========
    
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
        response = self._make_request(
            "POST",
            "/auth/token",
            data={
                "user_id": user_id,
                "permissions": permissions,
                "exp_seconds": exp_seconds
            }
        )
        return response.get("token", "")
    
    # ========== 状态查询 ==========
    
    def get_rate_limit_status(
        self,
        user_id: Optional[str] = None,
        api_name: str = "default"
    ) -> dict:
        """获取限流状态"""
        params = {"api_name": api_name}
        if user_id:
            params["user_id"] = user_id
        return self._make_request("GET", "/status/rate-limit", params=params)
    
    def get_breaker_status(self, name: Optional[str] = None) -> dict:
        """获取熔断器状态"""
        params = {}
        if name:
            params["name"] = name
        return self._make_request("GET", "/status/breaker", params=params)
    
    def close(self) -> None:
        """关闭客户端"""
        self.session.close()


class ShieldAPIError(Exception):
    """Shield API异常"""
    pass


# ========== 便捷函数 ==========

def create_client(
    api_key: str,
    base_url: str = "https://api.shield.local",
    **kwargs
) -> ShieldClient:
    """
    创建Shield客户端
    
    Args:
        api_key: API Key
        base_url: API基础URL
        **kwargs: 其他参数
        
    Returns:
        ShieldClient实例
    """
    return ShieldClient(api_key, base_url, **kwargs)


class ShieldSDK:
    """
    Shield SDK封装类
    提供更简洁的API调用方式
    """
    
    def __init__(
        self,
        api_key: str,
        base_url: str = "https://api.shield.local",
        **kwargs
    ):
        """
        初始化SDK
        
        Args:
            api_key: API Key
            base_url: API基础URL
            **kwargs: 其他参数
        """
        self.client = ShieldClient(api_key, base_url, **kwargs)
    
    # 权限管理
    def grant(self, user_id: str, permission: str, level: int) -> dict:
        """授予权限"""
        return self.client.grant_permission(user_id, permission, level)
    
    def revoke(self, user_id: str, permission: str) -> dict:
        """撤销权限"""
        return self.client.revoke_permission(user_id, permission)
    
    def check(self, user_id: str, permission: str) -> bool:
        """检查权限"""
        return self.client.check_permission(user_id, permission)
    
    # 审计日志
    def logs(self, **kwargs) -> list[dict]:
        """查询日志"""
        return self.client.query_logs(**kwargs)
    
    def export_logs(self, start: str, end: str, fmt: str = "json") -> bytes:
        """导出日志"""
        return self.client.export_logs(start, end, fmt)
    
    # 用户管理
    def create_user(self, user_id: str, **kwargs) -> dict:
        """创建用户"""
        return self.client.create_user(user_id, **kwargs)
    
    def update_user(self, user_id: str, **kwargs) -> dict:
        """更新用户"""
        return self.client.update_user(user_id, **kwargs)
    
    def get_user(self, user_id: str) -> Optional[dict]:
        """获取用户"""
        return self.client.get_user(user_id)
    
    def delete_user(self, user_id: str) -> bool:
        """删除用户"""
        return self.client.delete_user(user_id)
    
    def close(self) -> None:
        """关闭"""
        self.client.close()
    
    def __enter__(self) -> "ShieldSDK":
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.close()
