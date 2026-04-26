"""
test_cron_alert.py - cron_alert 模块测试

P1-3: Cron任务失败告警测试
"""

import pytest
import time
import threading
from unittest.mock import Mock, patch, MagicMock
from src.cron_alert import (
    CronAlertConfig, TaskAlertRecord, AlertChannel,
    FeishuWebhookChannel, CronAlertManager,
    get_default_alert_manager, alert_cron_failure,
)


class TestCronAlertConfig:
    """CronAlertConfig 测试"""
    
    def test_default_config(self):
        config = CronAlertConfig()
        assert config.feishu_webhook_url == ""
        assert config.dedup_window_seconds == 86400
        assert config.enabled is True
        assert config.alert_level == "error"
        assert config.alert_on_timeout is False
    
    def test_custom_config(self):
        config = CronAlertConfig(
            feishu_webhook_url="https://test.webhook",
            dedup_window_seconds=3600,
            alert_level="critical",
            alert_on_timeout=True,
        )
        assert config.feishu_webhook_url == "https://test.webhook"
        assert config.dedup_window_seconds == 3600
        assert config.alert_level == "critical"
        assert config.alert_on_timeout is True


class TestTaskAlertRecord:
    """TaskAlertRecord 测试"""
    
    def test_create(self):
        record = TaskAlertRecord(
            task_name="test_task",
            error_message="Something went wrong",
        )
        assert record.task_name == "test_task"
        assert record.error_message == "Something went wrong"
        assert record.alert_count == 0
        assert record.last_alert_at is None


class TestFeishuWebhookChannel:
    """FeishuWebhookChannel 测试"""
    
    def test_init_with_url(self):
        channel = FeishuWebhookChannel("https://test.webhook.url")
        assert channel.webhook_url == "https://test.webhook.url"
        assert channel._get_webhook_url() == "https://test.webhook.url"
    
    def test_level_to_color(self):
        channel = FeishuWebhookChannel()
        assert channel._level_to_color("warning") == "yellow"
        assert channel._level_to_color("error") == "red"
        assert channel._level_to_color("critical") == "red"
        assert channel._level_to_color("unknown") == "red"


class TestCronAlertManager:
    """CronAlertManager 测试"""
    
    def setup_method(self):
        """每个测试前重置单例"""
        CronAlertManager.reset_instance()
    
    def teardown_method(self):
        """每个测试后重置单例"""
        CronAlertManager.reset_instance()
    
    def test_singleton(self):
        """验证单例模式"""
        manager1 = CronAlertManager()
        manager2 = CronAlertManager()
        assert manager1 is manager2
    
    def test_configure_from_dict(self):
        """从字典配置"""
        manager = CronAlertManager()
        manager.configure_from_dict({
            "feishu_webhook_url": "https://test.webhook",
            "dedup_window_seconds": 3600,
            "enabled": True,
            "alert_level": "warning",
        })
        
        assert manager._config.feishu_webhook_url == "https://test.webhook"
        assert manager._config.dedup_window_seconds == 3600
        assert manager._config.alert_level == "warning"
    
    def test_add_channel(self):
        """添加告警渠道"""
        manager = CronAlertManager()
        mock_channel = Mock(spec=AlertChannel)
        manager.add_channel(mock_channel)
        
        assert mock_channel in manager._channels
    
    def test_alert_success(self):
        """告警成功发送"""
        manager = CronAlertManager()
        manager.configure_from_dict({"enabled": True})
        
        mock_channel = Mock(return_value=True)
        manager.add_channel(mock_channel)
        
        result = manager.alert_on_failure(
            "test_task",
            "Task failed with error",
            "execution_error"
        )
        
        assert result is True
        mock_channel.assert_called_once()
        
        # 验证调用参数
        call_args = mock_channel.call_args
        assert "test_task" in call_args[0][0]  # title
        assert "error" in call_args[0][2]  # level
    
    def test_alert_disabled(self):
        """告警禁用时跳过"""
        manager = CronAlertManager()
        manager.configure_from_dict({"enabled": False})
        
        mock_channel = Mock(return_value=True)
        manager.add_channel(mock_channel)
        
        result = manager.alert_on_failure(
            "test_task",
            "Task failed",
        )
        
        assert result is False
        mock_channel.assert_not_called()
    
    def test_alert_skip_timeout(self):
        """超时错误默认跳过"""
        manager = CronAlertManager()
        manager.configure_from_dict({
            "enabled": True,
            "alert_on_timeout": False,
        })
        
        mock_channel = Mock(return_value=True)
        manager.add_channel(mock_channel)
        
        result = manager.alert_on_failure(
            "test_task",
            "Task timed out",
            error_type="timeout"
        )
        
        assert result is False
        mock_channel.assert_not_called()
    
    def test_alert_timeout_when_enabled(self):
        """启用超时告警时"""
        manager = CronAlertManager()
        manager.configure_from_dict({
            "enabled": True,
            "alert_on_timeout": True,
        })
        
        mock_channel = Mock(return_value=True)
        manager.add_channel(mock_channel)
        
        result = manager.alert_on_failure(
            "test_task",
            "Task timed out",
            error_type="timeout"
        )
        
        assert result is True
        mock_channel.assert_called_once()
    
    def test_dedup_same_task(self):
        """同一任务24小时内不重复告警"""
        manager = CronAlertManager()
        manager.configure_from_dict({
            "enabled": True,
            "dedup_window_seconds": 86400,  # 24小时
        })
        
        mock_channel = Mock(return_value=True)
        manager.add_channel(mock_channel)
        
        # 第一次告警
        result1 = manager.alert_on_failure("test_task", "Error 1")
        assert result1 is True
        assert mock_channel.call_count == 1
        
        # 第二次相同任务（应该被抑制）
        result2 = manager.alert_on_failure("test_task", "Error 2")
        assert result2 is False
        assert mock_channel.call_count == 1  # 没有新的调用
    
    def test_dedup_same_task_any_error(self):
        """同一任务不管什么错误类型都去重（符合需求规范）"""
        manager = CronAlertManager()
        manager.configure_from_dict({
            "enabled": True,
            "dedup_window_seconds": 86400,
        })
        
        mock_channel = Mock(return_value=True)
        manager.add_channel(mock_channel)
        
        # 第一次：execution_error
        result1 = manager.alert_on_failure("test_task", "Error 1", "execution_error")
        assert result1 is True
        
        # 第二次：timeout（同一任务，仍被去重）
        result2 = manager.alert_on_failure("test_task", "Timeout", "timeout")
        assert result2 is False  # 同一任务，24小时内不重复告警
    
    def test_dedup_window_reset(self):
        """去重窗口过期后重新告警"""
        manager = CronAlertManager()
        manager.configure_from_dict({
            "enabled": True,
            "dedup_window_seconds": 1,  # 1秒窗口（用于测试）
        })
        
        mock_channel = Mock(return_value=True)
        manager.add_channel(mock_channel)
        
        # 第一次
        result1 = manager.alert_on_failure("test_task", "Error")
        assert result1 is True
        
        # 等待窗口过期
        time.sleep(1.1)
        
        # 第二次（窗口已过，应该能告警）
        result2 = manager.alert_on_failure("test_task", "Error")
        assert result2 is True
        assert mock_channel.call_count == 2
    
    def test_get_alert_status(self):
        """获取告警状态"""
        manager = CronAlertManager()
        manager.configure_from_dict({
            "enabled": True,
            "dedup_window_seconds": 86400,
        })
        
        manager.alert_on_failure("task_1", "Error 1")
        manager.alert_on_failure("task_2", "Error 2")
        
        status = manager.get_alert_status()
        assert status["total_tasks"] == 2
        
        # 获取单个任务状态
        task_status = manager.get_alert_status("task_1")
        assert task_status["task_name"] == "task_1"
        assert task_status["exists"] is True
    
    def test_clear_old_records(self):
        """清理过期记录"""
        manager = CronAlertManager()
        manager.configure_from_dict({
            "enabled": True,
            "dedup_window_seconds": 1,  # 1秒
        })
        
        manager.alert_on_failure("old_task", "Error")
        
        # 等待过期
        time.sleep(2)
        
        # 清理
        cleared = manager.clear_old_records()
        assert cleared >= 1
        
        status = manager.get_alert_status("old_task")
        assert status["exists"] is False
    
    def test_remove_channel_race_condition(self):
        """remove_channel 竞态条件测试：ValueError 不应抛出"""
        manager = CronAlertManager()
        mock_channel = Mock(spec=AlertChannel)
        manager.add_channel(mock_channel)
        assert mock_channel in manager._channels
        
        # 第一次移除：正常移除
        manager.remove_channel(mock_channel)
        assert mock_channel not in manager._channels
        
        # 第二次移除：模拟竞态（channel 已不在列表中）- 不应抛异常
        result = manager.remove_channel(mock_channel)
        assert result is None
    
    def test_remove_channel_nonexistent(self):
        """移除从未添加的渠道不抛异常"""
        manager = CronAlertManager()
        mock_channel = Mock(spec=AlertChannel)
        
        # 从未添加过的 channel，直接移除不应抛异常
        result = manager.remove_channel(mock_channel)
        assert result is None

    def test_reset(self):
        """重置管理器"""
        manager = CronAlertManager()
        manager.configure_from_dict({"enabled": True})
        manager.alert_on_failure("test_task", "Error")
        
        manager.reset()
        
        status = manager.get_alert_status()
        assert status["total_tasks"] == 0


class TestAlertFunction:
    """便捷函数测试"""
    
    def setup_method(self):
        CronAlertManager.reset_instance()
    
    def teardown_method(self):
        CronAlertManager.reset_instance()
    
    def test_alert_cron_failure_convenience(self):
        """便捷函数测试"""
        with patch.object(CronAlertManager, 'alert_on_failure', return_value=True) as mock:
            result = alert_cron_failure(
                "my_task",
                "Something broke",
                "crash",
                task_id="123",
            )
            # 便捷函数内部创建manager并调用
            assert result is True


class TestCronAlertManagerThreadSafety:
    """线程安全测试"""
    
    def setup_method(self):
        CronAlertManager.reset_instance()
    
    def teardown_method(self):
        CronAlertManager.reset_instance()
    
    def test_concurrent_alerts(self):
        """并发告警测试"""
        manager = CronAlertManager()
        manager.configure_from_dict({
            "enabled": True,
            "dedup_window_seconds": 86400,
        })
        
        mock_channel = Mock(return_value=True)
        manager.add_channel(mock_channel)
        
        errors = []
        
        def worker(thread_id):
            try:
                for i in range(10):
                    manager.alert_on_failure(f"task_{thread_id}", f"Error {i}")
            except Exception as e:
                errors.append((thread_id, e))
        
        threads = [threading.Thread(target=worker, args=(i,)) for i in range(5)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        
        assert len(errors) == 0
