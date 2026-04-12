"""Git Puller for Backend"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent.parent.parent / "scripts"))

from backend.core import Encryptor

class AuthType:
    NONE = "none"
    USER_PASS = "user_pass"
    TOKEN = "token"
    SSH_KEY = "ssh_key"
