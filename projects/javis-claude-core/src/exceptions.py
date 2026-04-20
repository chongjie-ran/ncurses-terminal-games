"""
exceptions.py - 用户友好异常处理模块

P1-1 实现 (V3.4 Phase3 Sprint1)
核心功能：
- FallbackSummaryError: 用户可理解的错误消息
- FallbackErrorTranslator: 将技术异常转换为友好消息
- 错误消息包含：问题描述、可能原因、用户可执行的恢复步骤
- 不暴露技术栈细节（Python异常类型、文件路径等）

使用方式：
    try:
        something()
    except Exception as e:
        friendly = FallbackErrorTranslator.translate(e)
        raise FallbackSummaryError.from_translator(friendly) from e
"""

from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum
from typing import Optional, List, Dict, Any, Callable, Type
import logging
import re
import traceback

logger = logging.getLogger(__name__)


# ============================================================================
# Error Category & Recovery Step
# ============================================================================

class ErrorCategory(Enum):
    """错误类别（用户可见）"""
    NETWORK = "network"                   # 网络连接问题
    AUTHENTICATION = "authentication"     # 认证/授权问题
    RESOURCE = "resource"                 # 资源不足
    QUOTA = "quota"                      # 配额超限
    TIMEOUT = "timeout"                  # 操作超时
    PERMISSION = "permission"             # 权限不足
    SERVICE = "service"                   # 服务暂时不可用
    INPUT = "input"                      # 输入/参数问题
    TOOL = "tool"                        # 工具执行失败
    UNKNOWN = "unknown"                  # 未知问题


@dataclass
class RecoveryStep:
    """用户可执行的恢复步骤"""
    order: int
    action: str                    # 简短动作描述，如"重新登录"
    detail: str                    # 详细说明，如"点击登录页的'退出登录'按钮后重新认证"


@dataclass
class FallbackErrorDetail:
    """转换后的友好错误详情"""
    category: ErrorCategory
    title: str                    # 简短标题，如"网络连接失败"
    what_happened: str            # 发生了什么，如"无法连接到服务器"
    possible_causes: List[str]    # 可能原因（2-3条）
    recovery_steps: List[RecoveryStep]  # 恢复步骤（2-4条）
    is_retryable: bool = False    # 是否可以重试
    retry_after_secs: Optional[int] = None  # 重试建议间隔


# ============================================================================
# FallbackSummaryError
# ============================================================================

class FallbackSummaryError(Exception):
    """
    用户友好的错误异常
    
    设计原则：
    - 用户可理解的消息，不包含技术栈细节
    - 提供清晰的恢复步骤
    - 不暴露Python异常类型、文件路径、库版本等敏感信息
    
    Attributes:
        category: 错误类别
        title: 简短标题
        what_happened: 发生了什么
        possible_causes: 可能原因列表
        recovery_steps: 恢复步骤列表
        is_retryable: 是否可重试
        retry_after_secs: 建议重试间隔（秒）
    """
    
    def __init__(
        self,
        category: ErrorCategory,
        title: str,
        what_happened: str,
        possible_causes: List[str],
        recovery_steps: List[RecoveryStep],
        is_retryable: bool = False,
        retry_after_secs: Optional[int] = None,
        *,
        detail: Optional[str] = None,
    ):
        self.category = category
        self.title = title
        self.what_happened = what_happened
        self.possible_causes = possible_causes
        self.recovery_steps = recovery_steps
        self.is_retryable = is_retryable
        self.retry_after_secs = retry_after_secs
        self.detail = detail
        
        # 构建友好消息
        message = self._build_message()
        super().__init__(message)
    
    def _build_message(self) -> str:
        """构建用户友好的错误消息"""
        lines = [f"【{self.title}】", self.what_happened, ""]
        
        if self.possible_causes:
            lines.append("可能原因：")
            for cause in self.possible_causes:
                lines.append(f"  • {cause}")
            lines.append("")
        
        if self.recovery_steps:
            lines.append("建议操作：")
            for step in sorted(self.recovery_steps, key=lambda s: s.order):
                lines.append(f"  {step.order}. {step.action}")
                if step.detail:
                    lines.append(f"     {step.detail}")
            lines.append("")
        
        if self.is_retryable:
            retry_hint = "可以尝试重新操作"
            if self.retry_after_secs:
                retry_hint += f"（建议等待{self.retry_after_secs}秒）"
            lines.append(retry_hint)
        
        return "\n".join(lines)
    
    def to_dict(self) -> Dict[str, Any]:
        """转换为字典（用于API/日志），不包含技术细节"""
        return {
            "category": self.category.value,
            "title": self.title,
            "what_happened": self.what_happened,
            "possible_causes": self.possible_causes,
            "recovery_steps": [
                {"order": s.order, "action": s.action, "detail": s.detail}
                for s in sorted(self.recovery_steps, key=lambda x: x.order)
            ],
            "is_retryable": self.is_retryable,
            "retry_after_secs": self.retry_after_secs,
        }
    
    @classmethod
    def from_translator(cls, detail: FallbackErrorDetail) -> "FallbackSummaryError":
        """从 FallbackErrorDetail 创建 FallbackSummaryError"""
        return cls(
            category=detail.category,
            title=detail.title,
            what_happened=detail.what_happened,
            possible_causes=detail.possible_causes,
            recovery_steps=detail.recovery_steps,
            is_retryable=detail.is_retryable,
            retry_after_secs=detail.retry_after_secs,
        )
    
    @classmethod
    def unknown(cls, what_happened: str = "发生了意外情况") -> "FallbackSummaryError":
        """创建通用未知错误"""
        return cls(
            category=ErrorCategory.UNKNOWN,
            title="操作失败",
            what_happened=what_happened,
            possible_causes=[
                "系统遇到了未预料的情况",
                "可能是临时性故障",
            ],
            recovery_steps=[
                RecoveryStep(1, "等待片刻后重试", "等待30秒到1分钟后重新尝试操作"),
                RecoveryStep(2, "联系技术支持", "如果问题持续存在，请联系客服获得帮助"),
            ],
            is_retryable=True,
            retry_after_secs=30,
        )
    
    @classmethod
    def network_error(cls, context: str = "连接服务器") -> "FallbackSummaryError":
        """创建网络错误"""
        return cls(
            category=ErrorCategory.NETWORK,
            title="网络连接失败",
            what_happened=f"无法{context}，网络连接出现问题",
            possible_causes=[
                "您的网络连接不稳定或已断开",
                "服务器暂时不可用",
                "防火墙或代理设置阻止了连接",
            ],
            recovery_steps=[
                RecoveryStep(1, "检查网络连接", "确认您的设备已连接到互联网"),
                RecoveryStep(2, "等待后重试", "如果是服务器维护，等待几分钟后再次尝试"),
                RecoveryStep(3, "检查代理设置", "如果您使用了代理或VPN，请尝试关闭后重试"),
            ],
            is_retryable=True,
            retry_after_secs=30,
        )
    
    @classmethod
    def timeout_error(cls, operation: str = "执行操作") -> "FallbackSummaryError":
        """创建超时错误"""
        return cls(
            category=ErrorCategory.TIMEOUT,
            title="操作超时",
            what_happened=f"{operation}耗时过长，系统中断了请求",
            possible_causes=[
                "网络连接不稳定导致响应延迟",
                "服务器负载过高，处理速度变慢",
                "请求的数据量过大",
            ],
            recovery_steps=[
                RecoveryStep(1, "检查网络状况", "确认网络连接稳定"),
                RecoveryStep(2, "减少操作范围", "尝试缩小请求范围，如减少文件数量或数据量"),
                RecoveryStep(3, "稍后重试", "等待1-2分钟后重新尝试"),
            ],
            is_retryable=True,
            retry_after_secs=60,
        )
    
    @classmethod
    def quota_exceeded(cls, resource: str = "使用量") -> "FallbackSummaryError":
        """创建配额超限错误"""
        return cls(
            category=ErrorCategory.QUOTA,
            title="使用量已达上限",
            what_happened=f"{resource}已达今日/本周/本月上限，当前无法继续使用",
            possible_causes=[
                "已达到套餐约定的使用额度",
                "系统为保障稳定性设置了合理限制",
            ],
            recovery_steps=[
                RecoveryStep(1, "查看配额详情", "进入账户设置查看各配额的使用情况"),
                RecoveryStep(2, "等待配额重置", "日配额通常在次日重置，周配额在周一重置"),
                RecoveryStep(3, "升级套餐", "如需更高配额，联系客服了解升级方案"),
            ],
            is_retryable=True,
            retry_after_secs=None,  # 配额类不自动重试
        )
    
    @classmethod
    def tool_error(cls, tool_name: str, reason: str = "无法正常执行") -> "FallbackSummaryError":
        """创建工具执行错误"""
        return cls(
            category=ErrorCategory.TOOL,
            title="工具执行失败",
            what_happened=f"工具【{tool_name}】{reason}",
            possible_causes=[
                "工具内部发生了意外情况",
                "工具依赖的外部服务暂时不可用",
                "工具的参数或输入数据有问题",
            ],
            recovery_steps=[
                RecoveryStep(1, "检查输入数据", "确认提供给工具的数据格式正确且完整"),
                RecoveryStep(2, "查看工具文档", "查阅工具使用说明，确认操作方式正确"),
                RecoveryStep(3, "重试操作", "关闭并重新打开工具，再次尝试执行"),
                RecoveryStep(4, "联系支持", "如问题持续，联系技术支持并提供操作详情"),
            ],
            is_retryable=True,
            retry_after_secs=15,
        )


# ============================================================================
# FallbackErrorTranslator
# ============================================================================

# 类型别名：异常类型 -> 转换函数
ExceptionTranslator = Callable[[Exception], Optional[FallbackErrorDetail]]

# 内置恢复步骤工厂
def _retry_step(secs: int = 30) -> List[RecoveryStep]:
    return [
        RecoveryStep(1, "等待后重试", f"建议等待{secs}秒后重新尝试操作"),
    ]

def _check_input_step() -> List[RecoveryStep]:
    return [
        RecoveryStep(1, "检查输入数据", "确认输入的数据格式正确、内容完整"),
    ]


class FallbackErrorTranslator:
    """
    技术异常 → 用户友好消息 转换器
    
    使用方式：
        try:
            do_something()
        except Exception as e:
            detail = FallbackErrorTranslator.translate(e)
            if detail:
                raise FallbackSummaryError.from_translator(detail) from e
            raise  # 无法转换，原样抛出
    """
    
    # 注册的转换器（按优先级排序，精确匹配优先）
    _translators: List[tuple[Type[Exception], ExceptionTranslator]] = []
    # 通用的错误消息模式（用于没有精确匹配时）
    _pattern_translators: List[tuple[str, ExceptionTranslator]] = []
    
    @classmethod
    def register(
        cls,
        exception_type: Type[Exception],
        translator: ExceptionTranslator,
    ) -> None:
        """注册异常类型转换器"""
        cls._translators.insert(0, (exception_type, translator))  # 后注册优先
    
    @classmethod
    def register_pattern(
        cls,
        error_pattern: str,
        translator: ExceptionTranslator,
    ) -> None:
        """注册错误消息模式转换器（按优先级插入）"""
        cls._pattern_translators.insert(0, (error_pattern, translator))
    
    @classmethod
    def translate(cls, error: Exception) -> Optional[FallbackErrorDetail]:
        """
        将异常转换为友好错误详情
        
        策略：
        1. 遍历注册的 translators，精确匹配异常类型
        2. 遍历注册的 pattern_translators，匹配错误消息
        3. 返回 None 表示无法转换（调用方应原样抛出）
        """
        # 策略1: 精确类型匹配
        for exc_type, translator in cls._translators:
            if isinstance(error, exc_type):
                try:
                    detail = translator(error)
                    if detail:
                        return detail
                except Exception as e:
                    logger.warning(f"Translator for {exc_type.__name__} raised: {e}")
        
        # 策略2: 错误消息模式匹配
        error_msg = str(error)
        for pattern, translator in cls._pattern_translators:
            if pattern in error_msg:
                try:
                    detail = translator(error)
                    if detail:
                        return detail
                except Exception as e:
                    logger.warning(f"Pattern translator for '{pattern}' raised: {e}")
        
        return None
    
    @classmethod
    def translate_or_raise(cls, error: Exception) -> FallbackErrorDetail:
        """转换异常，如果无法转换则抛出通用未知错误"""
        detail = cls.translate(error)
        if detail:
            return detail
        
        logger.warning(f"No friendly translation for {type(error).__name__}: {error}")
        return FallbackErrorDetail(
            category=ErrorCategory.UNKNOWN,
            title="操作失败",
            what_happened="发生了一个未预料的问题",
            possible_causes=[
                "系统遇到了意外情况",
                "可能是临时性故障",
            ],
            recovery_steps=[
                RecoveryStep(1, "稍后重试", "等待30秒后重新尝试操作"),
                RecoveryStep(2, "联系技术支持", "如果问题持续，请联系客服"),
            ],
            is_retryable=True,
            retry_after_secs=30,
        )
    
    @classmethod
    def reset(cls) -> None:
        """重置所有注册的转换器（用于测试）"""
        cls._translators.clear()
        cls._pattern_translators.clear()


# ============================================================================
# 内置转换器
# ============================================================================

def _translate_timeout(error: Exception) -> Optional[FallbackErrorDetail]:
    """超时类异常转换"""
    msg_lower = str(error).lower()
    
    if "timeout" in msg_lower or "timed out" in msg_lower:
        return FallbackErrorDetail(
            category=ErrorCategory.TIMEOUT,
            title="操作超时",
            what_happened="请求耗时过长，系统中断了操作",
            possible_causes=[
                "网络连接不稳定导致响应延迟",
                "服务器负载过高",
                "请求的数据量过大",
            ],
            recovery_steps=[
                RecoveryStep(1, "检查网络", "确认网络连接稳定后重试"),
                RecoveryStep(2, "减少数据量", "尝试缩小请求范围"),
                RecoveryStep(3, "等待后重试", "1分钟后再次尝试"),
            ],
            is_retryable=True,
            retry_after_secs=60,
        )
    return None


def _translate_connection(error: Exception) -> Optional[FallbackErrorDetail]:
    """连接类异常转换"""
    msg_lower = str(error).lower()
    
    connection_patterns = [
        "connection refused", "connection reset", "connection timed out",
        "network is unreachable", "no route to host", " ConnectionError",
        "HTTPConnectionPool", "HTTPSConnectionPool",
    ]
    
    for pattern in connection_patterns:
        if pattern.lower() in msg_lower:
            return FallbackErrorDetail(
                category=ErrorCategory.NETWORK,
                title="网络连接失败",
                what_happened="无法连接到服务器，网络连接出现问题",
                possible_causes=[
                    "您的网络连接不稳定或已断开",
                    "目标服务器暂时不可用",
                    "防火墙或代理设置阻止了连接",
                ],
                recovery_steps=[
                    RecoveryStep(1, "检查网络", "确认设备已连接互联网"),
                    RecoveryStep(2, "检查代理设置", "关闭VPN或代理后重试"),
                    RecoveryStep(3, "等待后重试", "若服务器维护中，等待数分钟后重试"),
                ],
                is_retryable=True,
                retry_after_secs=30,
            )
    return None


def _translate_auth(error: Exception) -> Optional[FallbackErrorDetail]:
    """认证授权类异常转换"""
    msg_lower = str(error).lower()
    
    auth_patterns = [
        "auth", "unauthorized", "401", "403", "forbidden",
        "permission denied", "access denied", "invalid token",
        "expired", "credential",
    ]
    
    for pattern in auth_patterns:
        if pattern in msg_lower:
            return FallbackErrorDetail(
                category=ErrorCategory.AUTHENTICATION,
                title="认证失败",
                what_happened="身份验证失败或权限不足，无法完成操作",
                possible_causes=[
                    "登录状态已过期，需要重新登录",
                    "当前账户没有执行此操作的权限",
                    "API密钥或凭证配置有误",
                ],
                recovery_steps=[
                    RecoveryStep(1, "重新登录", "退出当前登录状态后重新认证"),
                    RecoveryStep(2, "检查权限", "确认账户是否有相应操作权限"),
                    RecoveryStep(3, "检查API配置", "确认API密钥或许可证配置正确"),
                ],
                is_retryable=False,
            )
    return None


def _translate_quota(error: Exception) -> Optional[FallbackErrorDetail]:
    """配额超限类异常转换"""
    msg_lower = str(error).lower()
    
    quota_patterns = [
        "quota", "limit exceeded", "rate limit", "too many",
        "message limit", "token limit", "daily limit",
    ]
    
    for pattern in quota_patterns:
        if pattern in msg_lower:
            return FallbackErrorDetail(
                category=ErrorCategory.QUOTA,
                title="使用量已达上限",
                what_happened="当前操作已达到额度限制，无法继续执行",
                possible_causes=[
                    "已达到套餐约定的使用额度",
                    "触发了API调用频率限制",
                ],
                recovery_steps=[
                    RecoveryStep(1, "查看配额详情", "进入账户查看各配额使用情况"),
                    RecoveryStep(2, "等待重置", "日配额通常在次日重置"),
                    RecoveryStep(3, "联系支持", "如需提升配额，联系客服"),
                ],
                is_retryable=True,
                retry_after_secs=None,
            )
    return None


def _translate_file_not_found(error: Exception) -> Optional[FallbackErrorDetail]:
    """文件不存在类异常转换"""
    if isinstance(error, FileNotFoundError):
        return FallbackErrorDetail(
            category=ErrorCategory.INPUT,
            title="文件未找到",
            what_happened="指定的文件不存在或路径不正确",
            possible_causes=[
                "文件已被删除或移动到其他位置",
                "文件路径拼写有误",
                "文件位于当前环境无权访问的位置",
            ],
            recovery_steps=[
                RecoveryStep(1, "检查路径", "确认文件路径是否正确"),
                RecoveryStep(2, "确认文件存在", "在文件系统中查找该文件"),
                RecoveryStep(3, "上传文件", "如需要，请重新上传相关文件"),
            ],
            is_retryable=False,
        )
    return None


def _translate_permission(error: Exception) -> Optional[FallbackErrorDetail]:
    """权限不足类异常转换"""
    msg_lower = str(error).lower()
    
    perm_patterns = [
        "permission denied", "access denied", "not permitted",
        "read-only", "write protected", "readonly file system",
    ]
    
    for pattern in perm_patterns:
        if pattern in msg_lower:
            return FallbackErrorDetail(
                category=ErrorCategory.PERMISSION,
                title="权限不足",
                what_happened="当前操作需要更高的权限才能执行",
                possible_causes=[
                    "当前账户没有执行此操作的权限",
                    "文件或资源被设置为只读",
                    "需要管理员权限才能完成此操作",
                ],
                recovery_steps=[
                    RecoveryStep(1, "检查权限", "确认您有执行此操作的权限"),
                    RecoveryStep(2, "联系管理员", "如需权限提升，请联系账户管理员"),
                ],
                is_retryable=False,
            )
    return None


def _translate_resource(error: Exception) -> Optional[FallbackErrorDetail]:
    """资源不足类异常转换"""
    msg_lower = str(error).lower()
    
    resource_patterns = [
        "out of memory", "memory error", "disk full",
        "no space left", "resource exhausted",
    ]
    
    for pattern in resource_patterns:
        if pattern in msg_lower:
            return FallbackErrorDetail(
                category=ErrorCategory.RESOURCE,
                title="资源不足",
                what_happened="系统资源不足以完成当前操作",
                possible_causes=[
                    "可用内存不足",
                    "磁盘空间已满",
                    "并发任务数达到上限",
                ],
                recovery_steps=[
                    RecoveryStep(1, "释放资源", "关闭不需要的应用程序或进程"),
                    RecoveryStep(2, "清理空间", "删除不需要的文件释放磁盘空间"),
                    RecoveryStep(3, "分批处理", "将大任务拆分为多个小任务分批执行"),
                ],
                is_retryable=True,
                retry_after_secs=30,
            )
    return None


def _translate_tool_error(error: Exception) -> Optional[FallbackErrorDetail]:
    """工具执行类异常转换"""
    msg = str(error)
    
    # 工具名称提取（从异常消息中尝试提取）
    tool_name_match = re.search(r"tool['\"]?\s*[:\"]*\s*(\w+)", msg, re.IGNORECASE)
    tool_name = tool_name_match.group(1) if tool_name_match else "未知工具"
    
    return FallbackErrorDetail(
        category=ErrorCategory.TOOL,
        title="工具执行失败",
        what_happened=f"工具执行过程中发生错误",
        possible_causes=[
            "工具内部发生了意外情况",
            "工具依赖的服务暂时不可用",
            "工具的输入参数有问题",
        ],
        recovery_steps=[
            RecoveryStep(1, "检查输入", "确认工具的输入参数正确"),
            RecoveryStep(2, "重试", "稍后重新尝试执行该工具"),
            RecoveryStep(3, "换用其他工具", "如持续失败，尝试使用替代方案"),
        ],
        is_retryable=True,
        retry_after_secs=15,
    )


# 注册内置转换器（按注册顺序匹配，后注册优先）
# 内置转换器优先级较低，允许用户覆盖

# 基础异常类型注册（按类型继承链匹配，后注册的优先）
FallbackErrorTranslator.register(FileNotFoundError, _translate_file_not_found)

# 错误消息模式注册（按插入顺序匹配，后注册优先）
for pattern, translator in [
    ("timeout", _translate_timeout),
    ("timed out", _translate_timeout),
    ("connection refused", _translate_connection),
    ("connection reset", _translate_connection),
    ("connection timed out", _translate_connection),
    ("no route to host", _translate_connection),
    ("HTTPConnectionPool", _translate_connection),
    ("HTTPSConnectionPool", _translate_connection),
    ("401", _translate_auth),
    ("403", _translate_auth),
    ("unauthorized", _translate_auth),
    ("forbidden", _translate_auth),
    ("permission denied", _translate_permission),
    ("access denied", _translate_permission),
    ("rate limit", _translate_quota),
    ("quota", _translate_quota),
    ("limit exceeded", _translate_quota),
    ("out of memory", _translate_resource),
    ("disk full", _translate_resource),
    ("no space left", _translate_resource),
]:
    FallbackErrorTranslator.register_pattern(pattern, translator)


# ============================================================================
# 便捷函数
# ============================================================================

def translate_error(error: Exception) -> Optional[FallbackErrorDetail]:
    """translate_error(error) -> FallbackErrorDetail | None"""
    return FallbackErrorTranslator.translate(error)


def wrap_as_friendly(error: Exception) -> FallbackSummaryError:
    """将任意异常转换为用户友好错误"""
    detail = FallbackErrorTranslator.translate_or_raise(error)
    return FallbackErrorTranslator.translate_or_raise(error)  # Already has fallback


def is_fallback_error(error: Exception) -> bool:
    """判断是否为 FallbackSummaryError"""
    return isinstance(error, FallbackSummaryError)


__all__ = [
    # Core classes
    'FallbackSummaryError', 'FallbackErrorTranslator',
    # Supporting classes
    'ErrorCategory', 'FallbackErrorDetail', 'RecoveryStep',
    # Utilities
    'translate_error', 'wrap_as_friendly', 'is_fallback_error',
]
