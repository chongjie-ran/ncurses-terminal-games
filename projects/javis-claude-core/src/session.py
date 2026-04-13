"""
session.py - 会话管理模块

对应Claude Code Rust: session.rs
"""

from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum
from typing import Optional, List, Dict, Any
import json


class MessageRole(Enum):
    SYSTEM = "system"
    USER = "user"
    ASSISTANT = "assistant"
    TOOL = "tool"


@dataclass
class TokenUsage:
    input_tokens: int = 0
    output_tokens: int = 0
    cache_creation_input_tokens: int = 0
    cache_read_input_tokens: int = 0
    
    def total_tokens(self) -> int:
        return (self.input_tokens + self.output_tokens + 
                self.cache_creation_input_tokens + self.cache_read_input_tokens)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            "input_tokens": self.input_tokens,
            "output_tokens": self.output_tokens,
            "cache_creation_input_tokens": self.cache_creation_input_tokens,
            "cache_read_input_tokens": self.cache_read_input_tokens,
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "TokenUsage":
        return cls(
            input_tokens=data.get("input_tokens", 0),
            output_tokens=data.get("output_tokens", 0),
            cache_creation_input_tokens=data.get("cache_creation_input_tokens", 0),
            cache_read_input_tokens=data.get("cache_read_input_tokens", 0),
        )


@dataclass
class ContentBlock:
    type: str
    text: Optional[str] = None
    id: Optional[str] = None
    name: Optional[str] = None
    input: Optional[str] = None
    tool_use_id: Optional[str] = None
    tool_name: Optional[str] = None
    output: Optional[str] = None
    is_error: bool = False
    
    @classmethod
    def text_block(cls, text: str) -> "ContentBlock":
        return cls(type="text", text=text)
    
    @classmethod
    def tool_use(cls, id: str, name: str, input: str) -> "ContentBlock":
        return cls(type="tool_use", id=id, name=name, input=input)
    
    @classmethod
    def tool_result(cls, tool_use_id: str, tool_name: str,
                    output: str, is_error: bool = False) -> "ContentBlock":
        return cls(type="tool_result", tool_use_id=tool_use_id, 
                   tool_name=tool_name, output=output, is_error=is_error)
    
    def to_dict(self) -> Dict[str, Any]:
        result = {"type": self.type}
        if self.text is not None: result["text"] = self.text
        if self.id is not None: result["id"] = self.id
        if self.name is not None: result["name"] = self.name
        if self.input is not None: result["input"] = self.input
        if self.tool_use_id is not None: result["tool_use_id"] = self.tool_use_id
        if self.tool_name is not None: result["tool_name"] = self.tool_name
        if self.output is not None: result["output"] = self.output
        # Fix: 始终输出 is_error 字段，保持序列化一致性
        if self.type == "tool_result" or self.is_error:
            result["is_error"] = self.is_error
        return result
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "ContentBlock":
        return cls(
            type=data.get("type", "text"),
            text=data.get("text"),
            id=data.get("id"),
            name=data.get("name"),
            input=data.get("input"),
            tool_use_id=data.get("tool_use_id"),
            tool_name=data.get("tool_name"),
            output=data.get("output"),
            is_error=data.get("is_error", False),
        )


@dataclass
class ConversationMessage:
    role: MessageRole
    blocks: List[ContentBlock] = field(default_factory=list)
    usage: Optional[TokenUsage] = None
    
    @classmethod
    def user_text(cls, text: str) -> "ConversationMessage":
        return cls(role=MessageRole.USER, blocks=[ContentBlock.text_block(text)])
    
    @classmethod
    def assistant(cls, blocks: List[ContentBlock],
                  usage: Optional[TokenUsage] = None) -> "ConversationMessage":
        return cls(role=MessageRole.ASSISTANT, blocks=blocks, usage=usage)
    
    @classmethod
    def tool_result(cls, tool_use_id: str, tool_name: str,
                    output: str, is_error: bool = False) -> "ConversationMessage":
        return cls(
            role=MessageRole.TOOL,
            blocks=[ContentBlock.tool_result(tool_use_id, tool_name, output, is_error)]
        )
    
    def to_dict(self) -> Dict[str, Any]:
        result = {
            "role": self.role.value,
            "blocks": [b.to_dict() for b in self.blocks]
        }
        if self.usage:
            result["usage"] = self.usage.to_dict()
        return result
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "ConversationMessage":
        role = MessageRole(data.get("role", "user"))
        blocks = [ContentBlock.from_dict(b) for b in data.get("blocks", [])]
        usage = None
        if "usage" in data:
            usage = TokenUsage.from_dict(data["usage"])
        return cls(role=role, blocks=blocks, usage=usage)


@dataclass
class Session:
    version: int = 1
    messages: List[ConversationMessage] = field(default_factory=list)
    
    def add_message(self, message: ConversationMessage) -> None:
        self.messages.append(message)
    
    def add_user_message(self, text: str) -> None:
        self.messages.append(ConversationMessage.user_text(text))
    
    def add_assistant_message(self, blocks: List[ContentBlock],
                              usage: Optional[TokenUsage] = None) -> None:
        self.messages.append(ConversationMessage.assistant(blocks, usage))
    
    def add_tool_result(self, tool_use_id: str, tool_name: str,
                        output: str, is_error: bool = False) -> None:
        self.messages.append(ConversationMessage.tool_result(
            tool_use_id, tool_name, output, is_error
        ))
    
    def to_json(self) -> str:
        data = {
            "version": self.version,
            "messages": [m.to_dict() for m in self.messages]
        }
        return json.dumps(data, ensure_ascii=False)
    
    def save_to_path(self, path: str) -> None:
        with open(path, "w", encoding="utf-8") as f:
            f.write(self.to_json())
    
    @classmethod
    def from_json(cls, json_str: str) -> "Session":
        data = json.loads(json_str)
        messages = [ConversationMessage.from_dict(m) for m in data.get("messages", [])]
        return cls(version=data.get("version", 1), messages=messages)
    
    @classmethod
    def load_from_path(cls, path: str) -> "Session":
        with open(path, "r", encoding="utf-8") as f:
            return cls.from_json(f.read())
    
    def get_messages_for_api(self) -> List[Dict[str, Any]]:
        """转换为 API 格式。TOOL 消息中的每个 tool_result 产生一条独立记录。"""
        result = []
        for msg in self.messages:
            msg_dict = {"role": msg.role.value}
            if msg.role == MessageRole.USER:
                texts = [b.text for b in msg.blocks if b.text]
                msg_dict["content"] = " ".join(texts) if texts else ""
                result.append(msg_dict)
            elif msg.role == MessageRole.ASSISTANT:
                contents = []
                for b in msg.blocks:
                    if b.type == "text" and b.text:
                        contents.append({"type": "text", "text": b.text})
                    elif b.type == "tool_use":
                        try:
                            inp = json.loads(b.input) if b.input else {}
                        except (json.JSONDecodeError, TypeError):
                            inp = b.input if b.input else {}
                        contents.append({
                            "type": "tool_use",
                            "id": b.id,
                            "name": b.name,
                            "input": inp
                        })
                msg_dict["content"] = contents
                result.append(msg_dict)
            elif msg.role == MessageRole.TOOL:
                # 每个 tool_result block 产生一条独立的 tool 消息
                for b in msg.blocks:
                    if b.type == "tool_result":
                        result.append({
                            "role": "tool",
                            "tool_call_id": b.tool_use_id,
                            "content": b.output or ""
                        })
            elif msg.role == MessageRole.SYSTEM:
                texts = [b.text for b in msg.blocks if b.text]
                msg_dict["content"] = " ".join(texts) if texts else ""
                result.append(msg_dict)
        return result
