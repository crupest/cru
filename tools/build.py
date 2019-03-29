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
parser.add_argument('--clang-build-skia', action='store_true',
                    help='whether use clang to build skia')
args = parser.parse_args()

class ConflictArgumentError(Exception):
    def __init__(self, message):
        self.message = message

if args.arch == 'x86' and args.clang_build_skia:
    raise ConflictArgumentError('clang does not support to build skia in x86')

project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
os.chdir(project_root)

def detect_ninja():
    try:
        subprocess.check_call('ninja --version')
    except:
        print('No ninja, install ninja.')
        subprocess.check_call('choco install ninja', stdout=sys.stdout, stderr=sys.stderr)

detect_ninja()

def build_skia():
    subprocess.check_call('py -2 .\\skia\\tools\\git-sync-deps', stdout=sys.stdout, stderr=sys.stderr)

    gn_args = '''
    is_debug = {debug}

    skia_enable_ccpr = false
    skia_enable_discrete_gpu = false
    skia_enable_gpu = false
    skia_enable_nvpr = false
    skia_enable_particles = false
    skia_enable_pdf = false
    skia_enable_skottie = false
    skia_use_dng_sdk = false
    skia_use_lua = false
    skia_use_sfntly = false
    skia_use_xps = false
    '''.format(debug = 'true' if args.config == 'Debug' else 'false' )

    if args.arch == 'x86':
        gn_args += 'target_cpu="x86"\n'

    if args.clang_build_skia:
        gn_args += 'clang_win = "C:/Program Files/LLVM"\n'

    out_dir = '{}_{}'.format(args.config, args.arch)
    if args.clang_build_skia:
        out_dir += '_clang'

    os.makedirs("skia/out/{}".format(out_dir), exist_ok=True)
    gn_arg_file_path = os.path.join(project_root, 'skia/out', out_dir, 'args.gn')

    with open(gn_arg_file_path, 'w+') as f:
        f.write(gn_args)
    
    os.chdir('./skia')
    subprocess.check_call('.\\bin\\gn.exe gen .\\out\\{}\\'.format(out_dir), stdout=sys.stdout, stderr=sys.stderr)
    subprocess.check_call('ninja -C .\\out\\{} skia'.format(out_dir))

build_skia()

def build_cru_ui():
    os.environ['PreferredToolArchitecture'] = 'x64' # use vs x64 toolchain

    generater_vs_arch_map = {
        'x86': 'Win32',
        'x64': 'x64'
    }

    subprocess.run('cmake -S . -B build -G "Visual Studio 15 2017" -A {arch}'.format(arch=generater_vs_arch_map[args.arch]),
                   stdout=sys.stdout, stderr=sys.stderr)

    subprocess.run('cmake --build build --target ALL_BUILD --config {config}'.format(config=args.config),
                   stdout=sys.stdout, stderr=sys.stderr)

build_cru_ui()
