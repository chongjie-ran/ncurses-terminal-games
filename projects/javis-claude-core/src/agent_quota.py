"""
agent_quota.py - per-agent资源配额控制

P1-2 实现 (2026-04-17)
核心功能：
- 每个agent的对话数/消息数限制
- 配额检查点：在agent接收新任务前检查
- 配额超限处理：记录日志+通知
- 配额配置：可通过配置文件设置各agent配额
"""

from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum
from typing import Optional, Dict, List, Callable
import time
import threading
import logging
import json
import os
from pathlib import Path

from execution_trace import warn as _et_warn

logger = logging.getLogger(__name__)


class QuotaType(Enum):
    MESSAGE_COUNT = "message_count"      # 消息数限制
    DIALOG_COUNT = "dialog_count"        # 对话数限制
    TOKEN_USAGE = "token_usage"          # Token使用量限制


@dataclass
class AgentQuotaConfig:
    """单个agent的配额配置"""
    agent_id: str
    message_limit: Optional[int] = None       # 最大消息数
    dialog_limit: Optional[int] = None         # 最大对话数
    token_limit: Optional[int] = None          # 最大token数
    window_seconds: Optional[int] = None       # 时间窗口(秒)，用于滑动窗口限制
    warn_threshold: float = 0.8               # 警告阈值（百分比）
    
    def is_empty(self) -> bool:
        return all(v is None for v in [
            self.message_limit, self.dialog_limit, 
            self.token_limit, self.window_seconds
        ])
    
    def get_limit(self, quota_type: QuotaType) -> Optional[int]:
        """获取指定类型的限制值"""
        mapping = {
            QuotaType.MESSAGE_COUNT: self.message_limit,
            QuotaType.DIALOG_COUNT: self.dialog_limit,
            QuotaType.TOKEN_USAGE: self.token_limit,
        }
        return mapping.get(quota_type)


@dataclass
class AgentUsage:
    """单个agent的使用情况"""
    agent_id: str
    message_count: int = 0
    dialog_count: int = 0
    token_usage: int = 0
    window_start: float = field(default_factory=time.time)
    last_activity: float = field(default_factory=time.time)
    exceeded_at: Optional[float] = None      # 超限时间
    warn_emitted_at: Optional[float] = None  # 警告时间


class QuotaExceeded(Exception):
    """配额超限异常"""
    def __init__(self, agent_id: str, quota_type: QuotaType, 
                 current: int, limit: int, message: str = ""):
        self.agent_id = agent_id
        self.quota_type = quota_type
        self.current = current
        self.limit = limit
        self.message = message or (
            f"Agent '{agent_id}' quota exceeded: "
            f"{quota_type.value}={current}/{limit}"
        )
        super().__init__(self.message)


class QuotaWarning(Exception):
    """配额警告异常（达到阈值但未超限）"""
    def __init__(self, agent_id: str, quota_type: QuotaType,
                 current: int, limit: int, threshold: float):
        self.agent_id = agent_id
        self.quota_type = quota_type
        self.current = current
        self.limit = limit
        self.threshold = threshold
        self.message = (
            f"Agent '{agent_id}' quota warning: "
            f"{quota_type.value}={current}/{limit} "
            f"(threshold: {threshold*100:.0f}%)"
        )
        super().__init__(self.message)


class AgentQuotaManager:
    """Agent配额管理器 (单例模式，线程安全)
    
    设计要点：
    1. 使用threading.Lock保证线程安全
    2. 支持滑动窗口（基于window_seconds）
    3. 支持配置文件加载
    4. 提供装饰器 @quota_protected(agent_id) 用于自动检查
    """
    
    _instance: Optional['AgentQuotaManager'] = None
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
        self._configs: Dict[str, AgentQuotaConfig] = {}
        self._usage: Dict[str, AgentUsage] = {}
        self._config_lock = threading.Lock()
        self._usage_lock = threading.Lock()
        self._default_config: Optional[AgentQuotaConfig] = None
        self._notification_callbacks: List[Callable] = []
        self._state_file: Optional[Path] = None
    
    def configure(self, configs: Dict[str, AgentQuotaConfig], 
                  default: Optional[AgentQuotaConfig] = None,
                  state_file: Optional[str] = None) -> None:
        """配置各agent的配额
        
        Args:
            configs: {agent_id: AgentQuotaConfig} 映射
            default: 默认配额（未配置agent使用）
            state_file: 状态持久化文件路径
        """
        with self._config_lock:
            self._configs = dict(configs)
            self._default_config = default
            self._state_file = Path(state_file) if state_file else None
            # 尝试加载已有状态
            self._load_state()
    
    def configure_from_dict(self, config_dict: Dict) -> None:
        """从字典配置配额
        
        Args:
            config_dict: 
            {
                "default": {"message_limit": 100, "dialog_limit": 10},
                "真显": {"message_limit": 50, "warn_threshold": 0.9},
                "悟通": {"message_limit": 200},
            }
        """
        configs = {}
        default = None
        
        for agent_id, cfg in config_dict.items():
            if agent_id == "default":
                default = AgentQuotaConfig(
                    agent_id="default",
                    message_limit=cfg.get("message_limit"),
                    dialog_limit=cfg.get("dialog_limit"),
                    token_limit=cfg.get("token_limit"),
                    window_seconds=cfg.get("window_seconds"),
                    warn_threshold=cfg.get("warn_threshold", 0.8),
                )
            else:
                configs[agent_id] = AgentQuotaConfig(
                    agent_id=agent_id,
                    message_limit=cfg.get("message_limit"),
                    dialog_limit=cfg.get("dialog_limit"),
                    token_limit=cfg.get("token_limit"),
                    window_seconds=cfg.get("window_seconds"),
                    warn_threshold=cfg.get("warn_threshold", 0.8),
                )
        
        # 重置usage状态，避免旧数据干扰
        with self._usage_lock:
            self._usage.clear()
        
        self.configure(configs, default)
    
    def load_from_file(self, path: str) -> None:
        """从JSON文件加载配额配置"""
        path = Path(path)
        if not path.exists():
            _et_warn(f"Quota config file not found: {path}")
            return
        
        try:
            with open(path) as f:
                data = json.load(f)
            self.configure_from_dict(data)
            logger.info(f"Loaded quota config from {path}")
        except Exception as e:
            logger.error(f"Failed to load quota config from {path}: {e}")
    
    def save_to_file(self, path: Optional[str] = None) -> None:
        """保存配额状态到文件"""
        path = Path(path) if path else self._state_file
        if not path:
            return
        
        try:
            with self._usage_lock:
                state = {
                    agent_id: {
                        "message_count": usage.message_count,
                        "dialog_count": usage.dialog_count,
                        "token_usage": usage.token_usage,
                        "window_start": usage.window_start,
                        "last_activity": usage.last_activity,
                        "exceeded_at": usage.exceeded_at,
                        "warn_emitted_at": usage.warn_emitted_at,
                    }
                    for agent_id, usage in self._usage.items()
                }
            
            path.parent.mkdir(parents=True, exist_ok=True)
            with open(path, 'w') as f:
                json.dump(state, f, indent=2)
            _et_warn(f"Saved quota state to {path}")
        except Exception as e:
            logger.error(f"Failed to save quota state to {path}: {e}")
    
    def _load_state(self) -> None:
        """加载配额状态"""
        if not self._state_file or not self._state_file.exists():
            return
        
        try:
            with open(self._state_file) as f:
                state = json.load(f)
            
            with self._usage_lock:
                for agent_id, data in state.items():
                    self._usage[agent_id] = AgentUsage(
                        agent_id=agent_id,
                        message_count=data.get("message_count", 0),
                        dialog_count=data.get("dialog_count", 0),
                        token_usage=data.get("token_usage", 0),
                        window_start=data.get("window_start", time.time()),
                        last_activity=data.get("last_activity", time.time()),
                        exceeded_at=data.get("exceeded_at"),
                        warn_emitted_at=data.get("warn_emitted_at"),
                    )
            logger.info(f"Loaded quota state from {self._state_file}")
        except Exception as e:
            logger.error(f"Failed to load quota state: {e}")
    
    def register_notification_callback(self, callback: Callable) -> None:
        """注册配额超限通知回调
        
        Args:
            callback: (agent_id, quota_type, current, limit) -> None
        """
        self._notification_callbacks.append(callback)
    
    def _notify(self, agent_id: str, quota_type: QuotaType,
                current: int, limit: int, is_warning: bool = False) -> None:
        """触发通知回调"""
        for callback in self._notification_callbacks:
            try:
                callback(agent_id, quota_type, current, limit, is_warning)
            except Exception as e:
                logger.error(f"Quota notification callback failed: {e}")
    
    def get_config(self, agent_id: str) -> Optional[AgentQuotaConfig]:
        """获取agent的配额配置"""
        with self._config_lock:
            return self._configs.get(agent_id, self._default_config)
    
    def get_usage(self, agent_id: str) -> AgentUsage:
        """获取agent的使用情况"""
        with self._usage_lock:
            if agent_id not in self._usage:
                self._usage[agent_id] = AgentUsage(agent_id=agent_id)
            return self._usage[agent_id]
    
    def _get_effective_config(self, agent_id: str) -> Optional[AgentQuotaConfig]:
        """获取有效配置（优先agent特定配置，其次默认配置）"""
        with self._config_lock:
            return self._configs.get(agent_id) or self._default_config
    
    def check_quota(self, agent_id: str, 
                    quota_type: QuotaType = QuotaType.MESSAGE_COUNT,
                    increment: int = 0) -> tuple[bool, Optional[int], Optional[int]]:
        """检查配额是否允许操作
        
        Args:
            agent_id: agent标识
            quota_type: 配额类型
            increment: 增量（检查后增加）
            
        Returns:
            (allowed, current, limit): 是否允许，当前值，限制值
        """
        config = self._get_effective_config(agent_id)
        if not config or config.is_empty():
            return True, None, None
        
        limit = config.get_limit(quota_type)
        if limit is None:
            return True, None, None
        
        usage = self.get_usage(agent_id)
        
        # 检查滑动窗口
        if config.window_seconds:
            now = time.time()
            if now - usage.window_start > config.window_seconds:
                # 窗口过期，重置
                with self._usage_lock:
                    usage.message_count = 0
                    usage.dialog_count = 0
                    usage.token_usage = 0
                    usage.window_start = now
        
        # 获取当前值
        current = self._get_current_value(usage, quota_type)
        projected = current + increment
        
        if projected > limit:
            # 超限：发出超限通知（不同于警告通知）
            if usage.exceeded_at is None:
                self._notify(agent_id, quota_type, projected, limit, is_warning=False)
                usage.exceeded_at = time.time()
            return False, current, limit
        
        # 警告阈值检查（未超限时）
        threshold = config.warn_threshold
        if projected >= limit * threshold and usage.exceeded_at is None:
            if usage.warn_emitted_at is None or \
               time.time() - usage.warn_emitted_at > 3600:  # 最多1小时警告一次
                self._notify(agent_id, quota_type, projected, limit, is_warning=True)
                usage.warn_emitted_at = time.time()
        
        return True, current, limit
    
    def _get_current_value(self, usage: AgentUsage, 
                           quota_type: QuotaType) -> int:
        """从usage中获取指定配额类型的当前值"""
        mapping = {
            QuotaType.MESSAGE_COUNT: usage.message_count,
            QuotaType.DIALOG_COUNT: usage.dialog_count,
            QuotaType.TOKEN_USAGE: usage.token_usage,
        }
        return mapping.get(quota_type, 0)
    
    def _set_current_value(self, usage: AgentUsage,
                           quota_type: QuotaType, value: int) -> None:
        """设置usage中指定配额类型的值"""
        if quota_type == QuotaType.MESSAGE_COUNT:
            usage.message_count = value
        elif quota_type == QuotaType.DIALOG_COUNT:
            usage.dialog_count = value
        elif quota_type == QuotaType.TOKEN_USAGE:
            usage.token_usage = value
    
    def record_usage(self, agent_id: str,
                     quota_type: QuotaType = QuotaType.MESSAGE_COUNT,
                     delta: int = 1,
                     token_delta: int = 0) -> None:
        """记录使用量（原子操作）
        
        Args:
            agent_id: agent标识
            quota_type: 配额类型
            delta: 增量（默认+1表示1条消息）
            token_delta: token增量
        """
        config = self._get_effective_config(agent_id)
        
        with self._usage_lock:
            usage = self._usage.get(agent_id)
            if usage is None:
                usage = AgentUsage(agent_id=agent_id)
                self._usage[agent_id] = usage
            
            usage.last_activity = time.time()
            
            # 检查滑动窗口（仅当有配置且设置了window_seconds时）
            if config and config.window_seconds:
                if time.time() - usage.window_start > config.window_seconds:
                    usage.message_count = 0
                    usage.dialog_count = 0
                    usage.token_usage = 0
                    usage.window_start = time.time()
            
            # 更新配额类型对应的计数
            current = self._get_current_value(usage, quota_type)
            new_value = current + delta
            self._set_current_value(usage, quota_type, new_value)
            
            # 独立追踪 token 使用量（当 quota_type 不是 TOKEN_USAGE 时，
            # 通过 token_delta 参数额外记录消耗）
            # 当 quota_type == TOKEN_USAGE 时，delta 参数本身就是 token 数量
            if quota_type == QuotaType.TOKEN_USAGE:
                # TOKEN_USAGE: delta 直接作为 token 数量
                pass  # 已在上面通过 _set_current_value 处理
            elif token_delta:
                # 其他配额类型: token_delta 额外追踪 token 消耗
                usage.token_usage += token_delta
            
            # 检查超限（仅当有配置时）
            if config:
                limit = config.get_limit(quota_type)
                if limit and new_value > limit and usage.exceeded_at is None:
                    usage.exceeded_at = time.time()
                    self._notify(agent_id, quota_type, new_value, limit, is_warning=False)
                    _et_warn(
                        f"Agent '{agent_id}' quota exceeded: "
                        f"{quota_type.value}={new_value}/{limit}"
                    )
        
        # 持久化
        if self._state_file:
            self.save_to_file()
    
    def check_and_increment(self, agent_id: str,
                           quota_type: QuotaType = QuotaType.MESSAGE_COUNT,
                           delta: int = 1,
                           token_delta: int = 0) -> None:
        """检查配额并增加使用量（原子操作）
        
        Raises:
            QuotaExceeded: 配额超限时抛出
        """
        allowed, current, limit = self.check_quota(agent_id, quota_type, delta)
        
        if not allowed:
            raise QuotaExceeded(
                agent_id=agent_id,
                quota_type=quota_type,
                current=current,
                limit=limit,
            )
        
        self.record_usage(agent_id, quota_type, delta, token_delta)
    
    def reset_usage(self, agent_id: str) -> None:
        """重置agent的使用量"""
        with self._usage_lock:
            if agent_id in self._usage:
                usage = self._usage[agent_id]
                usage.message_count = 0
                usage.dialog_count = 0
                usage.token_usage = 0
                usage.window_start = time.time()
                usage.exceeded_at = None
                usage.warn_emitted_at = None
    
    def reset_all(self) -> None:
        """重置所有agent的使用量"""
        with self._usage_lock:
            for usage in self._usage.values():
                usage.message_count = 0
                usage.dialog_count = 0
                usage.token_usage = 0
                usage.window_start = time.time()
                usage.exceeded_at = None
                usage.warn_emitted_at = None
    
    @classmethod
    def reset_instance(cls) -> None:
        """重置单例（用于测试）"""
        with cls._lock:
            if cls._instance is not None:
                cls._instance._usage.clear()
                cls._instance._configs.clear()
                cls._instance._default_config = None
                cls._instance._notification_callbacks.clear()
                cls._instance._state_file = None
            cls._instance = None
        # 重置全局默认管理器
        global _default_manager
        _default_manager = None
    
    def get_status(self, agent_id: str) -> Dict:
        """获取agent的配额状态"""
        config = self._get_effective_config(agent_id)
        usage = self.get_usage(agent_id)
        
        if not config:
            return {"configured": False, "agent_id": agent_id}
        
        status = {
            "configured": True,
            "agent_id": agent_id,
            "message": {
                "current": usage.message_count,
                "limit": config.message_limit,
                "percentage": (usage.message_count / config.message_limit * 100) 
                              if config.message_limit else None,
            },
            "dialog": {
                "current": usage.dialog_count,
                "limit": config.dialog_limit,
                "percentage": (usage.dialog_count / config.dialog_limit * 100)
                              if config.dialog_limit else None,
            },
            "exceeded": usage.exceeded_at is not None,
            "last_activity": usage.last_activity,
        }
        
        return status
    
    def get_all_status(self) -> Dict[str, Dict]:
        """获取所有agent的配额状态"""
        with self._config_lock:
            agent_ids = set(self._configs.keys())
        with self._usage_lock:
            agent_ids.update(self._usage.keys())
        
        return {aid: self.get_status(aid) for aid in agent_ids}
    
    def is_exceeded(self, agent_id: str,
                    quota_type: QuotaType = QuotaType.MESSAGE_COUNT) -> bool:
        """检查是否已超限"""
        config = self._get_effective_config(agent_id)
        if not config:
            return False
        
        limit = config.get_limit(quota_type)
        if limit is None:
            return False
        
        usage = self.get_usage(agent_id)
        current = self._get_current_value(usage, quota_type)
        return current >= limit


# 默认实例
_default_manager: Optional[AgentQuotaManager] = None
_default_manager_lock = threading.Lock()


def get_default_manager() -> AgentQuotaManager:
    """获取默认配额管理器（线程安全单例）"""
    global _default_manager
    if _default_manager is None:
        with _default_manager_lock:
            if _default_manager is None:
                _default_manager = AgentQuotaManager()
    return _default_manager


def quota_protected(agent_id: str,
                   quota_type: QuotaType = QuotaType.MESSAGE_COUNT):
    """配额保护装饰器
    
    用于装饰需要配额检查的函数
    
    Example:
        @quota_protected("真显", QuotaType.MESSAGE_COUNT)
        def handle_message(message):
            ...
    """
    def decorator(func: Callable) -> Callable:
        def wrapper(*args, **kwargs):
            manager = get_default_manager()
            manager.check_and_increment(agent_id, quota_type)
            return func(*args, **kwargs)
        return wrapper
    return decorator


__all__ = [
    'QuotaType', 'AgentQuotaConfig', 'AgentUsage',
    'QuotaExceeded', 'QuotaWarning',
    'AgentQuotaManager', 'get_default_manager', 'quota_protected',
]
