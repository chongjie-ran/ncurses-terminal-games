from __future__ import annotations

from typing import Any

from pydantic import BaseModel, ConfigDict


class RuntimeModel(BaseModel):
    """单次编排运行所需的用户问题与三层上下文（资源 / 用户 / 会话）。"""

    model_config = ConfigDict(extra="forbid", validate_assignment=True)

    user_query: str
    resource_context: dict[str, Any]
    user_context: dict[str, Any]
    conversation_context: dict[str, Any]
    normalized_intent: str = ""
    ambiguity: str = ""
    exploration_scope: str = ""


# 历史命名：全库与测试均使用 RuntimeTask，保留别名以兼容既有导入与类型注解。
RuntimeTask = RuntimeModel


class StagePlan(BaseModel):
    """父编排一阶段内要拉起的子代理集合与调度原因。"""

    model_config = ConfigDict(extra="forbid", validate_assignment=True)

    stage_index: int
    child_agent_ids: list[str]
    reason: str
