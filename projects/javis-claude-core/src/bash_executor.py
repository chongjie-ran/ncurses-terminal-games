"""
bash_executor.py - Bash命令执行模块

对应Claude Code Rust: bash.rs
核心功能：
- BashCommandInput/Output数据结构
- execute_bash() 同步执行
- 超时控制
- 后台任务支持
"""

from __future__ import annotations
from dataclasses import dataclass, field
from typing import Optional, List
import subprocess
import asyncio
import shlex


@dataclass
class BashCommandInput:
    """Bash命令输入"""
    command: str
    timeout: Optional[int] = None  # 毫秒
    description: Optional[str] = None
    run_in_background: bool = False
    dangerously_disable_sandbox: bool = False


@dataclass
class BashCommandOutput:
    """Bash命令输出"""
    stdout: str
    stderr: str
    interrupted: bool = False
    return_code: Optional[int] = None
    raw_output_path: Optional[str] = None
    background_task_id: Optional[str] = None
    backgrounded_by_user: bool = False
    assistant_auto_backgrounded: bool = False
    is_image: bool = False
    no_output_expected: bool = False
    
    @property
    def return_code_interpretation(self) -> Optional[str]:
        """返回码解释"""
        if self.return_code is None:
            if self.interrupted:
                return "timeout"
            return None
        if self.return_code == 0:
            return None
        return f"exit_code:{self.return_code}"
    
    @property
    def success(self) -> bool:
        """是否成功"""
        return self.return_code == 0 and not self.interrupted


class BashExecutor:
    """
    Bash命令执行器
    
    对应Rust: execute_bash()
    """
    
    def __init__(self, default_timeout: int = 60000):
        """
        初始化
        
        Args:
            default_timeout: 默认超时(毫秒)，默认60秒
        """
        self.default_timeout = default_timeout / 1000  # 转为秒
    
    def execute(self, input: BashCommandInput) -> BashCommandOutput:
        """
        同步执行Bash命令
        
        对应Rust: execute_bash(input: BashCommandInput) -> BashCommandOutput
        """
        if input.run_in_background:
            return self._execute_background(input)
        
        timeout = input.timeout / 1000 if input.timeout else self.default_timeout
        
        try:
            result = subprocess.run(
                ["sh", "-lc", input.command],
                capture_output=True,
                text=True,
                timeout=timeout,
            )
            return BashCommandOutput(
                stdout=result.stdout,
                stderr=result.stderr,
                return_code=result.returncode,
                interrupted=False,
                no_output_expected=not result.stdout and not result.stderr,
            )
        except subprocess.TimeoutExpired:
            return BashCommandOutput(
                stdout="",
                stderr=f"Command exceeded timeout of {input.timeout} ms",
                return_code=None,
                interrupted=True,

            )
        except Exception as e:
            return BashCommandOutput(
                stdout="",
                stderr=str(e),
                return_code=None,
                interrupted=True,
            )
    
    def _execute_background(self, input: BashCommandInput) -> BashCommandOutput:
        """后台执行"""
        try:
            proc = subprocess.Popen(
                ["sh", "-lc", input.command],
                stdin=subprocess.DEVNULL,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )
            return BashCommandOutput(
                stdout="",
                stderr="",
                background_task_id=str(proc.pid),
                backgrounded_by_user=False,
                assistant_auto_backgrounded=True,
                no_output_expected=True,
            )
        except Exception as e:
            return BashCommandOutput(
                stdout="",
                stderr=str(e),
                interrupted=True,
            )
    
    async def execute_async(self, input: BashCommandInput) -> BashCommandOutput:
        """
        异步执行Bash命令
        
        对应Rust: execute_bash_async()
        """
        if input.run_in_background:
            return self._execute_background(input)
        
        timeout = input.timeout / 1000 if input.timeout else self.default_timeout
        
        try:
            proc = await asyncio.create_subprocess_exec(
                "sh", "-lc", input.command,
                stdout=asyncio.subprocess.PIPE,
                stderr=asyncio.subprocess.PIPE,
            )
            
            try:
                stdout, stderr = await asyncio.wait_for(
                    proc.communicate(),
                    timeout=timeout
                )
                stdout_str = stdout.decode() if stdout else ""
                stderr_str = stderr.decode() if stderr else ""
                return BashCommandOutput(
                    stdout=stdout_str,
                    stderr=stderr_str,
                    return_code=proc.returncode,
                    interrupted=False,
                    no_output_expected=not stdout_str and not stderr_str,
                )
            except asyncio.TimeoutExpired:
                proc.kill()
                await proc.wait()
                return BashCommandOutput(
                    stdout="",
                    stderr=f"Command exceeded timeout of {input.timeout} ms",
                    return_code=None,
                    interrupted=True,
    
                )
        except Exception as e:
            return BashCommandOutput(
                stdout="",
                stderr=str(e),
                return_code=None,
                interrupted=True,
            )


# 全局执行器实例
_default_executor: Optional[BashExecutor] = None

def get_default_executor() -> BashExecutor:
    """获取默认执行器"""
    global _default_executor
    if _default_executor is None:
        _default_executor = BashExecutor()
    return _default_executor


def execute_bash(command: str, timeout: Optional[int] = None,
                description: Optional[str] = None) -> BashCommandOutput:
    """
    快捷函数：执行Bash命令
    
    对应Rust: execute_bash()
    """
    input = BashCommandInput(
        command=command,
        timeout=timeout,
        description=description,
    )
    return get_default_executor().execute(input)
