import argparse
import os
import os.path
import re
import sys

parser = argparse.ArgumentParser()
parser.add_argument("-o", "--output", required=True, help="The path of output header and source files excluding extension.")
parser.add_argument("input", nargs="+", help="The input source files or folders to scan.")
parser.add_argument("-e", "--exclude", nargs="+", help="Filenames that exludes from merging.")
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

cpp_header_file_regex = re.compile(r"^.*\.(h|hpp)$")
cpp_source_file_regex = re.compile(r"^.*\.(cpp|cc|cxx)$")

def is_exclude(file_name):
    return args.exclude and file_name in args.exclude

for path in args.input:
    if os.path.isdir(path):
        input_dirs.append(path)
    elif os.path.isfile(path):
        file_name = os.path.basename(path)
        if is_exclude(file_name):
            sys.exit("{} is specified excluding.")
        if cpp_source_file_regex.match(file_name):
            add_source_file(path)
        elif cpp_header_file_regex.match(file_name):            
            add_header_file(path)
        else:
            sys.exit("{} is a file but it is neither c++ source nor header.".format(path))
    else:
        sys.exit("{} is neither a file nor a directory.".format(path))

actual_exclude_count = 0

for input_dir in input_dirs:
    for root, subdirs, files in os.walk(input_dir):
        for f in files:
            if is_exclude(f):
                actual_exclude_count += 1
            else:
                if cpp_source_file_regex.match(f):
                    path = os.path.join(root, f)
                    add_source_file(path)
                elif cpp_header_file_regex.match(f):
                    path = os.path.join(root, f)
                    add_header_file(path)

print()
print("total find {} header file(s)".format(len(header_file_list)))
print("total find {} source file(s)".format(len(source_file_list)))
print("total actual exclude {} file(s)".format(actual_exclude_count))
print()

norm_source_file_list = [os.path.normpath(path) for path in source_file_list]
norm_header_file_list = [os.path.normpath(path) for path in header_file_list]

output_header_file_path = args.output + '.hpp'
output_source_file_path = args.output + '.cpp'


output_header_file = open(output_header_file_path, 'w')
output_source_file = open(output_source_file_path, 'w')


processed_headers = []

header_pragma_once_regex = re.compile(r'#\s*pragma\s*once')
header_include_command_regex = re.compile(r'#\s*include\s*"([\w.\-_/\\]+)"')

def process_line(line):
    match_result = header_include_command_regex.match(line)
    if match_result:
        return match_result.group(1)
    else:
        return None

def find_header_path(base_path, header_path):
    path = os.path.join(base_path, header_path)
    if os.path.isfile(path):
        return path
    else:
        for d in input_dirs:
            path = os.path.join(d, header_path)
            if os.path.isfile(path):
                return path
    raise RuntimeError("Header not found.\nBase path: {}\nHeader path: {}".format(base_path, header_path))

def process_header_file(base_path, header_path):
    path = find_header_path(base_path, header_path)
    path = os.path.normpath(path)
    if path in processed_headers:
        return
    processed_headers.append(path)

    output_header_file.write("//--------------------------------------------------------\n")
    output_header_file.write("//-------begin of file: {}\n".format(path))
    output_header_file.write("//--------------------------------------------------------\n")
    header_f = open(path, 'r')
    found_pragma_once = False
    for line in header_f:
        if (not found_pragma_once):
            if header_pragma_once_regex.match(line):
                found_pragma_once = True
                continue
        new_header_name = process_line(line)
        if not new_header_name is None:
            process_header_file(os.path.dirname(path), new_header_name)
        else:
            output_header_file.write(line)
    output_header_file.write("//--------------------------------------------------------\n")
    output_header_file.write("//-------end of file: {}\n".format(path))
    output_header_file.write("//--------------------------------------------------------\n")


output_header_file.write("#pragma once\n")

output_source_file.write('#include "{}"\n'.format(os.path.basename(output_header_file_path)))


for source_path in source_file_list:
    output_source_file.write("//--------------------------------------------------------\n")
    output_source_file.write("//-------begin of file: {}\n".format(source_path))
    output_source_file.write("//--------------------------------------------------------\n")
    source_f = open(source_path, 'r')
    for line in source_f:
        new_header_name = process_line(line)
        if not new_header_name is None:
            process_header_file(os.path.dirname(source_path), new_header_name)
        else:
            output_source_file.write(line)
    output_source_file.write("//--------------------------------------------------------\n")
    output_source_file.write("//-------end of file: {}\n".format(source_path))
    output_source_file.write("//--------------------------------------------------------\n")

rest_header_count = len(norm_header_file_list) - len(processed_headers)

if rest_header_count > 0:
    print("{} header files are not included in any sources. Add them to header now.".format(rest_header_count))
    print()
    for header_path in norm_header_file_list:
        if header_path not in processed_headers:
            process_header_file('.', header_path)

print("Done!")
