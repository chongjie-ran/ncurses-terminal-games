#!/usr/bin/env python3
"""Deploy P2-6 overlay files into zcloud-bot checkout and patch tests."""
from __future__ import annotations

import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DEST = Path("/Users/chongjieran/SWproject/zCloud-bot/zcloud-bot")

COPIES: list[tuple[str, str]] = [
    ("aiops_v4/launch.py", "aiops_v4/launch.py"),
    ("aiops_v4/http/app.py", "aiops_v4/http/app.py"),
    ("run/app/run_app.py", "run/app/run_app.py"),
]

TEST_REPLACEMENTS = [
    (
        'monkeypatch.setattr("run.app.run_app._init_v3_wrappers",',
        'monkeypatch.setattr("run.app.run_app.init_legacy_tool_wrappers",',
    ),
    (
        'monkeypatch.setattr("aiops_v4.http.app._init_v3_wrappers",',
        'monkeypatch.setattr("aiops_v4.http.app.init_legacy_tool_wrappers",',
    ),
]


def main() -> int:
    if not DEST.is_dir():
        print(f"DEST missing: {DEST}", file=sys.stderr)
        return 1
    tests_dir = DEST / "aiops_v4/tests"
    if tests_dir.is_dir():
        names = sorted(p.name for p in tests_dir.iterdir() if p.name.startswith("test_") and p.suffix == ".py")
        print("P2-3 aiops_v4/tests (test_*.py):", len(names), "files")
        for n in names[:20]:
            print(" ", n)
        if len(names) > 20:
            print("  ...")
    for src_rel, dst_rel in COPIES:
        src = ROOT / src_rel
        dst = DEST / dst_rel
        if not src.is_file():
            print(f"missing overlay: {src}", file=sys.stderr)
            return 1
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dst)
        print(f"copied {src_rel} -> {dst}")

    test_path = DEST / "aiops_v4/tests/test_handler_smoke.py"
    text = test_path.read_text(encoding="utf-8")
    orig = text
    for a, b in TEST_REPLACEMENTS:
        if a not in text:
            print(f"warning: pattern not found in test file: {a!r}", file=sys.stderr)
        text = text.replace(a, b)
    if text != orig:
        test_path.write_text(text, encoding="utf-8")
        print(f"patched {test_path}")
    else:
        print("test file unchanged (patterns already applied?)")

    # Verification
    r = subprocess.run(
        [
            "bash",
            "-lc",
            "cd /Users/chongjieran/SWproject/zCloud-bot/zcloud-bot && "
            "source .venv/bin/activate && "
            "pytest aiops_v4/tests/ -v --tb=short",
        ],
        cwd=str(DEST),
    )
    return r.returncode


if __name__ == "__main__":
    raise SystemExit(main())
