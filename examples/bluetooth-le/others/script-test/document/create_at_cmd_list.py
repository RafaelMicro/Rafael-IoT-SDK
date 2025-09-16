import re
from os import walk
from dataclasses import dataclass
from typing import List

@dataclass
class Cmd:
    folder:str
    command:str
    data:str

def get(regex:str, data:str) -> str:
    result = re.search(regex,data)
    if result is None:
        return ""
    return result.groups()[0]

def get_cmd_list(folder:str) -> List[Cmd]:
    cmd_list:List[Cmd] = []
    for root, dirs, files in walk(folder):
        for file in files:
            path = root + "\\" + file
            with open(path,"r",encoding="utf-8") as f:
                data = f.read()
            datas = re.findall(r"f\(\s*(\"\+[^}]*)\n\s*\);", data)
            cmd = re.search(r"\"(\+\w+)\"",data)
            if datas!=[] and cmd is not None :
                cmd_list.append(Cmd(folder=root,command=cmd.groups()[0],data = datas[0]))
    return cmd_list

def write(s:str =""):
    with open("at_cmd_list.txt",'a') as f:
        f.write(s+"\n")

folder = '../Source/Command'
cmd_list = get_cmd_list(folder)
cmd_type_list = set()

for cmd in cmd_list:
    cmd_type = get(r"\\(\w*)$",cmd.folder)
    if cmd_type not in cmd_type_list:
        write(f"# {cmd_type}\n")
        cmd_type_list.add(cmd_type)
    # print("cmd:",cmd.command)
    write(f"## {cmd.command}\n")
    data = cmd.data.split("\n")
    # print(f"path:{cmd.folder}")
    for d in data:
        d = get(r"\"(.*)\\n\"",d)
        d = re.sub(r"^(\s*)(\+|notice|<|ex|format)",r"\1- \2",d)
        d = re.sub(r"(<\w*>)",r"`\1`",d)
        d += "  "
        write(d)
    write()

print("create .\\at_cmd_list.txt finish")