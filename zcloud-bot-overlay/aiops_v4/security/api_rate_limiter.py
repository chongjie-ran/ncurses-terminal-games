"""
限流熔断模块
基于令牌桶的限流器和API级别熔断保护
"""

from __future__ import annotations

import asyncio
import time
from collections import defaultdict
from dataclasses import dataclass, field
from enum import Enum
from typing import Any, Callable, Optional

from .models import ValidationResult


class CircuitState(str, Enum):
    """熔断器状态"""
    CLOSED = "closed"      # 正常状态
    OPEN = "open"          # 熔断状态
    HALF_OPEN = "half_open"  # 半开状态（尝试恢复）


@dataclass
class TokenBucket:
    """令牌桶"""
    capacity: float
    tokens: float
    refill_rate: float
    last_refill: float = field(default_factory=time.time)
    
    def _refill(self) -> None:
        """补充令牌"""
        now = time.time()
        elapsed = now - self.last_refill
        new_tokens = elapsed * self.refill_rate
        self.tokens = min(self.capacity, self.tokens + new_tokens)
        self.last_refill = now
    
    def try_acquire(self, tokens: float = 1.0) -> bool:
        """
        尝试获取令牌
        
        Args:
            tokens: 需要获取的令牌数
            
        Returns:
            是否成功获取
        """
        self._refill()
        if self.tokens >= tokens:
            self.tokens -= tokens
            return True
        return False


class RateLimiter:
    """基于令牌桶的限流器"""
    
    def __init__(
        self,
        default_capacity: int = 100,
        default_refill_rate: float = 10.0
    ):
        """
        初始化限流器
        
        Args:
            default_capacity: 默认桶容量
            default_refill_rate: 默认每秒补充令牌数
        """
        self.default_capacity = default_capacity
        self.default_refill_rate = default_refill_rate
        self._buckets: dict[str, TokenBucket] = {}
        self._lock = asyncio.Lock()
    
    def _get_bucket_key(self, user_id: Optional[str], api_name: str) -> str:
        """获取桶的键"""
        if user_id:
            return f"{user_id}:{api_name}"
        return f"anonymous:{api_name}"
    
    async def acquire(
        self,
        tokens: int = 1,
        user_id: Optional[str] = None,
        api_name: str = "default",
        capacity: Optional[int] = None,
        refill_rate: Optional[float] = None
    ) -> bool:
        """
        尝试获取令牌
        
        Args:
            tokens: 需要获取的令牌数
            user_id: 用户ID（用于区分不同用户）
            api_name: API名称（用于区分不同API）
            capacity: 自定义桶容量
            refill_rate: 自定义补充速率
            
        Returns:
            是否成功获取令牌
        """
        key = self._get_bucket_key(user_id, api_name)
        
        async with self._lock:
            if key not in self._buckets:
                self._buckets[key] = TokenBucket(
                    capacity=capacity or self.default_capacity,
                    tokens=capacity or self.default_capacity,
                    refill_rate=refill_rate or self.default_refill_rate
                )
            
            bucket = self._buckets[key]
        
        return bucket.try_acquire(tokens)
    
    async def try_acquire_or_raise(
        self,
        tokens: int = 1,
        user_id: Optional[str] = None,
        api_name: str = "default"
    ) -> None:
        """
        尝试获取令牌，失败则抛出异常
        
        Args:
            tokens: 需要获取的令牌数
            user_id: 用户ID
            api_name: API名称
            
        Raises:
            RateLimitExceeded: 限流异常
        """
        if not await self.acquire(tokens, user_id, api_name):
            raise RateLimitExceeded(
                f"Rate limit exceeded for user '{user_id}' on API '{api_name}'"
            )
    
    def reset(self, user_id: Optional[str] = None, api_name: Optional[str] = None) -> None:
        """
        重置限流器
        
        Args:
            user_id: 用户ID（None表示所有用户）
            api_name: API名称（None表示所有API）
        """
        if user_id is None and api_name is None:
            self._buckets.clear()
        elif user_id is None:
            keys_to_remove = [k for k in self._buckets if k.endswith(f":{api_name}")]
            for key in keys_to_remove:
                del self._buckets[key]
        elif api_name is None:
            keys_to_remove = [k for k in self._buckets if k.startswith(f"{user_id}:")]
            for key in keys_to_remove:
                del self._buckets[key]
        else:
            key = self._get_bucket_key(user_id, api_name)
            if key in self._buckets:
                del self._buckets[key]
    
    def get_status(self, user_id: Optional[str] = None, api_name: str = "default") -> dict:
        """
        获取限流状态
        
        Args:
            user_id: 用户ID
            api_name: API名称
            
        Returns:
            状态信息字典
        """
        key = self._get_bucket_key(user_id, api_name)
        if key not in self._buckets:
            return {
                "exists": False,
                "capacity": self.default_capacity,
                "refill_rate": self.default_refill_rate,
                "available_tokens": self.default_capacity
            }
        
        bucket = self._buckets[key]
        return {
            "exists": True,
            "capacity": bucket.capacity,
            "refill_rate": bucket.refill_rate,
            "available_tokens": round(bucket.tokens, 2)
        }


class RateLimitExceeded(Exception):
    """限流异常"""
    pass


@dataclass
class CircuitBreakerConfig:
    """熔断器配置"""
    failure_threshold: int = 5       # 失败次数阈值
    success_threshold: int = 2        # 半开状态下成功次数阈值
    timeout_seconds: float = 30.0     # 熔断超时时间
    half_open_max_calls: int = 3      # 半开状态下的最大调用数


@dataclass
class CircuitBreakerStats:
    """熔断器统计"""
    total_calls: int = 0
    failed_calls: int = 0
    successful_calls: int = 0
    rejected_calls: int = 0
    state_changed_count: int = 0


class CircuitBreakerOpen(Exception):
    """熔断器打开异常"""
    pass


class APIBreaker:
    """API级别熔断器"""
    
    def __init__(
        self,
        name: str,
        config: Optional[CircuitBreakerConfig] = None
    ):
        """
        初始化熔断器
        
        Args:
            name: API名称
            config: 熔断器配置
        """
        self.name = name
        self.config = config or CircuitBreakerConfig()
        self._state = CircuitState.CLOSED
        self._failure_count = 0
        self._success_count = 0
        self._last_failure_time: Optional[float] = None
        self._half_open_calls = 0
        self._lock = asyncio.Lock()
        self._stats = CircuitBreakerStats()
    
    @property
    def state(self) -> CircuitState:
        """获取当前状态"""
        if self._state == CircuitState.OPEN:
            # 检查是否应该转换到半开状态
            if self._last_failure_time:
                if time.time() - self._last_failure_time >= self.config.timeout_seconds:
                    return CircuitState.HALF_OPEN
        return self._state
    
    @property
    def stats(self) -> CircuitBreakerStats:
        """获取统计信息"""
        return self._stats
    
    async def call(
        self,
        func: Callable,
        *args,
        **kwargs
    ) -> Any:
        """
        带熔断的API调用
        
        Args:
            func: 要调用的函数
            *args: 位置参数
            **kwargs: 关键字参数
            
        Returns:
            函数返回值
            
        Raises:
            CircuitBreakerOpen: 熔断器打开时抛出
        """
        current_state = self.state
        
        async with self._lock:
            if current_state == CircuitState.OPEN:
                self._stats.rejected_calls += 1
                raise CircuitBreakerOpen(f"Circuit breaker is OPEN for '{self.name}'")
            
            if current_state == CircuitState.HALF_OPEN:
                if self._half_open_calls >= self.config.half_open_max_calls:
                    self._stats.rejected_calls += 1
                    raise CircuitBreakerOpen(
                        f"Circuit breaker is HALF_OPEN for '{self.name}', max calls reached"
                    )
                self._half_open_calls += 1
        
        # 执行调用
        self._stats.total_calls += 1
        try:
            if asyncio.iscoroutinefunction(func):
                result = await func(*args, **kwargs)
            else:
                result = func(*args, **kwargs)
            
            await self._on_success()
            return result
            
        except Exception as e:
            await self._on_failure()
            raise
    
    async def _on_success(self) -> None:
        """处理成功调用"""
        async with self._lock:
            self._stats.successful_calls += 1
            self._failure_count = 0
            
            if self._state == CircuitState.HALF_OPEN:
                self._success_count += 1
                if self._success_count >= self.config.success_threshold:
                    self._transition_to(CircuitState.CLOSED)
    
    async def _on_failure(self) -> None:
        """处理失败调用"""
        async with self._lock:
            self._stats.failed_calls += 1
            self._failure_count += 1
            self._last_failure_time = time.time()
            self._success_count = 0
            
            if self._state == CircuitState.CLOSED:
                if self._failure_count >= self.config.failure_threshold:
                    self._transition_to(CircuitState.OPEN)
            elif self._state == CircuitState.HALF_OPEN:
                self._transition_to(CircuitState.OPEN)
    
    def _transition_to(self, new_state: CircuitState) -> None:
        """状态转换"""
        if self._state != new_state:
            self._state = new_state
            self._stats.state_changed_count += 1
            
            if new_state == CircuitState.CLOSED:
                self._failure_count = 0
                self._success_count = 0
                self._half_open_calls = 0
            elif new_state == CircuitState.HALF_OPEN:
                self._success_count = 0
                self._half_open_calls = 0
            elif new_state == CircuitState.OPEN:
                self._half_open_calls = 0
    
    def reset(self) -> None:
        """重置熔断器"""
        self._state = CircuitState.CLOSED
        self._failure_count = 0
        self._success_count = 0
        self._last_failure_time = None
        self._half_open_calls = 0
    
    def get_status(self) -> dict:
        """获取熔断器状态"""
        return {
            "name": self.name,
            "state": self.state.value,
            "failure_count": self._failure_count,
            "success_count": self._success_count,
            "config": {
                "failure_threshold": self.config.failure_threshold,
                "success_threshold": self.config.success_threshold,
                "timeout_seconds": self.config.timeout_seconds,
                "half_open_max_calls": self.config.half_open_max_calls
            },
            "stats": {
                "total_calls": self._stats.total_calls,
                "failed_calls": self._stats.failed_calls,
                "successful_calls": self._stats.successful_calls,
                "rejected_calls": self._stats.rejected_calls,
                "state_changed_count": self._stats.state_changed_count
            }
        }


class BreakerRegistry:
    """熔断器注册表"""
    
    def __init__(self):
        self._breakers: dict[str, APIBreaker] = {}
        self._lock = asyncio.Lock()
    
    async def get_breaker(
        self,
        name: str,
        config: Optional[CircuitBreakerConfig] = None
    ) -> APIBreaker:
        """
        获取或创建熔断器
        
        Args:
            name: 熔断器名称
            config: 配置
            
        Returns:
            APIBreaker实例
        """
        async with self._lock:
            if name not in self._breakers:
                self._breakers[name] = APIBreaker(name, config)
            return self._breakers[name]
    
    async def call_with_breaker(
        self,
        name: str,
        func: Callable,
        *args,
        config: Optional[CircuitBreakerConfig] = None,
        **kwargs
    ) -> Any:
        """
        使用指定熔断器调用函数
        
        Args:
            name: 熔断器名称
            func: 函数
            *args: 位置参数
            config: 熔断器配置
            **kwargs: 关键字参数
            
        Returns:
            函数返回值
        """
        breaker = await self.get_breaker(name, config)
        return await breaker.call(func, *args, **kwargs)
    
    def get_all_status(self) -> dict:
        """获取所有熔断器状态"""
        return {
            name: breaker.get_status()
            for name, breaker in self._breakers.items()
        }
    
    def reset_all(self) -> None:
        """重置所有熔断器"""
        for breaker in self._breakers.values():
            breaker.reset()
