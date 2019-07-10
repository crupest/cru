import os
import os.path

clang_complete_file_name = '.clang_complete'

# project root dir
project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
os.chdir(project_root)

global_std_standard = 'c++17'  # change this to change standard
global_std_standard_arg = '-std={}'.format(global_std_standard)

global_include_paths = [
    os.path.abspath('include')
]

global_definitions = []


def generate_clang_complete_content(additional_include_paths=[], additional_definitions=[]):
    include_args = [
        '-I{}'.format(path) for path in additional_include_paths + global_include_paths]
    definition_args = [
        '-D{}'.format(definition) for definition in additional_definitions + global_definitions]
    args = [global_std_standard_arg] + include_args + definition_args
    return '\n'.join(args)


def generate_root_clang_complete():
    with open(os.path.join('src/', clang_complete_file_name), 'w') as f:
        print(generate_clang_complete_content(), file=f)


def generate_win_clang_complete_content():
    return generate_clang_complete_content(additional_definitions=['UNICODE', '_UNICODE', 'WIN32', '_WINDOWS'])


def generate_win_clang_complete():
    with open(os.path.join('src/win/', clang_complete_file_name), 'w') as f:
        print(generate_win_clang_complete_content(), file=f)

generate_root_clang_complete()
generate_win_clang_complete()
