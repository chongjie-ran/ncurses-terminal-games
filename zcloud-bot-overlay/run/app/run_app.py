#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
@Written by: 石海 (hai.shi@enmotech.com)
@Date: 2024/10/10

(c) Copyright 云和恩墨（北京）信息技术有限公司. All rights reserved.
"""
import os

import uvicorn

from aiops_v4.http.calling import set_handler
from aiops_v4.launch import (
    LaunchConfig,
    _build_default_auth_headers,
    _build_handler,
    init_legacy_tool_wrappers,
    load_launch_config,
    start_gateway_background,
    stop_gateway_background,
)
from .httpapi.fastapi.app import app


def _env_flag(name: str, default: bool = False) -> bool:
    value = os.getenv(name)
    if value is None:
        return default
    return value.strip().lower() in {"1", "true", "yes", "on"}


# ── 启动时初始化 handler ──────────────────────────────────────────
@app.on_event("startup")
async def _startup() -> None:
    zcloud_host = os.getenv('ZCLOUD_HOST')
    launch_cfg, handler, auth_headers = _bootstrap(
        config_path=os.getenv('NANO_CONFIG_PATH', 'aiops_v3/config.json'),
        zcloud_ip=zcloud_host,
        auth_token='',
        debug=_env_flag('AIOPS_DEBUG', default=True),
    )
    app.state.aiops_handler = handler
    app.state.gateway_bg = await start_gateway_background(
        handler=handler,
        runtime_config=launch_cfg.runtime_config,
        zcloud_ip=zcloud_host,
        default_auth_headers=auth_headers,
    )


@app.on_event("shutdown")
async def _shutdown() -> None:
    await stop_gateway_background(getattr(app.state, "gateway_bg", None))


def _bootstrap(
    *,
    config_path: str | None,
    zcloud_ip: str,
    auth_token: str,
    debug: bool = False,
) -> tuple[LaunchConfig, object, dict]:
    """构建 handler 并注册到 calling 模块。"""
    print(f"[aiops-v4] 正在初始化，config={config_path!r}  zcloud_ip={zcloud_ip}")

    launch_cfg: LaunchConfig = load_launch_config(config_path)
    launch_cfg = LaunchConfig(
        query=None,
        mode="gateway",
        workspace=launch_cfg.workspace,
        config_dir=launch_cfg.config_dir,
        config_path=launch_cfg.config_path,
        runtime_config=launch_cfg.runtime_config,
        stub_response=launch_cfg.stub_response,
        debug=debug,
        zcloud_ip=zcloud_ip,
        auth_token=auth_token,
    )

    handler = _build_handler(launch_cfg)
    auth_headers = _build_default_auth_headers(launch_cfg)
    init_legacy_tool_wrappers(zcloud_ip, auth_headers)

    set_handler(handler)
    print(f"[aiops-v4] Handler 就绪，监听 /api/calling")
    return launch_cfg, handler, auth_headers


def run_app(host: str, port: int):
    print('HTTP API START...')
    uvicorn.run(app=app, host=host, port=port)
