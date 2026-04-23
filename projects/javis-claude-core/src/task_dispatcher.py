"""
task_dispatcher.py - 任务分发器（Task Dispatcher）

P1-5 实现 (2026-04-20)
核心功能：
- 复杂任务拆解为多个子任务
- 分发给多个Agent并行处理
- 结果聚合返回
- 支持同步/异步执行模式

设计参考：Phase3 Scope v1.0 道衍架构师草案
"""

from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum
from typing import Optional, Dict, List, Callable, Any, Union, Tuple
from datetime import datetime, timezone
from concurrent.futures import ThreadPoolExecutor, Future, TimeoutError as FuturesTimeoutError
import uuid
import threading
import logging
import time
import traceback

from .agent_protocol import (
    AgentMessage, AgentMessageType, AgentMessenger, MessagePriority,
    TaskContent, ResultContent, ErrorContent
)
from .exceptions import FallbackSummaryError

from execution_trace import warn as _et_warn

logger = logging.getLogger(__name__)


# ============================================================================
# Task Decomposition
# ============================================================================

class TaskComplexity(Enum):
    """任务复杂度等级"""
    TRIVIAL = "trivial"      # 单一任务，无需拆分
    SIMPLE = "simple"       # 2-3个子任务
    MODERATE = "moderate"    # 4-6个子任务
    COMPLEX = "complex"      # 7+个子任务


class TaskStatus(Enum):
    """任务执行状态"""
    PENDING = "pending"
    RUNNING = "running"
    PARTIAL_SUCCESS = "partial_success"
    SUCCESS = "success"
    FAILED = "failed"
    TIMEOUT = "timeout"
    CANCELLED = "cancelled"


@dataclass
class SubTask:
    """子任务定义"""
    task_id: str
    name: str
    description: str
    payload: Any                       # 任务内容
    priority: MessagePriority = MessagePriority.NORMAL
    timeout_seconds: int = 300        # 默认5分钟超时
    retry_count: int = 1              # 重试次数
    assigned_agent: Optional[str] = None  # 指定Agent，空=自动分配
    dependencies: List[str] = field(default_factory=list)  # 依赖的task_id列表
    metadata: Dict[str, Any] = field(default_factory=dict)

    def __post_init__(self):
        if self.task_id is None:
            self.task_id = str(uuid.uuid4())


@dataclass
class TaskDecomposition:
    """任务拆解结果"""
    original_task_id: str
    complexity: TaskComplexity
    subtasks: List[SubTask]
    estimated_duration: float = 0.0    # 预估时长（秒）
    can_parallelize: bool = True       # 是否可并行
    execution_plan: str = "parallel"    # 执行计划：parallel/sequential/hybrid


# ============================================================================
# Dispatch Result
# ============================================================================

@dataclass
class SubTaskResult:
    """子任务执行结果"""
    task_id: str
    agent_id: str
    status: TaskStatus
    result: Any = None
    error: Optional[str] = None
    started_at: Optional[datetime] = None
    completed_at: Optional[datetime] = None
    duration_seconds: float = 0.0
    retry_attempts: int = 0
    metadata: Dict[str, Any] = field(default_factory=dict)

    @property
    def success(self) -> bool:
        return self.status == TaskStatus.SUCCESS


@dataclass
class DispatchResult:
    """分发执行结果"""
    dispatch_id: str
    original_task_id: str
    status: TaskStatus
    subtask_results: List[SubTaskResult]
    aggregated_result: Any = None
    total_duration: float = 0.0
    started_at: Optional[datetime] = None
    completed_at: Optional[datetime] = None
    metadata: Dict[str, Any] = field(default_factory=dict)

    @property
    def success_count(self) -> int:
        return sum(1 for r in self.subtask_results if r.success)

    @property
    def failure_count(self) -> int:
        return sum(1 for r in self.subtask_results if r.status == TaskStatus.FAILED)

    @property
    def partial_success(self) -> bool:
        return 0 < self.success_count < len(self.subtask_results)

    @property
    def all_success(self) -> bool:
        return self.success_count == len(self.subtask_results)


# ============================================================================
# Task Decomposer
# ============================================================================

class TaskDecomposer:
    """任务拆解器 - 将复杂任务拆解为可并行执行的子任务"""

    def __init__(self, max_subtasks: int = 10):
        self.max_subtasks = max_subtasks

    def decompose(
        self,
        task_id: str,
        task_description: str,
        payload: Any,
        options: Optional[Dict[str, Any]] = None
    ) -> TaskDecomposition:
        """
        拆解任务为子任务

        Args:
            task_id: 原始任务ID
            task_description: 任务描述
            payload: 任务数据
            options: 拆解选项

        Returns:
            TaskDecomposition: 拆解结果
        """
        options = options or {}

        # 根据 payload 类型和描述决定拆解策略
        if isinstance(payload, list) and len(payload) > 1:
            # 列表类型：按项拆分
            subtasks = self._decompose_list(task_id, task_description, payload, options)
        elif isinstance(payload, dict):
            # 字典类型：按key分组拆分
            subtasks = self._decompose_dict(task_id, task_description, payload, options)
        elif isinstance(payload, str) and len(payload) > 500:
            # 长文本：按段落/行拆分
            subtasks = self._decompose_text(task_id, task_description, payload, options)
        else:
            # 单一任务
            subtasks = [self._create_trivial_subtask(task_id, task_description, payload)]

        # 判断复杂度
        complexity = self._classify_complexity(len(subtasks))

        # 判断是否可并行（默认可并行，检查依赖关系）
        can_parallel = all(len(s.dependencies) == 0 for s in subtasks)

        return TaskDecomposition(
            original_task_id=task_id,
            complexity=complexity,
            subtasks=subtasks,
            estimated_duration=len(subtasks) * 30.0,  # 每任务预估30秒
            can_parallelize=can_parallel,
            execution_plan="parallel" if can_parallel else "hybrid"
        )

    def _decompose_list(
        self,
        task_id: str,
        task_description: str,
        payload: list,
        options: Dict
    ) -> List[SubTask]:
        """将列表拆分为多个子任务"""
        items_per_task = options.get("items_per_task", 1)
        subtasks = []

        for i in range(0, len(payload), items_per_task):
            chunk = payload[i:i + items_per_task]
            subtask_id = f"{task_id}-sub-{len(subtasks)}"
            subtasks.append(SubTask(
                task_id=subtask_id,
                name=f"{task_description} [chunk {len(subtasks)}]",
                description=f"处理 {len(chunk)} 个项目",
                payload=chunk,
                metadata={"chunk_index": i, "chunk_size": len(chunk)}
            ))

        return subtasks[:self.max_subtasks]

    def _decompose_dict(
        self,
        task_id: str,
        task_description: str,
        payload: dict,
        options: Dict
    ) -> List[SubTask]:
        """将字典拆分为多个子任务（按 key 分组）"""
        keys = list(payload.keys())
        group_size = options.get("group_size", max(1, len(keys) // 4))
        subtasks = []

        for i in range(0, len(keys), group_size):
            group_keys = keys[i:i + group_size]
            chunk = {k: payload[k] for k in group_keys}
            subtask_id = f"{task_id}-sub-{len(subtasks)}"
            subtasks.append(SubTask(
                task_id=subtask_id,
                name=f"{task_description} [group {len(subtasks)}]",
                description=f"处理 {len(group_keys)} 个字段",
                payload=chunk,
                metadata={"keys": group_keys}
            ))

        return subtasks[:self.max_subtasks]

    def _decompose_text(
        self,
        task_id: str,
        task_description: str,
        payload: str,
        options: Dict
    ) -> List[SubTask]:
        """将长文本拆分为多个子任务"""
        # 按行拆分
        lines = payload.split('\n')
        lines_per_task = options.get("lines_per_task", 50)
        subtasks = []

        for i in range(0, len(lines), lines_per_task):
            chunk_lines = lines[i:i + lines_per_task]
            chunk_text = '\n'.join(chunk_lines)
            subtask_id = f"{task_id}-sub-{len(subtasks)}"
            subtasks.append(SubTask(
                task_id=subtask_id,
                name=f"{task_description} [lines {i}-{i+len(chunk_lines)}]",
                description=f"处理 {len(chunk_lines)} 行",
                payload=chunk_text,
                metadata={"start_line": i, "line_count": len(chunk_lines)}
            ))

        return subtasks[:self.max_subtasks]

    def _create_trivial_subtask(
        self,
        task_id: str,
        task_description: str,
        payload: Any
    ) -> SubTask:
        """创建单一子任务"""
        return SubTask(
            task_id=f"{task_id}-sub-0",
            name=task_description,
            description="单一任务",
            payload=payload
        )

    def _classify_complexity(self, subtask_count: int) -> TaskComplexity:
        """分类任务复杂度"""
        if subtask_count == 1:
            return TaskComplexity.TRIVIAL
        elif subtask_count <= 3:
            return TaskComplexity.SIMPLE
        elif subtask_count <= 6:
            return TaskComplexity.MODERATE
        else:
            return TaskComplexity.COMPLEX


# ============================================================================
# Result Aggregator
# ============================================================================

class ResultAggregator:
    """结果聚合器 - 将多个子任务结果聚合成单一结果"""

    def __init__(self, aggregation_mode: str = "list"):
        """
        Args:
            aggregation_mode: 聚合模式
                - "list": 将所有结果放入列表
                - "dict": 将结果按 task_id 组织为字典
                - "first": 返回第一个成功的结果
                - "last": 返回最后一个结果
                - "custom": 使用自定义聚合函数
        """
        self.aggregation_mode = aggregation_mode

    def aggregate(
        self,
        subtask_results: List[SubTaskResult],
        custom_func: Optional[Callable[[List[SubTaskResult]], Any]] = None
    ) -> Tuple[Any, TaskStatus]:
        """
        聚合子任务结果

        Returns:
            (aggregated_result, overall_status)
        """
        if not subtask_results:
            return None, TaskStatus.PENDING

        # 检查整体状态
        overall_status = self._compute_overall_status(subtask_results)

        # 根据模式聚合
        if self.aggregation_mode == "list":
            result = [r.result for r in subtask_results if r.result is not None]
        elif self.aggregation_mode == "dict":
            result = {
                r.task_id: r.result for r in subtask_results if r.result is not None
            }
        elif self.aggregation_mode == "first":
            for r in subtask_results:
                if r.success and r.result is not None:
                    result = r.result
                    break
            else:
                result = None
        elif self.aggregation_mode == "last":
            result = None
            for r in reversed(subtask_results):
                if r.success and r.result is not None:
                    result = r.result
                    break
        elif self.aggregation_mode == "custom" and custom_func:
            result = custom_func(subtask_results)
        else:
            result = [r.result for r in subtask_results if r.result is not None]

        return result, overall_status

    def _compute_overall_status(self, results: List[SubTaskResult]) -> TaskStatus:
        """计算整体状态"""
        if not results:
            return TaskStatus.PENDING

        all_success = all(r.success for r in results)
        any_success = any(r.success for r in results)
        any_timeout = any(r.status == TaskStatus.TIMEOUT for r in results)
        any_cancelled = any(r.status == TaskStatus.CANCELLED for r in results)

        if all_success:
            return TaskStatus.SUCCESS
        elif any_cancelled:
            return TaskStatus.CANCELLED
        elif any_timeout:
            return TaskStatus.TIMEOUT
        elif any_success:
            return TaskStatus.PARTIAL_SUCCESS
        else:
            return TaskStatus.FAILED


# ============================================================================
# Task Dispatcher (Main Class)
# ============================================================================

class TaskDispatcher:
    """
    任务分发器 - 将复杂任务分发给多个Agent并行处理

    核心流程：
    1. 接收原始任务
    2. 拆解为子任务 (TaskDecomposer)
    3. 分发给可用Agent (通过 AgentMessenger)
    4. 并行执行 (ThreadPoolExecutor)
    5. 聚合结果 (ResultAggregator)
    6. 返回统一结果 (DispatchResult)
    """

    _instance: Optional[TaskDispatcher] = None
    _lock = threading.Lock()

    def __init__(
        self,
        messenger: Optional[AgentMessenger] = None,
        max_workers: int = 4,
        default_timeout: int = 300,
        enable_auto_decompose: bool = True
    ):
        """
        Args:
            messenger: Agent消息发送器（空=创建新的）
            max_workers: 最大并行工作线程数
            default_timeout: 默认超时时间（秒）
            enable_auto_decompose: 是否自动拆解任务
        """
        self.messenger = messenger or AgentMessenger()
        self.max_workers = max_workers
        self.default_timeout = default_timeout
        self.enable_auto_decompose = enable_auto_decompose
        self._executor = ThreadPoolExecutor(max_workers=max_workers)
        self._running_tasks: Dict[str, DispatchResult] = {}
        self._task_results: Dict[str, SubTaskResult] = {}
        self._callback_lock = threading.Lock()

    @classmethod
    def get_instance(cls) -> TaskDispatcher:
        """单例获取"""
        if cls._instance is None:
            with cls._lock:
                if cls._instance is None:
                    cls._instance = cls()
        return cls._instance

    def dispatch(
        self,
        task_description: str,
        payload: Any,
        agent_ids: Optional[List[str]] = None,
        options: Optional[Dict[str, Any]] = None,
        wait_for_result: bool = True,
        timeout: Optional[int] = None
    ) -> Union[DispatchResult, str]:
        """
        分发任务

        Args:
            task_description: 任务描述
            payload: 任务数据
            agent_ids: 指定执行的Agent列表（空=自动分配）
            options: 分发选项
                - auto_decompose: bool = True  是否自动拆解
                - items_per_task: int = 1      列表每task的项数
                - group_size: int = 4          字典每task的key数
                - aggregation_mode: str        聚合模式
                - custom_aggregate_func: Callable
                - skip_decompose: bool = False  跳过拆解，直接作为单一task
            wait_for_result: 是否等待结果（False=异步，返回task_id）
            timeout: 超时时间（秒），空=使用默认值

        Returns:
            DispatchResult（同步模式）或 task_id（异步模式）
        """
        options = options or {}
        timeout = timeout or self.default_timeout
        task_id = str(uuid.uuid4())

        started_at = datetime.now(timezone.utc)

        logger.info(f"[Dispatcher] dispatch task_id={task_id}, desc={task_description}, "
                    f"agents={agent_ids}, wait={wait_for_result}")

        # 拆解任务
        if options.get("skip_decompose", False) or not self.enable_auto_decompose:
            decomposition = TaskDecomposition(
                original_task_id=task_id,
                complexity=TaskComplexity.TRIVIAL,
                subtasks=[SubTask(
                    task_id=f"{task_id}-sub-0",
                    name=task_description,
                    description="单一任务",
                    payload=payload
                )]
            )
        else:
            decomposer = TaskDecomposer()
            decomposition = decomposer.decompose(task_id, task_description, payload, options)

        logger.info(f"[Dispatcher] decomposed into {len(decomposition.subtasks)} subtasks, "
                    f"complexity={decomposition.complexity.value}")

        # 创建分发结果
        result = DispatchResult(
            dispatch_id=task_id,
            original_task_id=task_id,
            status=TaskStatus.PENDING,
            subtask_results=[],
            started_at=started_at
        )
        self._running_tasks[task_id] = result

        # 分配Agent并设置超时
        subtask_timeout = options.get("subtask_timeout", timeout)
        if agent_ids and len(agent_ids) == len(decomposition.subtasks):
            for subtask, agent_id in zip(decomposition.subtasks, agent_ids):
                subtask.assigned_agent = agent_id
                subtask.timeout_seconds = subtask_timeout
        else:
            # 平均分配
            agent_cycle = agent_ids or ["default"]
            for i, subtask in enumerate(decomposition.subtasks):
                subtask.assigned_agent = agent_cycle[i % len(agent_cycle)]
                subtask.timeout_seconds = subtask_timeout

        if wait_for_result:
            return self._dispatch_sync(decomposition, result, timeout, options)
        else:
            # 异步执行
            self._executor.submit(
                self._dispatch_sync, decomposition, result, timeout, options
            )
            return task_id

    def _dispatch_sync(
        self,
        decomposition: TaskDecomposition,
        result: DispatchResult,
        timeout: int,
        options: Dict
    ) -> DispatchResult:
        """同步分发执行"""
        started_at = datetime.now(timezone.utc)
        result.status = TaskStatus.RUNNING
        result.started_at = started_at

        subtask_results: List[SubTaskResult] = []
        futures: Dict[Future, SubTask] = {}

        try:
            # 并行提交所有子任务
            for subtask in decomposition.subtasks:
                future = self._executor.submit(
                    self._execute_subtask, subtask
                )
                futures[future] = subtask

            # 等待结果（带超时）
            for future in futures:
                subtask = futures[future]
                try:
                    subtask_result = future.result(timeout=subtask.timeout_seconds)
                    subtask_results.append(subtask_result)
                except FuturesTimeoutError:
                    _et_warn(f"[Dispatcher] subtask {subtask.task_id} timeout")
                    subtask_results.append(SubTaskResult(
                        task_id=subtask.task_id,
                        agent_id=subtask.assigned_agent or "unknown",
                        status=TaskStatus.TIMEOUT,
                        error=f"任务执行超时（{subtask.timeout_seconds}秒）"
                    ))
                except Exception as e:
                    _et_warn(f"[Dispatcher] subtask {subtask.task_id} failed: {e}")
                    subtask_results.append(SubTaskResult(
                        task_id=subtask.task_id,
                        agent_id=subtask.assigned_agent or "unknown",
                        status=TaskStatus.FAILED,
                        error=str(e)
                    ))

        finally:
            completed_at = datetime.now(timezone.utc)
            total_duration = (completed_at - result.started_at).total_seconds()

            # 聚合结果
            agg_mode = options.get("aggregation_mode", "list")
            agg = ResultAggregator(aggregation_mode=agg_mode)
            aggregated, overall_status = agg.aggregate(
                subtask_results,
                options.get("custom_aggregate_func")
            )

            result.subtask_results = subtask_results
            result.aggregated_result = aggregated
            result.status = overall_status
            result.completed_at = completed_at
            result.total_duration = total_duration

            logger.info(f"[Dispatcher] dispatch_id={result.dispatch_id} completed, "
                        f"status={result.status.value}, success={result.success_count}, "
                        f"failed={result.failure_count}, duration={total_duration:.2f}s")

            # 清理
            if result.dispatch_id in self._running_tasks:
                del self._running_tasks[result.dispatch_id]

        return result

    def _execute_subtask(self, subtask: SubTask) -> SubTaskResult:
        """执行单个子任务"""
        started_at = datetime.now(timezone.utc)
        retry_count = 0
        last_error = None

        agent_id = subtask.assigned_agent or "default"

        for attempt in range(subtask.retry_count + 1):
            try:
                # 构建任务消息
                task_content = TaskContent(
                    task_id=subtask.task_id,
                    description=subtask.description,
                    input_data=subtask.payload,
                    tags=list(subtask.metadata.keys())
                )

                message = AgentMessage(
                    msg_id=str(uuid.uuid4()),
                    msg_type=AgentMessageType.TASK,
                    sender="dispatcher",
                    receiver=agent_id,
                    content=task_content,
                    session_id=f"dispatch-{subtask.task_id}",
                    trace_id=subtask.task_id,
                    ttl=3,
                    priority=subtask.priority
                )

                # 通过 messenger 发送并等待结果（同步）
                # 使用超时的方式等待
                result_data = self._send_and_wait(
                    message,
                    timeout=subtask.timeout_seconds
                )

                completed_at = datetime.now(timezone.utc)
                return SubTaskResult(
                    task_id=subtask.task_id,
                    agent_id=agent_id,
                    status=TaskStatus.SUCCESS if result_data else TaskStatus.FAILED,
                    result=result_data,
                    started_at=started_at,
                    completed_at=completed_at,
                    duration_seconds=(completed_at - started_at).total_seconds(),
                    retry_attempts=attempt,
                    metadata={"attempt": attempt + 1}
                )

            except TimeoutError as e:
                # 超时不重试，直接标记为TIMEOUT
                completed_at = datetime.now(timezone.utc)
                return SubTaskResult(
                    task_id=subtask.task_id,
                    agent_id=agent_id,
                    status=TaskStatus.TIMEOUT,
                    error=str(e),
                    started_at=started_at,
                    completed_at=completed_at,
                    duration_seconds=(completed_at - started_at).total_seconds(),
                    retry_attempts=attempt + 1,
                    metadata={"timeout": subtask.timeout_seconds}
                )
            except Exception as e:
                last_error = str(e)
                retry_count = attempt + 1
                _et_warn(f"[Dispatcher] subtask {subtask.task_id} attempt {attempt+1} "
                               f"failed: {e}, retrying...")

        # 所有重试都失败
        completed_at = datetime.now(timezone.utc)
        return SubTaskResult(
            task_id=subtask.task_id,
            agent_id=agent_id,
            status=TaskStatus.FAILED,
            error=last_error or "未知错误",
            started_at=started_at,
            completed_at=completed_at,
            duration_seconds=(completed_at - started_at).total_seconds(),
            retry_attempts=retry_count,
            metadata={"attempts": retry_count}
        )

    def _send_and_wait(
        self,
        message: AgentMessage,
        timeout: int
    ) -> Any:
        """
        通过 MessageBus 发送消息并同步等待结果
        使用 threading.Event 实现超时控制
        """
        from .agent_protocol import MessageBus

        result_event = threading.Event()
        result_container: Dict[str, Any] = {"result": None, "error": None}
        original_msg_id = message.msg_id

        def on_reply(response_msg: AgentMessage):
            if response_msg.reply_to != original_msg_id:
                return
            try:
                if response_msg.msg_type == AgentMessageType.RESULT:
                    content: ResultContent = response_msg.content
                    result_container["result"] = content.output_data
                elif response_msg.msg_type == AgentMessageType.ERROR:
                    content: ErrorContent = response_msg.content
                    result_container["error"] = content.error_message
                else:
                    result_container["result"] = response_msg.content
            except Exception as e:
                result_container["error"] = str(e)
            finally:
                result_event.set()

        bus = MessageBus.get_instance()
        subscriber_id = f"dispatcher-{original_msg_id}"
        bus.subscribe(subscriber_id, on_reply)

        try:
            self.messenger.send(message)
            if not result_event.wait(timeout=timeout):
                raise TimeoutError(f"等待结果超时（{timeout}秒）")
            if result_container["error"]:
                raise Exception(result_container["error"])
            return result_container["result"]
        finally:
            bus.unsubscribe(subscriber_id)

    def dispatch_simple(
        self,
        agent_id: str,
        task_description: str,
        payload: Any,
        timeout: int = 300
    ) -> SubTaskResult:
        """
        简单分发接口 - 单一任务直接分发给指定Agent
        """
        task_id = str(uuid.uuid4())
        subtask = SubTask(
            task_id=task_id,
            name=task_description,
            description=task_description,
            payload=payload,
            assigned_agent=agent_id,
            timeout_seconds=timeout
        )
        return self._execute_subtask(subtask)

    def get_task_status(self, dispatch_id: str) -> Optional[DispatchResult]:
        """获取任务执行状态"""
        return self._running_tasks.get(dispatch_id)

    def cancel_task(self, dispatch_id: str) -> bool:
        """取消任务（如果还在执行中）"""
        if dispatch_id in self._running_tasks:
            self._running_tasks[dispatch_id].status = TaskStatus.CANCELLED
            logger.info(f"[Dispatcher] task {dispatch_id} cancelled")
            return True
        return False

    def shutdown(self, wait: bool = True):
        """关闭分发器"""
        self._executor.shutdown(wait=wait)
        logger.info("[Dispatcher] shutdown")

    def __del__(self):
        try:
            self.shutdown(wait=False)
        except Exception:
            pass
