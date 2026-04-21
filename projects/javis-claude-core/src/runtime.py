"""
runtime.py - 对话运行时模块 V3.4

V3.4 Phase1-B 实现
整合 compact/agent_quota/cron_alert 模块

核心功能：
- ConversationRuntime: 整合所有模块的对话运行时
- Token预算控制 (SessionCompactor集成)
- Agent配额检查点 (AgentQuotaManager集成)
- Cron告警 (CronAlertManager集成)
"""

from __future__ import annotations
from abc import ABC, abstractmethod
from typing import Protocol, Optional, List, Dict, Any, Callable
from dataclasses import dataclass, field
from enum import Enum
import time
import threading
import logging

from .session import Session, ConversationMessage, ContentBlock, TokenUsage
from .compact import SessionCompactor, CompactionResult
from .agent_quota import (
    AgentQuotaManager, QuotaType, QuotaExceeded, QuotaWarning,
    get_default_manager as get_quota_manager,
)
from .cron_alert import (
    CronAlertManager, alert_cron_failure,
    get_default_alert_manager as get_alert_manager,
)

from execution_trace import warn as _et_warn

logger = logging.getLogger(__name__)


# ============================================================================
# Exceptions
# ============================================================================

class ToolError(Exception):
    """工具执行错误"""
    pass


class RuntimeError(Exception):
    """运行时错误"""
    pass


class ConversationEndedError(RuntimeError):
    """对话已结束"""
    pass


# ============================================================================
# Protocols
# ============================================================================

class ToolExecutor(Protocol):
    """工具执行器协议"""
    def execute(self, tool_name: str, input: str) -> str:
        ...


# ============================================================================
# Data Classes
# ============================================================================

@dataclass
class TurnSummary:
    """对话轮次摘要"""
    input_tokens: int
    output_tokens: int
    total_tokens: int
    tool_calls: int
    duration_ms: int
    compacted: bool = False
    compaction_result: Optional[CompactionResult] = None


@dataclass
class RuntimeConfig:
    """运行时配置"""
    max_iterations: int = 100
    compact_threshold_tokens: int = 150000
    compact_min_messages_to_keep: int = 10
    enable_quota_check: bool = True
    enable_cron_alert: bool = True
    quota_warning_threshold: float = 0.8


@dataclass
class AgentRunContext:
    """Agent运行上下文"""
    agent_id: str
    session: Session
    runtime_config: RuntimeConfig
    quota_manager: AgentQuotaManager
    alert_manager: CronAlertManager
    compactor: SessionCompactor
    _lock: threading.Lock = field(default_factory=threading.Lock)


# ============================================================================
# ConversationRuntime V3.4
# ============================================================================

class ConversationRuntime:
    """
    V3.4 对话运行时
    
    整合所有模块的统一运行时：
    1. SessionCompactor: Token预算控制，自动压缩过长会话
    2. AgentQuotaManager: Agent消息配额检查
    3. CronAlertManager: 任务失败告警
    
    使用方式:
        runtime = ConversationRuntime(session, agent_id="悟通")
        runtime.add_user_message("Hello")
        summary = runtime.run()
    """
    
    def __init__(
        self,
        session: Session,
        agent_id: str = "default",
        config: Optional[RuntimeConfig] = None,
        tool_executor: Optional[ToolExecutor] = None,
    ):
        self._session = session
        self.agent_id = agent_id
        self.config = config or RuntimeConfig()
        self.tool_executor = tool_executor
        
        # 初始化组件
        self._compactor = SessionCompactor(
            threshold_tokens=self.config.compact_threshold_tokens,
        )
        self._compactor.MIN_MESSAGES_TO_KEEP = self.config.compact_min_messages_to_keep
        
        self._quota_manager = get_quota_manager()
        self._alert_manager = get_alert_manager()
        
        # 运行时状态
        self._iteration = 0
        self._tool_calls = 0
        self._running = False
        self._last_compact_result: Optional[CompactionResult] = None
        
        # 注册配额警告回调
        self._quota_manager.register_notification_callback(self._on_quota_warning)
    
    @property
    def max_iterations(self) -> int:
        """最大迭代次数（兼容旧接口）"""
        return self.config.max_iterations
    
    @max_iterations.setter
    def max_iterations(self, value: int) -> None:
        self.config.max_iterations = value
    
    @property
    def iteration(self) -> int:
        """当前迭代次数（兼容旧接口）"""
        return self._iteration
    
    @property
    def session(self) -> Session:
        """会话对象"""
        return self._session
    
    @property
    def tool_calls(self) -> int:
        """工具调用次数"""
        return self._tool_calls
    
    # ========================================================================
    # Public API
    # ========================================================================
    
    def add_user_message(self, text: str) -> None:
        """添加用户消息"""
        self._session.add_user_message(text)
        self._check_quota()
    
    def add_assistant_message(self, blocks: List[ContentBlock]) -> None:
        """添加助手消息"""
        self._session.add_assistant_message(blocks)
    
    def run(self) -> TurnSummary:
        """
        运行对话循环
        
        Returns:
            TurnSummary: 本轮执行摘要
        """
        import time
        start = time.time()
        self._running = True
        self._iteration = 0
        self._tool_calls = 0
        
        try:
            while self._running and self._iteration < self.config.max_iterations:
                self._iteration += 1
                
                # 压缩检查
                compaction_result = self._maybe_compact()
                
                # 执行逻辑
                if not self._has_pending_tool_calls():
                    self._running = False
                    break
                
                # 处理工具调用
                self._process_tool_calls()
            
            # 记录使用量
            self._record_usage()
            
        except QuotaExceeded as e:
            _et_warn(f"Quota exceeded for agent '{self.agent_id}': {e}")
            raise
        except Exception as e:
            # 发送告警
            if self.config.enable_cron_alert:
                self._send_alert("runtime_error", str(e))
            raise
        finally:
            self._running = False
        
        duration_ms = int((time.time() - start) * 1000)
        total_tokens = self._estimate_total_tokens()
        
        return TurnSummary(
            input_tokens=0,
            output_tokens=0,
            total_tokens=total_tokens,
            tool_calls=self._tool_calls,
            duration_ms=duration_ms,
            compacted=self._last_compact_result is not None,
            compaction_result=self._last_compact_result,
        )
    
    def run_single_turn(self) -> TurnSummary:
        """单轮执行（不循环）"""
        import time
        start = time.time()
        
        # 压缩检查
        self._maybe_compact()
        
        # 处理工具调用
        if self._has_pending_tool_calls():
            self._process_tool_calls()
        
        # 记录使用量
        self._record_usage()
        
        duration_ms = int((time.time() - start) * 1000)
        total_tokens = self._estimate_total_tokens()
        
        return TurnSummary(
            input_tokens=0,
            output_tokens=0,
            total_tokens=total_tokens,
            tool_calls=self._tool_calls,
            duration_ms=duration_ms,
            compacted=self._last_compact_result is not None,
            compaction_result=self._last_compact_result,
        )
    
    def stop(self) -> None:
        """停止对话循环"""
        self._running = False
    
    def get_status(self) -> Dict[str, Any]:
        """获取运行时状态"""
        quota_status = self._quota_manager.get_status(self.agent_id)
        compact_tokens = self._compactor.estimate_tokens(self.session)
        
        return {
            "agent_id": self.agent_id,
            "iteration": self._iteration,
            "tool_calls": self._tool_calls,
            "running": self._running,
            "session_messages": len(self._session.messages),
            "session_tokens_estimate": compact_tokens,
            "should_compact": self._compactor.should_compact(self.session),
            "quota": quota_status,
        }
    
    # ========================================================================
    # Token Budget Control (SessionCompactor Integration)
    # ========================================================================
    
    def _maybe_compact(self) -> Optional[CompactionResult]:
        """检查并执行会话压缩"""
        if self._compactor.should_compact(self.session):
            logger.info(
                f"Session exceeds token budget ({self._compactor.estimate_tokens(self.session)} tokens), "
                f"triggering compaction"
            )
            result = self._compactor.compact(self.session)
            self._session = result.compacted_session
            self._last_compact_result = result
            logger.info(
                f"Compaction complete: {result.original_messages} -> {result.compacted_messages} messages, "
                f"tokens: {result.original_tokens} -> {result.compacted_tokens}"
            )
            return result
        return None
    
    def force_compact(self) -> CompactionResult:
        """强制执行会话压缩"""
        result = self._compactor.compact(self.session)
        self._session = result.compacted_session
        self._last_compact_result = result
        return result
    
    def get_compact_status(self) -> Dict[str, Any]:
        """获取压缩状态"""
        tokens = self._compactor.estimate_tokens(self.session)
        return {
            "estimated_tokens": tokens,
            "should_compact": self._compactor.should_compact(self.session),
            "threshold": self._compactor.threshold_tokens,
            "message_count": len(self._session.messages),
            "last_compacted": self._last_compact_result is not None,
        }
    
    # ========================================================================
    # Agent Quota Integration
    # ========================================================================
    
    def _check_quota(self) -> None:
        """检查配额（添加消息前调用）"""
        if not self.config.enable_quota_check:
            return
        
        config = self._quota_manager.get_config(self.agent_id)
        if not config:
            return
        
        try:
            self._quota_manager.check_and_increment(
                self.agent_id,
                QuotaType.MESSAGE_COUNT,
                delta=1,
            )
        except QuotaExceeded as e:
            logger.error(f"Quota exceeded for agent '{self.agent_id}': {e}")
            raise
    
    def _record_usage(self) -> None:
        """记录资源使用量"""
        if not self.config.enable_quota_check:
            return
        
        tokens = self._estimate_total_tokens()
        self._quota_manager.record_usage(
            self.agent_id,
            QuotaType.TOKEN_USAGE,
            delta=tokens,
        )
    
    def _on_quota_warning(
        self,
        agent_id: str,
        quota_type: QuotaType,
        current: int,
        limit: int,
        is_warning: bool,
    ) -> None:
        """配额警告回调"""
        if agent_id != self.agent_id:
            return
        
        level = "warning" if is_warning else "error"
        msg = (
            f"Agent '{agent_id}' quota {quota_type.value}: "
            f"{current}/{limit} ({current/limit*100:.1f}%)"
        )
        logger.log(
            logging.WARNING if is_warning else logging.ERROR,
            msg,
        )
    
    def check_quota(self) -> tuple[bool, Optional[int], Optional[int]]:
        """手动检查配额（不增加）"""
        return self._quota_manager.check_quota(
            self.agent_id,
            QuotaType.MESSAGE_COUNT,
            increment=0,
        )
    
    def reset_quota(self) -> None:
        """重置配额使用量"""
        self._quota_manager.reset_usage(self.agent_id)
    
    # ========================================================================
    # Cron Alert Integration
    # ========================================================================
    
    def _send_alert(self, error_type: str, message: str) -> None:
        """发送任务失败告警"""
        try:
            alert_cron_failure(
                task_name=f"runtime_{self.agent_id}",
                error_message=message,
                error_type=error_type,
                agent_id=self.agent_id,
                iteration=self._iteration,
            )
        except Exception as e:
            logger.error(f"Failed to send cron alert: {e}")
    
    def send_alert(self, task_name: str, error_message: str, error_type: str = "execution_error") -> None:
        """手动发送告警"""
        self._send_alert(error_type, error_message)
    
    # ========================================================================
    # Tool Call Processing
    # ========================================================================
    
    def _has_pending_tool_calls(self) -> bool:
        """检查是否有待处理的工具调用
        
        规则:
        - 逆序遍历消息，如果遇到 tool_result，说明之前的 tool_use 都已被执行
        - 如果遇到 tool_use 且尚未见过 tool_result，说明有 pending 的工具调用
        - tool_result 消息本身不算 pending tool call
        """
        seen_tool_result = False
        
        for msg in reversed(self._session.messages):
            if msg.role.value != "assistant":
                continue
            
            # 如果这个 assistant 消息包含 tool_result
            if any(block.type == "tool_result" for block in msg.blocks):
                seen_tool_result = True
                continue
            
            # 检查该消息中是否有 tool_use
            for block in msg.blocks:
                if block.type == "tool_use" and block.id:
                    if seen_tool_result:
                        # 已经执行过了，不是 pending
                        continue
                    # 还没见过 tool_result，有 pending 工具调用
                    return True
        return False
    
    def _process_tool_calls(self) -> None:
        """处理待处理的工具调用"""
        for msg in reversed(self._session.messages):
            if msg.role.value == "assistant":
                for block in msg.blocks:
                    if block.type == "tool_use" and block.id:
                        self._execute_tool_call(block)
    
    def _execute_tool_call(self, block: ContentBlock) -> None:
        """执行单个工具调用"""
        if not self.tool_executor:
            logger.debug(f"No tool executor configured, skipping tool call: {block.name}")
            return
        
        try:
            result = self.tool_executor.execute(block.name, block.input or "{}")
            
            # 添加工具结果消息
            self._session.add_assistant_message([
                ContentBlock.tool_result(
                    tool_use_id=block.id,
                    content=result,
                    is_error=False,
                )
            ])
            self._tool_calls += 1
            
        except Exception as e:
            logger.error(f"Tool call '{block.name}' failed: {e}")
            self._session.add_assistant_message([
                ContentBlock.tool_result(
                    tool_use_id=block.id,
                    content=f"Error: {str(e)}",
                    is_error=True,
                )
            ])
            
            # 发送告警
            if self.config.enable_cron_alert:
                self._send_alert("tool_error", f"Tool '{block.name}' failed: {str(e)}")
    
    # ========================================================================
    # Utilities
    # ========================================================================
    
    def _estimate_total_tokens(self) -> int:
        """估算总token数"""
        return self._compactor.estimate_tokens(self.session)
    
    def __repr__(self) -> str:
        return (
            f"ConversationRuntime(agent_id={self.agent_id!r}, "
            f"messages={len(self._session.messages)}, "
            f"iteration={self._iteration})"
        )


# ============================================================================
# MCP Integration Helpers
# ============================================================================

def create_mcp_runtime(
    session: Session,
    agent_id: str,
    tool_executor: Optional[ToolExecutor] = None,
    config: Optional[RuntimeConfig] = None,
) -> ConversationRuntime:
    """
    创建集成到MCP的运行时实例
    
    这是创建 ConversationRuntime 的推荐方式，
    会自动配置所有集成组件。
    """
    return ConversationRuntime(
        session=session,
        agent_id=agent_id,
        config=config,
        tool_executor=tool_executor,
    )


# ============================================================================
# API Request/Response Types
# ============================================================================

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


__all__ = [
    'ToolError', 'RuntimeError', 'ConversationEndedError',
    'ToolExecutor', 'TurnSummary', 'RuntimeConfig', 'AgentRunContext',
    'ConversationRuntime', 'create_mcp_runtime',
    'ApiRequest', 'AssistantEvent', 'ApiClient',
]
