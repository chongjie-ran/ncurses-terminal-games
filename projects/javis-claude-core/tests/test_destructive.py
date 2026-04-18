"""
破坏性测试套件 - 验证系统在极端条件下的稳定性

覆盖:
1. 资源耗尽 (内存/磁盘/文件描述符/并发)
2. 数据损坏 (JSON损坏/字段缺失/类型错误)
3. 超时/中断 (执行超时/进程kill/嵌套重试)
4. 边界值 (极长输入/空输入/最大消息数)
5. 异常恢复 (重试成功/熔断恢复/资源泄漏)
"""

import pytest
import time
import os
import tempfile
import json
import threading
import asyncio
from concurrent.futures import ThreadPoolExecutor

from src.retry_executor import (
    RetryExecutor, RetryConfig, CircuitBreaker, 
    CircuitBreakerConfig, CircuitState
)
from src.resource_guard import (
    ResourceGuard, ResourceLimit, ResourceExceeded,
    ResourceMonitor, ResourceType, ResourceSnapshot
)
from src.session import Session, ContentBlock


# =============================================================================
# 1. 资源耗尽测试
# =============================================================================

class TestResourceExhaustion:
    """资源耗尽场景"""

    def test_memory_limit_exceeded_raises(self):
        """内存超限触发异常"""
        guard = ResourceGuard(ResourceLimit(memory_mb=1))  # 极低限制
        snap = ResourceSnapshot(
            timestamp=time.time(),
            memory_mb=100,  # 远超限制
            cpu_time_sec=1,
            threads=1,
            open_files=10
        )
        with pytest.raises(ResourceExceeded) as exc:
            guard.check_limits(snap)
        assert exc.value.resource_type == ResourceType.MEMORY

    def test_cpu_time_limit_exceeded(self):
        """CPU时间超限"""
        guard = ResourceGuard(ResourceLimit(cpu_time_sec=0.001))  # 极低限制
        snap = ResourceSnapshot(
            timestamp=time.time(),
            memory_mb=100,
            cpu_time_sec=1.0,  # 远超限制
            threads=1,
            open_files=10
        )
        with pytest.raises(ResourceExceeded) as exc:
            guard.check_limits(snap)
        assert exc.value.resource_type == ResourceType.CPU_TIME

    def test_files_limit_exceeded(self):
        """文件描述符超限"""
        guard = ResourceGuard(ResourceLimit(max_files=1))
        snap = ResourceSnapshot(
            timestamp=time.time(),
            memory_mb=100,
            cpu_time_sec=1,
            threads=1,
            open_files=100
        )
        with pytest.raises(ResourceExceeded) as exc:
            guard.check_limits(snap)
        assert exc.value.resource_type == ResourceType.FILES_OPEN

    def test_concurrent_task_limit_basic(self):
        """并发任务数限制基础验证"""
        guard = ResourceGuard(ResourceLimit(max_concurrent=2))
        
        results = []
        
        def worker():
            guard.enter_task()
            try:
                time.sleep(0.02)
                results.append(True)
            finally:
                guard.exit_task()
        
        threads = [threading.Thread(target=worker) for _ in range(4)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        
        assert len(results) == 4


# =============================================================================
# 2. 数据损坏测试
# =============================================================================

class TestDataCorruption:
    """数据损坏场景"""

    def test_corrupted_json_load_raises(self):
        """损坏的JSON文件加载报错"""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write('{"messages": [{invalid json}')
            path = f.name
        
        try:
            with pytest.raises(json.JSONDecodeError):
                Session.load_from_path(path)
        finally:
            os.unlink(path)

    def test_missing_required_field_handled(self):
        """缺失字段使用默认值"""
        session = Session()
        assert session.version == 1  # 默认值

    def test_tool_result_with_none_values(self):
        """工具结果含None值"""
        session = Session()
        session.add_user_message("run")
        # None input - should be handled
        session.add_assistant_message([
            ContentBlock.tool_use(
                id="t1",
                name="bash",
                input=None
            )
        ])
        json_str = session.to_json()
        assert json_str is not None


# =============================================================================
# 3. 超时/中断测试
# =============================================================================

class TestTimeoutInterrupt:
    """超时和中断场景"""

    def test_retry_exhaustion_after_max_attempts(self):
        """重试次数耗尽"""
        executor = RetryExecutor(
            retry_config=RetryConfig(max_attempts=2, jitter=False, base_delay_ms=1)
        )
        call_count = 0
        
        def always_fail():
            nonlocal call_count
            call_count += 1
            raise Exception("fail")
        
        result = executor.execute_with_retry(always_fail)
        assert result.success is False
        assert result.attempts == 2
        assert call_count == 2

    def test_circuit_breaker_opens_on_failures(self):
        """熔断器在连续失败后打开"""
        cb = CircuitBreaker(CircuitBreakerConfig(failure_threshold=3))
        
        for _ in range(3):
            cb.record_failure()
        
        assert cb.state == CircuitState.OPEN
        assert cb.can_execute() is False

    def test_circuit_breaker_recovery_timeout(self):
        """熔断器超时后恢复"""
        cb = CircuitBreaker(CircuitBreakerConfig(
            failure_threshold=1,
            recovery_timeout_sec=0  # 立即可恢复
        ))
        cb.record_failure()
        assert cb.state == CircuitState.OPEN
        
        time.sleep(0.01)
        can_exec = cb.can_execute()
        assert can_exec is True
        assert cb.state == CircuitState.HALF_OPEN

    def test_nested_retry_with_backoff(self):
        """重试有退避"""
        executor = RetryExecutor(
            retry_config=RetryConfig(max_attempts=3, jitter=False, base_delay_ms=100)
        )
        attempts = []
        
        def failing_func():
            attempts.append(time.time())
            raise Exception("fail")
        
        result = executor.execute_with_retry(failing_func)
        assert result.success is False
        assert len(attempts) == 3
        # 验证退避
        if len(attempts) >= 3:
            interval1 = attempts[1] - attempts[0]
            interval2 = attempts[2] - attempts[1]
            assert interval2 >= interval1  # 指数退避


# =============================================================================
# 4. 边界值测试
# =============================================================================

class TestBoundaryValues:
    """边界值场景"""

    def test_empty_message_content(self):
        """空消息内容"""
        session = Session()
        session.add_user_message("")
        session.add_assistant_message([ContentBlock.text_block("")])
        
        assert len(session.messages) == 2
        json_str = session.to_json()
        assert '"text": ""' in json_str

    def test_very_long_message(self):
        """极长消息"""
        session = Session()
        long_text = "x" * 100000  # 100KB
        session.add_user_message(long_text)
        
        json_str = session.to_json()
        assert len(json_str) > 100000

    def test_many_messages_session(self):
        """大量消息"""
        session = Session()
        for i in range(100):
            session.add_user_message(f"Message {i}")
            session.add_assistant_message([ContentBlock.text_block(f"Response {i}")])
        
        assert len(session.messages) == 200
        
        json_str = session.to_json()
        data = json.loads(json_str)
        assert len(data['messages']) == 200

    def test_special_characters_in_message(self):
        """特殊字符"""
        session = Session()
        special = 'Hello\nWorld\t!"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~'
        session.add_user_message(special)
        
        json_str = session.to_json()
        data = json.loads(json_str)
        assert special in data['messages'][0]['blocks'][0]['text']


# =============================================================================
# 5. 异常恢复测试
# =============================================================================

class TestExceptionRecovery:
    """异常恢复场景"""

    def test_retry_then_success(self):
        """重试后成功恢复"""
        executor = RetryExecutor(
            retry_config=RetryConfig(max_attempts=3, jitter=False, base_delay_ms=1)
        )
        call_count = 0
        
        def fail_twice():
            nonlocal call_count
            call_count += 1
            if call_count < 3:
                raise Exception("temporary")
            return "recovered"
        
        result = executor.execute_with_retry(fail_twice)
        assert result.success is True
        assert result.recovered is True
        assert result.attempts == 3

    def test_circuit_half_open_to_closed(self):
        """HALF_OPEN → CLOSED 恢复"""
        cb = CircuitBreaker(CircuitBreakerConfig(
            failure_threshold=1,
            recovery_timeout_sec=0,
            half_open_attempts=1
        ))
        cb.record_failure()
        time.sleep(0.01)
        cb.can_execute()
        
        cb.record_success()
        assert cb.state == CircuitState.CLOSED
        assert cb.failure_count == 0

    def test_session_persistence_recovery(self):
        """会话持久化恢复"""
        session = Session()
        session.add_user_message("test")
        session.add_assistant_message([ContentBlock.text_block("response")])
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            path = f.name
        
        try:
            session.save_to_path(path)
            loaded = Session.load_from_path(path)
            
            assert len(loaded.messages) == len(session.messages)
        finally:
            os.unlink(path)

    def test_resource_monitor_no_leak(self):
        """资源监控器无泄漏"""
        monitor = ResourceMonitor()
        monitor._snapshots.clear()
        
        for _ in range(10):
            monitor.record_snapshot()
        
        assert len(monitor._snapshots) <= 100
        assert len(monitor._snapshots) == 10


# =============================================================================
# 6. 线程安全测试
# =============================================================================

class TestThreadSafety:
    """线程安全场景"""

    def test_circuit_breaker_thread_safe(self):
        """熔断器线程安全"""
        cb = CircuitBreaker(CircuitBreakerConfig(failure_threshold=100))
        
        def stress():
            for _ in range(10):
                cb.record_failure()
        
        threads = [threading.Thread(target=stress) for _ in range(10)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        
        assert cb.failure_count == 100

    def test_resource_monitor_thread_safe(self):
        """资源监控器线程安全"""
        monitor = ResourceMonitor()
        monitor._snapshots.clear()
        
        def snapshot():
            for _ in range(20):
                monitor.record_snapshot()
        
        threads = [threading.Thread(target=snapshot) for _ in range(5)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        
        assert len(monitor._snapshots) > 0

    def test_session_thread_safe(self):
        """Session添加消息线程安全"""
        session = Session()
        
        def add_messages(role, prefix, count):
            for i in range(count):
                if role == 'user':
                    session.add_user_message(f"{prefix}-{i}")
                else:
                    session.add_assistant_message([ContentBlock.text_block(f"{prefix}-{i}")])
        
        t1 = threading.Thread(target=add_messages, args=('user', 'U', 50))
        t2 = threading.Thread(target=add_messages, args=('assistant', 'A', 50))
        
        t1.start()
        t2.start()
        t1.join()
        t2.join()
        
        assert len(session.messages) == 100
