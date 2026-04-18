"""
session.py 测试套件 - 对话会话管理模块

测试覆盖：
- Session 会话管理（增删查、序列化）
- ConversationMessage 消息构建
- ContentBlock 内容块
- TokenUsage Token统计
- MessageRole 枚举
"""
import pytest
import json
import os
import tempfile
from src.session import (
    Session, MessageRole, ContentBlock, ConversationMessage, TokenUsage
)


class TestTokenUsage:
    """TokenUsage 测试"""

    def test_default(self):
        t = TokenUsage()
        assert t.total_tokens() == 0

    def test_input_only(self):
        t = TokenUsage(input_tokens=100)
        assert t.total_tokens() == 100

    def test_output_only(self):
        t = TokenUsage(output_tokens=200)
        assert t.total_tokens() == 200

    def test_all_fields(self):
        t = TokenUsage(
            input_tokens=50,
            output_tokens=100,
            cache_creation_input_tokens=25,
            cache_read_input_tokens=75,
        )
        assert t.total_tokens() == 250


class TestContentBlock:
    """ContentBlock 测试"""

    def test_text_block(self):
        block = ContentBlock.text_block("Hello world")
        assert block.type == "text"
        assert block.text == "Hello world"
        assert block.id is None

    def test_tool_use_block(self):
        block = ContentBlock.tool_use(
            id="tool_001",
            name="bash",
            input='{"command": "ls"}'
        )
        assert block.type == "tool_use"
        assert block.id == "tool_001"
        assert block.name == "bash"
        assert block.input == '{"command": "ls"}'

    def test_tool_result_block(self):
        block = ContentBlock.tool_result(
            tool_use_id="tool_001",
            tool_name="bash",
            output="file1.txt",
            is_error=False
        )
        assert block.type == "tool_result"
        assert block.tool_use_id == "tool_001"
        assert block.output == "file1.txt"
        assert block.is_error is False

    def test_tool_result_error(self):
        block = ContentBlock.tool_result(
            tool_use_id="tool_002",
            tool_name="bash",
            output="Command failed",
            is_error=True
        )
        assert block.is_error is True

    def test_to_dict_text(self):
        block = ContentBlock.text_block("Hello")
        d = block.to_dict()
        assert d["type"] == "text"
        assert d["text"] == "Hello"

    def test_to_dict_tool_use(self):
        block = ContentBlock.tool_use(id="t1", name="bash", input='{"cmd": "ls"}')
        d = block.to_dict()
        assert d["type"] == "tool_use"
        assert d["id"] == "t1"
        assert d["name"] == "bash"

    def test_from_dict_roundtrip(self):
        original = ContentBlock.tool_use(id="t1", name="bash", input='{"cmd": "ls"}')
        d = original.to_dict()
        restored = ContentBlock.from_dict(d)
        assert restored.type == original.type
        assert restored.id == original.id
        assert restored.name == original.name
        assert restored.input == original.input


class TestConversationMessage:
    """ConversationMessage 测试"""

    def test_user_text(self):
        msg = ConversationMessage.user_text("Hello")
        assert msg.role == MessageRole.USER
        assert len(msg.blocks) == 1
        assert msg.blocks[0].text == "Hello"
        assert msg.usage is None

    def test_assistant_with_usage(self):
        block = ContentBlock.text_block("Response")
        usage = TokenUsage(input_tokens=5, output_tokens=10)
        msg = ConversationMessage.assistant([block], usage=usage)
        assert msg.role == MessageRole.ASSISTANT
        assert msg.usage.total_tokens() == 15

    def test_tool_result(self):
        msg = ConversationMessage.tool_result(
            tool_use_id="t1",
            tool_name="bash",
            output="ls output",
            is_error=False
        )
        assert msg.role == MessageRole.TOOL
        assert len(msg.blocks) == 1
        assert msg.blocks[0].type == "tool_result"

    def test_to_dict(self):
        msg = ConversationMessage.user_text("Hello")
        d = msg.to_dict()
        assert d["role"] == "user"
        assert len(d["blocks"]) == 1

    def test_to_dict_with_usage(self):
        usage = TokenUsage(input_tokens=5, output_tokens=10)
        msg = ConversationMessage.assistant(
            [ContentBlock.text_block("Hi")],
            usage=usage
        )
        d = msg.to_dict()
        assert "usage" in d
        assert d["usage"]["input_tokens"] == 5
        assert d["usage"]["output_tokens"] == 10

    def test_from_dict_roundtrip(self):
        original = ConversationMessage.assistant(
            [ContentBlock.text_block("Test")],
            usage=TokenUsage(input_tokens=1, output_tokens=2)
        )
        d = original.to_dict()
        restored = ConversationMessage.from_dict(d)
        assert restored.role == original.role
        assert len(restored.blocks) == len(original.blocks)
        assert restored.blocks[0].text == original.blocks[0].text


class TestSession:
    """Session 会话管理测试"""

    def test_empty_session(self, empty_session):
        assert len(empty_session.messages) == 0
        assert empty_session.version == 1

    def test_add_user_message(self, empty_session):
        empty_session.add_user_message("Hello")
        assert len(empty_session.messages) == 1
        assert empty_session.messages[0].role == MessageRole.USER

    def test_add_assistant_message(self, empty_session):
        empty_session.add_assistant_message([ContentBlock.text_block("Hi")])
        assert len(empty_session.messages) == 1
        assert empty_session.messages[0].role == MessageRole.ASSISTANT

    def test_add_tool_result(self, empty_session):
        empty_session.add_tool_result("t1", "bash", "output")
        assert len(empty_session.messages) == 1
        assert empty_session.messages[0].role == MessageRole.TOOL

    def test_add_message_directly(self, empty_session):
        msg = ConversationMessage.user_text("Direct add")
        empty_session.add_message(msg)
        assert len(empty_session.messages) == 1

    def test_to_json(self, session_with_messages):
        """session_with_messages 有3条消息: user/assistant/user"""
        json_str = session_with_messages.to_json()
        data = json.loads(json_str)
        assert data["version"] == 1
        assert len(data["messages"]) == 3

    def test_from_json_roundtrip(self, session_with_messages):
        json_str = session_with_messages.to_json()
        restored = Session.from_json(json_str)
        assert len(restored.messages) == len(session_with_messages.messages)
        # 验证role
        for i, m in enumerate(restored.messages):
            assert m.role == session_with_messages.messages[i].role

    def test_save_and_load(self, session_with_messages):
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            path = f.name
        try:
            session_with_messages.save_to_path(path)
            loaded = Session.load_from_path(path)
            assert len(loaded.messages) == len(session_with_messages.messages)
            # 验证第一条消息
            assert loaded.messages[0].role == MessageRole.USER
        finally:
            os.unlink(path)

    def test_get_messages_for_api_user(self, empty_session):
        empty_session.add_user_message("Hello world")
        api_msgs = empty_session.get_messages_for_api()
        assert len(api_msgs) == 1
        assert api_msgs[0]["role"] == "user"
        assert "Hello world" in api_msgs[0]["content"]

    def test_get_messages_for_api_assistant(self, empty_session):
        empty_session.add_assistant_message([ContentBlock.text_block("Response")])
        api_msgs = empty_session.get_messages_for_api()
        assert len(api_msgs) == 1
        assert api_msgs[0]["role"] == "assistant"

    def test_get_messages_for_api_tool_call(self, session_with_tool_calls):
        api_msgs = session_with_tool_calls.get_messages_for_api()
        # 验证工具调用被正确转换
        tool_calls = [m for m in api_msgs if "content" in m]
        assert len(tool_calls) >= 1

    def test_get_messages_for_api_system(self, empty_session):
        """SYSTEM消息转换为API格式"""
        empty_session.messages = [
            ConversationMessage(role=MessageRole.SYSTEM, blocks=[ContentBlock.text_block("system prompt")])
        ]
        api_msgs = empty_session.get_messages_for_api()
        assert len(api_msgs) == 1
        assert api_msgs[0]["role"] == "system"
        assert api_msgs[0]["content"] == "system prompt"


class TestSessionEdgeCases:
    """Session 边界条件测试"""

    def test_empty_message_text(self):
        """空文本消息"""
        session = Session()
        session.add_user_message("")
        assert len(session.messages) == 1

    def test_very_long_message(self):
        """超长消息"""
        session = Session()
        long_text = "x" * 10000
        session.add_user_message(long_text)
        assert session.messages[0].blocks[0].text == long_text

    def test_special_characters_in_message(self):
        """特殊字符"""
        session = Session()
        special = "你好 🌍 \\n\\t\"'\u2022"
        session.add_user_message(special)
        assert session.messages[0].blocks[0].text == special

    def test_json_special_characters(self, empty_session):
        """JSON序列化特殊字符"""
        empty_session.add_user_message('{"key": "value with \"quotes\""}')
        json_str = empty_session.to_json()
        restored = Session.from_json(json_str)
        assert restored.messages[0].blocks[0].text == empty_session.messages[0].blocks[0].text

    def test_multiple_messages(self, empty_session):
        """多条消息"""
        for i in range(100):
            empty_session.add_user_message(f"Message {i}")
        assert len(empty_session.messages) == 100

    def test_session_version_default(self):
        """默认版本号"""
        session = Session()
        assert session.version == 1

    def test_tool_result_without_error_flag(self):
        """工具结果默认非错误"""
        session = Session()
        session.add_tool_result("t1", "bash", "output")
        block = session.messages[0].blocks[0]
        assert block.is_error is False
