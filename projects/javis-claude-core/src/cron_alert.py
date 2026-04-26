"""
cron_alert.py - Cron任务失败告警模块

P1-3 实现 (2026-04-17)
核心功能：
- 告警触发：Cron任务执行失败（非超时）时触发
- 告警内容：任务名、失败原因、发生时间
- 告警渠道：优先使用飞书webhook
- 告警频率：同一任务失败告警24小时内不重复
"""

from __future__ import annotations
from dataclasses import dataclass, field
from typing import Optional, Dict, Callable, List
from datetime import datetime, timedelta
import time
import threading
import logging
import json
import hashlib
import urllib.request
import urllib.error
from pathlib import Path

from execution_trace import warn as _et_warn

logger = logging.getLogger(__name__)


# 默认飞书webhook URL配置路径
DEFAULT_WEBHOOK_CONFIG_PATH = Path("~/.openclaw/config/feishu_webhook.json").expanduser()


@dataclass
class CronAlertConfig:
    """告警配置"""
    # 飞书webhook URL
    feishu_webhook_url: str = ""
    # 告警去重时间窗口（秒），默认24小时
    dedup_window_seconds: int = 86400
    # 是否启用告警
    enabled: bool = True
    # 告警级别: "warning", "error", "critical"
    alert_level: str = "error"
    # 超时任务是否告警（默认否，超时由单独机制处理）
    alert_on_timeout: bool = False
    # 告警重试次数
    retry_count: int = 3
    # 告警重试间隔（秒）
    retry_interval: float = 1.0


@dataclass
class TaskAlertRecord:
    """任务告警记录"""
    task_name: str
    error_message: str
    occurred_at: float = field(default_factory=time.time)
    last_alert_at: Optional[float] = None
    alert_count: int = 0
    task_hash: str = ""  # 用于去重的hash


class AlertChannel(Callable):
    """告警渠道基类"""
    def __call__(self, title: str, content: str, level: str = "error") -> bool:
        raise NotImplementedError


class FeishuWebhookChannel(AlertChannel):
    """飞书webhook告警渠道
    
    支持两种模式：
    1. 自定义机器人webhook（需传入webhook_url）
    2. 配置中心读取（从配置文件读取）
    """
    
    def __init__(self, webhook_url: Optional[str] = None, 
                 config_path: Optional[Path] = None):
        self.webhook_url = webhook_url
        self.config_path = config_path or DEFAULT_WEBHOOK_CONFIG_PATH
        self._cached_url: Optional[str] = None
    
    def _get_webhook_url(self) -> Optional[str]:
        """获取webhook URL（支持缓存）"""
        if self.webhook_url:
            return self.webhook_url
        
        if self._cached_url:
            return self._cached_url
        
        # 从配置文件读取
        if self.config_path and self.config_path.exists():
            try:
                with open(self.config_path) as f:
                    config = json.load(f)
                self._cached_url = config.get("cron_alert_webhook") or config.get("webhook")
                return self._cached_url
            except Exception as e:
                logger.error(f"Failed to load webhook config: {e}")
        
        return None
    
    def __call__(self, title: str, content: str, level: str = "error") -> bool:
        """发送飞书消息
        
        Args:
            title: 消息标题
            content: 消息内容（支持Markdown）
            level: 消息级别（warning/error/critical）
            
        Returns:
            bool: 是否发送成功
        """
        webhook_url = self._get_webhook_url()
        if not webhook_url:
            _et_warn("Feishu webhook URL not configured, alert not sent")
            return False
        
        # 构建消息体
        level_emoji = {
            "warning": "⚠️",
            "error": "❌",
            "critical": "🚨",
        }.get(level, "❌")
        
        message = {
            "msg_type": "interactive",
            "card": {
                "header": {
                    "title": {
                        "tag": "plain_text",
                        "content": f"{level_emoji} {title}"
                    },
                    "template": self._level_to_color(level)
                },
                "elements": [
                    {
                        "tag": "markdown",
                        "content": content
                    },
                    {
                        "tag": "note",
                        "elements": [
                            {
                                "tag": "plain_text",
                                "content": f"⏰ {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
                            }
                        ]
                    }
                ]
            }
        }
        
        # 发送请求
        for attempt in range(3):
            try:
                req = urllib.request.Request(
                    webhook_url,
                    data=json.dumps(message).encode('utf-8'),
                    headers={'Content-Type': 'application/json'},
                    method='POST'
                )
                with urllib.request.urlopen(req, timeout=10) as response:
                    result = json.loads(response.read().decode('utf-8'))
                    if result.get("code") == 0 or result.get("StatusCode") == 0:
                        logger.info(f"Feishu alert sent successfully: {title}")
                        return True
                    else:
                        _et_warn(f"Feishu alert API error: {result}")
            except urllib.error.URLError as e:
                _et_warn(f"Feishu alert request failed (attempt {attempt+1}): {e}")
                if attempt < 2:
                    time.sleep(1 * (attempt + 1))  # 指数退避
            except Exception as e:
                logger.error(f"Feishu alert unexpected error: {e}")
                break
        
        return False
    
    def _level_to_color(self, level: str) -> str:
        """级别转颜色"""
        mapping = {
            "warning": "yellow",
            "error": "red",
            "critical": "red",
        }
        return mapping.get(level, "red")


class CronAlertManager:
    """Cron任务告警管理器 (单例模式，线程安全)
    
    核心功能：
    1. 任务失败时发送告警
    2. 24小时内同一任务不重复告警
    3. 支持多种告警渠道（飞书webhook等）
    """
    
    _instance: Optional['CronAlertManager'] = None
    _lock = threading.Lock()
    
    def __new__(cls):
        if cls._instance is None:
            with cls._lock:
                if cls._instance is None:
                    cls._instance = super().__new__(cls)
                    cls._instance._init()
        return cls._instance
    
    def _init(self):
        """初始化"""
        self._config = CronAlertConfig()
        self._alert_records: Dict[str, TaskAlertRecord] = {}
        self._records_lock = threading.Lock()
        self._channels: List[AlertChannel] = []
        self._state_file: Optional[Path] = None
        self._cleanup_interval = 3600  # 清理间隔（秒）
        self._last_cleanup = time.time()
    
    def configure(self, config: CronAlertConfig) -> None:
        """配置告警管理器
        
        Args:
            config: CronAlertConfig配置对象
        """
        with self._records_lock:
            self._config = config
            
            # 初始化飞书渠道（如果启用）
            if config.enabled and config.feishu_webhook_url:
                feishu_channel = FeishuWebhookChannel(config.feishu_webhook_url)
                self._channels.append(feishu_channel)
    
    def configure_from_dict(self, config_dict: Dict) -> None:
        """从字典配置
        
        Args:
            config_dict:
            {
                "feishu_webhook_url": "https://open.feishu.cn/...",
                "dedup_window_seconds": 86400,
                "enabled": True,
                "alert_level": "error",
                "alert_on_timeout": False,
            }
        """
        config = CronAlertConfig(
            feishu_webhook_url=config_dict.get("feishu_webhook_url", ""),
            dedup_window_seconds=config_dict.get("dedup_window_seconds", 86400),
            enabled=config_dict.get("enabled", True),
            alert_level=config_dict.get("alert_level", "error"),
            alert_on_timeout=config_dict.get("alert_on_timeout", False),
            retry_count=config_dict.get("retry_count", 3),
            retry_interval=config_dict.get("retry_interval", 1.0),
        )
        self.configure(config)
    
    def load_from_file(self, path: str) -> None:
        """从JSON文件加载配置"""
        path = Path(path)
        if not path.exists():
            _et_warn(f"Cron alert config file not found: {path}")
            return
        
        try:
            with open(path) as f:
                config_dict = json.load(f)
            self.configure_from_dict(config_dict)
            logger.info(f"Loaded cron alert config from {path}")
        except Exception as e:
            logger.error(f"Failed to load cron alert config: {e}")
    
    def add_channel(self, channel: AlertChannel) -> None:
        """添加告警渠道"""
        self._channels.append(channel)
    
    def remove_channel(self, channel: AlertChannel) -> None:
        """移除告警渠道"""
        if channel in self._channels:
            try:
                self._channels.remove(channel)
            except ValueError:
                # 线程竞态：检查后、移除前，另一线程已移除该元素
                pass
    
    def _compute_task_hash(self, task_name: str, error_message: str) -> str:
        """计算任务标识hash（用于去重）
        
        同一任务多次失败使用相同的hash，确保24小时内不重复告警
        """
        return hashlib.md5(task_name.encode()).hexdigest()[:16]
    
    def _should_alert(self, task_name: str, error_message: str) -> tuple[bool, Optional[TaskAlertRecord]]:
        """检查是否应该发送告警
        
        Returns:
            (should_alert, existing_record)
        """
        task_hash = self._compute_task_hash(task_name, error_message)
        now = time.time()
        
        with self._records_lock:
            if task_hash in self._alert_records:
                record = self._alert_records[task_hash]
                elapsed = now - record.last_alert_at if record.last_alert_at else self._config.dedup_window_seconds + 1
                
                if elapsed < self._config.dedup_window_seconds:
                    # 在去重窗口内，不告警但更新记录
                    record.alert_count += 1
                    return False, record
                else:
                    # 窗口已过，重置告警
                    record.last_alert_at = now
                    record.alert_count = 1
                    record.occurred_at = now
                    record.error_message = error_message
                    return True, record
            
            # 新任务，创建记录
            new_record = TaskAlertRecord(
                task_name=task_name,
                error_message=error_message,
                task_hash=task_hash,
                last_alert_at=now,
                alert_count=1,
            )
            self._alert_records[task_hash] = new_record
            return True, new_record
    
    def alert_on_failure(self, task_name: str, error_message: str,
                         error_type: str = "execution_error",
                         context: Optional[Dict] = None) -> bool:
        """任务失败时调用此方法发送告警
        
        Args:
            task_name: 任务名称
            error_message: 错误信息
            error_type: 错误类型（用于分类）
            context: 额外上下文信息
            
        Returns:
            bool: 是否发送成功
        """
        if not self._config.enabled:
            _et_warn("Cron alert disabled, skipping")
            return False
        
        # 跳过超时错误（如果配置不告警超时）
        if error_type == "timeout" and not self._config.alert_on_timeout:
            _et_warn(f"Timeout error for task '{task_name}', skipping alert")
            return False
        
        # 检查去重
        should_alert, record = self._should_alert(task_name, error_message)
        if not should_alert:
            logger.info(
                f"Alert for task '{task_name}' suppressed (dedup window), "
                f"alert_count={record.alert_count}"
            )
            return False
        
        # 构建告警内容
        title = f"Cron任务执行失败: {task_name}"
        
        # 错误类型标签
        error_type_emoji = {
            "execution_error": "❌",
            "timeout": "⏱️",
            "crash": "💥",
            "resource_exceeded": "📛",
        }.get(error_type, "❌")
        
        content_lines = [
            f"**任务名称**: {task_name}",
            f"**错误类型**: {error_type_emoji} {error_type}",
            f"**错误信息**: {error_message[:500]}",  # 截断过长的错误信息
        ]
        
        if context:
            # 添加上下文信息
            if "task_id" in context:
                content_lines.append(f"**任务ID**: `{context['task_id']}`")
            if "schedule" in context:
                content_lines.append(f"**调度时间**: {context['schedule']}")
            if "duration" in context:
                content_lines.append(f"**运行时长**: {context['duration']:.1f}秒")
        
        if record.alert_count > 1:
            content_lines.append(f"**累计失败次数**: {record.alert_count}次")
        
        content = "\n".join(content_lines)
        
        # 发送到所有渠道
        success = False
        for channel in self._channels:
            try:
                result = channel(title, content, self._config.alert_level)
                success = success or result
            except Exception as e:
                logger.error(f"Alert channel failed: {e}")
        
        if success:
            logger.info(f"Cron alert sent for task '{task_name}'")
        else:
            _et_warn(f"Failed to send cron alert for task '{task_name}'")
        
        return success
    
    def get_alert_status(self, task_name: Optional[str] = None) -> Dict:
        """获取告警状态
        
        Args:
            task_name: 可选，指定任务名
            
        Returns:
            告警状态字典
        """
        with self._records_lock:
            if task_name:
                # 查找指定任务的记录
                for record in self._alert_records.values():
                    if record.task_name == task_name:
                        return self._record_to_dict(record)
                return {"task_name": task_name, "exists": False}
            else:
                # 返回所有记录
                return {
                    "total_tasks": len(self._alert_records),
                    "tasks": [
                        self._record_to_dict(r) for r in self._alert_records.values()
                    ],
                    "config": {
                        "enabled": self._config.enabled,
                        "dedup_window_seconds": self._config.dedup_window_seconds,
                        "channels": len(self._channels),
                    }
                }
    
    def _record_to_dict(self, record: TaskAlertRecord) -> Dict:
        """将记录转为字典"""
        return {
            "exists": True,
            "task_name": record.task_name,
            "error_message": record.error_message[:100],
            "last_alert_at": datetime.fromtimestamp(record.last_alert_at).isoformat() if record.last_alert_at else None,
            "alert_count": record.alert_count,
            "first_occurrence": datetime.fromtimestamp(record.occurred_at).isoformat(),
        }
    
    def clear_old_records(self) -> int:
        """清理过期的告警记录
        
        Returns:
            清理的记录数
        """
        now = time.time()
        cutoff = now - self._config.dedup_window_seconds * 2  # 超过2倍窗口期才清理
        
        with self._records_lock:
            to_remove = [
                hash_id for hash_id, record in self._alert_records.items()
                if record.last_alert_at and record.last_alert_at < cutoff
            ]
            
            for hash_id in to_remove:
                del self._alert_records[hash_id]
            
            if to_remove:
                logger.info(f"Cleared {len(to_remove)} old alert records")
            
            return len(to_remove)
    
    def reset(self) -> None:
        """重置告警管理器状态"""
        with self._records_lock:
            self._alert_records.clear()
        logger.info("Cron alert manager reset")
    
    @classmethod
    def reset_instance(cls) -> None:
        """重置单例（用于测试）"""
        with cls._lock:
            if cls._instance is not None:
                cls._instance._alert_records.clear()
                cls._instance._channels.clear()
            cls._instance = None


def get_default_alert_manager() -> CronAlertManager:
    """获取默认告警管理器"""
    return CronAlertManager()


# 便捷函数
def alert_cron_failure(task_name: str, error_message: str,
                       error_type: str = "execution_error",
                       **kwargs) -> bool:
    """便捷函数：发送Cron任务失败告警"""
    manager = get_default_alert_manager()
    return manager.alert_on_failure(task_name, error_message, error_type, kwargs)


__all__ = [
    'CronAlertConfig', 'TaskAlertRecord', 'AlertChannel',
    'FeishuWebhookChannel', 'CronAlertManager',
    'get_default_alert_manager', 'alert_cron_failure',
]
