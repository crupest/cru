import argparse
import os
import os.path
import re
import sys

parser = argparse.ArgumentParser()
parser.add_argument("-o", "--output", required=True, help="The name of output header and source files excluding extension.")
parser.add_argument("input", nargs="+", help="The input source files or folders to scan.")
args = parser.parse_args()

input_dirs = []

header_file_list = []
source_file_list = []

def add_source_file(path):
    print("find source file: {}".format(path))
    source_file_list.append(path)

def add_header_file(path):
    print("find header file: {}".format(path))
    header_file_list.append(path)

cpp_header_file_regex = re.compile(r".*\.(h|hpp)")
cpp_source_file_regex = re.compile(r".*\.(cpp|cc|cxx)")


for path in args.input:
    if os.path.isdir(path):
        input_dirs.append(path)
    elif os.path.isfile(path):
        file_name = os.path.basename(path)
        if cpp_source_file_regex.match(file_name):
            add_source_file(path)
        elif cpp_header_file_regex.match(file_name):
            add_header_file(path)
        else:
            sys.exit("{} is a file but it is neither c++ source nor header.".format(path))
    else:
        sys.exit("{} is neither a file nor a directory.".format(path))

for input_dir in input_dirs:
    for root, subdirs, files in os.walk(input_dir):
        for f in files:
            if cpp_source_file_regex.match(f):
                path = os.path.join(root, f)
                add_source_file(path)
            elif cpp_header_file_regex.match(f):
                path = os.path.join(root, f)
                add_header_file(path)

print()
print("total find {} header file(s)".format(len(header_file_list)))
print("total find {} source file(s)".format(len(source_file_list)))
print()

