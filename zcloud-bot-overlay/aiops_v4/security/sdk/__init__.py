"""
Shield Python SDK
提供对Shield安全模块REST API的Python客户端封装
"""

from .client import ShieldClient, ShieldSDK

__version__ = "1.0.0"

__all__ = [
    "ShieldClient",
    "ShieldSDK",
]
