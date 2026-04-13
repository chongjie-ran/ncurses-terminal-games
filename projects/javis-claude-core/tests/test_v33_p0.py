"""
V3.3 P0 功能测试套件

测试覆盖:
1. 错误恢复机制 (retry_executor.py)
2. 资源限制保护 (resource_guard.py)
3. 会话持久化 (session.py)
"""

import pytest
import time
import os
import tempfile
import threading
import json

from src.retry_executor import (
    RetryExecutor, RetryConfig, RetryResult,
    CircuitBreaker, CircuitBreakerConfig, CircuitState,
    ErrorSeverity, with_retry
)
from src.resource_guard import (
    ResourceGuard, ResourceLimit, ResourceExceeded,
    ResourceMonitor, ResourceType, ResourceSnapshot
)
from src.session import Session, ContentBlock


# =============================================================================
# 1. 错误恢复机制测试
# =============================================================================

class TestRetryConfig:
    """RetryConfig 配置测试"""

    def test_default_config(self):
        cfg = RetryConfig()
        assert cfg.max_attempts == 3
        assert cfg.base_delay_ms == 100
        assert cfg.max_delay_ms == 5000
        assert cfg.exponential_base == 2.0
        assert cfg.jitter is True

    def test_custom_config(self):
        cfg = RetryConfig(max_attempts=5, base_delay_ms=200, max_delay_ms=10000)
        assert cfg.max_attempts == 5
        assert cfg.base_delay_ms == 200

    def test_delay_calculation_no_jitter(self):
        cfg = RetryConfig(jitter=False, exponential_base=2.0, base_delay_ms=100)
        assert abs(cfg.get_delay(1) - 0.1) < 0.001
        assert abs(cfg.get_delay(2) - 0.2) < 0.001
        assert abs(cfg.get_delay(3) - 0.4) < 0.001

    def test_delay_respects_max(self):
        cfg = RetryConfig(jitter=False, exponential_base=2.0, base_delay_ms=1000, max_delay_ms=100)
        delay = cfg.get_delay(3)
        assert delay <= 0.1


class TestCircuitBreaker:
    """熔断器测试"""

    def test_initial_state_closed(self):
        cb = CircuitBreaker()
        assert cb.state == CircuitState.CLOSED
        assert cb.can_execute() is True

    def test_opens_after_threshold(self):
        cb = CircuitBreaker(CircuitBreakerConfig(failure_threshold=3))
        for _ in range(3):
            cb.record_failure()
        assert cb.state == CircuitState.OPEN
        assert cb.can_execute() is False

    def test_half_open_after_timeout(self):
        cb = CircuitBreaker(CircuitBreakerConfig(
            failure_threshold=1, recovery_timeout_sec=0
        ))
        cb.record_failure()
        assert cb.state == CircuitState.OPEN
        time.sleep(0.01)
        assert cb.can_execute() is True
        assert cb.state == CircuitState.HALF_OPEN

    def test_success_decrements_failure_count(self):
        cb = CircuitBreaker(CircuitBreakerConfig(failure_threshold=5))
        cb.record_failure()
        cb.record_failure()
        assert cb.failure_count == 2
        cb.record_success()
        assert cb.failure_count == 1

    def test_half_open_to_closed_on_success(self):
        cb = CircuitBreaker(CircuitBreakerConfig(
            failure_threshold=1, recovery_timeout_sec=0, half_open_attempts=2
        ))
        cb.record_failure()
        assert cb.state == CircuitState.OPEN
        time.sleep(0.01)
        cb.can_execute()
        assert cb.state == CircuitState.HALF_OPEN
        cb.record_success()
        cb.record_success()
        assert cb.state == CircuitState.CLOSED


class TestErrorSeverity:
    """错误分类测试"""

    def test_fatal_permission_error(self, executor):
        err = PermissionError("Permission denied")
        assert executor.classify_error(err) == ErrorSeverity.FATAL

    def test_file_not_found_recoverable(self, executor):
        """FileNotFoundError 可能因并发创建而暂时不可用，应重试"""
        err = FileNotFoundError("File not found")
        assert executor.classify_error(err) == ErrorSeverity.RECOVERABLE

    def test_transient_resource_error(self, executor):
        err = Exception("Resource busy")
        assert executor.classify_error(err) == ErrorSeverity.TRANSIENT

    def test_recoverable_generic_error(self, executor):
        err = Exception("Network timeout")
        assert executor.classify_error(err) == ErrorSeverity.RECOVERABLE


class TestRetryExecutor:
    """RetryExecutor 重试执行器测试"""

    def test_success_first_attempt(self, executor):
        call_count = 0
        def succeed():
            nonlocal call_count
            call_count += 1
            return "ok"
        
        result = executor.execute_with_retry(succeed)
        assert result.success is True
        assert result.attempts == 1
        assert result.recovered is False
        assert call_count == 1

    def test_retry_on_recoverable_error(self, executor):
        call_count = 0
        def fail_then_succeed():
            nonlocal call_count
            call_count += 1
            if call_count < 3:
                raise Exception("temporary failure")
            return "recovered"
        
        result = executor.execute_with_retry(fail_then_succeed, on_retry=lambda a, e: None)
        assert result.success is True
        assert result.attempts == 3
        assert result.recovered is True

    def test_fatal_error_no_retry(self, executor):
        call_count = 0
        def always_fail():
            nonlocal call_count
            call_count += 1
            raise PermissionError("Access denied")
        
        result = executor.execute_with_retry(always_fail)
        assert result.success is False
        assert result.attempts == 1
        assert "access denied" in (result.last_error or "").lower()
        assert call_count == 1

    def test_max_attempts_exceeded(self, executor):
        call_count = 0
        def always_fail():
            nonlocal call_count
            call_count += 1
            raise Exception("always fail")
        
        result = executor.execute_with_retry(always_fail)
        assert result.success is False
        assert result.attempts == 3
        assert call_count == 3

    def test_circuit_breaker_blocks(self, executor_with_breaker):
        call_count = 0
        def fail():
            nonlocal call_count
            call_count += 1
            raise Exception("fail")
        
        # Should be blocked - circuit is open
        result = executor_with_breaker.execute_with_retry(fail)
        assert result.success is False
        assert "Circuit breaker" in (result.last_error or "")
        assert call_count == 0  # Blocked before execution

    def test_on_retry_callback(self, executor):
        retry_logs = []
        def fail_twice():
            if len(retry_logs) < 2:
                raise Exception("retry me")
            return "ok"
        
        def track(attempt, err):
            retry_logs.append((attempt, str(err)))
        
        executor.execute_with_retry(fail_twice, on_retry=track)
        assert len(retry_logs) == 2
        assert retry_logs[0][0] == 1
        assert retry_logs[1][0] == 2


class TestWithRetryDecorator:
    """@with_retry 装饰器测试"""

    def test_sync_decorator_success(self):
        @with_retry(RetryConfig(max_attempts=3, jitter=False))
        def my_func():
            return "done"
        
        result = my_func()
        assert result.success is True
        assert result.attempts == 1


# =============================================================================
# 2. 资源限制保护测试
# =============================================================================

class TestResourceMonitor:
    """资源监控器测试"""

    def test_singleton(self):
        m1 = ResourceMonitor()
        m2 = ResourceMonitor()
        assert m1 is m2

    def test_snapshot_contains_fields(self):
        monitor = ResourceMonitor()
        snap = monitor.get_current_snapshot()
        assert snap.memory_mb >= 0
        assert snap.cpu_time_sec >= 0
        assert snap.threads >= 1
        assert snap.open_files >= 0
        assert snap.timestamp > 0

    def test_record_snapshot(self):
        monitor = ResourceMonitor()
        monitor._snapshots.clear()
        snap = monitor.record_snapshot()
        assert len(monitor._snapshots) == 1
        assert snap is monitor._snapshots[0]

    def test_stats_calculation(self):
        monitor = ResourceMonitor()
        monitor._snapshots.clear()
        for _ in range(5):
            monitor.record_snapshot()
            time.sleep(0.001)
        stats = monitor.get_stats()
        assert 'memory_peak_mb' in stats
        assert 'memory_avg_mb' in stats
        assert 'cpu_time_total_sec' in stats


class TestResourceLimit:
    """资源限制配置测试"""

    def test_empty_limit(self):
        limit = ResourceLimit()
        assert limit.is_empty() is True

    def test_non_empty_limit(self):
        limit = ResourceLimit(memory_mb=1024)
        assert limit.is_empty() is False


class TestResourceGuard:
    """资源守卫测试"""

    def test_check_memory_limit_pass(self):
        guard = ResourceGuard(ResourceLimit(memory_mb=100000))
        snap = ResourceSnapshot(
            timestamp=time.time(),
            memory_mb=100,
            cpu_time_sec=1,
            threads=1,
            open_files=10
        )
        guard.check_limits(snap)

    def test_check_memory_limit_exceeded(self):
        guard = ResourceGuard(ResourceLimit(memory_mb=50))
        snap = ResourceSnapshot(
            timestamp=time.time(),
            memory_mb=100,
            cpu_time_sec=1,
            threads=1,
            open_files=10
        )
        with pytest.raises(ResourceExceeded) as exc_info:
            guard.check_limits(snap)
        assert exc_info.value.resource_type == ResourceType.MEMORY
        assert exc_info.value.current == 100

    def test_check_cpu_limit_exceeded(self):
        guard = ResourceGuard(ResourceLimit(cpu_time_sec=1))
        snap = ResourceSnapshot(
            timestamp=time.time(),
            memory_mb=100,
            cpu_time_sec=10,
            threads=1,
            open_files=10
        )
        with pytest.raises(ResourceExceeded) as exc_info:
            guard.check_limits(snap)
        assert exc_info.value.resource_type == ResourceType.CPU_TIME

    def test_concurrent_task_limit(self):
        guard = ResourceGuard(ResourceLimit(max_concurrent=2))
        results = []

        def task():
            guard.enter_task()
            try:
                time.sleep(0.05)
                results.append(True)
            finally:
                guard.exit_task()

        threads = [threading.Thread(target=task) for _ in range(4)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()

        assert len(results) == 4

    def test_run_with_protection_no_memory_limit(self):
        """run_with_protection without memory limit - should pass"""
        guard = ResourceGuard(ResourceLimit())  # No limits set
        result = guard.run_with_protection(lambda: 42)
        assert result == 42

    def test_run_with_protection_concurrent_limit(self):
        """P1 Fix 验证: run_with_protection 并发限制
        
        问题: 之前 check_limits 在 enter_task 之前，check_limits 失败时
        enter_task 未调用但 exit_task 仍会运行，导致 _active_tasks 计数错误。
        修复: enter_task 在 check_limits 之前，确保信号量和计数器一致。
        """
        guard = ResourceGuard(ResourceLimit(max_concurrent=2))
        results = []
        errors = []
        lock = threading.Lock()

        def task():
            try:
                result = guard.run_with_protection(lambda: (time.sleep(0.05), 1)[1])
                with lock:
                    results.append(result)
            except ResourceExceeded as e:
                with lock:
                    errors.append(e)

        threads = [threading.Thread(target=task) for _ in range(5)]
        for t in threads:
            t.start()
        for t in threads:
            t.join(timeout=5)

        # 所有5个任务都应完成，无 ResourceExceeded
        assert len(results) == 5, f'Expected 5 results, got {len(results)}. Errors: {errors}'
        assert len(errors) == 0, f'Expected no errors, got {errors}'
        assert results == [1, 1, 1, 1, 1]

    def test_guard_status(self):
        guard = ResourceGuard(ResourceLimit(memory_mb=4096, cpu_time_sec=300))
        status = guard.get_guard_status()
        assert status['limits_active'] is True
        assert 'current' in status
        assert 'memory_mb' in status['current']

    def test_default_guard_exists(self):
        from src.resource_guard import get_default_guard
        guard = get_default_guard()
        assert guard is not None
        assert guard.limits.memory_mb == 4096

    def test_disk_space_check_graceful(self):
        """disk space check handles platforms gracefully"""
        guard = ResourceGuard(ResourceLimit(disk_space_gb=0.001))
        guard.check_disk_space()  # Should not raise on actual filesystem

    def test_files_limit_exceeded(self):
        guard = ResourceGuard(ResourceLimit(max_files=5))
        snap = ResourceSnapshot(
            timestamp=time.time(),
            memory_mb=100,
            cpu_time_sec=1,
            threads=1,
            open_files=10
        )
        with pytest.raises(ResourceExceeded) as exc_info:
            guard.check_limits(snap)
        assert exc_info.value.resource_type == ResourceType.FILES_OPEN


# =============================================================================
# 3. 会话持久化测试
# =============================================================================

class TestSessionPersistence:
    """会话状态持久化测试"""

    def test_save_and_load_roundtrip(self):
        session = Session()
        session.add_user_message("Hello")
        session.add_assistant_message([ContentBlock.text_block("Hi!")])

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            path = f.name

        try:
            session.save_to_path(path)
            assert os.path.getsize(path) > 0
            
            loaded = Session.load_from_path(path)
            assert len(loaded.messages) == 2
            assert loaded.messages[0].blocks[0].text == "Hello"
            assert loaded.messages[1].blocks[0].text == "Hi!"
        finally:
            os.unlink(path)

    def test_save_creates_valid_json(self):
        session = Session()
        session.add_user_message("Test message")
        session.add_user_message("Another message")

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            path = f.name

        try:
            session.save_to_path(path)
            with open(path) as f:
                data = json.load(f)
            assert 'messages' in data
            assert len(data['messages']) == 2
        finally:
            os.unlink(path)

    def test_load_nonexistent_raises(self):
        with pytest.raises(FileNotFoundError):
            Session.load_from_path("/nonexistent/path/session.json")

    def test_empty_session_save_load(self):
        session = Session()
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            path = f.name
        try:
            session.save_to_path(path)
            loaded = Session.load_from_path(path)
            assert len(loaded.messages) == 0
        finally:
            os.unlink(path)

    def test_complex_content_roundtrip(self):
        """测试复杂内容(工具调用等)的持久化"""
        session = Session()
        session.add_user_message("Run a command")
        session.add_assistant_message([
            ContentBlock.tool_use(
                id="tool_call_1",
                name="bash",
                input='{"command": "ls"}'
            )
        ])
        session.add_tool_result(
            tool_use_id="tool_call_1",
            tool_name="bash",
            output="file1 file2"
        )
        session.add_assistant_message([ContentBlock.text_block("Files listed")])

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            path = f.name

        try:
            session.save_to_path(path)
            loaded = Session.load_from_path(path)
            assert len(loaded.messages) == 4
            # Verify tool use content preserved
            tool_msg = loaded.messages[1]
            assert tool_msg.blocks[0].type == "tool_use"
        finally:
            os.unlink(path)


# =============================================================================
# Pytest Fixtures
# =============================================================================

@pytest.fixture
def executor():
    """默认重试执行器"""
    return RetryExecutor(
        retry_config=RetryConfig(max_attempts=3, jitter=False, base_delay_ms=1),
        circuit_config=CircuitBreakerConfig(failure_threshold=5, recovery_timeout_sec=60)
    )


@pytest.fixture
def executor_with_breaker():
    """熔断器已触发的执行器"""
    executor = RetryExecutor(
        retry_config=RetryConfig(max_attempts=3, jitter=False, base_delay_ms=1),
        circuit_config=CircuitBreakerConfig(failure_threshold=5, recovery_timeout_sec=60)
    )
    def failing_func():
        raise Exception("trigger failure")
    
    for _ in range(5):
        executor.execute_with_retry(failing_func)
    
    # Verify breaker is open
    assert executor.circuit_breaker.state == CircuitState.OPEN
    return executor




# =============================================================================
# 3. retry_executor 补测：覆盖异步装饰器和熔断器状态转换
# =============================================================================

class TestRetryExecutorExtended:
    """retry_executor 扩展测试 — 覆盖剩余未覆盖行"""

    def test_circuit_breaker_opens_after_threshold(self, executor):
        """触发5次失败，熔断器从CLOSED转为OPEN（覆盖 line 93: self._state = value）"""
        cb = CircuitBreaker(CircuitBreakerConfig(failure_threshold=5))
        for _ in range(5):
            cb.record_failure()
        assert cb.state == CircuitState.OPEN
        assert cb.can_execute() is False

    def test_circuit_breaker_half_open_to_open(self, executor):
        """熔断器从HALF_OPEN状态调用can_execute（覆盖 line 134: return True for HALF_OPEN）"""
        cb = CircuitBreaker(CircuitBreakerConfig(failure_threshold=2))
        cb.record_failure()
        cb.record_failure()
        assert cb.state == CircuitState.OPEN
        cb._last_failure_time = time.time() - 1000  # force elapsed > recovery_timeout
        cb._state = CircuitState.HALF_OPEN
        cb._half_open_successes = 0
        result = cb.can_execute()
        assert result is True

    def test_circuit_breaker_half_open_to_closed(self, executor):
        """HALF_OPEN状态下连续成功转为CLOSED"""
        cb = CircuitBreaker(CircuitBreakerConfig(
            failure_threshold=2,
            half_open_attempts=2,
            recovery_timeout_sec=0
        ))
        cb._state = CircuitState.HALF_OPEN
        cb._half_open_successes = 0
        cb.record_success()
        assert cb.state == CircuitState.HALF_OPEN
        cb.record_success()
        assert cb.state == CircuitState.CLOSED

    @pytest.mark.asyncio
    async def test_with_retry_decorator_async_function(self, executor):
        """@with_retry装饰器应用于async函数（覆盖 lines 298-305: async_wrapper返回路径）"""
        call_count = 0

        @with_retry(RetryConfig(max_attempts=2, jitter=False, base_delay_ms=1))
        async def async_func():
            nonlocal call_count
            call_count += 1
            if call_count == 1:
                raise Exception("temporary")
            return "async done"

        result = await async_func()
        assert result.success is True
        assert result.attempts == 2
        assert call_count == 2

    @pytest.mark.asyncio
    async def test_with_retry_decorator_async_fatal(self, executor):
        """@with_retry装饰器应用于async函数，遭遇FATAL错误不重试"""
        @with_retry(RetryConfig(max_attempts=3, jitter=False, base_delay_ms=1))
        async def async_fail():
            raise PermissionError("Access denied")

        result = await async_fail()
        assert result.success is False
        assert result.attempts == 1

    def test_retry_config_no_jitter(self, executor):
        """RetryConfig.get_delay 无jitter模式（确定性延迟）"""
        cfg = RetryConfig(max_attempts=3, jitter=False, base_delay_ms=100, exponential_base=2.0)
        d1 = cfg.get_delay(1)
        d2 = cfg.get_delay(2)
        d3 = cfg.get_delay(3)
        assert d1 == 0.1
        assert d2 == 0.2
        assert d3 == 0.4
        assert d1 == cfg.get_delay(1)  # deterministic

    @pytest.mark.asyncio
    async def test_retry_executor_async_with_callback(self, executor):
        """异步execute_with_retry_async的on_retry回调（覆盖 line 134附近）"""
        call_count = 0
        retry_log = []

        async def fail_twice():
            nonlocal call_count
            call_count += 1
            if call_count <= 2:
                raise Exception("temp")
            return "async recovered"

        def on_retry_cb(attempt, exc):
            retry_log.append((attempt, str(exc)))

        cfg = RetryConfig(max_attempts=3, jitter=False, base_delay_ms=1)
        ex = RetryExecutor(retry_config=cfg)
        result = await ex.execute_with_retry_async(fail_twice, on_retry=on_retry_cb)
        assert result.success is True
        assert result.attempts == 3
        assert len(retry_log) == 2
        assert retry_log[0][0] == 1
        assert retry_log[1][0] == 2

    def test_error_classify_file_not_found_recoverable(self, executor):
        """FileNotFoundError被分类为RECOVERABLE"""
        ex = RetryExecutor()
        assert ex.classify_error(FileNotFoundError("file not found")) == ErrorSeverity.RECOVERABLE

    def test_error_classify_transient_patterns(self, executor):
        """classify_error Transient模式覆盖"""
        ex = RetryExecutor()
        assert ex.classify_error(Exception("resource temporarily unavailable")) == ErrorSeverity.TRANSIENT
        assert ex.classify_error(Exception("database locked")) == ErrorSeverity.TRANSIENT
        assert ex.classify_error(Exception("connection pool exhausted")) == ErrorSeverity.TRANSIENT
