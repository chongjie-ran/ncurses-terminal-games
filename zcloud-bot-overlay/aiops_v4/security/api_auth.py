"""
API认证模块
支持API Key认证和JWT Token认证
"""

from __future__ import annotations

import hashlib
import hmac
import secrets
import time
from typing import Optional

import jwt

from .models import APICredentials, TokenPayload


class JWTAuth:
    """JWT认证管理器"""
    
    def __init__(self, secret_key: str = "default-secret-key-change-in-production", algorithm: str = "HS256"):
        self.secret_key = secret_key
        self.algorithm = algorithm
    
    def create_token(
        self,
        user_id: str,
        permissions: list[str],
        exp_seconds: int = 3600
    ) -> str:
        """
        创建JWT Token
        
        Args:
            user_id: 用户ID
            permissions: 用户权限列表
            exp_seconds: 过期时间（秒）
            
        Returns:
            JWT Token字符串
        """
        now = int(time.time())
        payload = {
            "user_id": user_id,
            "permissions": permissions,
            "exp": now + exp_seconds,
            "iat": now
        }
        return jwt.encode(payload, self.secret_key, algorithm=self.algorithm)
    
    def verify_token(self, token: str) -> Optional[dict]:
        """
        验证JWT Token
        
        Args:
            token: JWT Token字符串
            
        Returns:
            解码后的payload字典，验证失败返回None
        """
        try:
            payload = jwt.decode(token, self.secret_key, algorithms=[self.algorithm])
            return payload
        except jwt.ExpiredSignatureError:
            # Token已过期
            return None
        except jwt.InvalidTokenError:
            # Token无效
            return None
    
    def refresh_token(self, token: str, exp_seconds: int = 3600) -> Optional[str]:
        """
        刷新JWT Token
        
        Args:
            token: 原始Token
            exp_seconds: 新Token过期时间（秒）
            
        Returns:
            新的JWT Token，验证失败返回None
        """
        payload = self.verify_token(token)
        if payload is None:
            return None
        return self.create_token(
            user_id=payload["user_id"],
            permissions=payload.get("permissions", []),
            exp_seconds=exp_seconds
        )


class APIKeyAuth:
    """API Key认证管理器"""
    
    def __init__(self):
        self._api_keys: dict[str, dict] = {}  # api_key -> {api_secret, user_id, created_at}
    
    def generate_api_key(self, user_id: str, api_secret: Optional[str] = None) -> tuple[str, str]:
        """
        生成API Key
        
        Args:
            user_id: 用户ID
            api_secret: API Secret（可选，自动生成）
            
        Returns:
            (api_key, api_secret)元组
        """
        api_key = f"sk_{secrets.token_hex(16)}"
        if api_secret is None:
            api_secret = secrets.token_hex(32)
        
        self._api_keys[api_key] = {
            "api_secret": api_secret,
            "user_id": user_id,
            "created_at": int(time.time())
        }
        return api_key, api_secret
    
    def verify_api_key(self, api_key: str, api_secret: Optional[str] = None) -> Optional[str]:
        """
        验证API Key
        
        Args:
            api_key: API Key
            api_secret: API Secret（可选）
            
        Returns:
            用户ID，验证失败返回None
        """
        if api_key not in self._api_keys:
            return None
        
        key_info = self._api_keys[api_key]
        if api_secret is not None:
            if key_info["api_secret"] != api_secret:
                return None
        return key_info["user_id"]
    
    def revoke_api_key(self, api_key: str) -> bool:
        """
        撤销API Key
        
        Args:
            api_key: API Key
            
        Returns:
            是否成功撤销
        """
        if api_key in self._api_keys:
            del self._api_keys[api_key]
            return True
        return False
    
    def get_user_api_keys(self, user_id: str) -> list[str]:
        """
        获取用户的所有API Key
        
        Args:
            user_id: 用户ID
            
        Returns:
            API Key列表
        """
        return [
            key for key, info in self._api_keys.items()
            if info["user_id"] == user_id
        ]


class SignatureValidator:
    """请求签名验证器"""
    
    @staticmethod
    def generate_signature(
        api_secret: str,
        method: str,
        path: str,
        timestamp: str,
        body: Optional[str] = None
    ) -> str:
        """
        生成请求签名
        
        Args:
            api_secret: API Secret
            method: HTTP方法
            path: 请求路径
            timestamp: 时间戳
            body: 请求体（可选）
            
        Returns:
            签名字符串
        """
        message = f"{method.upper()}{path}{timestamp}"
        if body:
            message += body
        
        signature = hmac.new(
            api_secret.encode(),
            message.encode(),
            hashlib.sha256
        ).hexdigest()
        return signature
    
    @staticmethod
    def verify_signature(
        api_secret: str,
        method: str,
        path: str,
        timestamp: str,
        body: Optional[str],
        signature: str,
        tolerance_seconds: int = 300
    ) -> bool:
        """
        验证请求签名
        
        Args:
            api_secret: API Secret
            method: HTTP方法
            path: 请求路径
            timestamp: 时间戳
            body: 请求体
            signature: 签名
            tolerance_seconds: 时间容差（秒）
            
        Returns:
            是否验证通过
        """
        # 检查时间戳是否在容差范围内
        try:
            ts = int(timestamp)
            now = int(time.time())
            if abs(now - ts) > tolerance_seconds:
                return False
        except ValueError:
            return False
        
        # 计算期望的签名
        expected = SignatureValidator.generate_signature(
            api_secret, method, path, timestamp, body
        )
        
        # 使用constant-time比较防止时序攻击
        return hmac.compare_digest(expected, signature)


class AuthManager:
    """统一认证管理器"""
    
    def __init__(self, jwt_secret: str = "default-jwt-secret"):
        self.jwt_auth = JWTAuth(secret_key=jwt_secret)
        self.api_key_auth = APIKeyAuth()
    
    def authenticate(
        self,
        credentials: APICredentials
    ) -> Optional[dict]:
        """
        统一认证入口
        
        Args:
            credentials: 认证凭证
            
        Returns:
            认证结果包含user_id和permissions，失败返回None
        """
        # 优先尝试JWT认证
        if credentials.jwt_token:
            payload = self.jwt_auth.verify_token(credentials.jwt_token)
            if payload:
                return {
                    "user_id": payload["user_id"],
                    "permissions": payload.get("permissions", []),
                    "auth_type": "jwt"
                }
        
        # 尝试API Key认证
        if credentials.api_key:
            user_id = self.api_key_auth.verify_api_key(
                credentials.api_key,
                credentials.api_secret
            )
            if user_id:
                return {
                    "user_id": user_id,
                    "permissions": [],  # API Key认证不包含权限信息
                    "auth_type": "api_key"
                }
        
        return None
    
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
            exp_seconds: 过期时间
            
        Returns:
            JWT Token
        """
        return self.jwt_auth.create_token(user_id, permissions, exp_seconds)
