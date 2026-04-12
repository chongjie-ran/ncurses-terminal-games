"""
runtime.py - 对话运行时模块

对应Claude Code Rust: conversation.rs
核心功能：
- ToolExecutor trait
- ApiClient trait
- ConversationRuntime
"""

from __future__ import annotations
from abc import ABC, abstractmethod
from typing import Protocol, Optional, List, Dict, Any
from dataclasses import dataclass, field
from enum import Enum

from .session import Session, ConversationMessage, ContentBlock, TokenUsage


class ToolError(Exception):
    """工具执行错误"""
    pass


class RuntimeError(Exception):
    """运行时错误"""
    pass


class ToolExecutor(Protocol):
    """工具执行器协议"""
    def execute(self, tool_name: str, input: str) -> str:
        ...


@dataclass
class TurnSummary:
    """对话轮次摘要"""
    input_tokens: int
    output_tokens: int
    total_tokens: int
    tool_calls: int
    duration_ms: int


class ConversationRuntime:
    """对话运行时"""
    
    def __init__(
        self,
        session: Session,
        tool_executor: Optional[ToolExecutor] = None,
    ):
        self.session = session
        self.tool_executor = tool_executor
        self.max_iterations = 100
        self.iteration = 0
    
    def add_user_message(self, text: str) -> None:
        """添加用户消息"""
        self.session.add_user_message(text)
    
    def run(self) -> TurnSummary:
        """运行对话循环"""
        import time
        start = time.time()
        tool_calls = 0
        
        while self.iteration < self.max_iterations:
            self.iteration += 1
            # 执行逻辑
            if not self._has_pending_tool_calls():
                break
        
        duration_ms = int((time.time() - start) * 1000)
        total_tokens = sum(
            m.usage.total_tokens() if m.usage else 0
            for m in self.session.messages
        )
        
        return TurnSummary(
            input_tokens=0,
            output_tokens=0,
            total_tokens=total_tokens,
            tool_calls=tool_calls,
            duration_ms=duration_ms,
        )
    
    def _has_pending_tool_calls(self) -> bool:
        """检查是否有待处理的工具调用"""
        for msg in reversed(self.session.messages):
            if msg.role.value == "assistant":
                for block in msg.blocks:
                    if block.type == "tool_use" and block.id:
                        return True
        return False


@dataclass
class ApiRequest:
    """API请求"""
    system_prompt: List[str]
    messages: List[Dict[str, Any]]


@dataclass
class AssistantEvent:
    """助手事件"""
    type: str
    text: Optional[str] = None
    id: Optional[str] = None
    name: Optional[str] = None
    input: Optional[str] = None
    usage: Optional[TokenUsage] = None


class ApiClient(Protocol):
    """API客户端协议"""
    def stream(self, request: ApiRequest) -> List[AssistantEvent]:
        ...
