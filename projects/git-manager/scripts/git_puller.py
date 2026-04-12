#!/usr/bin/env python3
"""Git Puller - 幂等 Git 仓库拉取工具"""
import os, subprocess, logging, filelock, time, shutil
from pathlib import Path
from dataclasses import dataclass
from typing import Optional
from enum import Enum

logging.basicConfig(level=logging.INFO, format='%(asctime)s [%(levelname)s] %(message)s')
logger = logging.getLogger(__name__)

class AuthType(Enum):
    NONE = "none"
    PASSWORD = "password"
    TOKEN = "token"
    SSH_KEY = "ssh_key"

@dataclass
class GitPullerConfig:
    repo_url: str
    branch: str = "main"
    local_path: str = ""
    auth_type: AuthType = AuthType.NONE
    username: str = ""
    credentials: str = ""
    ssh_key_passphrase: str = ""
    depth: int = 1
    timeout: int = 300

class GitPuller:
    def __init__(self, config: GitPullerConfig):
        self.config = config
        self._lock_path = self._ensure_lock_dir()
        self._lock_file = os.path.join(self._lock_path, ".git_puller.lock")

    def _ensure_lock_dir(self) -> str:
        lock_dir = os.path.join(os.path.dirname(self.config.local_path) or ".", ".git_locks")
        os.makedirs(lock_dir, exist_ok=True)
        return lock_dir

    def pull(self) -> dict:
        start = time.time()
        lock = filelock.FileLock(self._lock_file, timeout=self.config.timeout)
        try:
            with lock:
                if self._is_repo_initialized():
                    return self._pull()
                else:
                    return self._clone()
        except filelock.Timeout:
            return {"success": False, "message": f"Lock timeout ({self.config.timeout}s)", "output": "", "duration": time.time() - start}
        except Exception as e:
            logger.exception("Git pull failed")
            return {"success": False, "message": f"Pull error: {str(e)}", "output": "", "duration": time.time() - start}

    def _is_repo_initialized(self) -> bool:
        return os.path.isdir(os.path.join(self.config.local_path, ".git"))

    def _build_env(self) -> dict:
        env = os.environ.copy()
        if self.config.auth_type == AuthType.SSH_KEY and self.config.credentials:
            env["GIT_SSH_COMMAND"] = f"ssh -i '{self.config.credentials}' -o StrictHostKeyChecking=no"
        return env

    def _get_authenticated_url(self) -> str:
        url = self.config.repo_url
        if self.config.auth_type == AuthType.PASSWORD and self.config.username:
            if '@' not in url.split('://', 1)[-1]:
                protocol = url.split('://', 1)[0] + '://'
                host_part = url.split('://', 1)[-1]
                url = f"{protocol}{self.config.username}:{self.config.credentials}@{host_part}"
        elif self.config.auth_type == AuthType.TOKEN and self.config.credentials:
            protocol = url.split('://', 1)[0] + '://'
            host_part = url.split('://', 1)[-1]
            if 'github.com' in host_part:
                url = f"{protocol}{self.config.credentials}@x-access-token:{host_part}"
            else:
                url = f"{protocol}{self.config.credentials}@{host_part}"
        return url

    def _run_git(self, args: list, cwd: Optional[str] = None) -> subprocess.CompletedProcess:
        env = self._build_env()
        return subprocess.run(
            ["git"] + args, cwd=cwd or self.config.local_path, env=env,
            capture_output=True, text=True, timeout=self.config.timeout
        )

    def _clone(self) -> dict:
        start = time.time()
        local_path = self.config.local_path
        if os.path.exists(local_path):
            backup_path = local_path + f".bak.{int(time.time())}"
            shutil.move(local_path, backup_path)
            logger.warning(f"Dir exists, backed up to {backup_path}")
        os.makedirs(local_path, exist_ok=True)
        auth_url = self._get_authenticated_url()
        cmd = ["clone"]
        if self.config.depth > 0:
            cmd += ["--depth", str(self.config.depth)]
        cmd += ["--branch", self.config.branch, "--single-branch", auth_url, local_path]
        logger.info(f"Executing: git {' '.join(cmd)}")
        result = self._run_git(cmd, cwd=local_path)
        if result.returncode == 0:
            logger.info(f"Clone success: {self.config.repo_url} -> {local_path}")
            return {"success": True, "message": "Clone success", "output": result.stdout, "duration": time.time() - start}
        else:
            logger.error(f"Clone failed: {result.stderr}")
            return {"success": False, "message": f"Clone failed: {result.stderr}", "output": result.stdout + result.stderr, "duration": time.time() - start}

    def _pull(self) -> dict:
        start = time.time()
        local_path = self.config.local_path
        result_fetch = self._run_git(["fetch", "origin", self.config.branch])
        if result_fetch.returncode != 0:
            return {"success": False, "message": f"Fetch failed: {result_fetch.stderr}", "output": "", "duration": time.time() - start}
        result_rev = self._run_git(["rev-parse", "HEAD"])
        result_origin_rev = self._run_git(["rev-parse", f"origin/{self.config.branch}"])
        if result_rev.returncode == 0 and result_origin_rev.returncode == 0:
            if result_rev.stdout.strip() == result_origin_rev.stdout.strip():
                logger.info("Already up to date")
                return {"success": True, "message": "Already up to date", "output": "Already up to date", "duration": time.time() - start}
        result = self._run_git(["pull", "origin", self.config.branch, "--no-edit"])
        if result.returncode == 0:
            logger.info(f"Pull success: {local_path}")
            return {"success": True, "message": "Pull success", "output": result.stdout, "duration": time.time() - start}
        else:
            logger.error(f"Pull failed: {result.stderr}")
            return {"success": False, "message": f"Pull failed: {result.stderr}", "output": result.stdout + result.stderr, "duration": time.time() - start}

    def get_status(self) -> dict:
        if not self._is_repo_initialized():
            return {"initialized": False, "message": "Repo not initialized"}
        result = self._run_git(["status", "-uno"])
        branch_result = self._run_git(["branch", "--show-current"])
        commit_result = self._run_git(["log", "-1", "--format=%H %s"])
        return {
            "initialized": True,
            "branch": branch_result.stdout.strip() if branch_result.returncode == 0 else "unknown",
            "clean": "nothing to commit" in result.stdout,
            "message": result.stdout.strip(),
            "last_commit": commit_result.stdout.strip() if commit_result.returncode == 0 else ""
        }

def main():
    import argparse, json, sys
    parser = argparse.ArgumentParser(description="Git Puller")
    sub = parser.add_subparsers(dest="cmd")
    p_pull = sub.add_parser("pull", help="Clone or pull")
    p_pull.add_argument("--repo", required=True)
    p_pull.add_argument("--branch", default="main")
    p_pull.add_argument("--path", required=True)
    p_pull.add_argument("--auth-type", choices=["none", "password", "token", "ssh_key"], default="none")
    p_pull.add_argument("--username", default="")
    p_pull.add_argument("--credentials", default="")
    p_pull.add_argument("--depth", type=int, default=1)
    p_status = sub.add_parser("status", help="Check status")
    p_status.add_argument("--path", required=True)
    args = parser.parse_args()
    if args.cmd == "pull":
        config = GitPullerConfig(
            repo_url=args.repo, branch=args.branch, local_path=args.path,
            auth_type=AuthType(args.auth_type), username=args.username,
            credentials=args.credentials, depth=args.depth
        )
        result = GitPuller(config).pull()
        print(json.dumps(result, ensure_ascii=False, indent=2))
        sys.exit(0 if result["success"] else 1)
    elif args.cmd == "status":
        config = GitPullerConfig(repo_url="", local_path=args.path)
        result = GitPuller(config).get_status()
        print(json.dumps(result, ensure_ascii=False, indent=2))
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
