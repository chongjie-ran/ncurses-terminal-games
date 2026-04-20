"""
shared_context.py - 共享上下文管理器

支持多Agent在同一个Session内共享上下文数据。
实现跨Agent的记忆传递和状态共享。

对应Claude Code Rust: shared_context.rs (新增)
"""

from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum
from typing import Optional, List, Dict, Any, Callable, Set
from datetime import datetime, timedelta
import threading
import json
import uuid


class ContextScope(Enum):
    """上下文作用域"""
    GLOBAL = "global"          # 全局共享，整个Session可见
    AGENT_LOCAL = "agent"      # 单Agent私有
    TASK_LOCAL = "task"       # 单次任务内可见
    EPHEMERAL = "ephemeral"    # 临时，一次读取后自动删除


class ContextEvent(Enum):
    """上下文事件类型"""
    CREATED = "created"
    UPDATED = "updated"
    DELETED = "deleted"
    EXPIRED = "expired"
    CLEARED = "cleared"


@dataclass
class ContextEntry:
    """上下文条目"""
    key: str
    value: Any
    scope: ContextScope
    owner_agent_id: Optional[str] = None       # 创建此条目的Agent
    created_at: datetime = field(default_factory=datetime.now)
    updated_at: datetime = field(default_factory=datetime.now)
    ttl_seconds: Optional[int] = None          # 生存时间，None表示永不过期
    tags: Set[str] = field(default_factory=set)  # 标签，用于分类检索
    metadata: Dict[str, Any] = field(default_factory=dict)  # 元数据
    
    def is_expired(self) -> bool:
        """检查是否已过期"""
        if self.ttl_seconds is None:
            return False
        expiry = self.created_at + timedelta(seconds=self.ttl_seconds)
        return datetime.now() > expiry
    
    def to_dict(self) -> Dict[str, Any]:
        """序列化为字典"""
        return {
            "key": self.key,
            "value": self.value,
            "scope": self.scope.value,
            "owner_agent_id": self.owner_agent_id,
            "created_at": self.created_at.isoformat(),
            "updated_at": self.updated_at.isoformat(),
            "ttl_seconds": self.ttl_seconds,
            "tags": list(self.tags),
            "metadata": self.metadata,
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "ContextEntry":
        """从字典反序列化"""
        return cls(
            key=data["key"],
            value=data["value"],
            scope=ContextScope(data["scope"]),
            owner_agent_id=data.get("owner_agent_id"),
            created_at=datetime.fromisoformat(data["created_at"]),
            updated_at=datetime.fromisoformat(data["updated_at"]),
            ttl_seconds=data.get("ttl_seconds"),
            tags=set(data.get("tags", [])),
            metadata=data.get("metadata", {}),
        )


@dataclass
class ContextQuery:
    """上下文查询条件"""
    keys: Optional[List[str]] = None           # 精确匹配key
    scopes: Optional[List[ContextScope]] = None  # 匹配作用域
    owner_agent_id: Optional[str] = None      # 匹配所有者
    tags: Optional[Set[str]] = None           # 匹配标签（需包含所有）
    tag_any: bool = False                      # tags为ANY匹配（默认ALL）
    include_expired: bool = False              # 包含已过期的
    created_after: Optional[datetime] = None   # 创建时间下限
    created_before: Optional[datetime] = None  # 创建时间上限
    
    def match(self, entry: ContextEntry) -> bool:
        """判断条目是否匹配此查询"""
        # 检查key
        if self.keys is not None and entry.key not in self.keys:
            return False
        # 检查scope
        if self.scopes is not None and entry.scope not in self.scopes:
            return False
        # 检查owner
        if self.owner_agent_id is not None and entry.owner_agent_id != self.owner_agent_id:
            return False
        # 检查tags
        if self.tags:
            if self.tag_any:
                if not any(t in entry.tags for t in self.tags):
                    return False
            else:
                if not self.tags.issubset(entry.tags):
                    return False
        # 检查创建时间
        if self.created_after and entry.created_at < self.created_after:
            return False
        if self.created_before and entry.created_at > self.created_before:
            return False
        # 检查过期
        if not self.include_expired and entry.is_expired():
            return False
        return True


# 上下文变化回调函数类型
ContextChangeCallback = Callable[[ContextEvent, ContextEntry], None]


class SharedContext:
    """
    共享上下文管理器
    
    支持多Agent在同一个Session内共享数据：
    - 全局上下文：所有Agent可见
    - Agent私有上下文：单Agent独占
    - 任务上下文：单次任务内可见
    - 临时上下文：一次读取后自动删除
    
    示例:
        ctx = SharedContext()
        
        # 设置全局共享数据
        ctx.set("global_plan", {"task": "analyze"}, agent_id="agent-1")
        
        # Agent-2读取
        value = ctx.get("global_plan")
        
        # 设置带TTL的临时数据
        ctx.set("temp_result", result, scope=ContextScope.EPHEMERAL, ttl_seconds=30)
        
        # 订阅变化
        ctx.subscribe(lambda event, entry: print(f"{event}: {entry.key}"))
    """
    
    _instance: Optional["SharedContext"] = None
    _lock = threading.RLock()
    
    def __new__(cls) -> "SharedContext":
        """单例模式"""
        if cls._instance is None:
            with cls._lock:
                if cls._instance is None:
                    cls._instance = super().__new__(cls)
                    cls._instance._init()
        return cls._instance
    
    def _init(self) -> None:
        """初始化"""
        self._data: Dict[str, ContextEntry] = {}
        self._subscribers: Dict[str, ContextChangeCallback] = {}
        self._subscriber_lock = threading.RLock()
        self._session_id: Optional[str] = None
        self._version = 0  # 乐观锁版本号
    
    @classmethod
    def get_instance(cls) -> "SharedContext":
        """获取单例实例"""
        return cls()
    
    @classmethod
    def reset_instance(cls) -> None:
        """重置单例（用于测试）"""
        with cls._lock:
            if cls._instance is not None:
                cls._instance._data.clear()
                cls._instance._subscribers.clear()
                cls._instance._session_id = None
                cls._instance._version = 0
            cls._instance = None
    
    def set_session_id(self, session_id: str) -> None:
        """设置关联的Session ID"""
        self._session_id = session_id
    
    def get_session_id(self) -> Optional[str]:
        """获取关联的Session ID"""
        return self._session_id
    
    # ========== 核心CRUD操作 ==========
    
    def set(
        self,
        key: str,
        value: Any,
        scope: ContextScope = ContextScope.GLOBAL,
        agent_id: Optional[str] = None,
        ttl_seconds: Optional[int] = None,
        tags: Optional[Set[str]] = None,
        metadata: Optional[Dict[str, Any]] = None,
        overwrite: bool = True,
    ) -> bool:
        """
        设置上下文值
        
        Args:
            key: 键名
            value: 值（必须是JSON可序列化的）
            scope: 作用域，默认GLOBAL
            agent_id: 创建者Agent ID（用于AGENT_LOCAL和追踪）
            ttl_seconds: 生存时间，None永不过期
            tags: 标签集合
            metadata: 元数据
            overwrite: 是否覆盖已存在的key
        
        Returns:
            True表示设置成功，False表示key已存在且overwrite=False
        """
        with self._lock:
            # 检查是否已存在且不允许覆盖
            if not overwrite and key in self._data:
                return False
            
            # 创建条目
            now = datetime.now()
            entry = ContextEntry(
                key=key,
                value=value,
                scope=scope,
                owner_agent_id=agent_id,
                created_at=now,
                updated_at=now,
                ttl_seconds=ttl_seconds,
                tags=tags or set(),
                metadata=metadata or {},
            )
            
            # 触发旧条目过期事件
            old_entry = self._data.get(key)
            if old_entry and old_entry.is_expired():
                self._emit_event(ContextEvent.EXPIRED, old_entry)
            
            self._data[key] = entry
            self._version += 1
            
            # 触发事件
            event = ContextEvent.UPDATED if old_entry else ContextEvent.CREATED
            self._emit_event(event, entry)
            
            return True
    
    def get(
        self,
        key: str,
        default: Any = None,
        include_expired: bool = False,
    ) -> Any:
        """
        获取上下文值
        
        Args:
            key: 键名
            default: 默认值（key不存在或已过期时返回）
            include_expired: 是否返回已过期的值
        
        Returns:
            存储的值或default
        """
        with self._lock:
            entry = self._data.get(key)
            if entry is None:
                return default
            
            # 检查过期
            if entry.is_expired():
                if not include_expired:
                    return default
            else:
                # 更新访问统计（如果需要）
                pass
            
            return entry.value
    
    def get_entry(self, key: str) -> Optional[ContextEntry]:
        """获取完整的上下文条目（含元数据）"""
        with self._lock:
            entry = self._data.get(key)
            if entry and entry.is_expired():
                return None
            return entry
    
    def update(
        self,
        key: str,
        value: Any,
        merge: bool = False,
    ) -> bool:
        """
        更新已存在的上下文值
        
        Args:
            key: 键名
            value: 新值
            merge: 是否合并（仅对dict有效）
        
        Returns:
            True表示更新成功，False表示key不存在
        """
        with self._lock:
            entry = self._data.get(key)
            if entry is None:
                return False
            
            # 处理合并
            if merge and isinstance(entry.value, dict) and isinstance(value, dict):
                new_value = entry.value.copy()
                new_value.update(value)
                value = new_value
            
            entry.value = value
            entry.updated_at = datetime.now()
            self._version += 1
            
            self._emit_event(ContextEvent.UPDATED, entry)
            return True
    
    def delete(self, key: str) -> bool:
        """
        删除上下文值
        
        Returns:
            True表示删除成功，False表示key不存在
        """
        with self._lock:
            entry = self._data.pop(key, None)
            if entry is None:
                return False
            
            self._version += 1
            self._emit_event(ContextEvent.DELETED, entry)
            return True
    
    def clear(
        self,
        scope: Optional[ContextScope] = None,
        agent_id: Optional[str] = None,
        tags: Optional[Set[str]] = None,
    ) -> int:
        """
        清除匹配的上下文值
        
        Args:
            scope: 仅清除此作用域的值
            agent_id: 仅清除此Agent创建的值
            tags: 仅清除包含这些标签的值
        
        Returns:
            清除的条目数量
        """
        with self._lock:
            query = ContextQuery(
                scopes=[scope] if scope else None,
                owner_agent_id=agent_id,
                tags=tags,
            )
            
            to_delete = [k for k, v in self._data.items() if query.match(v)]
            for key in to_delete:
                entry = self._data.pop(key)
                self._emit_event(ContextEvent.CLEARED, entry)
            
            self._version += len(to_delete)
            return len(to_delete)
    
    # ========== 批量操作 ==========
    
    def multi_get(self, keys: List[str]) -> Dict[str, Any]:
        """批量获取多个值"""
        with self._lock:
            return {k: self.get(k) for k in keys}
    
    def multi_set(
        self,
        items: Dict[str, Any],
        scope: ContextScope = ContextScope.GLOBAL,
        agent_id: Optional[str] = None,
    ) -> int:
        """
        批量设置多个值
        
        Returns:
            成功设置的数量
        """
        count = 0
        for key, value in items.items():
            if self.set(key, value, scope=scope, agent_id=agent_id):
                count += 1
        return count
    
    # ========== 查询操作 ==========
    
    def query(self, query: ContextQuery) -> List[ContextEntry]:
        """
        查询匹配的上下文条目
        
        Returns:
            匹配的条目列表（按创建时间升序）
        """
        with self._lock:
            return sorted(
                [v for v in self._data.values() if query.match(v)],
                key=lambda x: x.created_at
            )
    
    def query_keys(
        self,
        prefix: Optional[str] = None,
        scope: Optional[ContextScope] = None,
        agent_id: Optional[str] = None,
    ) -> List[str]:
        """
        查询匹配的键名
        
        Args:
            prefix: 键名前缀过滤
            scope: 作用域过滤
            agent_id: 创建者过滤
        
        Returns:
            匹配的键名列表
        """
        with self._lock:
            result = []
            for entry in self._data.values():
                if entry.is_expired():
                    continue
                if prefix and not entry.key.startswith(prefix):
                    continue
                if scope and entry.scope != scope:
                    continue
                if agent_id and entry.owner_agent_id != agent_id:
                    continue
                result.append(entry.key)
            return sorted(result)
    
    def exists(self, key: str) -> bool:
        """检查key是否存在且未过期"""
        with self._lock:
            entry = self._data.get(key)
            return entry is not None and not entry.is_expired()
    
    def get_stats(self) -> Dict[str, Any]:
        """获取统计信息"""
        with self._lock:
            total = len(self._data)
            expired = sum(1 for v in self._data.values() if v.is_expired())
            by_scope = {}
            for scope in ContextScope:
                by_scope[scope.value] = sum(
                    1 for v in self._data.values()
                    if v.scope == scope and not v.is_expired()
                )
            
            return {
                "total": total,
                "active": total - expired,
                "expired": expired,
                "by_scope": by_scope,
                "version": self._version,
                "session_id": self._session_id,
            }
    
    # ========== 订阅机制 ==========
    
    def subscribe(
        self,
        callback: ContextChangeCallback,
        subscriber_id: Optional[str] = None,
    ) -> str:
        """
        订阅上下文变化
        
        Args:
            callback: 回调函数 (event, entry) -> None
            subscriber_id: 订阅者ID，默认自动生成
        
        Returns:
            订阅ID（用于取消订阅）
        """
        with self._subscriber_lock:
            if subscriber_id is None:
                subscriber_id = str(uuid.uuid4())
            self._subscribers[subscriber_id] = callback
            return subscriber_id
    
    def unsubscribe(self, subscriber_id: str) -> bool:
        """
        取消订阅
        
        Returns:
            True表示取消成功，False表示订阅ID不存在
        """
        with self._subscriber_lock:
            return self._subscribers.pop(subscriber_id, None) is not None
    
    def _emit_event(self, event: ContextEvent, entry: ContextEntry) -> None:
        """触发事件通知"""
        with self._subscriber_lock:
            for callback in self._subscribers.values():
                try:
                    callback(event, entry)
                except Exception:
                    # 回调异常不影响主流程
                    pass
    
    # ========== 序列化 ==========
    
    def to_dict(self) -> Dict[str, Any]:
        """导出所有非过期的上下文为字典"""
        with self._lock:
            return {
                "session_id": self._session_id,
                "version": self._version,
                "entries": {
                    k: v.to_dict()
                    for k, v in self._data.items()
                    if not v.is_expired()
                },
            }
    
    def from_dict(self, data: Dict[str, Any]) -> None:
        """从字典导入上下文"""
        with self._lock:
            self._session_id = data.get("session_id")
            self._version = data.get("version", 0)
            
            entries = data.get("entries", {})
            for k, v in entries.items():
                self._data[k] = ContextEntry.from_dict(v)
    
    def export_json(self) -> str:
        """导出为JSON字符串"""
        return json.dumps(self.to_dict(), ensure_ascii=False)
    
    def import_json(self, json_str: str) -> None:
        """从JSON字符串导入"""
        self.from_dict(json.loads(json_str))
    
    # ========== 工具方法 ==========
    
    def increment(
        self,
        key: str,
        amount: int = 1,
        scope: ContextScope = ContextScope.GLOBAL,
        agent_id: Optional[str] = None,
    ) -> int:
        """
        原子递增（仅对数值类型有效）
        
        Returns:
            递增后的新值
        """
        with self._lock:
            current = self.get(key, 0)
            if not isinstance(current, (int, float)):
                raise TypeError(f"Cannot increment non-numeric value: {type(current)}")
            
            new_value = current + amount
            self.set(key, new_value, scope=scope, agent_id=agent_id, overwrite=True)
            return new_value
    
    def append_list(
        self,
        key: str,
        value: Any,
        scope: ContextScope = ContextScope.GLOBAL,
        agent_id: Optional[str] = None,
        max_length: Optional[int] = None,
    ) -> int:
        """
        向列表追加元素
        
        Args:
            key: 键名
            value: 要追加的值
            scope: 作用域
            agent_id: 创建者
            max_length: 列表最大长度，超过则移除最早的元素
        
        Returns:
            追加后列表的长度
        """
        with self._lock:
            current = self.get(key, [])
            if not isinstance(current, list):
                current = [current]
            
            current.append(value)
            
            if max_length and len(current) > max_length:
                current = current[-max_length:]
            
            self.set(key, current, scope=scope, agent_id=agent_id, overwrite=True)
            return len(current)
    
    def get_history(
        self,
        key: str,
        include_expired: bool = False,
    ) -> List[Any]:
        """
        获取历史值列表（用于列表类型的key）
        
        Returns:
            当前存储的列表
        """
        value = self.get(key, [])
        if not isinstance(value, list):
            return [value]
        return value
    
    # ========== 过期清理 ==========
    
    def cleanup_expired(self) -> int:
        """
        清理所有过期的条目
        
        Returns:
            清理的条目数量
        """
        with self._lock:
            expired_keys = [
                k for k, v in self._data.items()
                if v.is_expired()
            ]
            
            for key in expired_keys:
                entry = self._data.pop(key)
                self._emit_event(ContextEvent.EXPIRED, entry)
            
            self._version += len(expired_keys)
            return len(expired_keys)
