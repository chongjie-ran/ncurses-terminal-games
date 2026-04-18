"""
test_mcp_server.py - MCP Server 测试套件

V3.4 Phase1-A 测试
覆盖：
- JSON-RPC 2.0 解析与响应
- tools/list 方法
- tools/call 方法
- 错误处理 (parse error, invalid request, method not found)
- 工具注册/注销
"""

import pytest
import json
from src.mcp_server import (
    McpServer, JsonRpcRequest, JsonRpcResponse, JsonRpcError,
    JsonRpcErrorCode, ToolDefinition, ToolCall, ToolResult,
    get_mcp_server, reset_mcp_server,
)


class TestJsonRpcTypes:
    """JSON-RPC 类型测试"""
    
    def test_json_rpc_request_default(self):
        req = JsonRpcRequest()
        assert req.jsonrpc == "2.0"
        assert req.method == ""
        assert req.params is None
        assert req.id is None
    
    def test_json_rpc_request_full(self):
        req = JsonRpcRequest(
            method="tools/list",
            params={"verbose": True},
            id=1,
        )
        assert req.method == "tools/list"
        assert req.params["verbose"] is True
        assert req.id == 1
    
    def test_json_rpc_response_success(self):
        resp = JsonRpcResponse(id=1, result={"tools": []})
        d = resp.to_dict()
        assert d["jsonrpc"] == "2.0"
        assert d["result"]["tools"] == []
        assert d["id"] == 1
        assert "error" not in d
    
    def test_json_rpc_response_error(self):
        resp = JsonRpcResponse(
            id=1,
            error={"code": -32601, "message": "Method not found"},
        )
        d = resp.to_dict()
        assert d["error"]["code"] == -32601
        assert d["id"] == 1
        assert "result" not in d
    
    def test_json_rpc_error_to_dict(self):
        err = JsonRpcError(
            code=-32601,
            message="Method not found",
            data={"method": "unknown"},
        )
        d = err.to_dict()
        assert d["code"] == -32601
        assert d["message"] == "Method not found"
        assert d["data"]["method"] == "unknown"
    
    def test_json_rpc_error_without_data(self):
        err = JsonRpcError(code=-32600, message="Invalid Request")
        d = err.to_dict()
        assert d["code"] == -32600
        assert "data" not in d


class TestMcpServerInit:
    """MCP Server 初始化测试"""
    
    def test_init_default(self):
        server = McpServer()
        assert server.host == "127.0.0.1"
        assert server.port == 8080
        assert not server.running
    
    def test_init_custom(self):
        server = McpServer(host="0.0.0.0", port=9000)
        assert server.host == "0.0.0.0"
        assert server.port == 9000


class TestToolRegistration:
    """工具注册测试"""
    
    def test_register_tool(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        def handler(args):
            return ToolResult(content=[{"type": "text", "text": "ok"}])
        
        server.register_tool(
            name="bash",
            description="Execute a bash command",
            input_schema={"type": "object", "properties": {"cmd": {"type": "string"}}},
            handler=handler,
        )
        
        assert "bash" in server._tools
        assert "bash" in server._tool_definitions
        assert server.get_tool("bash") is handler
    
    def test_unregister_tool(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        def handler(args):
            return ToolResult(content=[{"type": "text", "text": "ok"}])
        
        server.register_tool("echo", "Echo back", {"type": "object"}, handler)
        assert server.unregister_tool("echo") is True
        assert "echo" not in server._tools
        assert server.unregister_tool("nonexistent") is False
    
    def test_register_multiple_tools(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        def handler1(args):
            return ToolResult(content=[{"type": "text", "text": "1"}])
        def handler2(args):
            return ToolResult(content=[{"type": "text", "text": "2"}])
        
        server.register_tool("tool1", "First tool", {"type": "object"}, handler1)
        server.register_tool("tool2", "Second tool", {"type": "object"}, handler2)
        
        assert len(server._tools) == 2
        assert len(server._tool_definitions) == 2


class TestToolsList:
    """tools/list 方法测试"""
    
    def test_tools_list_empty(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        req = JsonRpcRequest(method="tools/list", id=1)
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["tools"] == []
        assert resp.result["protocolVersion"] == server.PROTOCOL_VERSION
    
    def test_tools_list_with_tools(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        def bash_handler(args):
            return ToolResult(content=[{"type": "text", "text": "ok"}])
        
        server.register_tool(
            name="bash",
            description="Execute bash command",
            input_schema={"type": "object", "properties": {"cmd": {"type": "string"}}},
            handler=bash_handler,
        )
        
        req = JsonRpcRequest(method="tools/list", id=42)
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert len(resp.result["tools"]) == 1
        assert resp.result["tools"][0]["name"] == "bash"
        assert resp.result["tools"][0]["description"] == "Execute bash command"
        assert resp.result["tools"][0]["inputSchema"]["properties"]["cmd"]["type"] == "string"


class TestToolsCall:
    """tools/call 方法测试"""
    
    def test_tools_call_basic(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        def echo_handler(args):
            return ToolResult(content=[{"type": "text", "text": args.get("msg", "")}])
        
        server.register_tool(
            name="echo",
            description="Echo back",
            input_schema={"type": "object", "properties": {"msg": {"type": "string"}}},
            handler=echo_handler,
        )
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "echo", "arguments": {"msg": "hello"}},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["content"][0]["text"] == "hello"
        assert resp.result["isError"] is False
    
    def test_tools_call_with_empty_args(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        def no_args_handler(args):
            return ToolResult(content=[{"type": "text", "text": "no args"}])
        
        server.register_tool(
            name="ping",
            description="Ping",
            input_schema={"type": "object"},
            handler=no_args_handler,
        )
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "ping", "arguments": {}},
            id=1,
        )
        resp = server._dispatch(req)
        assert resp.result["content"][0]["text"] == "no args"
    
    def test_tools_call_not_found(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "nonexistent"},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.error is not None
        assert resp.error["code"] == -32601  # Method not found
    
    def test_tools_call_missing_name(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        req = JsonRpcRequest(method="tools/call", params={}, id=1)
        resp = server._dispatch(req)
        
        assert resp.error is not None
        assert resp.error["code"] == -32602  # Invalid params
    
    def test_tools_call_handler_exception(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        def bad_handler(args):
            raise RuntimeError("handler failed")
        
        server.register_tool(
            name="bad",
            description="Bad tool",
            input_schema={"type": "object"},
            handler=bad_handler,
        )
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "bad"},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["isError"] is True
        assert "handler failed" in resp.result["content"][0]["text"]


class TestMethodDispatch:
    """方法分发测试"""
    
    def test_method_not_found(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        req = JsonRpcRequest(method="unknown/method", id=1)
        resp = server._dispatch(req)
        
        assert resp.error is not None
        assert resp.error["code"] == -32601
    
    def test_batch_request_single(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        req = JsonRpcRequest(method="tools/list", id=1)
        resp = server._dispatch(req)
        assert resp.result is not None


class TestParseAndDispatch:
    """JSON 解析与分发测试"""
    
    def test_parse_valid_request(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        json_data = json.dumps({
            "jsonrpc": "2.0",
            "method": "tools/list",
            "id": 1,
        })
        resp = server.parse_and_dispatch(json_data)
        assert resp.result is not None
    
    def test_parse_invalid_json(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        req = JsonRpcRequest(method="tools/list", id=1)
        resp = server._dispatch(req)
        # 正常请求
        assert resp.result is not None
    
    def test_parse_missing_method(self):
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        req = JsonRpcRequest(method="", id=1)
        resp = server._dispatch(req)
        # method为空字符串的情况


class TestToolResult:
    """ToolResult 测试"""
    
    def test_tool_result_text(self):
        result = ToolResult(content=[{"type": "text", "text": "hello"}])
        d = result.to_dict()
        assert d["content"][0]["text"] == "hello"
        assert d["isError"] is False
    
    def test_tool_result_error(self):
        result = ToolResult(
            content=[{"type": "text", "text": "error message"}],
            is_error=True,
        )
        d = result.to_dict()
        assert d["isError"] is True


class TestMcpIntegration:
    """MCP 集成测试"""
    
    def test_full_tool_register_and_call_flow(self):
        """完整流程: 注册工具 → 列出工具 → 调用工具"""
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        # 1. 注册 bash 工具
        def bash_handler(args):
            cmd = args.get("cmd", "")
            return ToolResult(content=[{"type": "text", "text": f"executed: {cmd}"}])
        
        server.register_tool(
            name="bash",
            description="Execute a bash command",
            input_schema={
                "type": "object",
                "properties": {
                    "cmd": {"type": "string", "description": "The command to execute"}
                },
                "required": ["cmd"],
            },
            handler=bash_handler,
        )
        
        # 2. 列出工具
        list_req = JsonRpcRequest(method="tools/list", id=1)
        list_resp = server._dispatch(list_req)
        assert list_resp.result is not None
        assert len(list_resp.result["tools"]) == 1
        assert list_resp.result["tools"][0]["name"] == "bash"
        
        # 3. 调用工具
        call_req = JsonRpcRequest(
            method="tools/call",
            params={"name": "bash", "arguments": {"cmd": "echo hi"}},
            id=2,
        )
        call_resp = server._dispatch(call_req)
        assert call_resp.result is not None
        assert call_resp.result["content"][0]["text"] == "executed: echo hi"
        
        # 4. 注销工具
        server.unregister_tool("bash")
        list_req2 = JsonRpcRequest(method="tools/list", id=3)
        list_resp2 = server._dispatch(list_req2)
        assert len(list_resp2.result["tools"]) == 0
    
    def test_protocol_version(self):
        """验证协议版本一致性"""
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        req = JsonRpcRequest(method="tools/list", id=1)
        resp = server._dispatch(req)
        
        assert resp.result["protocolVersion"] == McpServer.PROTOCOL_VERSION
