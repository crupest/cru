import argparse
import os
import os.path
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument('-a', '--arch', choices=['x86', 'x64'],
                    default='x64', help='specify target cpu architecture')
parser.add_argument('-c', '--config', choices=['Debug', 'Release'],
                    required=True, help='specify build configuration')
args = parser.parse_args()


project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))


def build_cru_ui():
    os.chdir(project_root)

    os.environ['PreferredToolArchitecture'] = 'x64'  # use vs x64 toolchain

    generater_vs_arch_map = {
        'x86': 'Win32',
        'x64': 'x64'
    }

    subprocess.check_call('cmake -S . -B build -G "Visual Studio 16 2019" -A {arch}'
                   .format(arch=generater_vs_arch_map[args.arch]),
                   stdout=sys.stdout, stderr=sys.stderr)

    subprocess.check_call('cmake --build build --target ALL_BUILD --config {config}'.format(config=args.config),
                   stdout=sys.stdout, stderr=sys.stderr)


build_cru_ui()
