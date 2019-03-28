import argparse
import os
import os.path
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument('-a', '--arch', choices=['x86', 'x64'],
                    required=True, help='specify target cpu architecture')
parser.add_argument('-c', '--config', choices=['Debug', 'Release'],
                    required=True, help='specify build configuration')
parser.add_argument('-e', '--vs-edition', choices=['Community', 'Enterprise'],
                    default='Community', help='specify visual studio edition')
args = parser.parse_args()


project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))

vs_path = 'C:/Program Files (x86)/Microsoft Visual Studio/2017/{edition}'.format(
    edition=args.vs_edition)

def setup_vs_env():
    old_dir = os.getcwd()
    os.chdir(os.path.join(vs_path, 'VC/Auxiliary/Build'))
    output = subprocess.check_output('vcvarsall.bat {arch} & set'.format(arch=args.arch), encoding='utf-8')
    for line in output.splitlines():
        e = line.find('=')
        os.environ[line[:e]] = line[e+1:]
    os.chdir(old_dir)

setup_vs_env()

os.chdir(project_root)

subprocess.run('cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE="{config}"'.format(config=args.config),
               stdout=sys.stdout, stderr=sys.stderr)

subprocess.run('cmake --build build --target all',
               stdout=sys.stdout, stderr=sys.stderr)
