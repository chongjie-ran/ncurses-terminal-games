"""
pytest配置和共享fixtures
"""
import pytest
import os

PROJECT_ROOT = "/Users/chongjieran/.openclaw/workspace-developer/projects/javis-claude-core"

from src.session import Session, MessageRole, ContentBlock, ConversationMessage, TokenUsage
from src.runtime import ConversationRuntime, ToolExecutor, ToolError, RuntimeError


@pytest.fixture
def empty_session():
    """空会话"""
    return Session()


@pytest.fixture
def session_with_messages():
    """包含消息的会话"""
    session = Session()
    session.add_user_message("Hello")
    session.add_assistant_message(
        [ContentBlock.text_block("Hi there!")],
        usage=TokenUsage(input_tokens=5, output_tokens=10)
    )
    session.add_user_message("How are you?")
    return session


@pytest.fixture
def session_with_tool_calls():
    """包含工具调用的会话"""
    session = Session()
    session.add_user_message("Run ls command")
    session.add_assistant_message([
        ContentBlock.tool_use(
            id="tool_001",
            name="bash",
            input='{"command": "ls"}'
        )
    ])
    session.add_tool_result(
        tool_use_id="tool_001",
        tool_name="bash",
        output="file1.txt\nfile2.txt"
    )
    session.add_assistant_message([
        ContentBlock.text_block("Here are the files:")
    ])
    return session


@pytest.fixture
def session_with_error():
    """包含工具错误的会话"""
    session = Session()
    session.add_user_message("Run failing command")
    session.add_assistant_message([
        ContentBlock.tool_use(
            id="tool_002",
            name="bash",
            input='{"command": "exit 1"}'
        )
    ])
    session.add_tool_result(
        tool_use_id="tool_002",
        tool_name="bash",
        output="Command failed",
        is_error=True
    )
    return session


@pytest.fixture(autouse=True)
def set_cwd_to_project_root():
    """自动设置工作目录为项目根目录，确保相对路径测试正确解析"""
    old_cwd = os.getcwd()
    os.chdir(PROJECT_ROOT)
    yield
    os.chdir(old_cwd)


@pytest.fixture
def simple_tool_executor():
    """简单的工具执行器mock"""
    class SimpleToolExecutor:
        def execute(self, tool_name: str, input_str: str) -> str:
            if tool_name == "bash":
                return '{"stdout": "test output", "return_code": 0}'
            return '{"error": "unknown tool"}'
    return SimpleToolExecutor()
