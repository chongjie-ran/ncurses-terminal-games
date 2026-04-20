"""
请求验证模块
参数校验和JSON Schema验证
"""

from __future__ import annotations

import re
from typing import Any, Optional

from .models import (
    AuditAction,
    AuditLogFilter,
    PrivilegeEscalationStatus,
    UserCreate,
    UserUpdate,
    ValidationResult,
)


# JSON Schema定义
PERMISSION_REQUEST_SCHEMA = {
    "type": "object",
    "required": ["user_id", "permission", "level"],
    "properties": {
        "user_id": {
            "type": "string",
            "pattern": "^[a-zA-Z0-9_]+$",
            "minLength": 1,
            "maxLength": 64
        },
        "permission": {
            "type": "string",
            "enum": ["read", "write", "delete", "admin"]
        },
        "level": {
            "type": "integer",
            "minimum": 1,
            "maximum": 5
        }
    }
}

USER_CREATE_SCHEMA = {
    "type": "object",
    "required": ["user_id"],
    "properties": {
        "user_id": {
            "type": "string",
            "pattern": "^[a-zA-Z0-9_]+$",
            "minLength": 1,
            "maxLength": 64
        },
        "email": {
            "type": "string",
            "format": "email",
            "maxLength": 255
        },
        "name": {
            "type": "string",
            "maxLength": 128
        },
        "enabled": {
            "type": "boolean"
        }
    }
}

USER_UPDATE_SCHEMA = {
    "type": "object",
    "properties": {
        "email": {
            "type": "string",
            "format": "email",
            "maxLength": 255
        },
        "name": {
            "type": "string",
            "maxLength": 128
        },
        "enabled": {
            "type": "boolean"
        }
    }
}

AUDIT_QUERY_SCHEMA = {
    "type": "object",
    "properties": {
        "user_id": {
            "type": "string",
            "pattern": "^[a-zA-Z0-9_]*$",
            "maxLength": 64
        },
        "action": {
            "type": "string",
            "enum": [a.value for a in AuditAction]
        },
        "start_time": {
            "type": "string",
            "format": "datetime"
        },
        "end_time": {
            "type": "string",
            "format": "datetime"
        },
        "resource": {
            "type": "string",
            "maxLength": 256
        },
        "status": {
            "type": "string",
            "enum": ["success", "failure"]
        },
        "limit": {
            "type": "integer",
            "minimum": 1,
            "maximum": 1000
        },
        "offset": {
            "type": "integer",
            "minimum": 0
        }
    }
}

PRIVILEGE_ESCALATION_SCHEMA = {
    "type": "object",
    "required": ["user_id", "level", "reason"],
    "properties": {
        "user_id": {
            "type": "string",
            "pattern": "^[a-zA-Z0-9_]+$",
            "maxLength": 64
        },
        "level": {
            "type": "integer",
            "minimum": 1,
            "maximum": 5
        },
        "reason": {
            "type": "string",
            "minLength": 10,
            "maxLength": 1000
        }
    }
}


class RequestValidator:
    """请求验证器"""
    
    def __init__(self):
        self._schemas = {
            "permission": PERMISSION_REQUEST_SCHEMA,
            "user_create": USER_CREATE_SCHEMA,
            "user_update": USER_UPDATE_SCHEMA,
            "audit_query": AUDIT_QUERY_SCHEMA,
            "privilege_escalation": PRIVILEGE_ESCALATION_SCHEMA
        }
    
    def _validate_type(self, value: Any, expected_type: str) -> Optional[str]:
        """验证类型"""
        type_mapping = {
            "string": str,
            "integer": int,
            "boolean": bool,
            "number": (int, float),
            "object": dict,
            "array": list
        }
        
        if expected_type == "integer":
            # 特殊处理integer，因为它可以是bool
            if isinstance(value, bool):
                return f"Expected {expected_type}, got boolean"
            if not isinstance(value, int):
                return f"Expected {expected_type}, got {type(value).__name__}"
        elif expected_type not in type_mapping:
            return f"Unknown type: {expected_type}"
        elif not isinstance(value, type_mapping[expected_type]):
            return f"Expected {expected_type}, got {type(value).__name__}"
        
        return None
    
    def _validate_string_format(
        self,
        value: str,
        schema: dict
    ) -> Optional[str]:
        """验证字符串格式"""
        # 长度检查
        if "minLength" in schema:
            if len(value) < schema["minLength"]:
                return f"String length must be at least {schema['minLength']}"
        if "maxLength" in schema:
            if len(value) > schema["maxLength"]:
                return f"String length must be at most {schema['maxLength']}"
        
        # 模式检查
        if "pattern" in schema:
            pattern = schema["pattern"]
            if not re.match(pattern, value):
                return f"String does not match pattern: {pattern}"
        
        # 枚举检查
        if "enum" in schema:
            if value not in schema["enum"]:
                return f"Value must be one of: {schema['enum']}"
        
        # Email格式检查
        if schema.get("format") == "email":
            email_pattern = r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$"
            if not re.match(email_pattern, value):
                return "Invalid email format"
        
        return None
    
    def _validate_number_range(
        self,
        value: int,
        schema: dict
    ) -> Optional[str]:
        """验证数字范围"""
        if "minimum" in schema:
            if value < schema["minimum"]:
                return f"Value must be at least {schema['minimum']}"
        if "maximum" in schema:
            if value > schema["maximum"]:
                return f"Value must be at most {schema['maximum']}"
        
        return None
    
    def _validate_object(
        self,
        data: dict,
        schema: dict
    ) -> list[str]:
        """验证对象"""
        errors = []
        
        # 检查必需字段
        if "required" in schema:
            for field_name in schema["required"]:
                if field_name not in data:
                    errors.append(f"Missing required field: {field_name}")
        
        # 验证每个字段
        properties = schema.get("properties", {})
        for field_name, field_value in data.items():
            if field_name not in properties:
                continue
            
            # 跳过None值（可选字段）
            if field_value is None:
                continue
            
            field_schema = properties[field_name]
            field_type = field_schema.get("type", "string")
            
            # 类型验证
            type_error = self._validate_type(field_value, field_type)
            if type_error:
                errors.append(f"{field_name}: {type_error}")
                continue
            
            # 字符串特定验证
            if field_type == "string":
                str_error = self._validate_string_format(field_value, field_schema)
                if str_error:
                    errors.append(f"{field_name}: {str_error}")
            
            # 数字范围验证
            elif field_type in ("integer", "number"):
                num_error = self._validate_number_range(field_value, field_schema)
                if num_error:
                    errors.append(f"{field_name}: {num_error}")
        
        return errors
    
    def _validate(self, data: dict, schema_name: str) -> ValidationResult:
        """
        根据schema验证数据
        
        Args:
            data: 待验证的数据
            schema_name: schema名称
            
        Returns:
            验证结果
        """
        if schema_name not in self._schemas:
            return ValidationResult(
                valid=False,
                errors=[f"Unknown schema: {schema_name}"]
            )
        
        if not isinstance(data, dict):
            return ValidationResult(
                valid=False,
                errors=["Data must be an object"]
            )
        
        schema = self._schemas[schema_name]
        errors = self._validate_object(data, schema)
        
        return ValidationResult(
            valid=len(errors) == 0,
            errors=errors
        )
    
    def validate_permission_request(self, data: dict) -> ValidationResult:
        """
        验证权限请求
        
        Args:
            data: 请求数据
            
        Returns:
            验证结果
        """
        return self._validate(data, "permission")
    
    def validate_user_request(
        self,
        data: dict,
        is_create: bool = True
    ) -> ValidationResult:
        """
        验证用户创建/更新请求
        
        Args:
            data: 请求数据
            is_create: 是否为创建请求
            
        Returns:
            验证结果
        """
        schema_name = "user_create" if is_create else "user_update"
        return self._validate(data, schema_name)
    
    def validate_audit_query(self, data: dict) -> ValidationResult:
        """
        验证审计查询请求
        
        Args:
            data: 查询参数
            
        Returns:
            验证结果
        """
        return self._validate(data, "audit_query")
    
    def validate_privilege_escalation(self, data: dict) -> ValidationResult:
        """
        验证提权申请
        
        Args:
            data: 申请数据
            
        Returns:
            验证结果
        """
        return self._validate(data, "privilege_escalation")
    
    def validate_api_key_format(self, api_key: str) -> ValidationResult:
        """
        验证API Key格式
        
        Args:
            api_key: API Key
            
        Returns:
            验证结果
        """
        errors = []
        
        if not api_key:
            errors.append("API Key cannot be empty")
        elif not api_key.startswith("sk_"):
            errors.append("API Key must start with 'sk_'")
        elif len(api_key) < 32:
            errors.append("API Key must be at least 32 characters")
        
        return ValidationResult(
            valid=len(errors) == 0,
            errors=errors
        )
    
    def validate_jwt_format(self, token: str) -> ValidationResult:
        """
        验证JWT Token格式
        
        Args:
            token: JWT Token
            
        Returns:
            验证结果
        """
        errors = []
        
        if not token:
            errors.append("JWT Token cannot be empty")
        else:
            parts = token.split(".")
            if len(parts) != 3:
                errors.append("JWT Token must have 3 parts")
        
        return ValidationResult(
            valid=len(errors) == 0,
            errors=errors
        )


# 全局验证器实例
_default_validator: Optional[RequestValidator] = None


def get_validator() -> RequestValidator:
    """获取全局验证器实例"""
    global _default_validator
    if _default_validator is None:
        _default_validator = RequestValidator()
    return _default_validator
