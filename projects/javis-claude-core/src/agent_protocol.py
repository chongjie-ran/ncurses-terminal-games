"""
agent_protocol.py - Agent间通信协议

P1-4 实现 (2026-04-20)
核心功能：
- AgentMessage: 标准化的Agent↔Agent消息格式
- AgentMessageType: TASK / DIRECT / NOTIFICATION / RESULT / ERROR 五种模式
- AgentMessenger: 消息发送/接收/路由
- MessageBus: 消息总线（支持广播和点对点）

参考：A2A协议 + MCP协议设计
"""

from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum
from typing import Optional, Dict, List, Callable, Any, Set
from datetime import datetime, timezone
import uuid
import threading
import logging
import json
import queue
import time

logger = logging.getLogger(__name__)


# ============================================================================
# Message Types
# ============================================================================

class AgentMessageType(Enum):
    """Agent消息类型"""
    TASK = "task"           # 分发任务（期望接收方执行并返回结果）
    DIRECT = "direct"       # 直接消息（期望接收方处理，无需返回）
    NOTIFICATION = "notify" # 通知（广播或定向，无需回复）
    RESULT = "result"       # 结果返回（响应TASK消息）
    ERROR = "error"         # 错误报告（响应TASK消息或主动错误通知)


class MessagePriority(Enum):
    """消息优先级"""
    LOW = 0
    NORMAL = 1
    HIGH = 2
    URGENT = 3


# ============================================================================
# Message Format
# ============================================================================

@dataclass
class AgentMessage:
    """
    标准Agent消息格式
    
    Attributes:
        msg_id: 全局唯一消息ID
        msg_type: 消息类型
        sender: 发送方Agent ID
        receiver: 接收方Agent ID（空字符串=广播）
        content: 消息内容（任意类型）
        session_id: 关联的Session ID（用于上下文关联）
        trace_id: 追踪ID（跨Agent传递，用于调试）
        timestamp: 消息时间戳（UTC，timezone-aware）
        ttl: 跳数限制（防止消息循环传播）
        priority: 消息优先级
        reply_to: 要回复的消息ID（用于RESULT/ERROR关联TASK）
        metadata: 额外元数据
    """
    msg_id: str = field(default_factory=lambda: str(uuid.uuid4()))
    msg_type: AgentMessageType = AgentMessageType.DIRECT
    sender: str = ""
    receiver: str = ""  # 空=广播
    content: Any = None
    session_id: str = ""
    trace_id: str = ""
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))
    ttl: int = 3  # 跳数限制
    priority: MessagePriority = MessagePriority.NORMAL
    reply_to: Optional[str] = None  # 关联的原始消息ID
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def is_broadcast(self) -> bool:
        """是否为广播消息"""
        return not bool(self.receiver)
    
    def is_reply(self) -> bool:
        """是否为回复消息"""
        return self.reply_to is not None
    
    def decrement_ttl(self) -> bool:
        """递减TTL，返回是否还有有效跳数（调用前ttl>=1时返回True）"""
        if self.ttl <= 0:
            return False
        self.ttl -= 1
        return True
    
    def to_dict(self) -> Dict[str, Any]:
        """序列化为字典"""
        return {
            "msg_id": self.msg_id,
            "msg_type": self.msg_type.value,
            "sender": self.sender,
            "receiver": self.receiver,
            "content": self._serialize_content(self.content),
            "session_id": self.session_id,
            "trace_id": self.trace_id,
            "timestamp": self.timestamp.isoformat(),
            "ttl": self.ttl,
            "priority": self.priority.value,
            "reply_to": self.reply_to,
            "metadata": self.metadata,
        }
    
    def _serialize_content(self, content: Any) -> Any:
        """序列化内容（支持复杂类型）"""
        if content is None:
            return None
        if isinstance(content, (str, int, float, bool)):
            return content
        if isinstance(content, (list, tuple)):
            return [self._serialize_content(c) for c in content]
        if isinstance(content, dict):
            return {k: self._serialize_content(v) for k, v in content.items()}
        # 对于无法序列化的对象，转为字符串
        return str(content)
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "AgentMessage":
        """从字典反序列化"""
        msg_type = AgentMessageType(data.get("msg_type", "direct"))
        timestamp = data.get("timestamp")
        if isinstance(timestamp, str):
            timestamp = datetime.fromisoformat(timestamp.replace("Z", "+00:00"))
        else:
            timestamp = datetime.now(timezone.utc)
        
        priority = data.get("priority", 1)
        if isinstance(priority, str):
            priority = MessagePriority[priority.upper()].value
        elif isinstance(priority, int):
            priority = MessagePriority(priority).value
        
        return cls(
            msg_id=data.get("msg_id", str(uuid.uuid4())),
            msg_type=msg_type,
            sender=data.get("sender", ""),
            receiver=data.get("receiver", ""),
            content=data.get("content"),
            session_id=data.get("session_id", ""),
            trace_id=data.get("trace_id", ""),
            timestamp=timestamp,
            ttl=data.get("ttl", 3),
            priority=MessagePriority(priority),
            reply_to=data.get("reply_to"),
            metadata=data.get("metadata", {}),
        )


# ============================================================================
# Task Message Helpers
# ============================================================================

@dataclass
class TaskContent:
    """Task类型消息的内容结构"""
    task_id: str  # 任务唯一ID
    description: str  # 任务描述
    input_data: Any = None  # 输入数据
    expected_output: Optional[str] = None  # 期望输出类型
    deadline: Optional[datetime] = None  # 截止时间
    subtask_of: Optional[str] = None  # 父任务ID（如果是子任务）
    tags: List[str] = field(default_factory=list)  # 任务标签


@dataclass
class ResultContent:
    """Result类型消息的内容结构"""
    original_task_id: str  # 对应的原始任务ID
    success: bool
    output_data: Any = None
    error_message: Optional[str] = None
    execution_time_ms: int = 0
    artifacts: Dict[str, Any] = field(default_factory=dict)  # 产出物


@dataclass
class ErrorContent:
    """Error类型消息的内容结构"""
    error_code: str  # 错误码
    error_message: str
    original_task_id: Optional[str] = None
    recoverable: bool = True
    retry_after_seconds: Optional[int] = None


# ============================================================================
# Message Filter
# ============================================================================

@dataclass
class MessageFilter:
    """消息过滤器"""
    msg_types: Optional[Set[AgentMessageType]] = None
    sender: Optional[str] = None
    receiver: Optional[str] = None  # 空=None表示不限制
    session_id: Optional[str] = None
    trace_id: Optional[str] = None
    min_priority: MessagePriority = MessagePriority.LOW
    
    def matches(self, msg: AgentMessage) -> bool:
        """检查消息是否匹配过滤器"""
        # 消息类型过滤
        if self.msg_types and msg.msg_type not in self.msg_types:
            return False
        
        # 发送方过滤
        if self.sender and msg.sender != self.sender:
            return False
        
        # 接收方过滤（仅当filter指定了receiver且消息不是广播时）
        if self.receiver and msg.receiver and msg.receiver != self.receiver:
            return False
        
        # Session过滤
        if self.session_id and msg.session_id != self.session_id:
            return False
        
        # Trace过滤
        if self.trace_id and msg.trace_id != self.trace_id:
            return False
        
        # 优先级过滤
        if msg.priority.value < self.min_priority.value:
            return False
        
        return True


# ============================================================================
# Message Handler
# ============================================================================

MessageHandler = Callable[[AgentMessage], Optional[AgentMessage]]
"""
消息处理器类型

接收消息，返回可选的回复消息（用于TASK类型的自动回复）
返回None表示不自动回复
"""


# ============================================================================
# AgentMessenger
# ============================================================================

class AgentMessenger:
    """
    Agent消息发送器（线程安全）
    
    负责：
    1. 构造和发送消息
    2. 接收并路由消息
    3. 管理消息处理器
    
    使用方式:
        messenger = AgentMessenger(agent_id="悟通")
        messenger.register_handler(MessageFilter(msg_types={TASK}), handle_task)
        messenger.send_task(receiver="真显", task=...)
        messenger.send_notification(broadcast=True, content="状态更新")
    """
    
    def __init__(self, agent_id: str):
        self.agent_id = agent_id
        self._handlers: List[tuple[MessageFilter, MessageHandler]] = []
        self._inbox: queue.Queue = queue.Queue()
        self._inbox_lock = threading.Lock()
        self._running = False
        self._receive_thread: Optional[threading.Thread] = None
        self._pending_messages: Dict[str, AgentMessage] = {}  # 等待回复的消息
        self._pending_lock = threading.Lock()
        self._reply_timeout = 300.0  # 5分钟超时
        
        # 统计
        self._stats = {
            "sent": 0,
            "received": 0,
            "broadcasts": 0,
            "errors": 0,
        }
        self._stats_lock = threading.Lock()
    
    # ========================================================================
    # Handler Registration
    # ========================================================================
    
    def register_handler(self, filter: MessageFilter, handler: MessageHandler) -> None:
        """注册消息处理器"""
        self._handlers.append((filter, handler))
        logger.warning(f"Registered handler for {filter.msg_types or 'all'} messages")
    
    def unregister_handler(self, handler: MessageHandler) -> None:
        """注销消息处理器"""
        self._handlers = [(f, h) for f, h in self._handlers if h != handler]
    
    # ========================================================================
    # Message Sending
    # ========================================================================
    
    def send(self, msg: AgentMessage) -> bool:
        """
        发送消息
        
        Args:
            msg: 要发送的消息
            
        Returns:
            bool: 是否成功（发送到总线）
        """
        # 自动填充发送方
        if not msg.sender:
            msg.sender = self.agent_id
        
        # TTL检查
        if msg.ttl <= 0:
            logger.warning(f"Message {msg.msg_id} TTL exhausted, dropping")
            return False
        
        try:
            MessageBus.get_instance().publish(msg)
            
            with self._stats_lock:
                self._stats["sent"] += 1
                if msg.is_broadcast():
                    self._stats["broadcasts"] += 1
            
            logger.warning(
                f"[{self.agent_id}] Sent {msg.msg_type.value} to "
                f"{msg.receiver or 'BROADCAST'}: {msg.msg_id[:8]}"
            )
            return True
            
        except Exception as e:
            logger.error(f"Failed to send message: {e}")
            with self._stats_lock:
                self._stats["errors"] += 1
            return False
    
    def send_task(
        self,
        receiver: str,
        task_content: TaskContent,
        trace_id: Optional[str] = None,
        session_id: str = "",
        timeout: Optional[float] = None,
    ) -> Optional[AgentMessage]:
        """
        发送Task消息并等待结果
        
        Args:
            receiver: 接收方Agent ID
            task_content: 任务内容
            trace_id: 追踪ID
            session_id: Session ID
            timeout: 等待超时（秒）
            
        Returns:
            Result消息或None（超时/失败）
        """
        msg = AgentMessage(
            msg_type=AgentMessageType.TASK,
            sender=self.agent_id,
            receiver=receiver,
            content=task_content.__dict__,
            session_id=session_id,
            trace_id=trace_id or self._generate_trace_id(),
        )
        
        if not self.send(msg):
            return None
        
        # 等待回复
        with self._pending_lock:
            self._pending_messages[msg.msg_id] = msg
        
        try:
            reply = self._wait_for_reply(msg.msg_id, timeout or self._reply_timeout)
            return reply
        finally:
            with self._pending_lock:
                self._pending_messages.pop(msg.msg_id, None)
    
    def send_direct(
        self,
        receiver: str,
        content: Any,
        trace_id: Optional[str] = None,
        session_id: str = "",
    ) -> bool:
        """发送Direct消息"""
        msg = AgentMessage(
            msg_type=AgentMessageType.DIRECT,
            sender=self.agent_id,
            receiver=receiver,
            content=content,
            session_id=session_id,
            trace_id=trace_id or "",
        )
        return self.send(msg)
    
    def send_notification(
        self,
        content: Any,
        receiver: str = "",
        trace_id: Optional[str] = None,
        session_id: str = "",
        priority: MessagePriority = MessagePriority.NORMAL,
    ) -> bool:
        """
        发送通知
        
        Args:
            content: 通知内容
            receiver: 接收方（空=广播）
            trace_id: 追踪ID
            session_id: Session ID
            priority: 优先级
        """
        msg = AgentMessage(
            msg_type=AgentMessageType.NOTIFICATION,
            sender=self.agent_id,
            receiver=receiver,
            content=content,
            session_id=session_id,
            trace_id=trace_id or "",
            priority=priority,
        )
        return self.send(msg)
    
    def send_result(
        self,
        original_msg_id: str,
        output_data: Any,
        execution_time_ms: int = 0,
        artifacts: Optional[Dict[str, Any]] = None,
        error: Optional[str] = None,
    ) -> bool:
        """
        发送Result消息
        
        Args:
            original_msg_id: 原始Task消息的ID
            output_data: 输出数据
            execution_time_ms: 执行时间（毫秒）
            artifacts: 产出物
            error: 错误信息（如果有）
        """
        success = error is None
        content = ResultContent(
            original_task_id=original_msg_id,
            success=success,
            output_data=output_data if success else None,
            error_message=error,
            execution_time_ms=execution_time_ms,
            artifacts=artifacts or {},
        )
        
        msg = AgentMessage(
            msg_type=AgentMessageType.RESULT,
            sender=self.agent_id,
            reply_to=original_msg_id,
            content=content.__dict__,
        )
        return self.send(msg)
    
    def send_error(
        self,
        receiver: str,
        error_code: str,
        error_message: str,
        original_task_id: Optional[str] = None,
        recoverable: bool = True,
        retry_after_seconds: Optional[int] = None,
    ) -> bool:
        """
        发送Error消息
        
        Args:
            receiver: 接收方Agent ID
            error_code: 错误码
            error_message: 错误信息
            original_task_id: 关联的原始Task ID
            recoverable: 是否可恢复
            retry_after_seconds: 重试间隔（秒）
        """
        content = ErrorContent(
            original_task_id=original_task_id,
            error_code=error_code,
            error_message=error_message,
            recoverable=recoverable,
            retry_after_seconds=retry_after_seconds,
        )
        
        msg = AgentMessage(
            msg_type=AgentMessageType.ERROR,
            sender=self.agent_id,
            receiver=receiver,
            content=content.__dict__,
            reply_to=original_task_id,
        )
        return self.send(msg)
    
    # ========================================================================
    # Message Receiving
    # ========================================================================
    
    def receive(self, msg: AgentMessage) -> None:
        """接收消息（由MessageBus调用）"""
        # 检查TTL
        if not msg.decrement_ttl():
            logger.warning(f"Message {msg.msg_id} TTL expired, dropping")
            return
        
        # 检查是否接收方是自己（点对点）或广播
        if msg.receiver and msg.receiver != self.agent_id:
            logger.warning(f"Message {msg.msg_id} not for me ({msg.receiver}), dropping")
            return
        
        with self._stats_lock:
            self._stats["received"] += 1
        
        # 检查是否有等待中的pending消息（自动处理RESULT/ERROR）
        if msg.reply_to:
            with self._pending_lock:
                pending = self._pending_messages.get(msg.reply_to)
                if pending:
                    # 设置事件
                    pending._reply_event.set()
                    pending._reply_message = msg
                    return  # 不走handler
        
        # 放入收件箱
        self._inbox.put(msg)
        
        # 异步处理（如果running）
        if self._running:
            self._process_inbox_async()
    
    def _process_inbox_async(self) -> None:
        """异步处理收件箱"""
        if self._receive_thread and self._receive_thread.is_alive():
            return
        
        self._receive_thread = threading.Thread(
            target=self._process_inbox,
            daemon=True,
            name=f"Messenger-{self.agent_id}",
        )
        self._receive_thread.start()
    
    def _process_inbox(self) -> None:
        """处理收件箱中的消息"""
        while self._running:
            try:
                msg = self._inbox.get(timeout=0.1)
            except queue.Empty:
                continue
            
            try:
                self._dispatch_to_handlers(msg)
            except Exception as e:
                logger.error(f"Handler error for {msg.msg_id}: {e}")
    
    def _dispatch_to_handlers(self, msg: AgentMessage) -> None:
        """分发给匹配的处理器"""
        for filter_pattern, handler in self._handlers:
            if filter_pattern.matches(msg):
                try:
                    reply = handler(msg)
                    if reply:
                        self.send(reply)
                except Exception as e:
                    logger.error(f"Handler exception: {e}")
    
    def start(self) -> None:
        """启动消息接收"""
        if self._running:
            return
        self._running = True
        MessageBus.get_instance().subscribe(self.agent_id, self.receive)
        self._process_inbox_async()
        logger.info(f"[{self.agent_id}] Messenger started")
    
    def stop(self) -> None:
        """停止消息接收"""
        self._running = False
        MessageBus.get_instance().unsubscribe(self.agent_id)
        logger.info(f"[{self.agent_id}] Messenger stopped")
    
    # ========================================================================
    # Utilities
    # ========================================================================
    
    def _generate_trace_id(self) -> str:
        """生成追踪ID"""
        return f"{self.agent_id}-{int(time.time() * 1000)}"
    
    def _wait_for_reply(self, original_msg_id: str, timeout: float) -> Optional[AgentMessage]:
        """等待回复（用于send_task的同步等待）"""
        # 创建一个等待事件
        event = threading.Event()
        
        # 临时存储
        reply_msg = [None]  # 使用list以便在闭包中修改
        
        def reply_handler(msg: AgentMessage) -> None:
            if msg.reply_to == original_msg_id:
                reply_msg[0] = msg
                event.set()
        
        # 临时添加reply handler
        original_handlers = self._handlers[:]
        
        # 注册临时handler
        self.register_handler(
            MessageFilter(),
            lambda m: (reply_handler(m), None)[1]
        )
        
        try:
            event.wait(timeout=timeout)
            return reply_msg[0]
        except:
            return None
        finally:
            # 恢复原始handlers
            self._handlers[:] = original_handlers
    
    def get_stats(self) -> Dict[str, int]:
        """获取统计信息"""
        with self._stats_lock:
            return dict(self._stats)
    
    def __repr__(self) -> str:
        return f"AgentMessenger(agent_id={self.agent_id!r}, handlers={len(self._handlers)})"


# ============================================================================
# Message Bus (Singleton)
# ============================================================================

class MessageBus:
    """
    消息总线（单例模式，线程安全）
    
    负责：
    1. 消息发布/订阅
    2. 消息广播
    3. 消息持久化（可选）
    
    使用方式:
        bus = MessageBus.get_instance()
        bus.subscribe("悟通", callback)
        bus.publish(message)
    """
    
    _instance: Optional["MessageBus"] = None
    _lock = threading.Lock()
    
    def __new__(cls):
        if cls._instance is None:
            with cls._lock:
                if cls._instance is None:
                    cls._instance = super().__new__(cls)
                    cls._instance._init()
        return cls._instance
    
    def _init(self) -> None:
        """初始化"""
        self._subscribers: Dict[str, Callable[[AgentMessage], None]] = {}
        self._subscriber_lock = threading.Lock()
        self._message_history: List[AgentMessage] = []
        self._history_max = 1000  # 保留最近1000条
        self._history_lock = threading.Lock()
        self._bus_lock = threading.Lock()  # 消息发送锁
    
    @classmethod
    def get_instance(cls) -> "MessageBus":
        """获取单例实例"""
        return cls()
    
    @classmethod
    def reset_instance(cls) -> None:
        """重置单例（用于测试）"""
        with cls._lock:
            if cls._instance is not None:
                cls._instance._subscribers.clear()
                cls._instance._message_history.clear()
            cls._instance = None
    
    def subscribe(self, agent_id: str, callback: Callable[[AgentMessage], None]) -> None:
        """订阅消息"""
        with self._subscriber_lock:
            self._subscribers[agent_id] = callback
        logger.warning(f"Subscribed: {agent_id}")
    
    def unsubscribe(self, agent_id: str) -> None:
        """取消订阅"""
        with self._subscriber_lock:
            self._subscribers.pop(agent_id, None)
        logger.warning(f"Unsubscribed: {agent_id}")
    
    def publish(self, msg: AgentMessage) -> None:
        """
        发布消息到总线
        
        消息会：
        1. 被投递到接收方（点对点或广播）
        2. 被记录到历史
        """
        with self._bus_lock:
            # 记录历史
            self._add_to_history(msg)
            
            # 获取订阅者副本
            with self._subscriber_lock:
                subscribers = dict(self._subscribers)
            
            # 投递消息
            if msg.is_broadcast():
                # 广播：发给所有订阅者（除发送方）
                for agent_id, callback in subscribers.items():
                    if agent_id != msg.sender:
                        try:
                            callback(msg)
                        except Exception as e:
                            logger.error(f"Broadcast to {agent_id} failed: {e}")
            else:
                # 点对点：只发给接收方
                callback = subscribers.get(msg.receiver)
                if callback:
                    try:
                        callback(msg)
                    except Exception as e:
                        logger.error(f"Deliver to {msg.receiver} failed: {e}")
                else:
                    logger.warning(f"No subscriber for {msg.receiver}, message dropped")
    
    def _add_to_history(self, msg: AgentMessage) -> None:
        """添加消息到历史"""
        with self._history_lock:
            self._message_history.append(msg)
            # 限制历史长度
            if len(self._message_history) > self._history_max:
                self._message_history = self._message_history[-self._history_max:]
    
    def get_history(
        self,
        session_id: Optional[str] = None,
        limit: int = 100,
    ) -> List[AgentMessage]:
        """获取消息历史"""
        with self._history_lock:
            if session_id:
                filtered = [m for m in self._message_history if m.session_id == session_id]
                return filtered[-limit:]
            return self._message_history[-limit:]
    
    def clear_history(self) -> None:
        """清空历史"""
        with self._history_lock:
            self._message_history.clear()


# ============================================================================
# Task Builder (Helper)
# ============================================================================

def create_task(
    task_id: str,
    description: str,
    input_data: Any = None,
    tags: Optional[List[str]] = None,
    deadline: Optional[datetime] = None,
) -> TaskContent:
    """创建TaskContent的便捷函数"""
    return TaskContent(
        task_id=task_id,
        description=description,
        input_data=input_data,
        tags=tags or [],
        deadline=deadline,
    )


# ============================================================================
# Exports
# ============================================================================

__all__ = [
    # Core classes
    'AgentMessage', 'AgentMessageType', 'MessagePriority',
    'TaskContent', 'ResultContent', 'ErrorContent',
    'MessageFilter', 'MessageHandler',
    'AgentMessenger', 'MessageBus',
    # Helper functions
    'create_task',
    # Constants
    'DEFAULT_TRACE_ID_PREFIX',
]

# Agent ID constants for common agents
DEFAULT_TRACE_ID_PREFIX = "javis"
