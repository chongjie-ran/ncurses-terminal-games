"""
compact.py 测试套件 - 会话压缩模块

测试覆盖：
- SessionCompactor 压缩器
- CompactionResult 结果
- estimate_tokens 估算
- should_compact 决策
- compact 压缩逻辑
"""
import pytest
from src.compact import SessionCompactor, CompactionResult
from src.session import Session, ContentBlock, ConversationMessage, MessageRole


class TestCompactionResult:
    """CompactionResult 测试"""

    def test_create(self):
        result = CompactionResult(
            original_messages=10,
            compacted_messages=3,
            original_tokens=1000,
            compacted_tokens=200,
            summary="Test summary"
        )
        assert result.original_messages == 10
        assert result.compacted_messages == 3
        assert result.original_tokens == 1000
        assert result.compacted_tokens == 200


class TestSessionCompactor:
    """SessionCompactor 测试"""

    def test_init_default(self):
        compactor = SessionCompactor()
        assert compactor.threshold_tokens == 150000

    def test_init_custom_threshold(self):
        compactor = SessionCompactor(threshold_tokens=50000)
        assert compactor.threshold_tokens == 50000

    def test_estimate_tokens_empty(self):
        compactor = SessionCompactor()
        session = Session()
        assert compactor.estimate_tokens(session) == 0

    def test_estimate_tokens_user_message(self):
        compactor = SessionCompactor()
        session = Session()
        session.add_user_message("Hello world")
        tokens = compactor.estimate_tokens(session)
        assert tokens > 0

    def test_estimate_tokens_multiple(self):
        compactor = SessionCompactor()
        session = Session()
        session.add_user_message("Message 1")
        session.add_assistant_message([ContentBlock.text_block("Response 1")])
        session.add_user_message("Message 2")
        tokens = compactor.estimate_tokens(session)
        assert tokens > 0

    def test_should_compact_below_threshold(self):
        compactor = SessionCompactor(threshold_tokens=1000000)
        session = Session()
        for i in range(5):
            session.add_user_message(f"Message {i}")
            session.add_assistant_message([ContentBlock.text_block(f"Response {i}")])
        assert compactor.should_compact(session) is False

    def test_should_compact_above_threshold(self):
        compactor = SessionCompactor(threshold_tokens=10)
        session = Session()
        for i in range(100):
            long_text = "x" * 1000
            session.add_user_message(f"{long_text} message {i}")
        assert compactor.should_compact(session) is True

    def test_compact_no_change_needed(self):
        """消息数<=MIN_MESSAGES_TO_KEEP时应返回原样"""
        compactor = SessionCompactor(threshold_tokens=10)
        session = Session()
        for i in range(5):
            session.add_user_message(f"Short {i}")
        result = compactor.compact(session)
        assert result.original_messages == result.compacted_messages
        assert result.summary == "No compaction needed"

    def test_compact_reduces_messages(self):
        """大量消息应被压缩"""
        compactor = SessionCompactor(threshold_tokens=10)
        session = Session()
        for i in range(50):
            session.add_user_message(f"User message {i}")
            session.add_assistant_message([ContentBlock.text_block(f"Response {i}")])
        result = compactor.compact(session)
        assert result.compacted_messages < result.original_messages
        assert result.original_tokens >= result.compacted_tokens

    def test_compact_preserves_recent(self):
        """压缩后应保留最近消息"""
        compactor = SessionCompactor(threshold_tokens=10)
        session = Session()
        for i in range(30):
            session.add_user_message(f"User {i}")
            session.add_assistant_message([ContentBlock.text_block(f"Response {i}")])
        result = compactor.compact(session)
        # 最后一个消息应被保留
        assert result.compacted_messages <= 11  # 1 summary + 10 recent

    def test_compact_summary_format(self):
        """摘要格式验证：应包含用户消息记录"""
        compactor = SessionCompactor(threshold_tokens=10)
        session = Session()
        session.add_user_message("First user message")
        session.add_assistant_message([ContentBlock.text_block("First response")])
        for i in range(30):
            session.add_user_message(f"User {i}")
            session.add_assistant_message([ContentBlock.text_block(f"R {i}")])
        result = compactor.compact(session)
        # 摘要应包含用户消息
        assert "User:" in result.summary

    def test_compact_tool_calls_included(self):
        """工具调用应被记录在摘要中"""
        compactor = SessionCompactor(threshold_tokens=10)
        session = Session()
        session.add_user_message("Run command")
        session.add_assistant_message([
            ContentBlock.tool_use(id="t1", name="bash", input="{}")
        ])
        for i in range(30):
            session.add_user_message(f"User {i}")
            session.add_assistant_message([ContentBlock.text_block(f"R {i}")])
        result = compactor.compact(session)
        assert "bash" in result.summary

    def test_get_compact_continuation_message(self):
        compactor = SessionCompactor()
        msg = compactor.get_compact_continuation_message()
        assert "Earlier conversation has been summarized" in msg

    def test_estimate_tokens_with_tool_use(self):
        """带工具调用的token估算"""
        compactor = SessionCompactor()
        session = Session()
        session.add_user_message("Run command")
        session.add_assistant_message([
            ContentBlock.tool_use(id="t1", name="bash", input='{"cmd": "ls"}')
        ])
        tokens = compactor.estimate_tokens(session)
        assert tokens > 0
