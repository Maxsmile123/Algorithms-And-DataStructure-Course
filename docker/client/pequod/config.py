from pathlib import Path

# /path/to/{REPO_NAME}/docker/client/pequod/config.py
THIS_MODULE_PATH = Path(__file__).resolve()

HOST_REPO_DIR = THIS_MODULE_PATH.parent.parent.parent.parent
REPO_NAME = HOST_REPO_DIR.name

HOST_WORKSPACE_DIR = HOST_REPO_DIR.parent
HOST_DOCKER_DIR = HOST_REPO_DIR / "docker"
HOST_DOCKER_COMPOSE_PATH = HOST_DOCKER_DIR / "docker-compose.yml"

CONTAINER_NAME = REPO_NAME

CONTAINER_WORKSPACE_DIR = Path("/workspace")
CONTAINER_REPO_DIR = CONTAINER_WORKSPACE_DIR / REPO_NAME
CONTAINER_BASHRC_PATH = CONTAINER_REPO_DIR / "docker" / "config" / "bashrc"

