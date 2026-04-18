"""
test_agent_quota.py - agent_quota 模块测试

P1-2: per-agent资源配额测试
"""

import pytest
import time
import threading
from src.agent_quota import (
    QuotaType, AgentQuotaConfig, AgentUsage,
    QuotaExceeded, QuotaWarning,
    AgentQuotaManager, get_default_manager, quota_protected,
)


@pytest.fixture(autouse=True)
def reset_quota_manager():
    """每个测试前重置配额管理器单例"""
    AgentQuotaManager.reset_instance()
    yield
    AgentQuotaManager.reset_instance()


class TestAgentQuotaConfig:
    """AgentQuotaConfig 测试"""
    
    def test_create_full_config(self):
        config = AgentQuotaConfig(
            agent_id="test_agent",
            message_limit=100,
            dialog_limit=10,
            token_limit=50000,
            window_seconds=3600,
            warn_threshold=0.8,
        )
        assert config.agent_id == "test_agent"
        assert config.message_limit == 100
        assert config.dialog_limit == 10
        assert config.token_limit == 50000
        assert config.window_seconds == 3600
        assert config.warn_threshold == 0.8
    
    def test_create_minimal_config(self):
        config = AgentQuotaConfig(agent_id="test_agent")
        assert config.is_empty() is True
    
    def test_get_limit(self):
        config = AgentQuotaConfig(
            agent_id="test",
            message_limit=100,
            dialog_limit=10,
        )
        assert config.get_limit(QuotaType.MESSAGE_COUNT) == 100
        assert config.get_limit(QuotaType.DIALOG_COUNT) == 10
        assert config.get_limit(QuotaType.TOKEN_USAGE) is None


class TestAgentUsage:
    """AgentUsage 测试"""
    
    def test_create(self):
        usage = AgentUsage(agent_id="test")
        assert usage.agent_id == "test"
        assert usage.message_count == 0
        assert usage.dialog_count == 0
        assert usage.token_usage == 0
        assert usage.exceeded_at is None
        assert usage.warn_emitted_at is None


class TestQuotaExceeded:
    """QuotaExceeded 异常测试"""
    
    def test_create(self):
        exc = QuotaExceeded(
            agent_id="真显",
            quota_type=QuotaType.MESSAGE_COUNT,
            current=101,
            limit=100,
        )
        assert exc.agent_id == "真显"
        assert exc.quota_type == QuotaType.MESSAGE_COUNT
        assert exc.current == 101
        assert exc.limit == 100
        assert "quota exceeded" in str(exc).lower()
    
    def test_custom_message(self):
        exc = QuotaExceeded(
            agent_id="悟通",
            quota_type=QuotaType.DIALOG_COUNT,
            current=11,
            limit=10,
            message="悟通对话数超限",
        )
        assert str(exc) == "悟通对话数超限"


class TestAgentQuotaManager:
    """AgentQuotaManager 测试"""
    
    def test_singleton(self):
        """验证单例模式"""
        manager1 = AgentQuotaManager()
        manager2 = AgentQuotaManager()
        assert manager1 is manager2
        # 测试后重置
        AgentQuotaManager.reset_instance()
    
    def test_configure_from_dict(self):
        """从字典配置"""
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "default": {"message_limit": 100, "dialog_limit": 10},
            "真显": {"message_limit": 50},
            "悟通": {"message_limit": 200, "warn_threshold": 0.9},
        })
        
        # 检查默认配置（default作为默认配置存储，不是agent配置）
        default_config = manager.get_config("default")
        assert default_config is not None  # default配置存在
        assert default_config.message_limit == 100
        assert default_config.dialog_limit == 10
        
        # 检查真显配置
        config = manager.get_config("真显")
        assert config is not None
        assert config.message_limit == 50
        assert config.warn_threshold == 0.8  # 默认值
        
        # 检查悟通配置
        config = manager.get_config("悟通")
        assert config is not None
        assert config.message_limit == 200
        assert config.warn_threshold == 0.9
    
    def test_check_quota_allowed(self):
        """配额检查 - 允许"""
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "default": {"message_limit": 100},
        })
        
        allowed, current, limit = manager.check_quota(
            "test_agent", QuotaType.MESSAGE_COUNT
        )
        assert allowed is True
        assert current == 0
        assert limit == 100
    
    def test_check_quota_exceeded(self):
        """配额检查 - 超限"""
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "test_agent": {"message_limit": 5},
        })
        
        # 使用到接近限制
        for _ in range(5):
            manager.record_usage("test_agent", QuotaType.MESSAGE_COUNT)
        
        allowed, current, limit = manager.check_quota(
            "test_agent", QuotaType.MESSAGE_COUNT, increment=1
        )
        assert allowed is False
        assert current == 5
        assert limit == 5
    
    def test_record_usage(self):
        """记录使用量"""
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "test_agent": {"message_limit": 100},
        })
        
        manager.record_usage("test_agent", QuotaType.MESSAGE_COUNT)
        usage = manager.get_usage("test_agent")
        assert usage.message_count == 1
        
        manager.record_usage("test_agent", QuotaType.MESSAGE_COUNT, delta=5)
        usage = manager.get_usage("test_agent")
        assert usage.message_count == 6
    
    def test_check_and_increment(self):
        """检查并增加"""
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "test_agent": {"message_limit": 3},
        })
        
        # 前3次应该成功
        for i in range(3):
            manager.check_and_increment("test_agent", QuotaType.MESSAGE_COUNT)
            usage = manager.get_usage("test_agent")
            assert usage.message_count == i + 1
        
        # 第4次应该抛出异常
        with pytest.raises(QuotaExceeded):
            manager.check_and_increment("test_agent", QuotaType.MESSAGE_COUNT)
    
    def test_reset_usage(self):
        """重置使用量"""
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "test_agent": {"message_limit": 10},
        })
        
        for _ in range(5):
            manager.record_usage("test_agent", QuotaType.MESSAGE_COUNT)
        
        assert manager.get_usage("test_agent").message_count == 5
        
        manager.reset_usage("test_agent")
        assert manager.get_usage("test_agent").message_count == 0
    
    def test_notification_callback(self):
        """通知回调"""
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "test_agent": {"message_limit": 5},
        })
        
        notifications = []
        def callback(agent_id, quota_type, current, limit, is_warning):
            notifications.append({
                "agent_id": agent_id,
                "quota_type": quota_type,
                "current": current,
                "limit": limit,
                "is_warning": is_warning,
            })
        
        manager.register_notification_callback(callback)
        
        # 达到限制时应该触发通知
        for _ in range(6):
            try:
                manager.check_and_increment("test_agent", QuotaType.MESSAGE_COUNT)
            except QuotaExceeded:
                pass
        
        assert len(notifications) >= 1
        assert notifications[-1]["is_warning"] is False
    
    def test_get_status(self):
        """获取状态"""
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "test_agent": {"message_limit": 100, "dialog_limit": 10},
        })
        
        manager.record_usage("test_agent", QuotaType.MESSAGE_COUNT, delta=50)
        manager.record_usage("test_agent", QuotaType.DIALOG_COUNT, delta=3)
        
        status = manager.get_status("test_agent")
        assert status["configured"] is True
        assert status["message"]["current"] == 50
        assert status["message"]["limit"] == 100
        assert status["message"]["percentage"] == 50.0
        assert status["dialog"]["current"] == 3
        assert status["dialog"]["limit"] == 10
        assert status["exceeded"] is False
    
    def test_unconfigured_agent(self):
        """未配置的agent应该不受限制"""
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "other_agent": {"message_limit": 10},
        })
        
        # 未配置的agent应该允许任意数量
        for _ in range(1000):
            manager.record_usage("unconfigured_agent", QuotaType.MESSAGE_COUNT)
        
        usage = manager.get_usage("unconfigured_agent")
        assert usage.message_count == 1000
        
        allowed, _, _ = manager.check_quota(
            "unconfigured_agent", QuotaType.MESSAGE_COUNT
        )
        assert allowed is True
    
    def test_multiple_quota_types(self):
        """多种配额类型
        
        API说明:
        - record_usage(quota_type=MESSAGE_COUNT, delta=N): 记录N条消息
        - record_usage(quota_type=TOKEN_USAGE, delta=N): 记录N个token
        - record_usage(..., token_delta=N): 额外追踪token消耗（用于消息/对话计数时）
        """
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "test_agent": {
                "message_limit": 10,
                "dialog_limit": 5,
                "token_limit": 1000,
            },
        })
        
        # 记录消息（同时追踪token消耗）
        manager.record_usage("test_agent", QuotaType.MESSAGE_COUNT, delta=3, token_delta=100)
        # 记录对话
        manager.record_usage("test_agent", QuotaType.DIALOG_COUNT, delta=2)
        # 记录token（使用delta参数）
        manager.record_usage("test_agent", QuotaType.TOKEN_USAGE, delta=500)
        
        usage = manager.get_usage("test_agent")
        assert usage.message_count == 3
        assert usage.dialog_count == 2
        assert usage.token_usage == 600  # 100 (from messages) + 500 (from tokens)


class TestQuotaProtectedDecorator:
    """@quota_protected 装饰器测试"""
    
    def test_decorator_allows(self):
        """装饰器允许正常调用"""
        manager = get_default_manager()
        manager.configure_from_dict({
            "decorated_agent": {"message_limit": 100},
        })
        
        call_count = 0
        
        @quota_protected("decorated_agent", QuotaType.MESSAGE_COUNT)
        def my_function():
            nonlocal call_count
            call_count += 1
            return "success"
        
        for _ in range(10):
            result = my_function()
            assert result == "success"
        
        assert call_count == 10
    
    def test_decorator_blocks(self):
        """装饰器阻止超限调用"""
        manager = get_default_manager()
        manager.configure_from_dict({
            "blocked_agent": {"message_limit": 3},
        })
        
        call_count = 0
        
        @quota_protected("blocked_agent", QuotaType.MESSAGE_COUNT)
        def my_function():
            nonlocal call_count
            call_count += 1
            return "success"
        
        # 前3次成功
        for _ in range(3):
            result = my_function()
            assert result == "success"
        
        assert call_count == 3
        
        # 第4次失败
        with pytest.raises(QuotaExceeded):
            my_function()
        
        assert call_count == 3  # 没有增加


class TestQuotaManagerThreadSafety:
    """线程安全测试"""
    
    def test_concurrent_access(self):
        """并发访问"""
        manager = AgentQuotaManager()
        manager.configure_from_dict({
            "concurrent_agent": {"message_limit": 1000},
        })
        
        errors = []
        
        def worker(thread_id):
            try:
                for _ in range(100):
                    manager.record_usage("concurrent_agent", QuotaType.MESSAGE_COUNT)
            except Exception as e:
                errors.append((thread_id, e))
        
        threads = [threading.Thread(target=worker, args=(i,)) for i in range(10)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        
        assert len(errors) == 0
        usage = manager.get_usage("concurrent_agent")
        assert usage.message_count == 1000
