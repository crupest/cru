#!/usr/bin/env python3

import os.path

project_root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

def rename_hpp_to_h(dir):
    for root, dirs, files in os.walk(dir):
        for file in files:
            if file.endswith(".hpp"):
                os.rename(os.path.join(root, file), os.path.join(root, file[:-4] + ".h"))

rename_hpp_to_h(os.path.join(project_root, "include"))
rename_hpp_to_h(os.path.join(project_root, "src"))
rename_hpp_to_h(os.path.join(project_root, "test"))
rename_hpp_to_h(os.path.join(project_root, "demos"))

def replace_hpp_with_h(file):
    with open(file, "r") as f:
        content = f.read()
    content = content.replace(".hpp", ".h")
    with open(file, "w") as f:
        f.write(content)

def replace_hpp_with_h_recursive(dir):
    for root, dirs, files in os.walk(dir):
        for file in files:
            if file.endswith(".cpp") or file.endswith(".h") or file.endswith(".mm"):
                replace_hpp_with_h(os.path.join(root, file))

replace_hpp_with_h_recursive(os.path.join(project_root, "include"))
replace_hpp_with_h_recursive(os.path.join(project_root, "src"))
replace_hpp_with_h_recursive(os.path.join(project_root, "test"))
replace_hpp_with_h_recursive(os.path.join(project_root, "demos"))
