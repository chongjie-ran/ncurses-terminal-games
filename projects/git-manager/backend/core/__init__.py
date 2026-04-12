"""核心功能模块"""
import os
import sys
import json
import logging
import hashlib
from pathlib import Path
from typing import Optional, Dict, Any

try:
    from cryptography.fernet import Fernet
    HAS_CRYPTO = True
except ImportError:
    HAS_CRYPTO = False
    print("警告：cryptography 库未安装，认证信息将明文存储")

logger = logging.getLogger(__name__)

class Encryptor:
    """认证信息加密工具"""
    _key: Optional[bytes] = None

    @classmethod
    def get_key_path(cls) -> Path:
        return Path.home() / ".openclaw" / "workspace-developer" / ".git_manager_key"

    @classmethod
    def ensure_key(cls) -> bytes:
        if cls._key:
            return cls._key
        key_path = cls.get_key_path()
        if key_path.exists():
            cls._key = key_path.read_bytes()
        else:
            key_path.parent.mkdir(parents=True, exist_ok=True)
            cls._key = Fernet.generate_key()
            key_path.write_bytes(cls._key)
            os.chmod(key_path, 0o600)
        return cls._key

    @classmethod
    def encrypt(cls, data: str) -> str:
        if not HAS_CRYPTO or not data:
            return data
        return Fernet(cls.ensure_key()).encrypt(data.encode()).decode()

    @classmethod
    def decrypt(cls, encrypted: str) -> str:
        if not HAS_CRYPTO or not encrypted:
            return encrypted
        try:
            return Fernet(cls.ensure_key()).decrypt(encrypted.encode()).decode()
        except Exception:
            return encrypted

class ConfigManager:
    """配置文件管理"""
    def __init__(self, config_path: Optional[str] = None):
        if config_path:
            self.config_path = Path(config_path)
        else:
            self.config_path = Path.home() / ".openclaw" / "workspace-developer" / "projects" / "git-manager" / "config.json"
        self.config_path.parent.mkdir(parents=True, exist_ok=True)
        self._config = self._load()

    def _load(self) -> Dict[str, Any]:
        if self.config_path.exists():
            return json.loads(self.config_path.read_text())
        return {"db_path": str(self.config_path.parent / "data" / "git_manager.db")}

    def save(self):
        self.config_path.write_text(json.dumps(self._config, indent=2))

    def get(self, key: str, default=None):
        return self._config.get(key, default)

    def set(self, key: str, value):
        self._config[key] = value
        self.save()

if __name__ == "__main__":
    enc = Encryptor()
    encrypted = enc.encrypt("test_password")
    print(f"加密：{encrypted}")
    print(f"解密：{enc.decrypt(encrypted)}")
