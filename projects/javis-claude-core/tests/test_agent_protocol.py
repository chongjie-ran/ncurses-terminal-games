"""
test_agent_protocol.py - Agent间通信协议测试

测试覆盖：
1. AgentMessage 序列化和反序列化
2. 五种消息类型 (TASK/DIRECT/NOTIFICATION/RESULT/ERROR)
3. MessageFilter 匹配规则
4. AgentMessenger 发送/接收
5. MessageBus 发布/订阅
6. TaskContent / ResultContent / ErrorContent
"""

import pytest
import threading
import time
import queue
from datetime import datetime, timezone, timedelta
from unittest.mock import MagicMock, patch

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from src.agent_protocol import (
    AgentMessage, AgentMessageType, MessagePriority,
    TaskContent, ResultContent, ErrorContent,
    MessageFilter, AgentMessenger, MessageBus,
    create_task,
)


# ============================================================================
# Fixtures
# ============================================================================

@pytest.fixture(autouse=True)
def reset_message_bus():
    """每个测试前重置MessageBus单例"""
    MessageBus.reset_instance()
    yield
    MessageBus.reset_instance()


@pytest.fixture
def messenger_wu():
    """悟通的消息发送器"""
    m = AgentMessenger("悟通")
    yield m
    m.stop()


@pytest.fixture
def messenger_zx():
    """真显的消息发送器"""
    m = AgentMessenger("真显")
    yield m
    m.stop()


@pytest.fixture
def task_content():
    """标准任务内容"""
    return TaskContent(
        task_id="task-001",
        description="测试任务",
        input_data={"query": "hello"},
        tags=["test", "demo"],
        deadline=datetime.now(timezone.utc) + timedelta(hours=1),
    )


# ============================================================================
# AgentMessage Tests
# ============================================================================

class TestAgentMessage:
    
    def test_default_creation(self):
        """测试默认消息创建"""
        msg = AgentMessage()
        assert msg.msg_id is not None
        assert len(msg.msg_id) == 36  # UUID格式
        assert msg.msg_type == AgentMessageType.DIRECT
        assert msg.ttl == 3
        assert msg.priority == MessagePriority.NORMAL
    
    def test_full_creation(self):
        """测试完整参数创建"""
        now = datetime.now(timezone.utc)
        msg = AgentMessage(
            msg_id="test-123",
            msg_type=AgentMessageType.TASK,
            sender="悟通",
            receiver="真显",
            content={"key": "value"},
            session_id="sess-001",
            trace_id="trace-abc",
            timestamp=now,
            ttl=5,
            priority=MessagePriority.HIGH,
        )
        
        assert msg.msg_id == "test-123"
        assert msg.msg_type == AgentMessageType.TASK
        assert msg.sender == "悟通"
        assert msg.receiver == "真显"
        assert msg.content == {"key": "value"}
        assert msg.session_id == "sess-001"
        assert msg.trace_id == "trace-abc"
        assert msg.timestamp == now
        assert msg.ttl == 5
        assert msg.priority == MessagePriority.HIGH
    
    def test_is_broadcast(self):
        """测试广播判断"""
        broadcast_msg = AgentMessage(receiver="")
        assert broadcast_msg.is_broadcast() is True
        
        direct_msg = AgentMessage(receiver="真显")
        assert direct_msg.is_broadcast() is False
    
    def test_is_reply(self):
        """测试回复判断"""
        reply_msg = AgentMessage(reply_to="original-123")
        assert reply_msg.is_reply() is True
        
        normal_msg = AgentMessage()
        assert normal_msg.is_reply() is False
    
    def test_decrement_ttl(self):
        """测试TTL递减"""
        msg = AgentMessage()
        assert msg.ttl == 3
        
        assert msg.decrement_ttl() is True  # 3->2, had valid hop
        assert msg.ttl == 2
        
        assert msg.decrement_ttl() is True  # 2->1, had valid hop
        assert msg.ttl == 1
        
        assert msg.decrement_ttl() is True  # 1->0, had valid hop (last one)
        assert msg.ttl == 0
        
        assert msg.decrement_ttl() is False  # 0 is already dead, no decrement
        assert msg.ttl == 0
        assert msg.ttl == 0
    
    def test_serialization(self):
        """测试序列化/反序列化"""
        original = AgentMessage(
            msg_id="ser-001",
            msg_type=AgentMessageType.TASK,
            sender="悟通",
            receiver="真显",
            content={"data": [1, 2, 3], "nested": {"a": 1}},
            session_id="sess-x",
            trace_id="trace-y",
            priority=MessagePriority.URGENT,
        )
        
        data = original.to_dict()
        restored = AgentMessage.from_dict(data)
        
        assert restored.msg_id == original.msg_id
        assert restored.msg_type == original.msg_type
        assert restored.sender == original.sender
        assert restored.receiver == original.receiver
        assert restored.content == original.content
        assert restored.session_id == original.session_id
        assert restored.trace_id == original.trace_id
        assert restored.priority == original.priority
    
    def test_content_serialization_types(self):
        """测试各种内容类型的序列化"""
        test_cases = [
            None,
            "string",
            123,
            45.6,
            True,
            [1, "two", 3.0],
            {"key": "value", "num": 42},
        ]
        
        for content in test_cases:
            msg = AgentMessage(content=content)
            data = msg.to_dict()
            restored = AgentMessage.from_dict(data)
            assert restored.content == content, f"Failed for {content}"


# ============================================================================
# TaskContent / ResultContent / ErrorContent Tests
# ============================================================================

class TestContentStructs:
    
    def test_task_content(self):
        """测试TaskContent"""
        deadline = datetime.now(timezone.utc) + timedelta(hours=1)
        task = TaskContent(
            task_id="task-001",
            description="测试",
            input_data={"query": "test"},
            tags=["test"],
            deadline=deadline,
            subtask_of="parent-001",
        )
        
        assert task.task_id == "task-001"
        assert task.description == "测试"
        assert task.input_data == {"query": "test"}
        assert task.tags == ["test"]
        assert task.deadline == deadline
        assert task.subtask_of == "parent-001"
    
    def test_result_content(self):
        """测试ResultContent"""
        result = ResultContent(
            original_task_id="task-001",
            success=True,
            output_data={"answer": 42},
            execution_time_ms=150,
            artifacts={"file": "/path/to/output.txt"},
        )
        
        assert result.original_task_id == "task-001"
        assert result.success is True
        assert result.output_data == {"answer": 42}
        assert result.error_message is None
        assert result.execution_time_ms == 150
        assert result.artifacts == {"file": "/path/to/output.txt"}
    
    def test_error_content(self):
        """测试ErrorContent"""
        error = ErrorContent(
            original_task_id="task-001",
            error_code="TIMEOUT",
            error_message="Operation timed out",
            recoverable=True,
            retry_after_seconds=30,
        )
        
        assert error.original_task_id == "task-001"
        assert error.error_code == "TIMEOUT"
        assert error.error_message == "Operation timed out"
        assert error.recoverable is True
        assert error.retry_after_seconds == 30


# ============================================================================
# MessageFilter Tests
# ============================================================================

class TestMessageFilter:
    
    def test_no_filters_matches_all(self):
        """无过滤器匹配所有"""
        msg = AgentMessage()
        filter_pattern = MessageFilter()
        assert filter_pattern.matches(msg) is True
    
    def test_msg_type_filter(self):
        """测试消息类型过滤"""
        msg = AgentMessage(msg_type=AgentMessageType.TASK)
        
        # 匹配
        assert MessageFilter(msg_types={AgentMessageType.TASK}).matches(msg) is True
        
        # 不匹配
        assert MessageFilter(msg_types={AgentMessageType.DIRECT}).matches(msg) is False
        
        # 多类型
        assert MessageFilter(msg_types={AgentMessageType.TASK, AgentMessageType.DIRECT}).matches(msg) is True
    
    def test_sender_filter(self):
        """测试发送方过滤"""
        msg = AgentMessage(sender="悟通")
        
        assert MessageFilter(sender="悟通").matches(msg) is True
        assert MessageFilter(sender="真显").matches(msg) is False
    
    def test_receiver_filter(self):
        """测试接收方过滤"""
        msg = AgentMessage(receiver="真显")
        
        assert MessageFilter(receiver="真显").matches(msg) is True
        assert MessageFilter(receiver="悟通").matches(msg) is False
        
        # receiver=None表示不限制
        msg_broadcast = AgentMessage(receiver="")
        assert MessageFilter().matches(msg_broadcast) is True
    
    def test_session_filter(self):
        """测试Session过滤"""
        msg = AgentMessage(session_id="sess-001")
        
        assert MessageFilter(session_id="sess-001").matches(msg) is True
        assert MessageFilter(session_id="sess-002").matches(msg) is False
    
    def test_trace_filter(self):
        """测试Trace过滤"""
        msg = AgentMessage(trace_id="trace-abc")
        
        assert MessageFilter(trace_id="trace-abc").matches(msg) is True
        assert MessageFilter(trace_id="trace-xyz").matches(msg) is False
    
    def test_priority_filter(self):
        """测试优先级过滤"""
        low_msg = AgentMessage(priority=MessagePriority.LOW)
        high_msg = AgentMessage(priority=MessagePriority.HIGH)
        
        assert MessageFilter(min_priority=MessagePriority.LOW).matches(low_msg) is True
        assert MessageFilter(min_priority=MessagePriority.LOW).matches(high_msg) is True
        assert MessageFilter(min_priority=MessagePriority.HIGH).matches(low_msg) is False
        assert MessageFilter(min_priority=MessagePriority.HIGH).matches(high_msg) is True
    
    def test_combined_filters(self):
        """测试组合过滤器"""
        msg = AgentMessage(
            msg_type=AgentMessageType.TASK,
            sender="悟通",
            receiver="真显",
            session_id="sess-001",
            priority=MessagePriority.HIGH,
        )
        
        # 全部匹配
        f = MessageFilter(
            msg_types={AgentMessageType.TASK},
            sender="悟通",
            receiver="真显",
            session_id="sess-001",
            min_priority=MessagePriority.HIGH,
        )
        assert f.matches(msg) is True
        
        # 一个不匹配
        f2 = MessageFilter(
            msg_types={AgentMessageType.TASK},
            sender="悟通",
            receiver="真显",
            session_id="sess-999",  # 不匹配
        )
        assert f2.matches(msg) is False


# ============================================================================
# AgentMessenger Tests
# ============================================================================

class TestAgentMessenger:
    
    def test_creation(self):
        """测试创建"""
        m = AgentMessenger("悟通")
        assert m.agent_id == "悟通"
        assert len(m._handlers) == 0
        assert m._stats["sent"] == 0
        assert m._stats["received"] == 0
    
    def test_register_handler(self):
        """测试注册处理器"""
        m = AgentMessenger("悟通")
        
        handler = MagicMock()
        filter_pattern = MessageFilter(msg_types={AgentMessageType.TASK})
        
        m.register_handler(filter_pattern, handler)
        assert len(m._handlers) == 1
        
        m.unregister_handler(handler)
        assert len(m._handlers) == 0
    
    def test_send_direct_message(self):
        """测试发送Direct消息"""
        m = AgentMessenger("悟通")
        
        with patch.object(MessageBus.get_instance(), 'publish') as mock_publish:
            result = m.send_direct(receiver="真显", content="hello")
            
            assert result is True
            mock_publish.assert_called_once()
            
            msg = mock_publish.call_args[0][0]
            assert msg.msg_type == AgentMessageType.DIRECT
            assert msg.sender == "悟通"
            assert msg.receiver == "真显"
            assert msg.content == "hello"
    
    def test_send_task_message(self):
        """测试发送Task消息"""
        m = AgentMessenger("悟通")
        task = create_task("task-001", "测试任务", {"query": "test"})
        
        with patch.object(MessageBus.get_instance(), 'publish') as mock_publish:
            result = m.send_task(receiver="真显", task_content=task, timeout=1.0)
            
            assert result is None  # 同步等待会超时（没有reply）
            
            msg = mock_publish.call_args[0][0]
            assert msg.msg_type == AgentMessageType.TASK
            assert msg.sender == "悟通"
            assert msg.receiver == "真显"
            assert msg.content["task_id"] == "task-001"
    
    def test_send_notification(self):
        """测试发送通知"""
        m = AgentMessenger("悟通")
        
        with patch.object(MessageBus.get_instance(), 'publish') as mock_publish:
            # 广播
            result = m.send_notification(content={"event": "status_update"})
            msg = mock_publish.call_args[0][0]
            assert msg.msg_type == AgentMessageType.NOTIFICATION
            assert msg.receiver == ""
            
            # 定向
            result2 = m.send_notification(receiver="真显", content="注意")
            msg2 = mock_publish.call_args[0][0]
            assert msg2.receiver == "真显"
    
    def test_send_result(self):
        """测试发送Result消息"""
        m = AgentMessenger("悟通")
        
        with patch.object(MessageBus.get_instance(), 'publish') as mock_publish:
            result = m.send_result(
                original_msg_id="task-001",
                output_data={"answer": 42},
                execution_time_ms=100,
            )
            
            assert result is True
            msg = mock_publish.call_args[0][0]
            assert msg.msg_type == AgentMessageType.RESULT
            assert msg.reply_to == "task-001"
            assert msg.content["success"] is True
            assert msg.content["output_data"] == {"answer": 42}
    
    def test_send_error(self):
        """测试发送Error消息"""
        m = AgentMessenger("悟通")
        
        with patch.object(MessageBus.get_instance(), 'publish') as mock_publish:
            result = m.send_error(
                receiver="真显",
                error_code="TIMEOUT",
                error_message="Operation timed out",
                original_task_id="task-001",
            )
            
            assert result is True
            msg = mock_publish.call_args[0][0]
            assert msg.msg_type == AgentMessageType.ERROR
            assert msg.receiver == "真显"
            assert msg.content["error_code"] == "TIMEOUT"
    
    def test_receive_broadcast(self):
        """测试接收广播消息"""
        m = AgentMessenger("真显")
        m._running = True
        
        received = []
        def handler(msg):
            received.append(msg)
        
        m.register_handler(MessageFilter(), handler)
        
        msg = AgentMessage(
            msg_type=AgentMessageType.NOTIFICATION,
            sender="悟通",
            receiver="",  # 广播
            content="hello",
        )
        
        m.receive(msg)
        assert len(received) == 1
        assert received[0].content == "hello"
    
    def test_receive_direct_message(self):
        """测试接收定向消息"""
        m = AgentMessenger("真显")
        m._running = True
        
        received = []
        def handler(msg):
            received.append(msg)
        
        m.register_handler(MessageFilter(), handler)
        
        msg = AgentMessage(
            msg_type=AgentMessageType.DIRECT,
            sender="悟通",
            receiver="真显",  # 定向
            content="private",
        )
        
        m.receive(msg)
        assert len(received) == 1
        assert received[0].content == "private"
    
    def test_receive_ignores_other_agent(self):
        """测试忽略非本Agent的消息"""
        m = AgentMessenger("真显")
        m._running = True
        
        received = []
        def handler(msg):
            received.append(msg)
        
        m.register_handler(MessageFilter(), handler)
        
        msg = AgentMessage(
            msg_type=AgentMessageType.DIRECT,
            sender="悟通",
            receiver="悟通",  # 不是发给真显
            content="not for me",
        )
        
        m.receive(msg)
        assert len(received) == 0  # 应该被忽略
    
    def test_start_stop(self):
        """测试启动/停止"""
        m = AgentMessenger("悟通")
        
        with patch.object(MessageBus.get_instance(), 'subscribe') as mock_sub, \
             patch.object(MessageBus.get_instance(), 'unsubscribe') as mock_unsub:
            
            m.start()
            mock_sub.assert_called_once()
            
            m.stop()
            mock_unsub.assert_called_once()
    
    def test_get_stats(self):
        """测试统计信息"""
        m = AgentMessenger("悟通")
        
        stats = m.get_stats()
        assert "sent" in stats
        assert "received" in stats
        assert "broadcasts" in stats
        assert "errors" in stats


# ============================================================================
# MessageBus Tests
# ============================================================================

class TestMessageBus:
    
    def test_singleton(self):
        """测试单例模式"""
        bus1 = MessageBus.get_instance()
        bus2 = MessageBus.get_instance()
        assert bus1 is bus2
    
    def test_subscribe_unsubscribe(self):
        """测试订阅/取消订阅"""
        bus = MessageBus.get_instance()
        callback = MagicMock()
        
        bus.subscribe("悟通", callback)
        assert "悟通" in bus._subscribers
        
        bus.unsubscribe("悟通")
        assert "悟通" not in bus._subscribers
    
    def test_publish_to_subscriber(self):
        """测试发布到订阅者"""
        bus = MessageBus.get_instance()
        callback = MagicMock()
        bus.subscribe("真显", callback)
        
        msg = AgentMessage(
            msg_type=AgentMessageType.DIRECT,
            sender="悟通",
            receiver="真显",
            content="test",
        )
        bus.publish(msg)
        
        callback.assert_called_once()
        called_msg = callback.call_args[0][0]
        assert called_msg.content == "test"
    
    def test_publish_broadcast(self):
        """测试广播"""
        bus = MessageBus.get_instance()
        callback1 = MagicMock()
        callback2 = MagicMock()
        bus.subscribe("悟通", callback1)
        bus.subscribe("真显", callback2)
        
        msg = AgentMessage(
            msg_type=AgentMessageType.NOTIFICATION,
            sender="SC",
            receiver="",  # 广播
            content="announcement",
        )
        bus.publish(msg)
        
        # 两个订阅者都应该收到
        callback1.assert_called_once()
        callback2.assert_called_once()
    
    def test_publish_no_subscriber(self):
        """测试发布到不存在的订阅者"""
        bus = MessageBus.get_instance()
        
        msg = AgentMessage(
            msg_type=AgentMessageType.DIRECT,
            sender="悟通",
            receiver="不存在的Agent",
            content="test",
        )
        # 不应该抛出异常，只是记录warning
        bus.publish(msg)
    
    def test_history(self):
        """测试消息历史"""
        bus = MessageBus.get_instance()
        bus._history_max = 10  # 小历史方便测试
        
        # 发送多条消息
        for i in range(5):
            msg = AgentMessage(
                msg_type=AgentMessageType.DIRECT,
                sender="悟通",
                receiver="真显",
                content=f"msg-{i}",
                session_id="sess-001",
            )
            bus.publish(msg)
        
        # 获取全部历史
        history = bus.get_history()
        assert len(history) == 5
        
        # 按session过滤
        history = bus.get_history(session_id="sess-001")
        assert len(history) == 5
        
        history = bus.get_history(session_id="sess-999")
        assert len(history) == 0
        
        # 按limit
        history = bus.get_history(limit=3)
        assert len(history) == 3


# ============================================================================
# Helper Function Tests
# ============================================================================

class TestHelpers:
    
    def test_create_task(self):
        """测试create_task辅助函数"""
        task = create_task(
            task_id="task-001",
            description="测试任务",
            input_data={"query": "hello"},
            tags=["test"],
        )
        
        assert isinstance(task, TaskContent)
        assert task.task_id == "task-001"
        assert task.description == "测试任务"
        assert task.input_data == {"query": "hello"}
        assert task.tags == ["test"]
        assert task.deadline is None
        assert task.subtask_of is None


# ============================================================================
# Integration Tests
# ============================================================================

class TestIntegration:
    """端到端集成测试"""
    
    def test_two_agent_communication(self):
        """测试两个Agent之间的通信"""
        # 准备两个messenger
        wu = AgentMessenger("悟通")
        zx = AgentMessenger("真显")
        
        received_by_zx = []
        def handler_zx(msg):
            received_by_zx.append(msg)
            # 模拟真显处理TASK并返回RESULT
            if msg.msg_type == AgentMessageType.TASK:
                result = ResultContent(
                    original_task_id=msg.msg_id,
                    success=True,
                    output_data={"processed": True},
                    execution_time_ms=50,
                )
                reply = AgentMessage(
                    msg_type=AgentMessageType.RESULT,
                    sender="真显",
                    reply_to=msg.msg_id,
                    content=result.__dict__,
                )
                wu.receive(reply)
        
        zx.register_handler(MessageFilter(msg_types={AgentMessageType.TASK}), handler_zx)
        
        # 启动
        wu.start()
        zx.start()
        
        # 悟通发送任务
        task = create_task("task-001", "处理数据", {"data": [1, 2, 3]})
        task_msg = AgentMessage(
            msg_type=AgentMessageType.TASK,
            sender="悟通",
            receiver="真显",
            content=task.__dict__,
        )
        wu.send(task_msg)
        
        # 等待消息传递（实际上需要异步处理）
        time.sleep(0.1)
        
        # 停止
        wu.stop()
        zx.stop()
        
        # 验证：真显应该收到任务
        assert len(received_by_zx) >= 1
    
    def test_broadcast_notification(self):
        """测试广播通知"""
        # 准备多个订阅者
        agent_a = AgentMessenger("AgentA")
        agent_b = AgentMessenger("AgentB")
        agent_c = AgentMessenger("AgentC")
        
        received_a = []
        received_b = []
        received_c = []
        
        def handler_a(msg):
            received_a.append(msg)
        def handler_b(msg):
            received_b.append(msg)
        def handler_c(msg):
            received_c.append(msg)
        
        agent_a.register_handler(MessageFilter(), handler_a)
        agent_b.register_handler(MessageFilter(), handler_b)
        agent_c.register_handler(MessageFilter(), handler_c)
        
        # 启动
        for m in [agent_a, agent_b, agent_c]:
            m.start()
        
        # SC也需要启动才能订阅MessageBus
        sc = AgentMessenger("SC")
        sc.start()
        
        # SC广播通知
        sc.send_notification(
            content={"event": "shutdown", "reason": "maintenance"},
            priority=MessagePriority.HIGH,
        )
        
        time.sleep(0.2)
        
        # 停止
        sc.stop()
        for m in [agent_a, agent_b, agent_c]:
            m.stop()
        
        # 所有Agent都应该收到广播
        assert len(received_a) >= 1
        assert len(received_b) >= 1
        assert len(received_c) >= 1
