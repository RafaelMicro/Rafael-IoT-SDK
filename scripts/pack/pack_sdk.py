import os
import shutil
import platform
import argparse
from typing import Callable, Dict
from elftools.elf.elffile import ELFFile


class CopyManager:
    def __init__(self):
        self.rules: Dict[str, Callable[[str, str], None]] = {}

    def add_rule(self, path: str, func: Callable[[str, str], None]):
        self.rules[path] = func

    def apply_rules(self, src: str, dest: str) -> bool:
        for rule_path, rule_func in self.rules.items():
            if os.path.commonpath([src, rule_path]) == rule_path:
                rule_func(src, dest)
                return True
        return False


def cp_dir(src: str, dest: str, manager: CopyManager, original_src: str):
    if os.path.exists(dest):
        shutil.rmtree(dest)
    if not os.path.exists(dest):
        os.makedirs(dest)
    for item in os.listdir(src):
        src_item = os.path.join(src, item)
        dest_item = os.path.join(dest, item)
        if os.path.commonpath([original_src, dest_item]) == original_src:
            continue
        if os.path.isdir(src_item):
            if not manager.apply_rules(src_item, dest_item):
                cp_dir(src_item, dest_item, manager, original_src)
        elif os.path.basename(src) != "CMakeLists.txt.in":
            os.makedirs(os.path.dirname(dest_item), exist_ok=True)
            shutil.copy2(src_item, dest_item)
        elif item.endswith((".a")):
            state = True
            with open(src_item, 'rb') as f:
                elf = ELFFile(f)
                for section in elf.iter_sections():
                    if not (section.name == b'.text'):
                        state = False
                        break
            if state:
                os.makedirs(os.path.dirname(dest_item), exist_ok=True)
                shutil.copy2(src_item, dest_item)
            else:
                print("static library is not IoT SDK generated. Exiting the program...")
                exit(0)
    for root, dirs, files in os.walk(dest, topdown=False):
        for dir in dirs:
            dir_path = os.path.join(root, dir)
            if not os.listdir(dir_path):
                os.rmdir(dir_path)


def custom_cp(src: str, dest: str):
    if os.path.exists(dest):
        shutil.rmtree(dest)
    if not os.path.exists(dest):
        os.makedirs(dest)
    for item in os.listdir(src):
        if item.endswith((".c", ".cpp", ".txt")):
            continue
        src_item = os.path.join(src, item)
        dest_item = os.path.join(
            dest, item[:-3] if item.endswith(".in") else item)

        if os.path.isdir(src_item):
            custom_cp(src_item, dest_item)
        else:
            shutil.copy2(src_item, dest_item)
    for root, dirs, files in os.walk(dest, topdown=False):
        for dir in dirs:
            dir_path = os.path.join(root, dir)
            if not os.listdir(dir_path):
                os.rmdir(dir_path)


def skip_cp(src: str, dest: str):
    pass


def get_platform_specific_path(path: str) -> str:
    if platform.system() == "Windows":
        return path.replace("/", "\\")
    return path


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("conf")
    args = parser.parse_args()

    src = get_platform_specific_path(".")
    dest = os.path.join(src, "release")
    manager = CopyManager()
    e_dir = []
    if os.path.exists(args.conf):
        with open(args.conf, 'r') as f:
            e_dir = [line.strip() for line in f if line.strip()]
    for d in ["release", ".git", ".vsocde", "build"]:
        manager.add_rule(get_platform_specific_path(d), skip_cp)
    manager.add_rule(get_platform_specific_path("cmake"), custom_cp)
    for d in e_dir:
        manager.add_rule(get_platform_specific_path(
            "components/"+d), custom_cp)
    cp_dir(src, dest, manager, src)
