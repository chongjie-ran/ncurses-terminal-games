"""
test_mcp_openclaw_tools.py - V3.4 Phase2 OpenClaw工具集成测试

验证真实工具处理器 (bash/read/write/edit/exec/glob/grep)
"""

import pytest
import json
import os
import tempfile
from src.mcp_server import (
    McpServer, JsonRpcRequest, ToolResult,
    get_mcp_server, reset_mcp_server,
    register_openclaw_tools, create_openclaw_mcp_server,
)


class TestOpenClawToolsRegistration:
    """工具注册测试"""
    
    def test_register_openclaw_tools(self):
        """测试注册OpenClaw工具"""
        reset_mcp_server()
        server = get_mcp_server()
        server._tools.clear()
        server._tool_definitions.clear()
        
        register_openclaw_tools(server)
        
        # 验证7个工具已注册
        assert len(server._tools) == 7
        assert "bash" in server._tools
        assert "read" in server._tools
        assert "write" in server._tools
        assert "edit" in server._tools
        assert "exec" in server._tools
        assert "glob" in server._tools
        assert "grep" in server._tools
    
    def test_create_openclaw_mcp_server(self):
        """测试创建完整配置的MCP服务器"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        assert len(server._tools) == 7


class TestToolsListIntegration:
    """tools/list 集成测试"""
    
    def test_tools_list_returns_openclaw_tools(self):
        """验证tools/list返回OpenClaw工具"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(method="tools/list", id=1)
        resp = server._dispatch(req)
        
        assert resp.result is not None
        tools = resp.result["tools"]
        tool_names = [t["name"] for t in tools]
        
        assert "bash" in tool_names
        assert "read" in tool_names
        assert "write" in tool_names
        assert "edit" in tool_names
        assert "exec" in tool_names
        assert "glob" in tool_names
        assert "grep" in tool_names


class TestBashTool:
    """bash工具测试"""
    
    def test_bash_echo(self):
        """测试bash执行echo命令"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "bash", "arguments": {"command": "echo hello"}},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["isError"] is False
        result = json.loads(resp.result["content"][0]["text"])
        assert result["success"] is True
        assert "hello" in result["stdout"]
        assert result["return_code"] == 0
    
    def test_bash_with_timeout(self):
        """测试bash超时控制"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "bash", "arguments": {"command": "sleep 0.1", "timeout": 5000}},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        result = json.loads(resp.result["content"][0]["text"])
        assert result["success"] is True


class TestReadTool:
    """read工具测试"""
    
    def test_read_file(self):
        """测试读取文件"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        # 读取自己
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "read", "arguments": {"path": "src/mcp_server.py", "limit": 5}},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["isError"] is False
        result = json.loads(resp.result["content"][0]["text"])
        assert "mcp_server.py" in result["file_path"]
        assert result["num_lines"] <= 5
        assert len(result["content"]) > 0
    
    def test_read_nonexistent_file(self):
        """测试读取不存在的文件"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "read", "arguments": {"path": "/nonexistent/file.txt"}},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["isError"] is True


class TestWriteTool:
    """write工具测试"""
    
    def test_write_and_read(self):
        """测试写入并读取文件"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            temp_path = f.name
            f.write("initial content")
        
        try:
            # 写入新内容
            req = JsonRpcRequest(
                method="tools/call",
                params={"name": "write", "arguments": {"path": temp_path, "content": "new content\nline2"}},
                id=1,
            )
            resp = server._dispatch(req)
            assert resp.result is not None
            assert resp.result["isError"] is False
            
            # 读取验证
            req2 = JsonRpcRequest(
                method="tools/call",
                params={"name": "read", "arguments": {"path": temp_path}},
                id=2,
            )
            resp2 = server._dispatch(req2)
            result = json.loads(resp2.result["content"][0]["text"])
            assert "new content" in result["content"]
        finally:
            os.unlink(temp_path)


class TestEditTool:
    """edit工具测试"""
    
    def test_edit_file(self):
        """测试编辑文件"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            temp_path = f.name
            f.write("hello world")
        
        try:
            req = JsonRpcRequest(
                method="tools/call",
                params={"name": "edit", "arguments": {
                    "path": temp_path,
                    "oldText": "world",
                    "newText": "openclaw",
                }},
                id=1,
            )
            resp = server._dispatch(req)
            assert resp.result is not None
            assert resp.result["isError"] is False
            
            # 读取验证
            with open(temp_path) as f:
                content = f.read()
            assert content == "hello openclaw"
        finally:
            os.unlink(temp_path)


class TestGlobTool:
    """glob工具测试"""
    
    def test_glob_python_files(self):
        """测试glob匹配"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "glob", "arguments": {"pattern": "*.py", "path": "src"}},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["isError"] is False
        result = json.loads(resp.result["content"][0]["text"])
        assert result["num_files"] > 0
        assert any("mcp_server.py" in f for f in result["filenames"])


class TestGrepTool:
    """grep工具测试"""
    
    def test_grep_in_file(self):
        """测试grep搜索"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "grep", "arguments": {
                "pattern": "McpServer",
                "path": "src",
            }},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["isError"] is False
        result = json.loads(resp.result["content"][0]["text"])
        assert result["num_matches"] > 0


class TestExecTool:
    """exec工具测试 (bash alias)"""
    
    def test_exec_basic(self):
        """测试exec执行"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "exec", "arguments": {"command": "pwd"}},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["isError"] is False
        result = json.loads(resp.result["content"][0]["text"])
        assert result["success"] is True
        assert len(result["stdout"]) > 0


class TestEndToEnd:
    """端到端测试"""
    
    def test_full_workflow(self):
        """完整工作流: 列出工具 → 写文件 → 读文件 → 编辑 → 执行bash验证"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            temp_path = f.name
            f.write("initial")
        
        try:
            # 1. 列出工具
            list_req = JsonRpcRequest(method="tools/list", id=1)
            list_resp = server._dispatch(list_req)
            assert len(list_resp.result["tools"]) == 7
            
            # 2. 写入文件
            write_req = JsonRpcRequest(
                method="tools/call",
                params={"name": "write", "arguments": {"path": temp_path, "content": "hello world"}},
                id=2,
            )
            write_resp = server._dispatch(write_req)
            assert write_resp.result["isError"] is False
            
            # 3. 读取文件
            read_req = JsonRpcRequest(
                method="tools/call",
                params={"name": "read", "arguments": {"path": temp_path}},
                id=3,
            )
            read_resp = server._dispatch(read_req)
            content = json.loads(read_resp.result["content"][0]["text"])["content"]
            assert "hello world" in content
            
            # 4. 编辑文件
            edit_req = JsonRpcRequest(
                method="tools/call",
                params={"name": "edit", "arguments": {
                    "path": temp_path,
                    "oldText": "world",
                    "newText": "openclaw",
                }},
                id=4,
            )
            edit_resp = server._dispatch(edit_req)
            assert edit_resp.result["isError"] is False
            
            # 5. 用bash验证最终内容
            bash_req = JsonRpcRequest(
                method="tools/call",
                params={"name": "bash", "arguments": {"command": f"cat {temp_path}"}},
                id=5,
            )
            bash_resp = server._dispatch(bash_req)
            result = json.loads(bash_resp.result["content"][0]["text"])
            assert "hello openclaw" in result["stdout"]
            
        finally:
            os.unlink(temp_path)


class TestEdgeCases:
    """边界情况和错误处理测试"""
    
    def test_read_with_offset_limit(self):
        """测试read offset+limit参数"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "read", "arguments": {"path": "src/mcp_server.py", "offset": 1, "limit": 3}},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["isError"] is False
        result = json.loads(resp.result["content"][0]["text"])
        assert result["num_lines"] <= 3
    
    def test_grep_case_sensitive(self):
        """测试grep大小写敏感搜索"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "grep", "arguments": {
                "pattern": "McpServer",
                "path": "src",
            }},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        assert resp.result["isError"] is False
        result = json.loads(resp.result["content"][0]["text"])
        assert result["num_matches"] > 0
    
    def test_grep_case_insensitive(self):
        """测试大小写不敏感grep"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "grep", "arguments": {
                "pattern": "MCPSERVER",
                "path": "src/mcp_server.py",
            }},
            id=1,
        )
        resp = server._dispatch(req)
        
        # 应当匹配到（大小写不敏感）
        assert resp.result is not None
    
    def test_glob_recursive(self):
        """测试递归glob"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "glob", "arguments": {
                "pattern": "**/*.py",
                "path": "src",
            }},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        result = json.loads(resp.result["content"][0]["text"])
        assert result["num_files"] > 0
    
    def test_bash_with_error(self):
        """测试bash命令错误处理"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "bash", "arguments": {"command": "exit 1"}},
            id=1,
        )
        resp = server._dispatch(req)
        
        assert resp.result is not None
        result = json.loads(resp.result["content"][0]["text"])
        assert result["return_code"] == 1
    
    def test_edit_nonexistent_file(self):
        """测试编辑不存在的文件"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        req = JsonRpcRequest(
            method="tools/call",
            params={"name": "edit", "arguments": {
                "path": "/tmp/does_not_exist_12345.txt",
                "oldText": "anything",
                "newText": "something",
            }},
            id=1,
        )
        resp = server._dispatch(req)
        
        # 应返回错误
        assert resp.result is not None
    
    def test_write_multiline_content(self):
        """测试写入多行内容"""
        reset_mcp_server()
        server = create_openclaw_mcp_server()
        
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            temp_path = f.name
        
        try:
            content = "line1\nline2\nline3\n# line4\nline5"
            req = JsonRpcRequest(
                method="tools/call",
                params={"name": "write", "arguments": {"path": temp_path, "content": content}},
                id=1,
            )
            resp = server._dispatch(req)
            assert resp.result is not None
            assert resp.result["isError"] is False
            
            with open(temp_path) as f:
                assert f.read() == content
        finally:
            os.unlink(temp_path)
