"""Resource Guard tests - 真显补充，覆盖protected装饰器"""
import pytest
import time
from src.resource_guard import ResourceGuard, ResourceLimit, ResourceType, protected


class TestProtectedDecorator:
    """测试 protected 装饰器（232行wrapper入口，未被其他测试覆盖）"""

    def test_protected_decorator_basic(self):
        """验证protected装饰器包装函数正常执行"""
        @protected()
        def dummy_task():
            return "success"
        
        result = dummy_task()
        assert result == "success"

    def test_protected_decorator_with_limits(self):
        """验证protected装饰器接受ResourceLimit参数"""
        limits = ResourceLimit(cpu_time_sec=10)
        
        @protected(limits)
        def limited_task():
            return True
        
        result = limited_task()
        assert result is True

    def test_protected_decorator_propagates_exception(self):
        """验证protected装饰器会传播被装饰函数的异常"""
        @protected()
        def failing_task():
            raise ValueError("expected error")
        
        with pytest.raises(ValueError, match="expected error"):
            failing_task()

    def test_protected_decorator_passes_args(self):
        """验证protected装饰器传递参数和关键字参数"""
        @protected()
        def task_with_args(a, b, key=None):
            return f"{a}+{b}+{key}"
        
        assert task_with_args(1, 2, key=3) == "1+2+3"
        assert task_with_args("x", "y") == "x+y+None"


class TestResourceGuardEdgeCases:
    """补充 resource_guard 边界用例（提升覆盖率91%→95%+）"""

    def test_check_limits_max_files_exceeded(self):
        """行105-106：max_files超限应抛出ResourceExceeded"""
        from src.resource_guard import ResourceGuard, ResourceLimit, ResourceSnapshot, ResourceType
        
        guard = ResourceGuard(ResourceLimit(max_files=1))
        # 构造open_files=10的快照
        snap = ResourceSnapshot(
            timestamp=time.time(),
            memory_mb=100,
            cpu_time_sec=1,
            threads=1,
            open_files=10,  # 超过max_files=1
        )
        
        with pytest.raises(Exception) as exc_info:
            guard.check_limits(snap)
        # 验证异常类型
        assert "files_open" in str(exc_info.value).lower() or "max_files" in str(exc_info.value).lower()

    def test_check_disk_space_oserror_skipped(self):
        """行143-146：磁盘路径不可访问时跳过检查，不抛异常"""
        from src.resource_guard import ResourceGuard, ResourceLimit
        
        guard = ResourceGuard(ResourceLimit(disk_space_gb=1000))
        # 传入不存在的路径，OSError应被捕获
        guard.check_disk_space("/nonexistent/path/that/cannot/be/accessed")
        # 不应抛异常

    def test_resource_limit_is_empty_true(self):
        """行44：is_empty()所有字段为None时返回True"""
        from src.resource_guard import ResourceLimit
        
        limit = ResourceLimit()
        assert limit.is_empty() is True

    def test_resource_limit_is_empty_false(self):
        """行44：is_empty()有字段非None时返回False"""
        from src.resource_guard import ResourceLimit
        
        limit = ResourceLimit(memory_mb=1024)
        assert limit.is_empty() is False

    def test_resource_exceeded_attributes(self):
        """行53-56：ResourceExceeded异常属性验证"""
        from src.resource_guard import ResourceExceeded, ResourceType
        
        exc = ResourceExceeded(ResourceType.MEMORY, 5000.0, 4096.0)
        assert exc.resource_type == ResourceType.MEMORY
        assert exc.current == 5000.0
        assert exc.limit == 4096.0
        assert "memory" in str(exc).lower()
        assert "5000.0" in str(exc)
        assert "4096.0" in str(exc)

    def test_resource_guard_check_limits_no_exception_when_ok(self):
        """行96-104：资源未超限时check_limits不抛异常"""
        from src.resource_guard import ResourceGuard, ResourceLimit, ResourceSnapshot
        
        guard = ResourceGuard(ResourceLimit(memory_mb=10000, cpu_time_sec=600))
        snap = ResourceSnapshot(
            timestamp=time.time(),
            memory_mb=500,   # 未超限
            cpu_time_sec=10,  # 未超限
            threads=2,
            open_files=5,
        )
        guard.check_limits(snap)  # 不抛异常即为通过
