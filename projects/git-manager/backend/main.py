#!/usr/bin/env python3
"""Git Project Manager - FastAPI Backend"""
import os, sys, json, sqlite3, logging, secrets
from pathlib import Path
from datetime import datetime
from typing import Optional, List

import uvicorn
from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel, Field

BASE_DIR = Path(__file__).parent.parent
DATA_DIR = BASE_DIR / "data"
DATA_DIR.mkdir(exist_ok=True)
DB_PATH = DATA_DIR / "git_manager.db"

logging.basicConfig(level=logging.INFO, format='%(asctime)s [%(levelname)s] %(message)s')
logger = logging.getLogger(__name__)

def get_db():
    conn = sqlite3.connect(str(DB_PATH), check_same_thread=False)
    conn.row_factory = sqlite3.Row
    return conn

def init_db():
    conn = get_db()
    conn.execute("""CREATE TABLE IF NOT EXISTS projects (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL, repo_url TEXT NOT NULL, branch TEXT DEFAULT 'main',
        local_path TEXT NOT NULL UNIQUE, auth_type TEXT DEFAULT 'none',
        credentials TEXT DEFAULT '', rules TEXT DEFAULT '[]', plugins TEXT DEFAULT '[]',
        status TEXT DEFAULT 'idle', last_pull TEXT DEFAULT '',
        created_at TEXT DEFAULT CURRENT_TIMESTAMP, updated_at TEXT DEFAULT CURRENT_TIMESTAMP
    )""")
    conn.commit()
    conn.close()

def _get_cipher_key() -> bytes:
    key_file = DATA_DIR / ".key"
    if key_file.exists():
        return key_file.read_bytes()[:32]
    key = secrets.token_bytes(32)
    key_file.write_bytes(key)
    os.chmod(str(key_file), 0o600)
    return key

def encrypt(text: str) -> str:
    if not text: return ""
    import base64
    key = _get_cipher_key()
    enc = bytearray(c ^ key[i % len(key)] for i, c in enumerate(text.encode()))
    return base64.b64encode(bytes(enc)).decode()

def decrypt(encoded: str) -> str:
    if not encoded: return ""
    import base64
    key = _get_cipher_key()
    data = base64.b64decode(encoded.encode())
    return "".join(chr(data[i] ^ key[i % len(key)]) for i in range(len(data)))

class ProjectCreate(BaseModel):
    name: str = Field(..., min_length=1, max_length=100)
    repo_url: str = Field(..., min_length=1)
    branch: str = Field(default="main")
    local_path: str = Field(..., min_length=1)
    auth_type: str = Field(default="none")
    credentials: str = Field(default="")
    rules: List[dict] = Field(default_factory=list)
    plugins: List[dict] = Field(default_factory=list)

class ProjectUpdate(BaseModel):
    name: Optional[str] = None
    repo_url: Optional[str] = None
    branch: Optional[str] = None
    local_path: Optional[str] = None
    auth_type: Optional[str] = None
    credentials: Optional[str] = None
    rules: Optional[List[dict]] = None
    plugins: Optional[List[dict]] = None

class PullRequest(BaseModel):
    force: bool = Field(default=False)

app = FastAPI(title="Git Project Manager", version="1.0.0")
app.add_middleware(CORSMiddleware, allow_origins=["*"], allow_credentials=True, allow_methods=["*"], allow_headers=["*"])

def row_to_project(row: sqlite3.Row) -> dict:
    d = dict(row)
    if d.get("credentials"): d["credentials"] = decrypt(d["credentials"])
    for f in ("rules", "plugins"):
        raw = d.get(f, "[]")
        d[f] = json.loads(raw) if isinstance(raw, str) else raw
    return d

@app.on_event("startup")
def startup():
    init_db()
    logger.info(f"DB ready: {DB_PATH}")

@app.get("/projects")
def list_projects():
    conn = get_db()
    rows = conn.execute("SELECT * FROM projects ORDER BY id DESC").fetchall()
    conn.close()
    return [row_to_project(row) for row in rows]

@app.post("/projects")
def create_project(p: ProjectCreate):
    conn = get_db()
    if conn.execute("SELECT id FROM projects WHERE local_path = ?", (p.local_path,)).fetchone():
        conn.close()
        raise HTTPException(409, "Path already exists")
    now = datetime.now().isoformat()
    cur = conn.execute(
        "INSERT INTO projects (name,repo_url,branch,local_path,auth_type,credentials,rules,plugins,created_at,updated_at) VALUES (?,?,?,?,?,?,?,?,?,?)",
        (p.name, p.repo_url, p.branch, p.local_path, p.auth_type, encrypt(p.credentials),
         json.dumps(p.rules), json.dumps(p.plugins), now, now)
    )
    conn.commit()
    new_id = cur.lastrowid
    conn.close()
    logger.info(f"Created project {p.name} id={new_id}")
    return {"id": new_id, "message": "Created"}

@app.get("/projects/{pid}")
def get_project(pid: int):
    conn = get_db()
    row = conn.execute("SELECT * FROM projects WHERE id = ?", (pid,)).fetchone()
    conn.close()
    if not row: raise HTTPException(404, "Not found")
    return row_to_project(row)

@app.put("/projects/{pid}")
def update_project(pid: int, p: ProjectUpdate):
    conn = get_db()
    if not conn.execute("SELECT id FROM projects WHERE id = ?", (pid,)).fetchone():
        conn.close()
        raise HTTPException(404, "Not found")
    updates, values = [], []
    for f, v in p.model_dump(exclude_unset=True).items():
        if f == "credentials": v = encrypt(v)
        if f in ("rules", "plugins"): v = json.dumps(v)
        updates.append(f + " = ?")
        values.append(v)
    if updates:
        updates.append("updated_at = ?")
        values.append(datetime.now().isoformat())
        values.append(pid)
        conn.execute(f"UPDATE projects SET {','.join(updates)} WHERE id = ?", values)
        conn.commit()
    conn.close()
    return {"message": "Updated"}

@app.delete("/projects/{pid}")
def delete_project(pid: int):
    conn = get_db()
    if not conn.execute("SELECT id FROM projects WHERE id = ?", (pid,)).fetchone():
        conn.close()
        raise HTTPException(404, "Not found")
    conn.execute("DELETE FROM projects WHERE id = ?", (pid,))
    conn.commit()
    conn.close()
    return {"message": "Deleted"}

@app.post("/projects/{pid}/pull")
def pull_project(pid: int, req: PullRequest = PullRequest()):
    conn = get_db()
    row = conn.execute("SELECT * FROM projects WHERE id = ?", (pid,)).fetchone()
    if not row:
        conn.close()
        raise HTTPException(404, "Not found")
    conn.close()
    proj = row_to_project(row)
    sys.path.insert(0, str(BASE_DIR / "scripts"))
    from git_puller import GitPuller, GitPullerConfig, AuthType
    auth_map = {"none": AuthType.NONE, "password": AuthType.PASSWORD, "token": AuthType.TOKEN, "ssh_key": AuthType.SSH_KEY}
    cfg = GitPullerConfig(repo_url=proj["repo_url"], branch=proj["branch"], local_path=proj["local_path"],
                          auth_type=auth_map.get(proj["auth_type"], AuthType.NONE), credentials=proj.get("credentials",""))
    import time
    start = time.time()
    conn2 = get_db()
    conn2.execute("UPDATE projects SET status='running' WHERE id=?", (pid,))
    conn2.commit()
    conn2.close()
    result = GitPuller(cfg).pull()
    new_status = "success" if result["success"] else "failed"
    now = datetime.now().isoformat()
    conn3 = get_db()
    conn3.execute("UPDATE projects SET status=?,last_pull=?,updated_at=? WHERE id=?", (new_status, now, now, pid))
    conn3.commit()
    conn3.close()
    return {"project_id": pid, "status": new_status, "message": result["message"], "duration": round(time.time()-start, 2)}

@app.get("/projects/{pid}/status")
def get_project_status(pid: int):
    conn = get_db()
    row = conn.execute("SELECT local_path,status,last_pull FROM projects WHERE id=?", (pid,)).fetchone()
    conn.close()
    if not row: raise HTTPException(404, "Not found")
    sys.path.insert(0, str(BASE_DIR / "scripts"))
    from git_puller import GitPuller, GitPullerConfig
    git_status = GitPuller(GitPullerConfig(repo_url="", local_path=row["local_path"])).get_status()
    return {"db_status": row["status"], "last_pull": row["last_pull"], "git_status": git_status}

@app.get("/health")
def health():
    return {"status": "ok", "db": str(DB_PATH)}

def main():
    import argparse
    a = argparse.ArgumentParser()
    a.add_argument("--host", default="0.0.0.0")
    a.add_argument("--port", type=int, default=8000)
    a.add_argument("--reload", action="store_true")
    args = a.parse_args()
    logger.info(f"Starting http://{args.host}:{args.port}")
    uvicorn.run("main:app", host=args.host, port=args.port, reload=args.reload)

if __name__ == "__main__":
    main()
