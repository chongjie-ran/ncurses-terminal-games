"""
mcp_server.py - MCP (Model Context Protocol) HTTP/JSON-RPC 2.0 Server

V3.4 Phase1-A 实现
核心功能：
- JSON-RPC 2.0 协议解析
- HTTP /mcp 端点
- tools/list: 列出所有可用工具
- tools/call: 调用指定工具
"""

from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any, Dict, List, Optional, Callable, Union
from enum import Enum
import json
import uuid
import logging
import asyncio
import threading
import os
from aiohttp import web
from datetime import datetime

from execution_trace import warn as _et_warn

logger = logging.getLogger(__name__)

# Import real executors for OpenClaw tools
from .bash_executor import BashExecutor, BashCommandInput
from .file_ops import FileOps


# ============================================================================
# JSON-RPC 2.0 Types
# ============================================================================

class JsonRpcVersion(str):
    VERSION = "2.0"


@dataclass
class JsonRpcRequest:
    """JSON-RPC 2.0 Request"""
    jsonrpc: str = "2.0"
    method: str = ""
    params: Optional[Dict[str, Any]] = None
    id: Union[str, int, None] = None


@dataclass
class JsonRpcResponse:
    """JSON-RPC 2.0 Response (success or error)"""
    jsonrpc: str = "2.0"
    id: Union[str, int, None] = None
    result: Optional[Any] = None
    error: Optional[Dict[str, Any]] = None

    def to_dict(self) -> Dict[str, Any]:
        if self.error is not None:
            return {
                "jsonrpc": self.jsonrpc,
                "error": self.error,
                "id": self.id,
            }
        return {
            "jsonrpc": self.jsonrpc,
            "result": self.result,
            "id": self.id,
        }


@dataclass
class JsonRpcError(Exception):
    """JSON-RPC 2.0 Error Object"""
    code: int
    message: str
    data: Optional[Any] = None

    def to_dict(self) -> Dict[str, Any]:
        result = {
            "code": self.code,
            "message": self.message,
        }
        if self.data is not None:
            result["data"] = self.data
        return result

    def __post_init__(self):
        Exception.__init__(self, self.message)


# JSON-RPC Error Codes
class JsonRpcErrorCode(Enum):
    PARSE_ERROR = (-32700, "Parse error")
    INVALID_REQUEST = (-32600, "Invalid Request")
    METHOD_NOT_FOUND = (-32601, "Method not found")
    INVALID_PARAMS = (-32602, "Invalid params")
    INTERNAL_ERROR = (-32603, "Internal error")


# ============================================================================
# MCP Types
# ============================================================================

@dataclass
class ToolDefinition:
    """MCP Tool Definition"""
    name: str
    description: str
    input_schema: Dict[str, Any]

    def to_dict(self) -> Dict[str, Any]:
        return {
            "name": self.name,
            "description": self.description,
            "inputSchema": self.input_schema,
        }


@dataclass
class ToolCall:
    """MCP Tool Call"""
    name: str
    arguments: Dict[str, Any]
    call_id: str = field(default_factory=lambda: str(uuid.uuid4()))


@dataclass
class ToolResult:
    """MCP Tool Call Result"""
    content: List[Dict[str, Any]]  # [{type: "text", "text": "..."}]
    is_error: bool = False

    def to_dict(self) -> Dict[str, Any]:
        return {
            "content": self.content,
            "isError": self.is_error,
        }


# ============================================================================
# MCP Server
# ============================================================================

class McpServer:
    """
    MCP (Model Context Protocol) HTTP/JSON-RPC 2.0 Server
    
    提供标准的 MCP 协议接口：
    - POST /mcp - JSON-RPC 2.0 请求端点
    - GET /mcp/health - 健康检查
    """
    
    PROTOCOL_VERSION = "2024-11-05"
    
    def __init__(self, host: str = "127.0.0.1", port: int = 8080):
        self.host = host
        self.port = port
        self._app: Optional[web.Application] = None
        self._runner: Optional[web.AppRunner] = None
        self._site: Optional[web.TCPSite] = None
        self._tools: Dict[str, Callable] = {}
        self._tool_definitions: Dict[str, ToolDefinition] = {}
        self._lock = threading.Lock()
        self._running = False
    
    # ========================================================================
    # Tool Registration
    # ========================================================================
    
    def register_tool(
        self,
        name: str,
        description: str,
        input_schema: Dict[str, Any],
        handler: Callable[[Dict[str, Any]], ToolResult],
    ) -> None:
        """
        注册一个工具
        
        Args:
            name: 工具名称
            description: 工具描述
            input_schema: JSON Schema格式的输入定义
            handler: 处理函数，接受 Dict 返回 ToolResult
        """
        with self._lock:
            self._tools[name] = handler
            self._tool_definitions[name] = ToolDefinition(
                name=name,
                description=description,
                input_schema=input_schema,
            )
        logger.info(f"Registered MCP tool: {name}")
    
    def unregister_tool(self, name: str) -> bool:
        """注销一个工具"""
        with self._lock:
            if name in self._tools:
                del self._tools[name]
                del self._tool_definitions[name]
                logger.info(f"Unregistered MCP tool: {name}")
                return True
        return False
    
    def get_tool(self, name: str) -> Optional[Callable]:
        """获取工具处理器"""
        with self._lock:
            return self._tools.get(name)
    
    # ========================================================================
    # JSON-RPC Method Handlers
    # ========================================================================
    
    def _handle_tools_list(self, params: Optional[Dict[str, Any]]) -> Dict[str, Any]:
        """处理 tools/list 请求"""
        with self._lock:
            tools = list(self._tool_definitions.values())
        
        return {
            "tools": [t.to_dict() for t in tools],
            "protocolVersion": self.PROTOCOL_VERSION,
        }
    
    def _handle_tools_call(self, params: Optional[Dict[str, Any]]) -> Dict[str, Any]:
        """处理 tools/call 请求"""
        if not params:
            raise JsonRpcError(
                code=JsonRpcErrorCode.INVALID_PARAMS.value[0],
                message="Missing params for tools/call",
            )
        
        name = params.get("name")
        arguments = params.get("arguments", {})
        
        if not name:
            raise JsonRpcError(
                code=JsonRpcErrorCode.INVALID_PARAMS.value[0],
                message="Missing 'name' parameter",
            )
        
        with self._lock:
            handler = self._tools.get(name)
        
        if not handler:
            raise JsonRpcError(
                code=JsonRpcErrorCode.METHOD_NOT_FOUND.value[0],
                message=f"Tool '{name}' not found",
            )
        
        try:
            result = handler(arguments)
            if not isinstance(result, ToolResult):
                # Auto-wrap non-ToolResult returns
                result = ToolResult(content=[{"type": "text", "text": str(result)}])
            
            return result.to_dict()
        
        except Exception as e:
            logger.error(f"Tool '{name}' execution failed: {e}")
            return ToolResult(
                content=[{"type": "text", "text": f"Error: {str(e)}"}],
                is_error=True,
            ).to_dict()
    
    def _dispatch(self, request: JsonRpcRequest) -> JsonRpcResponse:
        """分发 JSON-RPC 请求到对应处理方法"""
        method = request.method
        params = request.params
        
        try:
            if method == "tools/list":
                result = self._handle_tools_list(params)
            elif method == "tools/call":
                result = self._handle_tools_call(params)
            else:
                raise JsonRpcError(
                    code=JsonRpcErrorCode.METHOD_NOT_FOUND.value[0],
                    message=f"Method '{method}' not found",
                )
            
            return JsonRpcResponse(id=request.id, result=result)
        
        except JsonRpcError as e:
            return JsonRpcResponse(
                id=request.id,
                error=e.to_dict(),
            )
        except Exception as e:
            logger.error(f"Unexpected error in method '{method}': {e}")
            return JsonRpcResponse(
                id=request.id,
                error=JsonRpcError(
                    code=JsonRpcErrorCode.INTERNAL_ERROR.value[0],
                    message=f"Internal error: {str(e)}",
                ).to_dict(),
            )
    
    # ========================================================================
    # HTTP Request Handlers
    # ========================================================================
    
    async def _handle_mcp(self, request: web.Request) -> web.Response:
        """
        处理 POST /mcp 请求
        支持单次请求和批量请求
        """
        try:
            body = await request.text()
            data = json.loads(body)
        except json.JSONDecodeError as e:
            _et_warn(f"JSON parse error: {e}")
            return web.json_response(
                JsonRpcResponse(
                    id=None,
                    error=JsonRpcError(
                        code=JsonRpcErrorCode.PARSE_ERROR.value[0],
                        message="Parse error: Invalid JSON",
                    ).to_dict(),
                ).to_dict(),
                status=400,
            )
        
        # 单个请求
        if isinstance(data, dict):
            try:
                rpc_request = self._parse_request(data)
                rpc_response = self._dispatch(rpc_request)
                return web.json_response(rpc_response.to_dict())
            except JsonRpcError as e:
                return web.json_response(
                    JsonRpcResponse(id=None, error=e.to_dict()).to_dict(),
                    status=400,
                )
        
        # 批量请求
        elif isinstance(data, list):
            responses = []
            for item in data:
                if isinstance(item, dict):
                    try:
                        rpc_request = self._parse_request(item)
                        responses.append(self._dispatch(rpc_request).to_dict())
                    except Exception:
                        responses.append(
                            JsonRpcResponse(
                                id=None,
                                error=JsonRpcError(
                                    code=JsonRpcErrorCode.INVALID_REQUEST.value[0],
                                    message="Invalid request",
                                ).to_dict(),
                            ).to_dict()
                        )
            return web.json_response(responses)
        
        # 无效请求
        return web.json_response(
            JsonRpcResponse(
                id=None,
                error=JsonRpcError(
                    code=JsonRpcErrorCode.INVALID_REQUEST.value[0],
                    message="Invalid request format",
                ).to_dict(),
            ).to_dict(),
            status=400,
        )
    
    async def _handle_health(self, request: web.Request) -> web.Response:
        """处理 GET /mcp/health 请求"""
        with self._lock:
            tool_count = len(self._tools)
            tool_names = list(self._tools.keys())
        
        return web.json_response({
            "status": "healthy",
            "protocolVersion": self.PROTOCOL_VERSION,
            "tools": tool_names,
            "toolCount": tool_count,
            "timestamp": datetime.now().isoformat(),
        })
    
    def _parse_request(self, data: Dict[str, Any]) -> JsonRpcRequest:
        """解析 JSON-RPC 请求"""
        if data.get("jsonrpc") != "2.0":
            raise JsonRpcError(
                code=JsonRpcErrorCode.INVALID_REQUEST.value[0],
                message="JSON-RPC version must be '2.0'",
            )
        
        if not data.get("method"):
            raise JsonRpcError(
                code=JsonRpcErrorCode.INVALID_REQUEST.value[0],
                message="Missing 'method' field",
            )
        
        return JsonRpcRequest(
            jsonrpc=data.get("jsonrpc", "2.0"),
            method=data["method"],
            params=data.get("params"),
            id=data.get("id"),
        )
    
    # ========================================================================
    # Server Lifecycle
    # ========================================================================
    
    async def _create_app(self) -> web.Application:
        """创建 aiohttp Application"""
        app = web.Application()
        app.router.add_post("/mcp", self._handle_mcp)
        app.router.add_get("/mcp/health", self._handle_health)
        self._app = app
        return app
    
    async def start(self) -> None:
        """启动 MCP Server"""
        if self._running:
            _et_warn("MCP Server already running")
            return
        
        app = await self._create_app()
        self._runner = web.AppRunner(app)
        await self._runner.setup()
        self._site = web.TCPSite(self._runner, self.host, self.port)
        await self._site.start()
        self._running = True
        logger.info(f"MCP Server started at http://{self.host}:{self.port}/mcp")
    
    async def stop(self) -> None:
        """停止 MCP Server"""
        if not self._running:
            return
        
        if self._runner:
            await self._runner.cleanup()
        self._running = False
        self._runner = None
        self._site = None
        logger.info("MCP Server stopped")
    
    @property
    def running(self) -> bool:
        return self._running
    
    # ========================================================================
    # Synchronous wrapper for non-async contexts
    # ========================================================================
    
    def dispatch_sync(self, request: JsonRpcRequest) -> JsonRpcResponse:
        """同步分发（用于测试）"""
        return self._dispatch(request)
    
    def parse_and_dispatch(self, json_data: str) -> JsonRpcResponse:
        """解析并分发（用于测试）"""
        data = json.loads(json_data)
        rpc_request = self._parse_request(data)
        return self._dispatch(rpc_request)


# ============================================================================
# Global MCP Server Instance (for integration)
# ============================================================================

_mcp_server: Optional[McpServer] = None
_server_lock = threading.Lock()


def get_mcp_server(host: str = "127.0.0.1", port: int = 8080) -> McpServer:
    """获取全局 MCP Server 单例"""
    global _mcp_server
    with _server_lock:
        if _mcp_server is None:
            _mcp_server = McpServer(host=host, port=port)
        return _mcp_server


def reset_mcp_server() -> None:
    """重置全局 MCP Server（用于测试）"""
    global _mcp_server
    with _server_lock:
        if _mcp_server is not None:
            _mcp_server = None


# ============================================================================
# OpenClaw Real Tool Handlers (V3.4 Phase2)
# ============================================================================

# Global executors for tool handlers
_bash_executor = BashExecutor()
_file_ops = FileOps()


def _tool_handler_wrapper(name: str, impl: Callable[[Dict[str, Any]], Any]) -> Callable[[Dict[str, Any]], ToolResult]:
    """Wrap a tool implementation to return ToolResult"""
    def wrapper(args: Dict[str, Any]) -> ToolResult:
        try:
            result = impl(args)
            # Convert result to string if needed
            if isinstance(result, str):
                content = result
            elif hasattr(result, 'to_dict'):
                content = json.dumps(result.to_dict(), ensure_ascii=False)
            else:
                content = json.dumps(result, ensure_ascii=False, default=str)
            return ToolResult(content=[{"type": "text", "text": content}])
        except Exception as e:
            logger.error(f"Tool '{name}' failed: {e}")
            return ToolResult(
                content=[{"type": "text", "text": f"Error: {str(e)}"}],
                is_error=True,
            )
    return wrapper


# ---- bash tool handler ----
def _bash_impl(args: Dict[str, Any]) -> str:
    """Execute a bash command"""
    cmd = args.get("command", "")
    if not cmd:
        raise ValueError("Missing 'command' argument")
    timeout = args.get("timeout", 60000)  # default 60s in ms
    input_obj = BashCommandInput(command=cmd, timeout=timeout)
    result = _bash_executor.execute(input_obj)
    if result.interrupted:
        return json.dumps({
            "success": False,
            "stdout": result.stdout,
            "stderr": result.stderr,
            "interrupted": True,
            "return_code": None,
        })
    return json.dumps({
        "success": result.success,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "return_code": result.return_code,
        "interrupted": False,
    })


# ---- read tool handler ----
def _read_impl(args: Dict[str, Any]) -> str:
    """Read a file"""
    path = args.get("path")
    if not path:
        raise ValueError("Missing 'path' argument")
    offset = args.get("offset")
    limit = args.get("limit")
    result = FileOps.read_file(path, offset=offset, limit=limit)
    return json.dumps({
        "file_path": result.file.file_path,
        "content": result.file.content,
        "num_lines": result.file.num_lines,
        "start_line": result.file.start_line,
        "total_lines": result.file.total_lines,
    }, ensure_ascii=False)


# ---- write tool handler ----
def _write_impl(args: Dict[str, Any]) -> str:
    """Write a file"""
    path = args.get("path")
    content = args.get("content")
    if not path:
        raise ValueError("Missing 'path' argument")
    if content is None:
        raise ValueError("Missing 'content' argument")
    result = FileOps.write_file(path, content)
    return json.dumps({
        "file_path": result.file_path,
        "content": result.content,
        "type": "write",
        "original_file": result.original_file,
    }, ensure_ascii=False)


# ---- edit tool handler ----
def _edit_impl(args: Dict[str, Any]) -> str:
    """Edit a file"""
    path = args.get("path")
    old_text = args.get("oldText")
    new_text = args.get("newText")
    if not path:
        raise ValueError("Missing 'path' argument")
    if old_text is None or new_text is None:
        raise ValueError("Missing 'oldText' or 'newText' argument")
    replace_all = args.get("replace_all", False)
    result = FileOps.edit_file(path, old_text, new_text, replace_all=replace_all)
    return json.dumps({
        "file_path": result.file_path,
        "old_string": result.old_string,
        "new_string": result.new_string,
        "original_file": result.original_file,
        "replace_all": result.replace_all,
    }, ensure_ascii=False)


# ---- exec tool handler (same as bash) ----
def _exec_impl(args: Dict[str, Any]) -> str:
    """Execute a command (alias for bash)"""
    return _bash_impl(args)


# ---- glob tool handler ----
def _glob_impl(args: Dict[str, Any]) -> str:
    """Glob file matching"""
    pattern = args.get("pattern", "*")
    path = args.get("path")
    result = FileOps.glob_search(pattern, path=path)
    return json.dumps({
        "duration_ms": result.duration_ms,
        "num_files": result.num_files,
        "filenames": result.filenames,
        "truncated": result.truncated,
    }, ensure_ascii=False)


# ---- grep tool handler ----
def _grep_impl(args: Dict[str, Any]) -> str:
    """Grep search"""
    pattern = args.get("pattern", "")
    if not pattern:
        raise ValueError("Missing 'pattern' argument")
    path = args.get("path")
    case_insensitive = args.get("case_insensitive", False)
    result = FileOps.grep_search(pattern, path=path, case_insensitive=case_insensitive)
    return json.dumps({
        "num_files": result.num_files,
        "filenames": result.filenames,
        "content": result.content,
        "num_lines": result.num_lines,
        "num_matches": result.num_matches,
    }, ensure_ascii=False)


# ============================================================================
# Tool Registration
# ============================================================================

def register_openclaw_tools(server: McpServer) -> None:
    """
    Register all OpenClaw tools (bash, read, write, edit, exec, glob, grep)
    
    These use the real BashExecutor and FileOps implementations.
    """
    tools = [
        (
            "bash",
            "Execute a bash command",
            {
                "type": "object",
                "properties": {
                    "command": {"type": "string", "description": "The bash command to execute"},
                    "timeout": {"type": "integer", "description": "Timeout in milliseconds", "default": 60000},
                },
                "required": ["command"],
            },
            _tool_handler_wrapper("bash", _bash_impl),
        ),
        (
            "read",
            "Read a file",
            {
                "type": "object",
                "properties": {
                    "path": {"type": "string", "description": "File path to read"},
                    "offset": {"type": "integer", "description": "Starting line offset"},
                    "limit": {"type": "integer", "description": "Max lines to read"},
                },
                "required": ["path"],
            },
            _tool_handler_wrapper("read", _read_impl),
        ),
        (
            "write",
            "Write content to a file",
            {
                "type": "object",
                "properties": {
                    "path": {"type": "string", "description": "File path to write"},
                    "content": {"type": "string", "description": "Content to write"},
                },
                "required": ["path", "content"],
            },
            _tool_handler_wrapper("write", _write_impl),
        ),
        (
            "edit",
            "Edit a file by replacing old text with new text",
            {
                "type": "object",
                "properties": {
                    "path": {"type": "string", "description": "File path to edit"},
                    "oldText": {"type": "string", "description": "Text to find and replace"},
                    "newText": {"type": "string", "description": "Replacement text"},
                    "replace_all": {"type": "boolean", "description": "Replace all occurrences", "default": False},
                },
                "required": ["path", "oldText", "newText"],
            },
            _tool_handler_wrapper("edit", _edit_impl),
        ),
        (
            "exec",
            "Execute a command (alias for bash)",
            {
                "type": "object",
                "properties": {
                    "command": {"type": "string", "description": "The command to execute"},
                    "timeout": {"type": "integer", "description": "Timeout in milliseconds", "default": 60000},
                },
                "required": ["command"],
            },
            _tool_handler_wrapper("exec", _exec_impl),
        ),
        (
            "glob",
            "Find files matching a glob pattern",
            {
                "type": "object",
                "properties": {
                    "pattern": {"type": "string", "description": "Glob pattern (e.g., *.py)"},
                    "path": {"type": "string", "description": "Base directory to search"},
                },
                "required": ["pattern"],
            },
            _tool_handler_wrapper("glob", _glob_impl),
        ),
        (
            "grep",
            "Search for text in files",
            {
                "type": "object",
                "properties": {
                    "pattern": {"type": "string", "description": "Regex pattern to search"},
                    "path": {"type": "string", "description": "Directory to search in"},
                    "case_insensitive": {"type": "boolean", "description": "Case insensitive search", "default": False},
                },
                "required": ["pattern"],
            },
            _tool_handler_wrapper("grep", _grep_impl),
        ),
    ]
    
    for name, description, schema, handler in tools:
        server.register_tool(name, description, schema, handler)
        logger.info(f"Registered OpenClaw tool: {name}")


def create_openclaw_mcp_server(host: str = "127.0.0.1", port: int = 8080) -> McpServer:
    """Create and configure an MCP server with OpenClaw tools"""
    server = get_mcp_server(host=host, port=port)
    register_openclaw_tools(server)
    return server


__all__ = [
    'McpServer', 'McpError', 'ToolDefinition', 'ToolCall', 'ToolResult',
    'JsonRpcRequest', 'JsonRpcResponse', 'JsonRpcError', 'JsonRpcErrorCode',
    'get_mcp_server', 'reset_mcp_server', 'register_openclaw_tools',
    'create_openclaw_mcp_server',
]
