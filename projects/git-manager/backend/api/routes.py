"""API 路由"""
import sys
import os
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from fastapi import APIRouter, HTTPException, Depends
from sqlalchemy.orm import Session
from typing import List
from datetime import datetime

from backend.models.database import init_db, Project, PullLog
from backend.schemas.schemas import (
    ProjectCreate, ProjectUpdate, ProjectResponse, ApiResponse,
    PullResult, ProjectListResponse, PullLogResponse
)
from backend.core import Encryptor

router = APIRouter()
_db_session = None

def get_db():
    global _db_session
    if _db_session is None:
        engine = init_db()
        from sqlalchemy.orm import sessionmaker
        SessionLocal = sessionmaker(bind=engine)
        _db_session = SessionLocal
    return _db_session()

@router.get("/projects", response_model=ProjectListResponse)
def list_projects(db: Session = Depends(get_db)):
    projects = db.query(Project).all()
    return ProjectListResponse(success=True, total=len(projects), data=projects)

@router.get("/projects/{project_id}", response_model=ApiResponse)
def get_project(project_id: int, db: Session = Depends(get_db)):
    project = db.query(Project).filter(Project.id == project_id).first()
    if not project:
        raise HTTPException(status_code=404, detail="项目不存在")
    return ApiResponse(success=True, message="获取成功", data=project)

@router.post("/projects", response_model=ApiResponse)
def create_project(project: ProjectCreate, db: Session = Depends(get_db)):
    existing = db.query(Project).filter(Project.name == project.name).first()
    if existing:
        raise HTTPException(status_code=400, detail="项目名称已存在")
    
    encrypted_password = Encryptor.encrypt(project.auth_password or "")
    db_project = Project(
        name=project.name,
        repo_url=project.repo_url,
        branch=project.branch,
        local_path=project.local_path,
        auth_type=project.auth_type.value,
        auth_username=project.auth_username or "",
        auth_password_encrypted=encrypted_password,
        auth_ssh_key_path=project.auth_ssh_key_path or "",
        rules=project.rules,
        plugins=project.plugins,
        is_active=project.is_active
    )
    db.add(db_project)
    db.commit()
    db.refresh(db_project)
    return ApiResponse(success=True, message="创建成功", data={"id": db_project.id})

@router.put("/projects/{project_id}", response_model=ApiResponse)
def update_project(project_id: int, update: ProjectUpdate, db: Session = Depends(get_db)):
    project = db.query(Project).filter(Project.id == project_id).first()
    if not project:
        raise HTTPException(status_code=404, detail="项目不存在")
    
    update_data = update.model_dump(exclude_unset=True)
    if "auth_password" in update_data and update_data["auth_password"]:
        update_data["auth_password_encrypted"] = Encryptor.encrypt(update_data.pop("auth_password"))
    
    for key, value in update_data.items():
        if hasattr(project, key):
            setattr(project, key, value)
    
    db.commit()
    return ApiResponse(success=True, message="更新成功")

@router.delete("/projects/{project_id}", response_model=ApiResponse)
def delete_project(project_id: int, db: Session = Depends(get_db)):
    project = db.query(Project).filter(Project.id == project_id).first()
    if not project:
        raise HTTPException(status_code=404, detail="项目不存在")
    db.delete(project)
    db.commit()
    return ApiResponse(success=True, message="删除成功")

@router.post("/projects/{project_id}/pull", response_model=ApiResponse)
def pull_project(project_id: int, db: Session = Depends(get_db)):
    from backend.core.puller import GitPuller
    from backend.models.database import AuthType
    
    project = db.query(Project).filter(Project.id == project_id).first()
    if not project:
        raise HTTPException(status_code=404, detail="项目不存在")
    
    project.status = "running"
    db.commit()
    
    auth_type = AuthType(project.auth_type)
    password = Encryptor.decrypt(project.auth_password_encrypted) if project.auth_password_encrypted else ""
    
    puller = GitPuller(
        repo_url=project.repo_url,
        local_path=project.local_path,
        branch=project.branch,
        auth_type=auth_type,
        username=project.auth_username,
        password=password,
        ssh_key_path=project.auth_ssh_key_path
    )
    
    result = puller.pull()
    
    log = PullLog(
        project_id=project_id,
        status="success" if result.success else "failed",
        message=result.message,
        commit_before=result.commit_before,
        commit_after=result.commit_after,
        diff_count=result.diff_count
    )
    db.add(log)
    
    project.last_pull_at = datetime.now()
    project.last_commit = result.commit_after
    project.status = "idle" if result.success else "error"
    db.commit()
    
    return ApiResponse(success=result.success, message=result.message, data=PullResult(
        success=result.success,
        message=result.message,
        repo_path=result.repo_path,
        changed=result.changed,
        commit_before=result.commit_before,
        commit_after=result.commit_after,
        diff_count=result.diff_count
    ))

@router.get("/projects/{project_id}/logs", response_model=ApiResponse)
def get_project_logs(project_id: int, limit: int = 20, db: Session = Depends(get_db)):
    logs = db.query(PullLog).filter(PullLog.project_id == project_id).order_by(
        PullLog.created_at.desc()
    ).limit(limit).all()
    return ApiResponse(success=True, message="获取成功", data=logs)
