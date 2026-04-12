"""
compact.py - 会话压缩模块

对应Claude Code Rust: compact.rs
"""

from __future__ import annotations
from dataclasses import dataclass
from typing import List, Optional

from src.session import Session, ConversationMessage, ContentBlock


@dataclass
class CompactionResult:
    original_messages: int
    compacted_messages: int
    original_tokens: int
    compacted_tokens: int
    summary: str


class SessionCompactor:
    CHARS_PER_TOKEN = 4
    DEFAULT_THRESHOLD_TOKENS = 150000
    MIN_MESSAGES_TO_KEEP = 10
    
    def __init__(self, threshold_tokens: int = None):
        self.threshold_tokens = threshold_tokens or self.DEFAULT_THRESHOLD_TOKENS
    
    def estimate_tokens(self, session: Session) -> int:
        total_chars = 0
        for msg in session.messages:
            total_chars += len(msg.role.value)
            for block in msg.blocks:
                if block.text: total_chars += len(block.text)
                if block.input: total_chars += len(block.input)
                if block.output: total_chars += len(block.output)
        return total_chars // self.CHARS_PER_TOKEN
    
    def should_compact(self, session: Session) -> bool:
        return self.estimate_tokens(session) >= self.threshold_tokens
    
    def compact(self, session: Session) -> CompactionResult:
        original_count = len(session.messages)
        original_tokens = self.estimate_tokens(session)
        
        keep_count = self.MIN_MESSAGES_TO_KEEP
        if len(session.messages) <= keep_count:
            return CompactionResult(
                original_messages=original_count,
                compacted_messages=original_count,
                original_tokens=original_tokens,
                compacted_tokens=original_tokens,
                summary="No compaction needed",
            )
        
        to_compact = session.messages[:-keep_count]
        to_keep = session.messages[-keep_count:]
        
        summary = self._generate_summary(to_compact)
        
        compacted = Session(version=session.version, messages=[])
        compacted.messages.append(ConversationMessage(
            role=session.messages[0].role,
            blocks=[ContentBlock.text_block(f"[Earlier conversation summarized: {summary}]")],
        ))
        compacted.messages.extend(to_keep)
        
        compacted_tokens = self.estimate_tokens(compacted)
        
        return CompactionResult(
            original_messages=original_count,
            compacted_messages=len(compacted.messages),
            original_tokens=original_tokens,
            compacted_tokens=compacted_tokens,
            summary=summary,
        )
    
    def _generate_summary(self, messages: List[ConversationMessage]) -> str:
        summaries = []
        for msg in messages:
            if msg.role.value == "user":
                texts = [b.text for b in msg.blocks if b.text]
                if texts:
                    summaries.append(f"User: {texts[0][:100]}")
            elif msg.role.value == "assistant":
                tools = [b.name for b in msg.blocks if b.type == "tool_use" and b.name]
                if tools:
                    summaries.append(f"Assistant used: {', '.join(tools)}")
        return " | ".join(summaries[:20])
    
    def get_compact_continuation_message(self) -> str:
        return "Earlier conversation has been summarized. Key context preserved."
