import os
import subprocess
import sys
import venv

WORKSPACE_DIR = "zephyr-zl27arm-workspace"
REPO_URL = "https://github.com/ErgoAsh/zephyr-zl27arm.git"
APP_DIR_NAME = "zephyr-zl27arm"

def run(cmd, cwd=None):
    print(f"--> Executing: {cmd}")
    subprocess.check_call(cmd, shell=True, cwd=cwd)

if not os.path.exists(WORKSPACE_DIR):
    os.makedirs(WORKSPACE_DIR)

os.chdir(WORKSPACE_DIR)

if not os.path.exists(".venv"):
    venv.create(".venv", with_pip=True)

if sys.platform == "win32":
    pip_exe = os.path.join(".venv", "Scripts", "pip")
    west_exe = os.path.join(".venv", "Scripts", "west")
else:
    pip_exe = os.path.join(".venv", "bin", "pip")
    west_exe = os.path.join(".venv", "bin", "west")

run(f"{pip_exe} install -U west ninja")

if not os.path.exists(APP_DIR_NAME):
    run(f"git clone {REPO_URL} {APP_DIR_NAME}")
else:
    run("git pull", cwd=APP_DIR_NAME)

if not os.path.exists(".west"):
    run(f"{west_exe} init -l {APP_DIR_NAME}")

run(f"{west_exe} update")

if os.path.exists("zephyr"):
    run(f"{west_exe} zephyr-export")
    req_file = os.path.join("zephyr", "scripts", "requirements.txt")
    if os.path.exists(req_file):
        run(f"{pip_exe} install -r {req_file}")

print("\n=== Setup Complete ===")