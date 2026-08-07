from git import *
from typing import Optional
import os.path 
import sys
import os

FIRST_SECTOR_LOCATE     = 0x08004000
FIRMWARE_INFO_LOCATE    = 0x08028000


firm_path = sys.argv[1]


class FirmwareInfo():
    project_name: str
    commit_hash: str
    branch_name: str    
    commit_tag: Optional[str]
    
    def get_cur_tag(self, repo):
        for tag in repo.tags:
            if tag.commit == repo.head.commit: 
                return tag
        return None
    
    def write_info_to_firmware(self):
        with open(firm_path, mode='rb') as file:
            f = open(firm_path, "rb+")
            seek = (FIRMWARE_INFO_LOCATE - FIRST_SECTOR_LOCATE)
            f.seek(seek)
            res = "git_info" + '\0' + '\n'  + self.branch_name + '\n' + self.commit_hash + '\n' + (self.commit_tag if self.commit_tag != None else "Not exist") + '\n'
            print (res)
            f.write(bytes(res, 'ascii'))

    def __init__(self):
        print(os.path.dirname(firm_path) + "../../..")
        repo = Repo(os.path.dirname(firm_path) + "../../..")
        self.commit_hash = str(repo.head.commit)
        self.branch_name = str(repo.head.reference)
        
        self.project_name = os.path.dirname(os.path.dirname(__file__)).replace("\\", "/").split('/').pop()
        
        self.commit_tag  = self.get_cur_tag(repo)
        #tagref = TagReference.list_items(repo)[0]
        #print(tagref.tag.message)
        #print(repo.heads[0])
        
        # print(self.project_name)
        # print(self.branch_name)
        # print(self.commit_hash)
        # print(self.commit_tag)

        print(list(bytes(self.branch_name, 'ascii')))

        print(len(self.project_name) + len(self.branch_name) + len(self.commit_hash))

FirmwareInfo().write_info_to_firmware()

