"""aiops_v4 HTTP 入口模块。

启动方式
--------
直接运行（开发）::

    python -m aiops_v4.http.app \\
        --config  aiops_v3/config.json \\
        --host    0.0.0.0 \\
        --port    8000 \\
        --zcloud-ip 172.20.22.221 \\
        --auth-token YOUR_TOKEN

生产（uvicorn）::

    uvicorn aiops_v4.http.app:create_app --factory \\
        --host 0.0.0.0 --port 8000 --workers 1

注意：workers 必须为 1，ChatProviderAdapter 的后台事件循环不能跨进程共享。
"""

from __future__ import annotations

import argparse
import logging
import sys

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from aiops_v4.launch import (
    LaunchConfig,
    _build_default_auth_headers,
    _build_handler,
    init_legacy_tool_wrappers,
    load_launch_config,
    start_gateway_background,
    stop_gateway_background,
)
from aiops_v4.http.calling import set_handler

logger = logging.getLogger(__name__)


# ---------------------------------------------------------------------------
# App 工厂
# ---------------------------------------------------------------------------

def create_app(
    *,
    config_path: str | None = None,
    zcloud_ip: str = "172.20.22.221",
    auth_token: str = "",
    debug: bool = False,
) -> FastAPI:
    """创建并返回配置好的 FastAPI 实例。

    可被 uvicorn --factory 调用，也可直接在代码里 import 使用::

        from aiops_v4.http.app import create_app
        app = create_app(config_path="aiops_v3/config.json")
    """
    app = FastAPI(
        title="AIOps v4 API",
        version="4.0.0",
        docs_url="/docs" if debug else None,
        redoc_url=None,
    )

    # CORS（按需收紧 allow_origins）
    app.add_middleware(
        CORSMiddleware,
        allow_origins=["*"],
        allow_methods=["*"],
        allow_headers=["*"],
    )

    # ── 启动时初始化 handler ──────────────────────────────────────────
    @app.on_event("startup")
    async def _startup() -> None:
        launch_cfg, handler, auth_headers = _bootstrap(
            config_path=config_path,
            zcloud_ip=zcloud_ip,
            auth_token=auth_token,
            debug=debug,
        )
        app.state.aiops_handler = handler
        app.state.gateway_bg = await start_gateway_background(
            handler=handler,
            runtime_config=launch_cfg.runtime_config,
            zcloud_ip=zcloud_ip,
            default_auth_headers=auth_headers,
        )
        channels = app.state.gateway_bg.get("channels") if getattr(app.state, "gateway_bg", None) else None
        enabled = getattr(channels, "enabled_channels", []) if channels is not None else []
        logger.info(
            "[aiops-v4] Gateway 通道已启动：%s",
            ", ".join(enabled) if enabled else "none",
        )

    @app.on_event("shutdown")
    async def _shutdown() -> None:
        await stop_gateway_background(getattr(app.state, "gateway_bg", None))

    # ── 挂载路由 ─────────────────────────────────────────────────────
    from aiops_v4.http.router import router
    app.include_router(router)

    return app


# ---------------------------------------------------------------------------
# 初始化逻辑（可单独调用，方便集成到已有 FastAPI 项目）
# ---------------------------------------------------------------------------

def _bootstrap(
    *,
    config_path: str | None,
    zcloud_ip: str,
    auth_token: str,
    debug: bool = False,
) -> tuple[LaunchConfig, object, dict]:
    """构建 handler 并注册到 calling 模块。"""
    logger.info(
        "[aiops-v4] 正在初始化，config=%r  zcloud_ip=%s",
        config_path,
        zcloud_ip,
    )

    launch_cfg: LaunchConfig = load_launch_config(config_path)
    # 用命令行参数覆盖配置文件中的值
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
    logger.info("[aiops-v4] Handler 就绪，监听 /api/calling")
    return launch_cfg, handler, auth_headers


# ---------------------------------------------------------------------------
# CLI 入口
# ---------------------------------------------------------------------------

def _parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(prog="python -m aiops_v4.http.app")
    parser.add_argument("--config", default=None, help="config.json 路径（默认自动查找）")
    parser.add_argument("--host", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=8000)
    parser.add_argument("--zcloud-ip", default="172.20.22.221")
    parser.add_argument("--auth-token", default="")
    parser.add_argument("--debug", action="store_true")
    parser.add_argument("--reload", action="store_true", help="开发热重载（不可与 workers>1 同用）")
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> None:
    args = _parse_args(argv)

    try:
        import uvicorn
    except ImportError:
        logger.info("请先安装 uvicorn：pip install uvicorn")
        sys.exit(1)

    app = create_app(
        config_path=args.config,
        zcloud_ip=args.zcloud_ip,
        auth_token=args.auth_token,
        debug=args.debug,
    )

    uvicorn.run(
        app,
        host=args.host,
        port=args.port,
        reload=args.reload,
        # workers 固定为 1：ChatProviderAdapter 的后台事件循环不能跨进程
        workers=1,
        log_level="debug" if args.debug else "info",
    )


if __name__ == "__main__":
    main()
