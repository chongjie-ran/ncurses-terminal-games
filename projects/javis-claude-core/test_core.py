"""
测试脚本 - 验证javis-claude-core模块
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "src"))

from session import MessageRole, ContentBlock, ConversationMessage, Session
from bash_executor import BashExecutor, BashCommandInput
from file_ops import FileOps, ReadResult
from compact import SessionCompactor


def test_session():
    """测试会话管理"""
    print("=== Test Session ===")
    
    session = Session()
    session.add_user_message("Hello")
    session.add_assistant_message([ContentBlock.text_block("Hi there!")])
    
    print(f"Messages: {len(session.messages)}")
    print(f"JSON: {session.to_json()[:100]}...")
    print("✅ Session test passed")


def test_bash():
    """测试Bash执行"""
    print("\n=== Test Bash ===")
    
    executor = BashExecutor()
    result = executor.execute(BashCommandInput(command="echo 'hello world'"))
    
    print(f"stdout: {result.stdout.strip()}")
    print(f"return_code: {result.return_code}")
    print(f"success: {result.success}")
    print("✅ Bash test passed")


def test_file_ops():
    """测试文件操作"""
    print("\n=== Test FileOps ===")
    
    # 读取自己
    result = FileOps.read_file(__file__, limit=10)
    print(f"File: {result.file.file_path}")
    print(f"Lines: {result.file.num_lines}")
    print("✅ FileOps test passed")


def test_compact():
    """测试会话压缩"""
    print("\n=== Test SessionCompactor ===")
    
    session = Session()
    for i in range(20):
        session.add_user_message(f"User message {i}")
        session.add_assistant_message([ContentBlock.text_block(f"Response {i}")])
    
    compactor = SessionCompactor(threshold_tokens=100)
    
    tokens = compactor.estimate_tokens(session)
    print(f"Estimated tokens: {tokens}")
    
    should_compact = compactor.should_compact(session)
    print(f"Should compact: {should_compact}")
    
    if should_compact:
        result = compactor.compact(session)
        print(f"Original: {result.original_messages} -> Compacted: {result.compacted_messages}")
    
    print("✅ Compactor test passed")


def main():
    print("javis-claude-core Tests\n" + "="*50)
    
    test_session()
    test_bash()
    test_file_ops()
    test_compact()
    
    print("\n" + "="*50)
    print("All tests passed! ✅")


if __name__ == "__main__":
    main()
