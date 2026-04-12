"""
file_ops.py - 文件操作模块

对应Claude Code Rust: file_ops.rs
核心功能：
- read_file: 读取文件(支持offset/limit)
- write_file: 写入文件
- edit_file: 编辑文件
- grep_search: 搜索
- glob_search: 文件匹配
"""

from __future__ import annotations
from dataclasses import dataclass, field
from typing import Optional, List, Dict, Any
import os
import re
import glob as glob_module


@dataclass
class TextFilePayload:
    """文本文件载荷"""
    file_path: str
    content: str
    num_lines: int
    start_line: int
    total_lines: int


@dataclass
class ReadResult:
    """读取结果"""
    file: TextFilePayload
    type: str = "text"
    
    @classmethod
    def create(cls, path: str, content: str, offset: int = 0, limit: Optional[int] = None):
        """创建读取结果"""
        lines = content.split("\n")
        total = len(lines)
        start = min(offset, total)
        end = limit if limit else total
        selected_lines = lines[start:start+end] if limit else lines[start:]
        
        return cls(
            file=TextFilePayload(
                file_path=os.path.abspath(path),
                content="\n".join(selected_lines),
                num_lines=len(selected_lines),
                start_line=start,
                total_lines=total,
            )
        )


@dataclass
class StructuredPatchHunk:
    """补丁块"""
    old_start: int
    old_lines: int
    new_start: int
    new_lines: int
    lines: List[str]


@dataclass
class WriteResult:
    """写入结果"""
    file_path: str
    content: str
    type: str = "write"
    structured_patch: List[StructuredPatchHunk] = field(default_factory=list)
    original_file: Optional[str] = None


@dataclass
class EditResult:
    """编辑结果"""
    file_path: str
    old_string: str
    new_string: str
    original_file: str
    structured_patch: List[StructuredPatchHunk] = field(default_factory=list)
    user_modified: bool = False
    replace_all: bool = False


@dataclass
class GrepResult:
    """搜索结果"""
    num_files: int = 0
    filenames: List[str] = field(default_factory=list)
    mode: Optional[str] = None
    content: Optional[str] = None
    num_lines: Optional[int] = None
    num_matches: Optional[int] = None


@dataclass
class GlobResult:
    """glob搜索结果"""
    duration_ms: int = 0
    num_files: int = 0
    filenames: List[str] = field(default_factory=list)
    truncated: bool = False


class FileOps:
    """文件操作类"""
    
    @staticmethod
    def read_file(path: str, offset: Optional[int] = None, 
                  limit: Optional[int] = None) -> ReadResult:
        """读取文件"""
        abs_path = os.path.abspath(path)
        
        if not os.path.exists(abs_path):
            raise FileNotFoundError(f"File not found: {abs_path}")
        
        with open(abs_path, "r", encoding="utf-8", errors="replace") as f:
            content = f.read()
        
        return ReadResult.create(abs_path, content, offset or 0, limit)
    
    @staticmethod
    def write_file(path: str, content: str) -> WriteResult:
        """写入文件"""
        abs_path = os.path.abspath(path)
        
        original = None
        if os.path.exists(abs_path):
            with open(abs_path, "r", encoding="utf-8") as f:
                original = f.read()
        
        os.makedirs(os.path.dirname(abs_path), exist_ok=True)
        with open(abs_path, "w", encoding="utf-8") as f:
            f.write(content)
        
        return WriteResult(
            file_path=abs_path,
            content=content,
            original_file=original,
        )
    
    @staticmethod
    def edit_file(path: str, old_string: str, new_string: str,
                 replace_all: bool = False) -> EditResult:
        """编辑文件"""
        abs_path = os.path.abspath(path)
        
        with open(abs_path, "r", encoding="utf-8") as f:
            original = f.read()
        
        if replace_all:
            new_content = original.replace(old_string, new_string)
        else:
            new_content = original.replace(old_string, new_string, 1)
        
        if new_content == original:
            raise ValueError(f"String not found: {old_string}")
        
        with open(abs_path, "w", encoding="utf-8") as f:
            f.write(new_content)
        
        return EditResult(
            file_path=abs_path,
            old_string=old_string,
            new_string=new_string,
            original_file=original,
            replace_all=replace_all,
        )
    
    @staticmethod
    def grep_search(pattern: str, path: Optional[str] = None,
                   case_insensitive: bool = False,
                   line_numbers: bool = True) -> GrepResult:
        """搜索文件内容"""
        flags = re.IGNORECASE if case_insensitive else 0
        regex = re.compile(pattern, flags)
        
        filenames = []
        matches = []
        search_path = path or "."
        
        for root, dirs, files in os.walk(search_path):
            dirs[:] = [d for d in dirs if not d.startswith('.') and d not in ['node_modules', '__pycache__']]
            for file in files:
                if file.startswith('.'):
                    continue
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, "r", encoding="utf-8", errors="replace") as f:
                        for i, line in enumerate(f):
                            if regex.search(line):
                                filenames.append(file_path)
                                prefix = f"{i+1}: " if line_numbers else ""
                                matches.append(f"{prefix}{line.rstrip()}")
                except Exception:
                    pass
        
        return GrepResult(
            num_files=len(set(filenames)),
            filenames=list(set(filenames)),
            content="\n".join(matches),
            num_lines=len(matches),
            num_matches=len(matches),
        )
    
    @staticmethod
    def glob_search(pattern: str, path: Optional[str] = None) -> GlobResult:
        """Glob文件匹配"""
        import time
        start = time.time()
        
        search_path = path or "."
        full_pattern = os.path.join(search_path, "**", pattern)
        
        matches = []
        for match in glob_module.iglob(full_pattern, recursive=True):
            if os.path.isfile(match):
                matches.append(match)
        
        return GlobResult(
            duration_ms=int((time.time() - start) * 1000),
            num_files=len(matches),
            filenames=matches,
        )
