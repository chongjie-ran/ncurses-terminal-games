from __future__ import annotations

import argparse
import asyncio
import asyncio as _asyncio
import hashlib
import json
import logging
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from types import SimpleNamespace
from typing import Protocol, runtime_checkable

from aiops_v4.bootstrap import build_runtime_components
from aiops_v4.bridge import wrappers as wrapper_bridge
from aiops_v4.bridge.wrappers import WrapperBridgeState
from aiops_v4.handler import AIOpsV4Handler
from aiops_v4.models.runtime import RuntimeTask
from nanobot.cli.commands import _make_provider
from nanobot.config.loader import load_config, resolve_config_env_vars, set_config_path

logger = logging.getLogger(__name__)


@runtime_checkable
class LegacyToolWrappersPort(Protocol):
    """v4 启动路径与 v3 HTTP/tool 封装之间的稳定边界（默认实现：`aiops_v4.bridge.wrappers`）。"""

    def init_wrappers(self, zcloud_ip: str, auth_headers: dict) -> WrapperBridgeState:
        ...

    def build_default_auth_headers(
        self, *, auth_token: str = "", user_info: dict | None = None
    ) -> dict[str, str]:
        ...

    def apply_message_auth(self, message, default_auth_headers: dict):
        ...


def get_legacy_tool_wrappers_port() -> LegacyToolWrappersPort:
    """返回当前桥接实现；测试可通过 patch ``aiops_v4.launch.wrapper_bridge`` 替换行为。"""
    return wrapper_bridge  # type: ignore[return-value]


def init_legacy_tool_wrappers(zcloud_ip: str, auth_headers: dict) -> WrapperBridgeState:
    """初始化 v3 侧 tool 模块（供 HTTP/CLI/Gateway 与 `main()` 共用）。"""
    return get_legacy_tool_wrappers_port().init_wrappers(zcloud_ip, auth_headers)


DEFAULT_STUB_RESPONSE = json.dumps(
    {
        "summary": "Stub analysis result",
        "findings": [],
        "actions_required": [],
        "confidence": 0.6,
        "needs_followup": False,
        "recommended_next_agents": [],
        "alerts": [],
        "severity": "info",
    },
    ensure_ascii=False,
)


@dataclass(slots=True)
class LaunchConfig:
    query: str | None
    mode: str
    workspace: str
    config_dir: str
    config_path: Path | None = None
    runtime_config: object | None = None
    stub_response: str = DEFAULT_STUB_RESPONSE
    debug: bool = False
    zcloud_ip: str | None = None
    user_info: dict | None = None
    auth_token: str = ""
    session_key: str | None = None
    fallback_policy: int = 1
    session_memory_enabled: bool = False
    replay_session_key: str | None = None
    replay_limit: int = 20


class ChatProviderAdapter:
    """Synchronous wrapper around an async nanobot LLMProvider.

    A single background thread runs a persistent event loop for all LLM calls.
    This avoids creating and tearing down a new event loop on every call, which
    caused transient "Connection error" failures on the first request (the HTTP
    session had not been established yet when asyncio.run() closed the loop).
    """

    def __init__(self, provider, model: str) -> None:
        import threading
        self._provider = provider
        self._model = model
        self._loop = asyncio.new_event_loop()
        self._thread = threading.Thread(
            target=self._loop.run_forever,
            daemon=True,
            name="aiops-provider-loop",
        )
        self._thread.start()

    def run(self, system_prompt: str, user_prompt: str) -> str:
        messages = [
            {"role": "system", "content": system_prompt},
            {"role": "user", "content": user_prompt},
        ]
        future = asyncio.run_coroutine_threadsafe(
            self._provider.chat(messages=messages, model=self._model),
            self._loop,
        )
        response = future.result()  # blocks until the coroutine completes
        if response.finish_reason == "error":
            raise RuntimeError(response.content or "provider returned an error")
        return response.content or ""

    def run_coroutine(self, coro):
        """Block until *coro* completes on the shared background loop.

        Used by child agent_runner so all async LLM and tool calls share the
        same event loop as the parent, avoiding cross-loop aiohttp session
        conflicts.

        Captures the calling thread's contextvars context and passes it to the
        task created on the background loop, so that per-request auth headers
        set via ContextVar (e.g. by set_context_headers) are visible to async
        tool calls running in that task.
        """
        import concurrent.futures as _cf
        import contextvars

        ctx = contextvars.copy_context()
        done: _cf.Future = _cf.Future()

        def _create_task() -> None:
            task = self._loop.create_task(coro, context=ctx)

            def _on_done(t: asyncio.Task) -> None:
                exc = t.exception()
                if exc is not None:
                    done.set_exception(exc)
                else:
                    done.set_result(t.result())

            task.add_done_callback(_on_done)

        self._loop.call_soon_threadsafe(_create_task)
        return done.result()

    def run_stream_to_slot(self, system_prompt: str, user_prompt: str, slot) -> str:
        """Stream LLM synthesis to a SlotHandle, returning the full response text.

        This method runs the streaming coroutine on the shared background loop.
        The ``on_content_delta`` callback is async and executes within that loop,
        so it can directly ``await slot.append_answer(chunk)`` without deadlock.

        Falls back to a regular non-streaming call if the provider does not
        implement chat_stream, or if slot is None.
        """
        if slot is None:
            return self.run(system_prompt, user_prompt)

        messages = [
            {"role": "system", "content": system_prompt},
            {"role": "user", "content": user_prompt},
        ]
        collected: list[str] = []

        async def _on_delta(chunk: str) -> None:
            collected.append(chunk)
            slot.append_answer(chunk)  # sync: dispatches fire-and-forget to gateway loop

        async def _run_stream() -> str:
            slot.begin_answer()  # sync: dispatches to gateway loop
            try:
                response = await self._provider.chat_stream(
                    messages=messages,
                    model=self._model,
                    on_content_delta=_on_delta,
                )
                return response.content or "".join(collected)
            except Exception:
                # Fall back to non-streaming
                response = await self._provider.chat(messages=messages, model=self._model)
                full = response.content or ""
                if full:
                    slot.append_answer(full)  # sync
                return full

        future = asyncio.run_coroutine_threadsafe(_run_stream(), self._loop)
        return future.result()


def _resolve_path(base_dir: Path, raw_path: str | None, fallback: Path) -> str:
    if not raw_path:
        return str(fallback.resolve())
    candidate = Path(raw_path).expanduser()
    if not candidate.is_absolute():
        candidate = (base_dir / candidate).resolve()
    return str(candidate)


def _resolve_workspace_path(base_dir: Path, workspace_path: Path, raw_workspace: str | None) -> str:
    candidate = workspace_path.expanduser()
    if candidate.is_absolute():
        return str(candidate.resolve())
    raw_text = raw_workspace or str(candidate)
    raw_candidate = Path(raw_text).expanduser()
    if raw_candidate.parts and raw_candidate.parts[0] == ".":
        return str((base_dir / raw_candidate).resolve())
    return str(candidate.resolve())


def load_launch_config(config_path: str | None = None) -> LaunchConfig:
    resolved_config_path = None
    if config_path is not None:
        resolved_config_path = Path(config_path).expanduser().resolve()
        if not resolved_config_path.exists():
            raise ValueError(f"Config not found: {resolved_config_path}")
        set_config_path(resolved_config_path)

    runtime_config = resolve_config_env_vars(load_config(resolved_config_path))
    base_dir = resolved_config_path.parent if resolved_config_path is not None else Path.cwd()
    workspace = _resolve_workspace_path(
        base_dir,
        runtime_config.workspace_path,
        getattr(runtime_config.agents.defaults, "workspace", None),
    )
    default_config_dir = Path(__file__).resolve().parent / "config"
    config_dir = str(default_config_dir.resolve())

    return LaunchConfig(
        query=None,
        mode="cli",
        workspace=workspace,
        config_dir=config_dir,
        config_path=resolved_config_path,
        runtime_config=runtime_config,
    )


def build_v4_runtime(
    provider,
    workspace: str,
    config_dir: str | None = None,
    *,
    fallback_policy: int = 1,
    session_memory_enabled: bool = False,
):
    import os

    if session_memory_enabled:
        os.environ["AIOPS_SESSION_MEMORY_ENABLED"] = "1"
    resolved_config_dir = Path(config_dir) if config_dir is not None else Path(__file__).resolve().parent / "config"
    return build_runtime_components(
        config_dir=resolved_config_dir,
        workspace=workspace,
        provider=provider,
        fallback_policy=fallback_policy,
    )


def build_default_task(
    query: str,
    *,
    zcloud_ip: str | None = None,
    user_info: dict | None = None,
    debug: bool = False,
    session_key: str | None = None,
) -> RuntimeTask:
    resource_context = {"databases": []}
    if zcloud_ip:
        resource_context["zcloud_ip"] = zcloud_ip

    user_context = {"channel": "cli", "debug": debug}
    if session_key:
        user_context["session_key"] = session_key
    if user_info is not None:
        user_context["user_info"] = user_info

    return RuntimeTask(
        user_query=query,
        resource_context=resource_context,
        user_context=user_context,
        conversation_context={"history": []},
    )


def parse_args(argv: list[str] | None = None) -> LaunchConfig:
    args = list(sys.argv[1:] if argv is None else argv)
    if args and args[0] == "replay-compaction":
        parser = argparse.ArgumentParser(prog="python -m aiops_v4.launch replay-compaction")
        parser.add_argument("--config")
        parser.add_argument("--session-key", required=True)
        parser.add_argument("--limit", type=int, default=20)
        namespace = parser.parse_args(args[1:])
        launch_config = load_launch_config(namespace.config)
        return LaunchConfig(
            query=None,
            mode="replay",
            workspace=launch_config.workspace,
            config_dir=launch_config.config_dir,
            config_path=launch_config.config_path,
            runtime_config=launch_config.runtime_config,
            stub_response=launch_config.stub_response,
            replay_session_key=namespace.session_key,
            replay_limit=max(1, namespace.limit),
        )

    parser = argparse.ArgumentParser(prog="python -m aiops_v4.launch")
    parser.add_argument("--debug", action="store_true")
    parser.add_argument("--mode", choices=("cli", "single", "gateway"), default="cli")
    parser.add_argument("--config")
    parser.add_argument("--query")
    parser.add_argument("--zcloud-ip", default="172.20.22.221", help="zCloud IP（默认: 172.20.22.221）")
    parser.add_argument("--auth-token", default="", help="zCloud API Token")
    parser.add_argument("--user-info")
    parser.add_argument("--session-key")
    parser.add_argument("--fallback-policy", type=int, choices=(0, 1), default=1)
    parser.add_argument("--session-memory-enabled", action="store_true")
    namespace = parser.parse_args(args)

    launch_config = load_launch_config(namespace.config)
    user_info = None
    if namespace.user_info:
        try:
            parsed_user_info = json.loads(namespace.user_info)
        except json.JSONDecodeError as exc:
            raise ValueError(f"Invalid --user-info JSON: {exc.msg}") from exc
        if not isinstance(parsed_user_info, dict):
            raise ValueError("Invalid --user-info JSON: expected object")
        user_info = parsed_user_info

    return LaunchConfig(
        query=namespace.query,
        mode=namespace.mode,
        workspace=launch_config.workspace,
        config_dir=launch_config.config_dir,
        config_path=launch_config.config_path,
        runtime_config=launch_config.runtime_config,
        stub_response=launch_config.stub_response,
        debug=namespace.debug,
        zcloud_ip=namespace.zcloud_ip,
        user_info=user_info,
        auth_token=namespace.auth_token,
        session_key=namespace.session_key,
        fallback_policy=namespace.fallback_policy,
        session_memory_enabled=namespace.session_memory_enabled,
    )


def build_provider(runtime_config):
    provider = _make_provider(runtime_config)
    model = runtime_config.agents.defaults.model
    return ChatProviderAdapter(provider, model)


def _build_handler(config: LaunchConfig) -> AIOpsV4Handler:
    provider = build_provider(config.runtime_config)
    components = build_v4_runtime(
        provider=provider,
        workspace=config.workspace,
        config_dir=config.config_dir,
        fallback_policy=config.fallback_policy,
        session_memory_enabled=config.session_memory_enabled,
    )
    handler = AIOpsV4Handler(
        parent_runtime=components.parent_runtime,
        parent_output_schema=components.registry.config_dir / components.registry.parent.output_schema,
    )
    handler.debug = config.debug
    handler.zcloud_ip = config.zcloud_ip
    handler.user_info = config.user_info
    handler.session_key = config.session_key
    return handler


def build_gateway_task(
    msg,
    *,
    zcloud_ip: str | None = None,
) -> RuntimeTask:
    """Build a RuntimeTask from an inbound gateway message."""
    session_key = getattr(msg, "session_key", "")
    channel = getattr(msg, "channel", "gateway")
    sender_id = getattr(msg, "sender_id", "")
    metadata = getattr(msg, "metadata", None) or {}

    resource_context: dict = {"databases": []}
    if zcloud_ip:
        resource_context["zcloud_ip"] = zcloud_ip

    user_context: dict = {
        "channel": channel,
        "session_key": session_key,
        "user_id": sender_id,
    }
    if metadata.get("tenant_id"):
        user_context["tenant_id"] = metadata["tenant_id"]
    if metadata.get("username"):
        user_context["username"] = metadata["username"]

    return RuntimeTask(
        user_query=msg.content,
        resource_context=resource_context,
        user_context=user_context,
        conversation_context={"history": []},
    )


async def _run_gateway_v4(
    handler: AIOpsV4Handler,
    runtime_config,
    zcloud_ip: str,
    default_auth_headers: dict,
) -> None:
    """Gateway 模式：接入 nanobot 多通道消息总线，处理来自飞书/企微等渠道的消息。

    架构：
        Channel → MessageBus.inbound
            → _gateway_loop (per-session RuntimeTask)
                → AIOpsV4Handler（context_bridge 持久化会话上下文）
                    → MessageBus.outbound → ChannelManager → Channel
    """
    from nanobot.bus.queue import MessageBus
    from nanobot.channels.manager import ChannelManager

    bus = MessageBus()
    channels = ChannelManager(runtime_config, bus)

    enabled = getattr(channels, "enabled_channels", [])
    if enabled:
        logger.info(
            "[launch] AIOps v4 Gateway 已启动，已启用通道：%s",
            ", ".join(enabled),
        )
    else:
        logger.info(
            "[launch] 警告：未启用任何通道，请检查 config.json 中的 channels 配置"
        )

    try:
        await _asyncio.gather(
            _gateway_consume_loop(
                handler=handler,
                channels=channels,
                bus=bus,
                zcloud_ip=zcloud_ip,
                default_auth_headers=default_auth_headers,
            ),
            channels.start_all(),
        )
    except KeyboardInterrupt:
        logger.info("\n[launch] Gateway 已停止")
    finally:
        await channels.stop_all()



async def _process_gateway_message(
    msg,
    *,
    handler: AIOpsV4Handler,
    channels,
    bus,
    zcloud_ip: str,
    default_auth_headers: dict,
) -> None:
    from nanobot.bus.events import OutboundMessage

    session_key = getattr(msg, "session_key", "")
    channel_name = getattr(msg, "channel", "gateway")
    chat_id = getattr(msg, "chat_id", "")
    msg_id = getattr(msg, "message_id", None) or f"{channel_name}:{chat_id}:{id(msg)}"

    with get_legacy_tool_wrappers_port().apply_message_auth(msg, default_auth_headers):
        task = build_gateway_task(msg, zcloud_ip=zcloud_ip)
        channel_obj = channels.get_channel(channel_name)
        slot = channel_obj.make_slot_handle(msg_id, chat_id) if channel_obj else None

        try:
            loop = _asyncio.get_event_loop()
            result_text = await loop.run_in_executor(None, handler.handle_text, task, slot)
            if slot is None:
                await bus.publish_outbound(OutboundMessage(
                    channel=channel_name,
                    chat_id=chat_id,
                    content=result_text,
                ))
        except Exception as exc:
            try:
                from loguru import logger
                logger.error("Gateway: handle failed for session {}: {}", session_key, exc)
            except ImportError:
                logger.info(
                    "[gateway-error] session=%s error=%s",
                    session_key,
                    exc,
                )
            error_text = f"处理消息时出现错误：{exc}"
            if slot is not None and channel_obj is not None:
                try:
                    await channel_obj.close_slot(msg_id, chat_id, error_text)
                except Exception:
                    pass
            else:
                await bus.publish_outbound(OutboundMessage(
                    channel=channel_name,
                    chat_id=chat_id,
                    content=error_text,
                ))


async def _gateway_consume_loop(
    *,
    handler: AIOpsV4Handler,
    channels,
    bus,
    zcloud_ip: str,
    default_auth_headers: dict,
) -> None:
    try:
        from loguru import logger
        logger.info("AIOps v4 Gateway loop started, waiting for messages...")
    except ImportError:
        logger.info("[launch] Gateway loop started, waiting for messages...")

    while True:
        try:
            msg = await bus.consume_inbound()
            _asyncio.create_task(_process_gateway_message(
                msg,
                handler=handler,
                channels=channels,
                bus=bus,
                zcloud_ip=zcloud_ip,
                default_auth_headers=default_auth_headers,
            ))
        except _asyncio.CancelledError:
            break
        except Exception as exc:
            try:
                from loguru import logger as _log
                _log.error("Gateway loop error: {}", exc)
            except ImportError:
                logger.info("[gateway-loop-error] %s", exc)


async def start_gateway_background(
    *,
    handler: AIOpsV4Handler,
    runtime_config,
    zcloud_ip: str,
    default_auth_headers: dict,
) -> dict:
    from nanobot.bus.queue import MessageBus
    from nanobot.channels.manager import ChannelManager

    bus = MessageBus()
    channels = ChannelManager(runtime_config, bus)
    loop_task = asyncio.create_task(_gateway_consume_loop(
        handler=handler,
        channels=channels,
        bus=bus,
        zcloud_ip=zcloud_ip,
        default_auth_headers=default_auth_headers,
    ))
    channels_task = asyncio.create_task(channels.start_all())
    return {
        "bus": bus,
        "channels": channels,
        "loop_task": loop_task,
        "channels_task": channels_task,
    }


async def stop_gateway_background(state: dict | None) -> None:
    if not state:
        return

    loop_task = state.get("loop_task")
    channels_task = state.get("channels_task")
    channels = state.get("channels")

    if loop_task is not None:
        loop_task.cancel()
        try:
            await loop_task
        except asyncio.CancelledError:
            pass

    if channels is not None:
        try:
            await channels.stop_all()
        except Exception:
            pass

    if channels_task is not None and not channels_task.done():
        channels_task.cancel()
        try:
            await channels_task
        except asyncio.CancelledError:
            pass


def _iter_failed_stage_results(report: dict | None) -> list[dict]:
    if not isinstance(report, dict):
        return []

    failed_results: list[dict] = []
    for stage in report.get("stages", []):
        if not isinstance(stage, dict):
            continue
        for result in stage.get("results", []):
            if isinstance(result, dict) and result.get("success") is False:
                failed_results.append(result)
    return failed_results


def _print_failure_diagnostics(report: dict | None) -> None:
    for result in _iter_failed_stage_results(report):
        raw_output = result.get("raw_output")
        raw_log = raw_output.get("raw_response_log") if isinstance(raw_output, dict) else None
        logger.info(
            "[child-failure] agent=%s summary=%s",
            result.get("agent_id"),
            result.get("summary"),
        )
        if raw_log is not None:
            logger.info("[child-failure] raw=%s", raw_log)


def run_cli(handler: AIOpsV4Handler) -> None:
    logger.info("zCloud 智能运维管家 v4 已启动。输入 'exit' 退出。")
    while True:
        try:
            user_input = input("用户: ").strip()
        except (EOFError, KeyboardInterrupt):
            logger.info("\n再见！")
            break

        if not user_input:
            continue
        if user_input.lower() in {"exit", "quit", "退出"}:
            logger.info("再见！")
            break

        try:
            response = handler.handle_text(
                build_default_task(
                    user_input,
                    zcloud_ip=getattr(handler, "zcloud_ip", None),
                    user_info=getattr(handler, "user_info", None),
                    debug=getattr(handler, "debug", False),
                    session_key=getattr(handler, "session_key", None),
                )
            )
            logger.info("%s", response)
            _print_failure_diagnostics(getattr(handler, "last_report", None))
        except Exception as exc:
            logger.info("\n[错误] %s\n", exc)


def _build_default_auth_headers(config: LaunchConfig) -> dict:
    return get_legacy_tool_wrappers_port().build_default_auth_headers(
        auth_token=config.auth_token,
        user_info=config.user_info,
    )


def _safe_session_file_name(session_key: str) -> str:
    return re.sub(r"[^\w\-]", "_", session_key)[:128]


def _sha256_file(path: Path) -> str | None:
    try:
        h = hashlib.sha256()
        with open(path, "rb") as f:
            for chunk in iter(lambda: f.read(8192), b""):
                h.update(chunk)
        return h.hexdigest()
    except Exception:
        return None


def _load_compaction_events(workspace: str, session_key: str) -> list[dict]:
    audit_dir = Path(workspace) / "audit"
    if not audit_dir.exists():
        return []
    events: list[dict] = []
    for file in sorted(audit_dir.glob("*.jsonl")):
        try:
            for line in file.read_text(encoding="utf-8").splitlines():
                if not line.strip():
                    continue
                entry = json.loads(line)
                if not isinstance(entry, dict):
                    continue
                if entry.get("event_type") != "compaction_event":
                    continue
                data = entry.get("data")
                if isinstance(data, dict) and data.get("session_key") == session_key:
                    events.append(entry)
        except Exception:
            continue
    return events


def run_replay_compaction(config: LaunchConfig) -> int:
    session_key = (config.replay_session_key or "").strip()
    if not session_key:
        logger.info("--session-key is required")
        return 2

    session_path = Path(config.workspace) / "sessions" / f"{_safe_session_file_name(session_key)}.json"
    if not session_path.exists():
        logger.info("session not found: %s", session_path)
        return 2

    try:
        state = json.loads(session_path.read_text(encoding="utf-8"))
    except Exception as exc:
        logger.info("failed to read session: %s", exc)
        return 2

    history = state.get("history", [])
    summaries: list[dict] = []
    for turn in history:
        if not isinstance(turn, dict):
            continue
        agent_data = turn.get("agent_data")
        if not isinstance(agent_data, dict):
            continue
        summary = agent_data.get("__compaction_summary__")
        if not isinstance(summary, dict):
            continue
        summaries.append(
            {
                "turn": turn.get("turn"),
                "schema_version": summary.get("_compaction_schema_version"),
                "fallback_policy": summary.get("fallback_policy"),
                "fallback_taken": summary.get("fallback_taken"),
                "fallback_reason": summary.get("fallback_reason"),
                "summary": summary.get("summary"),
            }
        )

    compaction_events = _load_compaction_events(config.workspace, session_key)
    latest_event_data = compaction_events[-1]["data"] if compaction_events else {}
    if not isinstance(latest_event_data, dict):
        latest_event_data = {}
    archive_path = latest_event_data.get("archive_path")
    archive_exists = bool(archive_path) and Path(str(archive_path)).exists()
    archive_file_hash = _sha256_file(Path(str(archive_path))) if archive_exists else None
    archive_hash_expected = latest_event_data.get("archive_hash")
    archive_hash_matched = bool(
        archive_exists
        and archive_hash_expected
        and archive_file_hash
        and archive_hash_expected == archive_file_hash
    )

    out = {
        "session_key": session_key,
        "session_path": str(session_path),
        "history_turns": len(history) if isinstance(history, list) else 0,
        "summary_turns": len(summaries),
        "audit_events_count": len(compaction_events),
        "latest_compaction_event": (
            {
                "compaction_mode": latest_event_data.get("compaction_mode"),
                "fallback_policy": latest_event_data.get("fallback_policy"),
                "input_hash": latest_event_data.get("input_hash"),
                "output_hash": latest_event_data.get("output_hash"),
                "archive_path": archive_path,
                "archive_hash": archive_hash_expected,
                "archive_exists": archive_exists,
                "archive_file_hash": archive_file_hash,
                "archive_hash_matched": archive_hash_matched,
            }
            if latest_event_data
            else None
        ),
        "items": summaries[: max(1, config.replay_limit)],
    }
    logger.info("%s", json.dumps(out, ensure_ascii=False, indent=2))
    return 0


def main(argv: list[str] | None = None) -> int:
    try:
        config = parse_args(argv)
        if config.mode == "replay":
            return run_replay_compaction(config)
        handler = _build_handler(config)

        # Init v3 wrappers for all modes when a zCloud IP is available
        zcloud_ip = config.zcloud_ip or "172.20.22.221"
        auth_headers = _build_default_auth_headers(config)
        init_legacy_tool_wrappers(zcloud_ip, auth_headers)

        if config.mode == "cli":
            run_cli(handler)
            return 0

        if config.mode == "gateway":
            asyncio.run(_run_gateway_v4(
                handler=handler,
                runtime_config=config.runtime_config,
                zcloud_ip=zcloud_ip,
                default_auth_headers=auth_headers,
            ))
            return 0

        if not config.query:
            raise ValueError("--query is required when --mode single")
        logger.info(
            "%s",
            handler.handle_text(
                build_default_task(
                    config.query,
                    zcloud_ip=config.zcloud_ip,
                    user_info=config.user_info,
                    debug=config.debug,
                )
            ),
        )
        return 0
    except ValueError as exc:
        logger.info("%s", exc)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
