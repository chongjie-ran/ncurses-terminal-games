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
