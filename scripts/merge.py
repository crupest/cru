import os
import os.path

script_dir = os.path.dirname(os.path.realpath(__file__))

tool_path =os.path.relpath(os.path.join(script_dir, '../tools/cppmerge/main.py'))
input_dir = os.path.relpath(os.path.join(script_dir, '../src'))
output_path = os.path.relpath(os.path.join(script_dir, '../CruUI-Generate/cru_ui'))

print('Input dir is {}'.format(input_dir))
print('Output path is {}'.format(output_path))

os.system('python {} -o {} {}'.format(tool_path, output_path, input_dir))
