#!/usr/bin/env python3
"""One-off patch for zcloud-bot aiops_v4/launch.py — run from workspace."""
from pathlib import Path

launch = Path("/Users/chongjieran/SWproject/zCloud-bot/zcloud-bot/aiops_v4/launch.py")
text = launch.read_text(encoding="utf-8")

old_a = """from types import SimpleNamespace

from aiops_v4.bootstrap import build_runtime_components
from aiops_v4.bridge import wrappers as wrapper_bridge
from aiops_v4.handler import AIOpsV4Handler"""
new_a = """from types import SimpleNamespace
from typing import Protocol, runtime_checkable

from aiops_v4.bootstrap import build_runtime_components
from aiops_v4.bridge import wrappers as wrapper_bridge
from aiops_v4.bridge.wrappers import WrapperBridgeState
from aiops_v4.handler import AIOpsV4Handler"""
if old_a not in text:
    raise SystemExit("block A not found")
text = text.replace(old_a, new_a, 1)

old_b = """from nanobot.config.loader import load_config, resolve_config_env_vars, set_config_path

DEFAULT_STUB_RESPONSE = json.dumps("""
new_b = """from nanobot.config.loader import load_config, resolve_config_env_vars, set_config_path


@runtime_checkable
class LegacyToolWrappersPort(Protocol):
    \"\"\"v4 启动路径与 v3 HTTP/tool 封装之间的稳定边界（默认实现：`aiops_v4.bridge.wrappers`）。\"\"\"

    def init_wrappers(self, zcloud_ip: str, auth_headers: dict) -> WrapperBridgeState:
        ...

    def build_default_auth_headers(
        self, *, auth_token: str = "", user_info: dict | None = None
    ) -> dict[str, str]:
        ...

    def apply_message_auth(self, message, default_auth_headers: dict):
        ...


def get_legacy_tool_wrappers_port() -> LegacyToolWrappersPort:
    \"\"\"返回当前桥接实现；测试可通过 patch ``aiops_v4.launch.wrapper_bridge`` 替换行为。\"\"\"
    return wrapper_bridge  # type: ignore[return-value]


def init_legacy_tool_wrappers(zcloud_ip: str, auth_headers: dict) -> WrapperBridgeState:
    \"\"\"初始化 v3 侧 tool 模块（供 HTTP/CLI/Gateway 与 `main()` 共用）。\"\"\"
    return get_legacy_tool_wrappers_port().init_wrappers(zcloud_ip, auth_headers)


DEFAULT_STUB_RESPONSE = json.dumps("""
if old_b not in text:
    raise SystemExit("block B not found")
text = text.replace(old_b, new_b, 1)

old_c = """    return handler


def _init_v3_wrappers(zcloud_ip: str, auth_headers: dict) -> None:
    \"\"\"Initialize v3 wrapper modules with the given IP and auth headers.\"\"\"
    try:
        import aiops_v3.wrappers.resources as _resources
        import aiops_v3.wrappers.inspection as _inspection
        import aiops_v3.wrappers.alerts as _alerts
        import aiops_v3.wrappers.alert_diagnosis as _alert_diagnosis
        import aiops_v3.wrappers.metric_query as _metric_query
        import aiops_v3.wrappers.performance as _performance
        import aiops_v3.wrappers.prometheus as _prometheus
        _resources.init(zcloud_ip)
        _inspection.init(zcloud_ip)
        _alerts.init(zcloud_ip)
        _alert_diagnosis.init(zcloud_ip)
        _performance.init(zcloud_ip)
        _prometheus.init(zcloud_ip)
        _metric_query.init(zcloud_ip)
        _resources.set_request_headers(auth_headers)
        _inspection.set_request_headers(auth_headers)
        _alerts.set_request_headers(auth_headers)
        _alert_diagnosis.set_request_headers(auth_headers)
        _performance.set_request_headers(auth_headers)
        _prometheus.set_request_headers(auth_headers)
        _metric_query.set_request_headers(auth_headers)
    except ImportError:
        pass


def _build_msg_auth_headers(msg, default_auth_headers: dict) -> dict:
    \"\"\"Build per-message auth headers by merging defaults with message metadata.\"\"\"
    msg_auth = dict(default_auth_headers)
    metadata = getattr(msg, "metadata", None) or {}
    if metadata.get("auth_token"):
        msg_auth["Authorization"] = f"Bearer {metadata['auth_token']}"
    if metadata.get("user_info"):
        msg_auth["userInfo"] = metadata["user_info"]
    return msg_auth


def _apply_msg_auth_to_wrappers(msg_auth: dict) -> None:
    \"\"\"Update v3 wrapper auth headers for the current async context (per-message).

    Uses set_context_headers so concurrent gateway messages each get their own
    headers without overwriting the global defaults.
    \"\"\"
    try:
        import aiops_v3.wrappers.resources as _resources
        import aiops_v3.wrappers.inspection as _inspection
        import aiops_v3.wrappers.alerts as _alerts
        import aiops_v3.wrappers.alert_diagnosis as _alert_diagnosis
        import aiops_v3.wrappers.metric_query as _metric_query
        import aiops_v3.wrappers.performance as _performance
        import aiops_v3.wrappers.prometheus as _prometheus
        _resources._w.set_context_headers(msg_auth)
        _inspection._w.set_context_headers(msg_auth)
        _alerts._w.set_context_headers(msg_auth)
        _alert_diagnosis._w.set_context_headers(msg_auth)
        _performance._w.set_context_headers(msg_auth)
        _prometheus._w.set_context_headers(msg_auth)
        _metric_query._w.set_context_headers(msg_auth)
    except (ImportError, AttributeError):
        pass


def build_gateway_task("""
new_c = """    return handler


def build_gateway_task("""
if old_c not in text:
    raise SystemExit("block C (delete funcs) not found")
text = text.replace(old_c, new_c, 1)

text = text.replace(
    "    with wrapper_bridge.apply_message_auth(msg, default_auth_headers):",
    "    with get_legacy_tool_wrappers_port().apply_message_auth(msg, default_auth_headers):",
    1,
)
text = text.replace(
    """def _build_default_auth_headers(config: LaunchConfig) -> dict:
    return wrapper_bridge.build_default_auth_headers(
        auth_token=config.auth_token,
        user_info=config.user_info,
    )""",
    """def _build_default_auth_headers(config: LaunchConfig) -> dict:
    return get_legacy_tool_wrappers_port().build_default_auth_headers(
        auth_token=config.auth_token,
        user_info=config.user_info,
    )""",
    1,
)
text = text.replace(
    "        wrapper_bridge.init_wrappers(zcloud_ip, auth_headers)",
    "        init_legacy_tool_wrappers(zcloud_ip, auth_headers)",
    1,
)

launch.write_text(text, encoding="utf-8")
print("OK")
