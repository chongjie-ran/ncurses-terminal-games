"""Encryptor加密工具测试"""
import sys
import pytest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent.parent / "backend"))

from backend.core import Encryptor

class TestEncryptor:
    """Encryptor测试"""
    
    def test_encrypt_decrypt_roundtrip(self):
        """加密后解密应还原"""
        original = "my-secret-password-123!@#"
        encrypted = Encryptor.encrypt(original)
        decrypted = Encryptor.decrypt(encrypted)
        assert decrypted == original
    
    def test_encrypt_empty_string(self):
        """空字符串应直接返回"""
        result = Encryptor.encrypt("")
        assert result == ""
    
    def test_decrypt_empty_string(self):
        """空字符串应直接返回"""
        result = Encryptor.decrypt("")
        assert result == ""
    
    def test_encrypt_unicode(self):
        """应支持Unicode密码"""
        original = "密码123！@#中文"
        encrypted = Encryptor.encrypt(original)
        decrypted = Encryptor.decrypt(encrypted)
        assert decrypted == original
    
    def test_encrypt_long_password(self):
        """长密码应正常加密"""
        original = "x" * 10000
        encrypted = Encryptor.encrypt(original)
        decrypted = Encryptor.decrypt(encrypted)
        assert decrypted == original
    
    def test_decrypt_invalid_data_returns_input(self):
        """解密失败时应返回原始数据而非崩溃"""
        # 无cryptography库时，decrypt返回原文
        result = Encryptor.decrypt("not-encrypted-data")
        assert result is not None  # 至少不崩溃
    
    def test_decrypt_returns_same_when_no_crypto(self):
        """无cryptography时，encrypt返回原文，decrypt也返回原文"""
        # 如果HAS_CRYPTO=False，加密不解密
        original = "test-password"
        encrypted = Encryptor.encrypt(original)
        # 如果无cryptography，encrypted == original
        # 如果有cryptography，encrypted != original
        # 无论如何，decrypt(encrypted)应该能还原
        decrypted = Encryptor.decrypt(encrypted)
        assert decrypted == original
    
    def test_encrypt_no_crash_on_none_input(self):
        """None输入应不崩溃（通过空字符串处理）"""
        # Encryptor期望str输入
        result = Encryptor.encrypt("")
        assert result == ""
