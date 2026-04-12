"""Pydantic Schemas"""
from pydantic import BaseModel, Field
from typing import Optional, List, Dict, Any
from datetime import datetime
from enum import Enum

class AuthTypeEnum(str, Enum):
    NONE = "none"
    USER_PASS = "user_pass"
    TOKEN = "token"
    SSH_KEY = "ssh_key"

class ProjectBase(BaseModel):
    name: str = Field(..., min_length=1, max_length=255)
    repo_url: str = Field(..., min_length=1, max_length=1024)
    branch: str = Field(default="main", max_length=255)
    local_path: str = Field(..., min_length=1, max_length=1024)
    auth_type: AuthTypeEnum = Field(default=AuthTypeEnum.NONE)
    auth_username: Optional[str] = Field(default="")
    auth_password: Optional[str] = Field(default="", exclude=True)
    auth_ssh_key_path: Optional[str] = Field(default="")
    rules: Optional[Dict[str, Any]] = Field(default_factory=dict)
    plugins: Optional[List[str]] = Field(default_factory=list)
    is_active: bool = Field(default=True)

class ProjectCreate(ProjectBase):
    pass

class ProjectUpdate(BaseModel):
    name: Optional[str] = Field(None, min_length=1, max_length=255)
    repo_url: Optional[str] = Field(None, min_length=1, max_length=1024)
    branch: Optional[str] = Field(None, max_length=255)
    local_path: Optional[str] = Field(None, min_length=1, max_length=1024)
    auth_type: Optional[AuthTypeEnum] = None
    auth_username: Optional[str] = None
    auth_password: Optional[str] = None
    auth_ssh_key_path: Optional[str] = None
    rules: Optional[Dict[str, Any]] = None
    plugins: Optional[List[str]] = None
    is_active: Optional[bool] = None

class ProjectResponse(ProjectBase):
    id: int
    auth_password_encrypted: Optional[str] = ""
    last_pull_at: Optional[datetime] = None
    last_commit: Optional[str] = None
    status: str = "idle"
    created_at: datetime
    updated_at: datetime
    class Config:
        from_attributes = True

class PullLogResponse(BaseModel):
    id: int
    project_id: int
    status: str
    message: Optional[str]
    commit_before: Optional[str]
    commit_after: Optional[str]
    diff_count: int
    created_at: datetime
    class Config:
        from_attributes = True

class PullResult(BaseModel):
    success: bool
    message: str
    repo_path: str
    changed: bool
    commit_before: Optional[str] = None
    commit_after: Optional[str] = None
    diff_count: int = 0

class ApiResponse(BaseModel):
    success: bool
    message: str
    data: Optional[Any] = None

class ProjectListResponse(BaseModel):
    success: bool
    total: int
    data: List[ProjectResponse]
