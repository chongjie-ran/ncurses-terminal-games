"""
test_shared_context.py - 共享上下文管理器测试

测试 SharedContext 的所有功能：
- 基础CRUD操作
- 作用域管理
- TTL过期机制
- 批量操作
- 查询过滤
- 订阅机制
- 序列化/反序列化
- 线程安全
"""

import pytest
from datetime import datetime, timedelta
from unittest.mock import Mock, patch
import threading
import json

from src.shared_context import (
    SharedContext,
    ContextScope,
    ContextEvent,
    ContextEntry,
    ContextQuery,
)


class TestContextEntry:
    """ContextEntry数据类测试"""
    
    def test_is_expired_no_ttl(self):
        """无TTL永不过期"""
        entry = ContextEntry(key="test", value="data", scope=ContextScope.GLOBAL)
        assert entry.is_expired() is False
    
    def test_is_expired_with_ttl(self):
        """带TTL检查"""
        entry = ContextEntry(
            key="test",
            value="data",
            scope=ContextScope.GLOBAL,
            ttl_seconds=1,
        )
        assert entry.is_expired() is False
        
        # 模拟过期
        entry.created_at = datetime.now() - timedelta(seconds=2)
        assert entry.is_expired() is True
    
    def test_to_dict_from_dict(self):
        """序列化/反序列化"""
        entry = ContextEntry(
            key="test",
            value={"nested": "value"},
            scope=ContextScope.GLOBAL,
            owner_agent_id="agent-1",
            tags={"tag1", "tag2"},
            metadata={"meta": "data"},
        )
        
        data = entry.to_dict()
        restored = ContextEntry.from_dict(data)
        
        assert restored.key == entry.key
        assert restored.value == entry.value
        assert restored.scope == entry.scope
        assert restored.owner_agent_id == entry.owner_agent_id
        assert restored.tags == entry.tags
        assert restored.metadata == entry.metadata


class TestContextQuery:
    """ContextQuery查询匹配测试"""
    
    def test_match_keys(self):
        """key匹配"""
        query = ContextQuery(keys=["a", "b"])
        entry_a = ContextEntry(key="a", value=1, scope=ContextScope.GLOBAL)
        entry_c = ContextEntry(key="c", value=2, scope=ContextScope.GLOBAL)
        
        assert query.match(entry_a) is True
        assert query.match(entry_c) is False
    
    def test_match_scopes(self):
        """作用域匹配"""
        query = ContextQuery(scopes=[ContextScope.GLOBAL, ContextScope.AGENT_LOCAL])
        entry_global = ContextEntry(key="g", value=1, scope=ContextScope.GLOBAL)
        entry_task = ContextEntry(key="t", value=2, scope=ContextScope.TASK_LOCAL)
        
        assert query.match(entry_global) is True
        assert query.match(entry_task) is False
    
    def test_match_tags_all(self):
        """标签全部匹配"""
        query = ContextQuery(tags={"urgent", "important"}, tag_any=False)
        entry1 = ContextEntry(key="e1", value=1, scope=ContextScope.GLOBAL, tags={"urgent", "important"})
        entry2 = ContextEntry(key="e2", value=2, scope=ContextScope.GLOBAL, tags={"urgent"})
        
        assert query.match(entry1) is True
        assert query.match(entry2) is False
    
    def test_match_tags_any(self):
        """标签任意匹配"""
        query = ContextQuery(tags={"urgent", "important"}, tag_any=True)
        entry1 = ContextEntry(key="e1", value=1, scope=ContextScope.GLOBAL, tags={"urgent"})
        entry2 = ContextEntry(key="e2", value=2, scope=ContextScope.GLOBAL, tags={"normal"})
        
        assert query.match(entry1) is True
        assert query.match(entry2) is False
    
    def test_match_owner(self):
        """所有者匹配"""
        query = ContextQuery(owner_agent_id="agent-1")
        entry1 = ContextEntry(key="e1", value=1, scope=ContextScope.GLOBAL, owner_agent_id="agent-1")
        entry2 = ContextEntry(key="e2", value=2, scope=ContextScope.GLOBAL, owner_agent_id="agent-2")
        
        assert query.match(entry1) is True
        assert query.match(entry2) is False


class TestSharedContextBasic:
    """SharedContext基础功能测试"""
    
    def setup_method(self):
        """每个测试前重置单例"""
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_singleton(self):
        """单例模式"""
        ctx1 = SharedContext.get_instance()
        ctx2 = SharedContext.get_instance()
        assert ctx1 is ctx2
    
    def test_set_and_get(self):
        """设置和获取"""
        self.ctx.set("key1", "value1", agent_id="agent-1")
        assert self.ctx.get("key1") == "value1"
    
    def test_get_nonexistent(self):
        """获取不存在的key"""
        assert self.ctx.get("nonexistent") is None
        assert self.ctx.get("nonexistent", "default") == "default"
    
    def test_set_overwrite_false(self):
        """不允许覆盖"""
        self.ctx.set("key1", "value1")
        result = self.ctx.set("key1", "value2", overwrite=False)
        
        assert result is False
        assert self.ctx.get("key1") == "value1"
    
    def test_set_overwrite_true(self):
        """允许覆盖"""
        self.ctx.set("key1", "value1")
        result = self.ctx.set("key1", "value2", overwrite=True)
        
        assert result is True
        assert self.ctx.get("key1") == "value2"
    
    def test_update_existing(self):
        """更新已存在的key"""
        self.ctx.set("key1", "value1")
        result = self.ctx.update("key1", "new_value")
        
        assert result is True
        assert self.ctx.get("key1") == "new_value"
    
    def test_update_nonexistent(self):
        """更新不存在的key"""
        result = self.ctx.update("nonexistent", "value")
        assert result is False
    
    def test_update_merge(self):
        """合并更新"""
        self.ctx.set("key1", {"a": 1, "b": 2})
        self.ctx.update("key1", {"b": 3, "c": 4}, merge=True)
        
        assert self.ctx.get("key1") == {"a": 1, "b": 3, "c": 4}
    
    def test_delete(self):
        """删除"""
        self.ctx.set("key1", "value1")
        assert self.ctx.get("key1") == "value1"
        
        result = self.ctx.delete("key1")
        assert result is True
        assert self.ctx.get("key1") is None
    
    def test_delete_nonexistent(self):
        """删除不存在的key"""
        result = self.ctx.delete("nonexistent")
        assert result is False
    
    def test_exists(self):
        """存在性检查"""
        self.ctx.set("key1", "value1")
        assert self.ctx.exists("key1") is True
        assert self.ctx.exists("nonexistent") is False
    
    def test_get_entry(self):
        """获取完整条目"""
        self.ctx.set("key1", "value1", agent_id="agent-1", tags={"tag1"})
        entry = self.ctx.get_entry("key1")
        
        assert entry is not None
        assert entry.key == "key1"
        assert entry.value == "value1"
        assert entry.owner_agent_id == "agent-1"
        assert "tag1" in entry.tags


class TestSharedContextScope:
    """作用域测试"""
    
    def setup_method(self):
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_scope_filtering(self):
        """按作用域过滤"""
        self.ctx.set("global_key", "global_value", scope=ContextScope.GLOBAL)
        self.ctx.set("agent_key", "agent_value", scope=ContextScope.AGENT_LOCAL, agent_id="agent-1")
        self.ctx.set("task_key", "task_value", scope=ContextScope.TASK_LOCAL)
        
        # 按key前缀查询
        global_keys = self.ctx.query_keys(prefix="global")
        agent_keys = self.ctx.query_keys(prefix="agent")
        task_keys = self.ctx.query_keys(prefix="task")
        
        assert "global_key" in global_keys
        assert "agent_key" in agent_keys
        assert "task_key" in task_keys
    
    def test_agent_id_tracking(self):
        """Agent ID追踪"""
        self.ctx.set("key1", "v1", agent_id="agent-1")
        self.ctx.set("key2", "v2", agent_id="agent-2")
        
        keys_agent1 = self.ctx.query_keys(agent_id="agent-1")
        keys_agent2 = self.ctx.query_keys(agent_id="agent-2")
        
        assert "key1" in keys_agent1
        assert "key2" not in keys_agent1
        assert "key2" in keys_agent2


class TestSharedContextTTL:
    """TTL过期机制测试"""
    
    def setup_method(self):
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_ttl_not_expired(self):
        """TTL未过期"""
        self.ctx.set("key1", "value1", ttl_seconds=3600)
        assert self.ctx.get("key1") == "value1"
        assert self.ctx.exists("key1") is True
    
    def test_ttl_expired(self):
        """TTL已过期"""
        # 创建一个已过期的条目
        entry = ContextEntry(
            key="expired_key",
            value="expired_value",
            scope=ContextScope.GLOBAL,
            ttl_seconds=1,
        )
        entry.created_at = datetime.now() - timedelta(seconds=2)
        
        with self.ctx._lock:
            self.ctx._data["expired_key"] = entry
        
        assert self.ctx.exists("expired_key") is False
        assert self.ctx.get("expired_key") is None
    
    def test_cleanup_expired(self):
        """清理过期条目"""
        # 添加一个正常条目
        self.ctx.set("normal_key", "value")
        
        # 添加一个过期条目
        expired_entry = ContextEntry(
            key="expired_key",
            value="expired_value",
            scope=ContextScope.GLOBAL,
            ttl_seconds=1,
        )
        expired_entry.created_at = datetime.now() - timedelta(seconds=2)
        
        with self.ctx._lock:
            self.ctx._data["expired_key"] = expired_entry
        
        count = self.ctx.cleanup_expired()
        assert count == 1
        assert self.ctx.exists("expired_key") is False
        assert self.ctx.exists("normal_key") is True


class TestSharedContextBatch:
    """批量操作测试"""
    
    def setup_method(self):
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_multi_get(self):
        """批量获取"""
        self.ctx.set("key1", "value1")
        self.ctx.set("key2", "value2")
        self.ctx.set("key3", "value3")
        
        result = self.ctx.multi_get(["key1", "key2", "nonexistent"])
        assert result == {"key1": "value1", "key2": "value2", "nonexistent": None}
    
    def test_multi_set(self):
        """批量设置"""
        items = {
            "key1": "value1",
            "key2": "value2",
            "key3": "value3",
        }
        
        count = self.ctx.multi_set(items, agent_id="agent-1")
        assert count == 3
        
        assert self.ctx.get("key1") == "value1"
        assert self.ctx.get("key2") == "value2"
        assert self.ctx.get("key3") == "value3"
    
    def test_clear_by_scope(self):
        """按作用域清除"""
        self.ctx.set("g1", "v1", scope=ContextScope.GLOBAL)
        self.ctx.set("g2", "v2", scope=ContextScope.GLOBAL)
        self.ctx.set("a1", "v3", scope=ContextScope.AGENT_LOCAL, agent_id="agent-1")
        
        count = self.ctx.clear(scope=ContextScope.GLOBAL)
        assert count == 2
        
        assert self.ctx.exists("g1") is False
        assert self.ctx.exists("g2") is False
        assert self.ctx.exists("a1") is True
    
    def test_clear_by_agent(self):
        """按Agent清除"""
        self.ctx.set("k1", "v1", agent_id="agent-1")
        self.ctx.set("k2", "v2", agent_id="agent-2")
        self.ctx.set("k3", "v3", agent_id="agent-1")
        
        count = self.ctx.clear(agent_id="agent-1")
        assert count == 2
        
        assert self.ctx.exists("k1") is False
        assert self.ctx.exists("k2") is True
        assert self.ctx.exists("k3") is False


class TestSharedContextQuery:
    """查询测试"""
    
    def setup_method(self):
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_query_all(self):
        """查询所有"""
        self.ctx.set("key1", "v1")
        self.ctx.set("key2", "v2")
        
        query = ContextQuery()
        results = self.ctx.query(query)
        
        assert len(results) == 2
    
    def test_query_by_scope(self):
        """按作用域查询"""
        self.ctx.set("g1", "v1", scope=ContextScope.GLOBAL)
        self.ctx.set("a1", "v2", scope=ContextScope.AGENT_LOCAL, agent_id="agent-1")
        
        query = ContextQuery(scopes=[ContextScope.GLOBAL])
        results = self.ctx.query(query)
        
        assert len(results) == 1
        assert results[0].key == "g1"


class TestSharedContextSubscribe:
    """订阅机制测试"""
    
    def setup_method(self):
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_subscribe_and_unsubscribe(self):
        """订阅和取消订阅"""
        events = []
        
        def callback(event, entry):
            events.append((event, entry.key))
        
        sub_id = self.ctx.subscribe(callback)
        assert sub_id is not None
        
        self.ctx.set("key1", "value1")
        self.ctx.update("key1", "value2")
        self.ctx.delete("key1")
        
        assert len(events) == 3
        assert events[0][0] == ContextEvent.CREATED
        assert events[1][0] == ContextEvent.UPDATED
        assert events[2][0] == ContextEvent.DELETED
        
        # 取消订阅
        self.ctx.unsubscribe(sub_id)
        self.ctx.set("key2", "value2")
        assert len(events) == 3  # 不再增加


class TestSharedContextSerialization:
    """序列化测试"""
    
    def setup_method(self):
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_to_dict_from_dict(self):
        """导出和导入"""
        self.ctx.set_session_id("session-123")
        self.ctx.set("key1", "value1", agent_id="agent-1")
        self.ctx.set("key2", {"nested": "value"}, scope=ContextScope.AGENT_LOCAL)
        
        data = self.ctx.to_dict()
        assert data["session_id"] == "session-123"
        assert len(data["entries"]) == 2
        
        # 重新创建并导入
        SharedContext.reset_instance()
        new_ctx = SharedContext.get_instance()
        new_ctx.from_dict(data)
        
        assert new_ctx.get_session_id() == "session-123"
        assert new_ctx.get("key1") == "value1"
        assert new_ctx.get("key2") == {"nested": "value"}
    
    def test_export_import_json(self):
        """JSON导出导入"""
        self.ctx.set("key1", "value1")
        self.ctx.set("key2", 123)
        
        json_str = self.ctx.export_json()
        assert "key1" in json_str
        assert "value1" in json_str
        
        SharedContext.reset_instance()
        new_ctx = SharedContext.get_instance()
        new_ctx.import_json(json_str)
        
        assert new_ctx.get("key1") == "value1"
        assert new_ctx.get("key2") == 123


class TestSharedContextUtilities:
    """工具方法测试"""
    
    def setup_method(self):
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_increment(self):
        """原子递增"""
        self.ctx.set("counter", 10)
        new_val = self.ctx.increment("counter")
        
        assert new_val == 11
        assert self.ctx.get("counter") == 11
    
    def test_increment_nonexistent(self):
        """递增不存在的key"""
        new_val = self.ctx.increment("new_counter")
        assert new_val == 1
        assert self.ctx.get("new_counter") == 1
    
    def test_increment_invalid_type(self):
        """递增非数值类型"""
        self.ctx.set("string_val", "not a number")
        with pytest.raises(TypeError):
            self.ctx.increment("string_val")
    
    def test_append_list(self):
        """追加列表元素"""
        self.ctx.set("my_list", [1, 2])
        length = self.ctx.append_list("my_list", 3)
        
        assert length == 3
        assert self.ctx.get("my_list") == [1, 2, 3]
    
    def test_append_list_max_length(self):
        """追加列表元素并限制长度"""
        self.ctx.set("bounded_list", [1, 2, 3])
        length = self.ctx.append_list("bounded_list", 4, max_length=3)
        
        assert length == 3
        assert self.ctx.get("bounded_list") == [2, 3, 4]
    
    def test_get_stats(self):
        """统计信息"""
        self.ctx.set("key1", "v1")
        self.ctx.set("key2", "v2", scope=ContextScope.AGENT_LOCAL)
        
        stats = self.ctx.get_stats()
        
        assert stats["total"] == 2
        assert stats["active"] == 2
        assert stats["by_scope"]["global"] == 1
        assert stats["by_scope"]["agent"] == 1


class TestSharedContextThreadSafety:
    """线程安全测试"""
    
    def setup_method(self):
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_concurrent_set(self):
        """并发设置"""
        errors = []
        results = []
        
        def worker(i):
            try:
                key = f"key_{i}"
                self.ctx.set(key, f"value_{i}", agent_id=f"agent_{i % 3}")
                results.append(self.ctx.get(key))
            except Exception as e:
                errors.append(e)
        
        threads = [threading.Thread(target=worker, args=(i,)) for i in range(50)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        
        assert len(errors) == 0
        assert len(results) == 50
    
    def test_concurrent_get_set(self):
        """并发读写"""
        self.ctx.set("shared_key", 0)
        errors = []
        
        def incrementer():
            try:
                for _ in range(100):
                    val = self.ctx.get("shared_key")
                    self.ctx.set("shared_key", val + 1)
            except Exception as e:
                errors.append(e)
        
        threads = [threading.Thread(target=incrementer) for _ in range(5)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        
        assert len(errors) == 0
        # 注意：由于不是原子操作，最终值可能小于500
        # 但至少不应该有异常


class TestSharedContextEphemeral:
    """临时上下文测试"""
    
    def setup_method(self):
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_ephemeral_basic(self):
        """临时上下文基本用法"""
        self.ctx.set(
            "temp_result",
            "result_data",
            scope=ContextScope.EPHEMERAL,
            ttl_seconds=30,
        )
        
        assert self.ctx.get("temp_result") == "result_data"
        assert self.ctx.exists("temp_result") is True
    
    def test_ephemeral_auto_expire(self):
        """临时上下文自动过期"""
        # 创建一个已过期的临时条目
        entry = ContextEntry(
            key="auto_expire",
            value="data",
            scope=ContextScope.EPHEMERAL,
            ttl_seconds=1,
        )
        entry.created_at = datetime.now() - timedelta(seconds=2)
        
        with self.ctx._lock:
            self.ctx._data["auto_expire"] = entry
        
        assert self.ctx.exists("auto_expire") is False


class TestSharedContextSession:
    """Session关联测试"""
    
    def setup_method(self):
        SharedContext.reset_instance()
        self.ctx = SharedContext.get_instance()
    
    def test_session_id(self):
        """Session ID关联"""
        self.ctx.set_session_id("session-abc")
        assert self.ctx.get_session_id() == "session-abc"
    
    def test_session_id_in_export(self):
        """导出时包含Session ID"""
        self.ctx.set_session_id("session-xyz")
        self.ctx.set("key1", "value1")
        
        data = self.ctx.to_dict()
        assert data["session_id"] == "session-xyz"
