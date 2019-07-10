import argparse
import os
import os.path
import shutil
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument('command', choices=[
                    'configure', 'build'], nargs='?', default='build', help='specify command to execute')
parser.add_argument('-a', '--arch', choices=['x86', 'x64'],
                    default='x64', help='specify target cpu architecture')
parser.add_argument('-c', '--config', choices=['Debug', 'Release'],
                    default='Debug', help='specify build configuration')
args = parser.parse_args()


project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))


def init_vc_environment(arch):
    arch_bat_map = {
        'x86': 'vcvarsamd64_x86',
        'x64': 'vcvars64'
    }
    vars = subprocess.check_output(['C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\{}'.format(
        arch_bat_map[arch]), '&&', 'set'], shell=True, text=True)
    for var in vars.splitlines():
        k, _, v = map(str.strip, var.strip().partition('='))
        if k.startswith('?'):
            continue
        os.environ[k] = v


def configure():
    generater_vs_arch_map = {
        'x86': 'Win32',
        'x64': 'x64'
    }
    subprocess.check_call('cmake -S . -B build -G "Visual Studio 16 2019" -A {arch} -T host=x64 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON'
                          .format(arch=generater_vs_arch_map[args.arch]),
                          stdout=sys.stdout, stderr=sys.stderr)


def build():
    subprocess.check_call('cmake --build build --target ALL_BUILD --config {config}'.format(config=args.config),
                          stdout=sys.stdout, stderr=sys.stderr)


os.chdir(project_root)

configure()
if args.command == 'build':
    build()
