#!/usr/bin/env python3
"""
game_stats_collector.py
自动收集项目代码统计信息，生成 Markdown 报告
练习时间: 2026-03-30 22:30
"""

import os
import re
from pathlib import Path
from datetime import datetime

WORKSPACE = Path.home() / ".openclaw" / "workspace-developer" / "projects"
OUTPUT = Path.home() / ".openclaw" / "workspace-developer" / "memory" / "code-stats-20260330.md"

# 支持统计的代码后缀
CODE_EXTENSIONS = {
    ".py": "Python",
    ".js": "JavaScript",
    ".ts": "TypeScript",
    ".c": "C",
    ".cpp": "C++",
    ".h": "C Header",
    ".hpp": "C++ Header",
    ".rs": "Rust",
    ".go": "Go",
    ".java": "Java",
    ".cs": "C#",
    ".rb": "Ruby",
    ".swift": "Swift",
    ".kt": "Kotlin",
    ".sh": "Shell",
    ".lua": "Lua",
}

# 忽略的目录
IGNORE_DIRS = {".git", "__pycache__", "node_modules", ".venv", "venv", "build", "dist"}


def count_lines(file_path: Path) -> int:
    """统计文件行数（排除空行和纯注释行）"""
    try:
        with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
            lines = f.readlines()
        code_lines = 0
        for line in lines:
            stripped = line.strip()
            # 跳过空行和纯注释行
            if stripped and not stripped.startswith("//") and not stripped.startswith("#") and not stripped.startswith("/*") and stripped != "*/":
                code_lines += 1
        return code_lines
    except Exception:
        return 0


def scan_project(project_path: Path) -> dict:
    """扫描单个项目，返回统计信息"""
    stats = {
        "total_lines": 0,
        "total_files": 0,
        "by_language": {},
    }
    
    for root, dirs, files in os.walk(project_path):
        # 过滤忽略目录（原地修改避免遍历）
        dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
        
        for file in files:
            ext = Path(file).suffix.lower()
            if ext in CODE_EXTENSIONS or ext in [".h", ".hpp"]:
                full_path = Path(root) / file
                lines = count_lines(full_path)
                lang = CODE_EXTENSIONS.get(ext, ext)
                stats["total_lines"] += lines
                stats["total_files"] += 1
                stats["by_language"][lang] = stats["by_language"].get(lang, 0) + lines
    
    return stats


def main():
    print(f"[{datetime.now().strftime('%H:%M:%S')}] 开始扫描项目目录...")
    
    projects = sorted([p for p in WORKSPACE.iterdir() if p.is_dir()])
    
    all_stats = []
    grand_total_lines = 0
    grand_total_files = 0
    
    for project in projects:
        stats = scan_project(project)
        stats["name"] = project.name
        all_stats.append(stats)
        grand_total_lines += stats["total_lines"]
        grand_total_files += stats["total_files"]
        print(f"  ✓ {project.name}: {stats['total_lines']} 行, {stats['total_files']} 文件")
    
    # 生成 Markdown 报告
    report_lines = [
        "# 游戏项目代码统计报告",
        "",
        f"> 生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M')} | 扫描路径: `{WORKSPACE}`",
        "",
        "## 概览",
        "",
        f"| 指标 | 值 |",
        f"|------|-----|",
        f"| 项目总数 | {len(all_stats)} |",
        f"| 总代码行数 | {grand_total_lines} |",
        f"| 总文件数 | {grand_total_files} |",
        f"| 平均项目规模 | {grand_total_lines // len(all_stats)} 行/项目 |",
        "",
        "## 项目详情",
        "",
        f"| 项目 | 语言分布 | 总行数 | 文件数 |",
        f"|------|----------|-------|--------|",
    ]
    
    # 按代码行数排序
    all_stats.sort(key=lambda x: x["total_lines"], reverse=True)
    
    for s in all_stats:
        lang_str = ", ".join([f"{lang}({lines})" for lang, lines in sorted(s["by_language"].items(), key=lambda x: -x[1])])
        report_lines.append(f"| {s['name']} | {lang_str} | {s['total_lines']} | {s['total_files']} |")
    
    # Top 语言统计
    lang_totals = {}
    for s in all_stats:
        for lang, lines in s["by_language"].items():
            lang_totals[lang] = lang_totals.get(lang, 0) + lines
    
    report_lines += [
        "",
        "## 语言分布（按代码行数）",
        "",
        f"| 语言 | 代码行数 | 占比 |",
        f"|------|---------|------|",
    ]
    for lang, lines in sorted(lang_totals.items(), key=lambda x: -x[1]):
        pct = lines * 100 // grand_total_lines
        report_lines.append(f"| {lang} | {lines} | {pct}% |")
    
    report_lines += [
        "",
        "## 规模分布",
        "",
        f"| 规模区间 | 项目数 |",
        f"|----------|--------|",
    ]
    
    buckets = {"< 100行": 0, "100-500行": 0, "500-1000行": 0, "1000-2000行": 0, "> 2000行": 0}
    for s in all_stats:
        t = s["total_lines"]
        if t < 100:
            buckets["< 100行"] += 1
        elif t < 500:
            buckets["100-500行"] += 1
        elif t < 1000:
            buckets["500-1000行"] += 1
        elif t < 2000:
            buckets["1000-2000行"] += 1
        else:
            buckets["> 2000行"] += 1
    
    for bucket, count in buckets.items():
        report_lines.append(f"| {bucket} | {count} |")
    
    report_text = "\n".join(report_lines)
    
    # 写入报告
    OUTPUT.write_text(report_text, encoding="utf-8")
    print(f"\n✅ 报告已生成: {OUTPUT}")
    print(f"   总计: {grand_total_lines} 行代码, {grand_total_files} 个文件, {len(all_stats)} 个项目")


if __name__ == "__main__":
    main()
