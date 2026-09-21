from pathlib import Path
import subprocess
import sys

FIRST_SECTOR_LOCATE     = 0x08004000
FIRMWARE_INFO_LOCATE    = 0x08028000


firm_path = Path(sys.argv[1]).resolve()
repo_path = Path(__file__).resolve().parent


def git_output(*args: str) -> str:
    return subprocess.check_output(
        ["git", "-C", str(repo_path), *args],
        text=True,
        encoding="utf-8",
    ).strip()


class FirmwareInfo():
    project_name: str
    commit_hash: str
    branch_name: str    
    commit_tag: str
    
    def write_info_to_firmware(self):
        with firm_path.open("rb+") as f:
            seek = (FIRMWARE_INFO_LOCATE - FIRST_SECTOR_LOCATE)
            f.seek(seek)
            res = "git_info" + '\0' + '\n' + self.branch_name + '\n' + self.commit_hash + '\n' + self.commit_tag + '\n'
            print (res)
            f.write(bytes(res, 'ascii'))

    def __init__(self):
        self.commit_hash = git_output("rev-parse", "HEAD")
        self.branch_name = git_output("branch", "--show-current") or "DETACHED"
        self.project_name = repo_path.name
        tags = git_output("tag", "--points-at", "HEAD").splitlines()
        self.commit_tag = tags[0] if tags else "Not exist"

FirmwareInfo().write_info_to_firmware()

