"""
test_v34_runtime_integration.py - V3.4 Phase1-B 运行时集成测试

测试覆盖：
- ConversationRuntime 整合 SessionCompactor (Token预算控制)
- ConversationRuntime 整合 AgentQuotaManager
- ConversationRuntime 整合 CronAlertManager
- 配额检查点
- 会话压缩触发
"""

import pytest
from src.runtime import (
    ConversationRuntime, RuntimeConfig, RuntimeError,
    TurnSummary, ToolError,
)
from src.session import Session, ContentBlock, ConversationMessage, MessageRole
from src.compact import SessionCompactor, CompactionResult
from src.agent_quota import AgentQuotaManager, QuotaType, QuotaExceeded


class TestRuntimeIntegrationInit:
    """运行时初始化测试"""
    
    def test_init_default(self):
        session = Session()
        runtime = ConversationRuntime(session=session)
        
        assert runtime.agent_id == "default"
        assert runtime.config.max_iterations == 100
        assert runtime.config.compact_threshold_tokens == 150000
        assert runtime.config.enable_quota_check is True
        assert runtime.config.enable_cron_alert is True
    
    def test_init_custom_agent_id(self):
        session = Session()
        runtime = ConversationRuntime(session=session, agent_id="悟通")
        assert runtime.agent_id == "悟通"
    
    def test_init_custom_config(self):
        session = Session()
        config = RuntimeConfig(
            max_iterations=50,
            compact_threshold_tokens=50000,
            enable_quota_check=False,
            enable_cron_alert=False,
        )
        runtime = ConversationRuntime(session=session, config=config)
        
        assert runtime.config.max_iterations == 50
        assert runtime.config.compact_threshold_tokens == 50000
        assert runtime.config.enable_quota_check is False
        assert runtime.config.enable_cron_alert is False


class TestRuntimeCompactIntegration:
    """运行时 - 会话压缩集成测试"""
    
    def test_compact_status_below_threshold(self):
        session = Session()
        runtime = ConversationRuntime(session=session)
        
        status = runtime.get_compact_status()
        assert status["should_compact"] is False
        assert status["estimated_tokens"] == 0
        assert status["last_compacted"] is False
    
    def test_compact_status_with_messages(self):
        session = Session()
        for i in range(5):
            session.add_user_message(f"Message {i}")
            session.add_assistant_message([ContentBlock.text_block(f"Response {i}")])
        
        runtime = ConversationRuntime(session=session)
        status = runtime.get_compact_status()
        
        assert status["message_count"] == 10
        assert status["estimated_tokens"] > 0
    
    def test_compact_triggers_above_threshold(self):
        session = Session()
        # 添加大量消息，超过默认阈值
        for i in range(200):
            session.add_user_message("x" * 500)
        
        config = RuntimeConfig(compact_threshold_tokens=10)
        runtime = ConversationRuntime(session=session, config=config)
        
        status = runtime.get_compact_status()
        assert status["should_compact"] is True
    
    def test_force_compact_reduces_messages(self):
        session = Session()
        for i in range(50):
            session.add_user_message(f"User message {i}")
            session.add_assistant_message([ContentBlock.text_block(f"Response {i}")])
        
        runtime = ConversationRuntime(session=session)
        result = runtime.force_compact()
        
        assert isinstance(result, CompactionResult)
        assert result.compacted_messages < result.original_messages
        assert len(result.compacted_session.messages) < 100  # 大幅减少
        assert result.compacted_tokens < result.original_tokens
    
    def test_maybe_compact_no_op_when_below_threshold(self):
        session = Session()
        session.add_user_message("short message")
        
        config = RuntimeConfig(compact_threshold_tokens=1000000)
        runtime = ConversationRuntime(session=session, config=config)
        
        result = runtime._maybe_compact()
        assert result is None
    
    def test_maybe_compact_triggers_when_above_threshold(self):
        session = Session()
        for i in range(50):
            session.add_user_message("x" * 500)
        
        config = RuntimeConfig(compact_threshold_tokens=100)
        runtime = ConversationRuntime(session=session, config=config)
        
        result = runtime._maybe_compact()
        assert result is not None
        assert result.compacted_messages < 100


class TestRuntimeQuotaIntegration:
    """运行时 - 配额管理器集成测试"""
    
    def test_quota_check_passes_when_no_config(self):
        session = Session()
        AgentQuotaManager.reset_instance()
        runtime = ConversationRuntime(session=session, agent_id="test_no_config")
        
        allowed, current, limit = runtime.check_quota()
        # 无配置时应该允许
        assert allowed is True
    
    def test_quota_warning_threshold_default(self):
        session = Session()
        runtime = ConversationRuntime(session=session)
        assert runtime.config.quota_warning_threshold == 0.8


class TestRuntimeStatus:
    """运行时状态测试"""
    
    def test_get_status_initial(self):
        session = Session()
        runtime = ConversationRuntime(session=session, agent_id="test_status")
        
        status = runtime.get_status()
        
        assert status["agent_id"] == "test_status"
        assert status["iteration"] == 0
        assert status["tool_calls"] == 0
        assert status["running"] is False
        assert status["session_messages"] == 0
        assert "session_tokens_estimate" in status
        assert "should_compact" in status
        assert "quota" in status
    
    def test_get_status_with_messages(self):
        session = Session()
        for i in range(5):
            session.add_user_message(f"Message {i}")
        
        runtime = ConversationRuntime(session=session, agent_id="status_test")
        status = runtime.get_status()
        
        assert status["session_messages"] == 5


class TestRuntimeToolCalls:
    """运行时工具调用处理测试"""
    
    def test_add_user_message(self):
        session = Session()
        runtime = ConversationRuntime(session=session)
        
        runtime.add_user_message("Hello")
        assert len(session.messages) == 1
        assert session.messages[0].role == MessageRole.USER
    
    def test_add_assistant_message(self):
        session = Session()
        runtime = ConversationRuntime(session=session)
        
        runtime.add_assistant_message([ContentBlock.text_block("Hi there")])
        assert len(session.messages) == 1
        assert session.messages[0].role == MessageRole.ASSISTANT
    
    def test_has_pending_tool_calls_empty(self):
        session = Session()
        runtime = ConversationRuntime(session=session)
        
        assert runtime._has_pending_tool_calls() is False
    
    def test_has_pending_tool_calls_with_text(self):
        session = Session()
        session.add_user_message("Hello")
        session.add_assistant_message([ContentBlock.text_block("Hi")])
        
        runtime = ConversationRuntime(session=session)
        assert runtime._has_pending_tool_calls() is False
    
    def test_has_pending_tool_calls_with_tool_use(self):
        session = Session()
        session.add_user_message("Run command")
        session.add_assistant_message([
            ContentBlock.tool_use(id="call_123", name="bash", input='{"cmd": "ls"}')
        ])
        
        runtime = ConversationRuntime(session=session)
        assert runtime._has_pending_tool_calls() is True
    
    def test_has_pending_tool_calls_with_tool_result(self):
        session = Session()
        session.add_user_message("Run command")
        session.add_assistant_message([
            ContentBlock.tool_use(id="call_123", name="bash", input='{"cmd": "ls"}')
        ])
        session.add_assistant_message([
            ContentBlock.tool_result(tool_use_id="call_123", tool_name="bash", output="file1\nfile2")
        ])
        
        runtime = ConversationRuntime(session=session)
        # tool_result 不算 pending tool call
        assert runtime._has_pending_tool_calls() is False


class TestRuntimeRun:
    """运行时执行测试"""
    
    def test_run_single_turn_no_tools(self):
        session = Session()
        session.add_user_message("Hello")
        
        runtime = ConversationRuntime(session=session)
        summary = runtime.run_single_turn()
        
        assert isinstance(summary, TurnSummary)
        assert summary.duration_ms >= 0
        assert summary.tool_calls == 0
        assert summary.compacted is False
    
    def test_run_updates_status(self):
        session = Session()
        session.add_user_message("Hello")
        
        runtime = ConversationRuntime(session=session, agent_id="run_test")
        runtime.run_single_turn()
        
        assert runtime.iteration == 0  # single turn doesn't increment iteration
        assert runtime.tool_calls == 0
    
    def test_stop(self):
        session = Session()
        runtime = ConversationRuntime(session=session)
        
        runtime._running = True
        runtime.stop()
        assert runtime._running is False


class TestRuntimeQuotaReset:
    """配额重置测试"""
    
    def test_reset_quota(self):
        session = Session()
        AgentQuotaManager.reset_instance()
        runtime = ConversationRuntime(session=session, agent_id="reset_test")
        
        # 应该不抛出异常
        runtime.reset_quota()


class TestRuntimeCompactResult:
    """压缩结果测试"""
    
    def test_turn_summary_with_compaction(self):
        session = Session()
        for i in range(100):
            session.add_user_message("x" * 500)
        
        config = RuntimeConfig(compact_threshold_tokens=100)
        runtime = ConversationRuntime(session=session, config=config)
        
        runtime.force_compact()
        summary = runtime.run_single_turn()
        
        assert summary.compacted is True
        assert summary.compaction_result is not None
        assert isinstance(summary.compaction_result, CompactionResult)
