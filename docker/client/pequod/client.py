import os

from typing import List

from pequod import helpers
from pequod.logging import log
from pequod.config import (
    REPO_NAME,
    CONTAINER_NAME,
    HOST_WORKSPACE_DIR,
    HOST_DOCKER_COMPOSE_PATH,
    CONTAINER_WORKSPACE_DIR,
    CONTAINER_BASHRC_PATH,
)


class Client:
    """Orchestrator for a single Docker container.

    Usage:
        Client().login()  # or one of the public methods below.
    """

    @helpers.ensure_not_running_as_root
    def login(self) -> None:
        self._bash(user=f"{helpers.user_id()}:{helpers.group_id()}")

    @helpers.ensure_not_running_as_root
    def create(self) -> None:
        self._compose_config()
        self._compose_build()
        self._add_group()
        self._add_user()
        self._change_owner()
        log.info("Container was built successfully")

    def remove(self) -> None:
        self._stop()
        self._rm()

    @helpers.ensure_not_running_as_root
    def restart(self) -> None:
        self._compose_config()
        self._compose_restart()

    @helpers.ensure_not_running_as_root
    def root(self) -> None:
        self._bash(user="root")

    # --------------------------------------------------------------------------
    # self.login() / self.root()

    def _bash(self, user: str) -> None:
        cmd = [
            "--user", f"{user}", "--env", "REPO_NAME", f"{CONTAINER_NAME}",
            "/bin/bash", "--rcfile", f"{CONTAINER_BASHRC_PATH}"
        ]
        helpers_run_kwargs = {"panic_on_error": False, "log_on_error": False}
        self._exec(cmd, **helpers_run_kwargs)

    # --------------------------------------------------------------------------
    # self.create()

    def _compose_build(self) -> None:
        cmd = [
            "docker-compose", "-f", f"{HOST_DOCKER_COMPOSE_PATH}", "up", "-d",
            "--build", "--force-recreate"
        ]
        helpers.run(cmd, env=self._env)

    def _add_group(self) -> None:
        cmd = [
            f"{CONTAINER_NAME}", "groupadd", "-g", f"{helpers.group_id()}",
            "grp"
        ]
        helpers_run_kwargs = {"panic_on_error": False}
        self._exec(cmd, **helpers_run_kwargs)

    def _add_user(self) -> None:
        cmd = [
            f"{CONTAINER_NAME}", "useradd", "-u", f"{helpers.user_id()}", "-g",
            f"{helpers.group_id()}", "-m", f"{helpers.user()}"
        ]
        self._exec(cmd)

    def _change_owner(self) -> None:
        cmd = [
            f"{CONTAINER_NAME}", "chown", "-R", f"{helpers.user()}",
            f"{CONTAINER_WORKSPACE_DIR}"
        ]
        self._exec(cmd)

    # --------------------------------------------------------------------------
    # self.remove()

    def _stop(self) -> None:
        cmd = ["docker", "container", "stop", f"{CONTAINER_NAME}"]
        helpers.run(cmd, env=self._env)

    def _rm(self) -> None:
        cmd = ["docker", "container", "rm", f"{CONTAINER_NAME}"]
        helpers.run(cmd, env=self._env)

    # --------------------------------------------------------------------------
    # self.restart()

    def _compose_restart(self) -> None:
        cmd = [
            "docker-compose", "-f", f"{HOST_DOCKER_COMPOSE_PATH}", "up", "-d"
        ]
        helpers.run(cmd, env=self._env)

    # --------------------------------------------------------------------------
    # Internal methods

    def _exec(self, cmd: List[str], interactive: bool = True, **kwargs) -> None:
        cmd = ["docker", "exec"] + (["-it"] if interactive else []) + cmd
        helpers.run(cmd, env=self._env, **kwargs)

    def _compose_config(self) -> None:
        cmd = ["docker-compose", "-f", f"{HOST_DOCKER_COMPOSE_PATH}", "config"]
        helpers.run(cmd, env=self._env)

    # --------------------------------------------------------------------------
    # Initialisation

    def __init__(self) -> None:
        self._env = self._setup_env()

    def _setup_env(self) -> helpers.EnvLike:
        env: helpers.EnvLike = os.environ.copy()

        env.update({
            "REPO_NAME":               str(REPO_NAME),
            "CONTAINER_NAME":          str(CONTAINER_NAME),
            "HOST_WORKSPACE_DIR":      str(HOST_WORKSPACE_DIR),
            "CONTAINER_WORKSPACE_DIR": str(CONTAINER_WORKSPACE_DIR),
            "COMPOSE_PROJECT_NAME":    str(REPO_NAME),
        })

        return env
