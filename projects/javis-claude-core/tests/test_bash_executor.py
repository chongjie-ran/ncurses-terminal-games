"""
bash_executor.py 测试套件 - Bash命令执行模块

测试覆盖：
- BashCommandInput/BashCommandOutput 数据结构
- BashExecutor.execute() 同步执行
- execute_bash() 快捷函数
- BashExecutor.execute_async() 异步执行
- 超时处理
"""
import pytest
from src.bash_executor import (
    BashExecutor, BashCommandInput, BashCommandOutput,
    execute_bash, get_default_executor
)


class TestBashCommandInput:
    """BashCommandInput 测试"""

    def test_create_minimal(self):
        inp = BashCommandInput(command="echo hello")
        assert inp.command == "echo hello"
        assert inp.timeout is None
        assert inp.run_in_background is False

    def test_create_full(self):
        inp = BashCommandInput(
            command="ls -la",
            timeout=5000,
            description="List files",
            run_in_background=True,
            dangerously_disable_sandbox=True,
        )
        assert inp.timeout == 5000
        assert inp.description == "List files"
        assert inp.run_in_background is True


class TestBashCommandOutput:
    """BashCommandOutput 测试"""

    def test_success_property(self):
        out = BashCommandOutput(stdout="ok", stderr="")
        out.return_code = 0
        assert out.success is True

    def test_failure_property(self):
        out = BashCommandOutput(stdout="", stderr="error")
        out.return_code = 1
        assert out.success is False

    def test_interrupted_property(self):
        out = BashCommandOutput(stdout="", stderr="")
        out.interrupted = True
        out.return_code = None
        assert out.success is False
        assert out.interrupted is True

    def test_return_code_interpretation_zero(self):
        out = BashCommandOutput(stdout="ok", stderr="")
        out.return_code = 0
        assert out.return_code_interpretation is None

    def test_return_code_interpretation_nonzero(self):
        out = BashCommandOutput(stdout="", stderr="error")
        out.return_code = 1
        assert out.return_code_interpretation == "exit_code:1"

    def test_return_code_interpretation_none(self):
        out = BashCommandOutput(stdout="", stderr="")
        out.return_code = None
        assert out.return_code_interpretation is None

    def test_no_output_expected(self):
        out = BashCommandOutput(stdout="", stderr="")
        out.no_output_expected = True
        assert out.no_output_expected is True


class TestBashExecutor:
    """BashExecutor 测试"""

    def test_execute_simple(self):
        executor = BashExecutor()
        out = executor.execute(BashCommandInput(command="echo hello"))
        assert out.return_code == 0
        assert "hello" in out.stdout
        assert out.interrupted is False

    def test_execute_with_stderr(self):
        executor = BashExecutor()
        out = executor.execute(BashCommandInput(command="ls /nonexistent 2>&1"))
        assert out.return_code != 0

    def test_execute_nonzero_return(self):
        executor = BashExecutor()
        out = executor.execute(BashCommandInput(command="exit 1"))
        assert out.return_code == 1
        assert out.success is False

    def test_execute_multiline(self):
        executor = BashExecutor()
        out = executor.execute(BashCommandInput(command="echo line1; echo line2"))
        assert "line1" in out.stdout
        assert "line2" in out.stdout

    def test_execute_with_pipes(self):
        executor = BashExecutor()
        out = executor.execute(BashCommandInput(command="echo 'test123' | grep test"))
        assert out.return_code == 0
        assert "test123" in out.stdout

    def test_execute_background(self):
        executor = BashExecutor()
        out = executor.execute(BashCommandInput(command="sleep 10", run_in_background=True))
        assert out.background_task_id is not None
        assert out.assistant_auto_backgrounded is True


class TestExecuteBashFunction:
    """execute_bash 快捷函数测试"""

    def test_execute_bash_basic(self):
        out = execute_bash("echo hello")
        assert out.return_code == 0
        assert "hello" in out.stdout

    def test_execute_bash_with_timeout(self):
        out = execute_bash("echo test", timeout=5000)
        assert out.return_code == 0


class TestDefaultExecutor:
    """默认执行器测试"""

    def test_get_default_executor(self):
        executor = get_default_executor()
        assert isinstance(executor, BashExecutor)

    def test_singleton(self):
        e1 = get_default_executor()
        e2 = get_default_executor()
        assert e1 is e2


class TestBashExecutorAsync:
    """BashExecutor.execute_async 异步测试"""

    @pytest.mark.asyncio
    async def test_execute_async_success(self):
        """1. 正常异步执行成功（echo test）"""
        executor = BashExecutor()
        inp = BashCommandInput(command="echo hello async world")
        out = await executor.execute_async(inp)
        
        assert out.return_code == 0
        assert "hello async world" in out.stdout
        assert out.interrupted is False
        assert out.success is True

    @pytest.mark.asyncio
    async def test_execute_async_multiline(self):
        """1b. 多行异步执行"""
        executor = BashExecutor()
        inp = BashCommandInput(command="printf 'line1\\nline2\\nline3'")
        out = await executor.execute_async(inp)
        
        assert out.return_code == 0
        assert "line1" in out.stdout
        assert "line2" in out.stdout
        assert "line3" in out.stdout

    @pytest.mark.asyncio
    async def test_execute_async_timeout_killed(self):
        """2. 超时中断（sleep 100 被 SIGTERM kill）
        
        注意: Python 3.14+ 中 asyncio.TimeoutExpired 被移除，
        超时时会触发 asyncio.exceptions.TimeoutError。
        """
        executor = BashExecutor()
        inp = BashCommandInput(
            command="sleep 100",
            timeout=500  # 500ms超时
        )
        out = await executor.execute_async(inp)
        
        # 超时情况下 interrupted=True，return_code 为 None
        assert out.interrupted is True
        assert out.return_code is None
        # stderr 包含超时信息或异常信息
        assert len(out.stderr) > 0

    @pytest.mark.asyncio
    async def test_execute_async_timeout_short_command(self):
        """2b. 短命令不会触发超时"""
        executor = BashExecutor()
        inp = BashCommandInput(
            command="echo quick",
            timeout=10000  # 10s足够长
        )
        out = await executor.execute_async(inp)
        
        assert out.interrupted is False
        assert out.return_code == 0
        assert "quick" in out.stdout

    @pytest.mark.asyncio
    async def test_execute_async_command_not_found(self):
        """3. 异常处理（命令不存在，返回退出码127）"""
        executor = BashExecutor()
        inp = BashCommandInput(command="nonexistent_command_xyz_12345")
        out = await executor.execute_async(inp)
        
        # 命令不存在时返回退出码 127 (command not found)
        assert out.return_code == 127
        assert out.success is False
        assert "not found" in out.stderr

    @pytest.mark.asyncio
    async def test_execute_async_with_pipes(self):
        """1c. 异步执行带管道"""
        executor = BashExecutor()
        inp = BashCommandInput(command="echo 'async pipe test' | grep -o 'pipe'")
        out = await executor.execute_async(inp)
        
        assert out.return_code == 0
        assert "pipe" in out.stdout
        assert out.interrupted is False

    @pytest.mark.asyncio
    async def test_execute_async_nonzero_exit(self):
        """异常处理（非零退出码）"""
        executor = BashExecutor()
        inp = BashCommandInput(command="exit 42")
        out = await executor.execute_async(inp)
        
        assert out.return_code == 42
        assert out.success is False
        assert out.interrupted is False
        assert out.return_code_interpretation == "exit_code:42"

    @pytest.mark.asyncio
    async def test_execute_async_empty_command(self):
        """空命令处理"""
        executor = BashExecutor()
        inp = BashCommandInput(command="true")  # true 什么都不做但返回0
        out = await executor.execute_async(inp)
        
        assert out.return_code == 0
        assert out.success is True


class TestBashExecutorBoundary:
    """边界条件和异常处理测试 - 补充覆盖率"""

    def test_execute_timeout_subprocess_expired(self):
        """【覆盖行100-109】subprocess.TimeoutExpired 异常处理
        
        当 subprocess.run 超时时，触发 TimeoutExpired 分支。
        """
        import subprocess
        executor = BashExecutor()
        inp = BashCommandInput(
            command="sleep 10",  # 10秒睡眠
            timeout=1  # 1毫秒超时 - 几乎立即超时
        )
        out = executor.execute(inp)
        
        assert out.interrupted is True
        assert out.return_code is None
        assert out.return_code_interpretation == "timeout"
        assert "timeout" in out.stderr.lower()

    def test_execute_background_pid_returned(self):
        """【覆盖行133-134】后台任务返回正确的background_task_id
        
        验证 background_task_id 和 backgrounded_by_user 字段正确设置。
        """
        executor = BashExecutor()
        inp = BashCommandInput(command="echo test", run_in_background=True)
        out = executor.execute(inp)
        
        # 验证后台任务ID是有效的非空字符串
        assert out.background_task_id is not None
        assert isinstance(out.background_task_id, str)
        assert len(out.background_task_id) > 0
        # 验证是 assistant 自动后台化
        assert out.assistant_auto_backgrounded is True
        assert out.backgrounded_by_user is False
        # 验证没有输出（后台任务不等待输出）
        assert out.stdout == ""
        assert out.stderr == ""
        assert out.no_output_expected is True

    def test_execute_background_exception(self):
        """【覆盖行147】_execute_background 异常处理分支
        
        当 subprocess.Popen 抛出异常时，验证正确处理。
        """
        import unittest.mock as mock
        executor = BashExecutor()
        
        # Mock Popen 使其抛出异常
        with mock.patch('subprocess.Popen') as mock_popen:
            mock_popen.side_effect = OSError("Cannot create process")
            inp = BashCommandInput(command="anything", run_in_background=True)
            out = executor.execute(inp)
        
        # 验证异常被正确捕获和处理
        assert out.interrupted is True
        assert "Cannot create process" in out.stderr

    @pytest.mark.asyncio
    async def test_execute_async_background_exception(self):
        """【覆盖行147异步版】execute_async 后台执行异常处理
        
        当 _execute_background 抛出异常时（subprocess.Popen 失败），验证正确处理。
        注意: run_in_background=True 时，execute_async 调用 _execute_background，
        该方法使用 subprocess.Popen 而非 asyncio.create_subprocess_exec。
        """
        import unittest.mock as mock
        executor = BashExecutor()
        
        # _execute_background 使用 subprocess.Popen，所以 mock 它
        with mock.patch('subprocess.Popen') as mock_popen:
            mock_popen.side_effect = OSError("Cannot create async process")
            inp = BashCommandInput(command="anything", run_in_background=True)
            out = await executor.execute_async(inp)
        
        # 验证异常被正确捕获
        assert out.interrupted is True
        assert "Cannot create async process" in out.stderr

    @pytest.mark.asyncio
    async def test_execute_async_asyncio_timeout_expired(self):
        """【覆盖行173-175】asyncio.TimeoutExpired 异常处理
        
        注意: Python 3.14+ 中 asyncio.wait_for 抛出 TimeoutError 而非 asyncio.TimeoutExpired。
        此测试验证代码对 TimeoutError 的处理（fallback 到 except Exception）。
        """
        import asyncio
        executor = BashExecutor()
        
        # 短超时长命令，Python 3.14+ 会抛出 TimeoutError 而非 asyncio.TimeoutExpired
        inp = BashCommandInput(
            command="sleep 10",
            timeout=1  # 1ms超时
        )
        out = await executor.execute_async(inp)
        
        # 在 Python 3.14+ 中走的是 except Exception 分支
        # 在 Python 3.13- 中走的是 except asyncio.TimeoutExpired 分支
        # 两者都应设置 interrupted=True
        assert out.interrupted is True
        assert out.return_code is None
