import argparse
import os
import os.path
import shutil
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument('command', choices=[
                    'configure', 'build', 'test'], nargs='?', default='test', help='specify command to execute')
parser.add_argument('-a', '--arch', choices=['x86', 'x64'],
                    default='x64', help='specify target cpu architecture')
parser.add_argument('-c', '--config', choices=['Debug', 'Release'],
                    default='Debug', help='specify build configuration')
parser.add_argument('-d', '--work-dir', default='build',
                    help='specify working directory for building')
args = parser.parse_args()


project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
work_dir = os.path.join(project_root, args.work_dir)

# this is not used when generator is Visual Studio


# def init_vc_environment(arch):
#     arch_bat_map = {
#         'x86': 'vcvarsamd64_x86',
#         'x64': 'vcvars64'
#     }
#     vars = subprocess.check_output(['C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\{}'.format(
#         arch_bat_map[arch]), '&&', 'set'], shell=True, text=True)
#     for var in vars.splitlines():
#         k, _, v = map(str.strip, var.strip().partition('='))
#         if k.startswith('?'):
#             continue
#         os.environ[k] = v

def configure():
    generater_vs_arch_map = {
        'x86': 'Win32',
        'x64': 'x64'
    }

    # -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    subprocess.check_call('cmake -S . -B {build_dir} -G "Visual Studio 16 2019" -A {arch} -T host=x64'
                          .format(build_dir=work_dir, arch=generater_vs_arch_map[args.arch]),
                          stdout=sys.stdout, stderr=sys.stderr)


def build():
    subprocess.check_call('cmake --build {build_dir} --target ALL_BUILD --config {config}'
                          .format(build_dir=work_dir, config=args.config),
                          stdout=sys.stdout, stderr=sys.stderr)


def test():
    os.chdir(work_dir)
    subprocess.check_call('ctest')
    os.chdir(project_root)


os.chdir(project_root)

configure()
if args.command == 'configure':
    exit(0)
build()
if args.command == 'build':
    exit(0)
test()
