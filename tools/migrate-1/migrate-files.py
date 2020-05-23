import re
import pathlib

regex = re.compile(r'#include\s+"(.*\/)*(.*)"')


def xstr(s):
    if s is None:
        return ''
    return str(s)


def migrate_includes(path: pathlib.Path):
    text = ''
    with path.open(mode='r', encoding='utf-8') as f:
        for line in f.readlines():
            m = regex.match(line)
            if m:
                t = m.group(2)
                t = ''.join(([i.capitalize() for i in t.split('_')]))
                t = '#include "{}{}"\n'.format(xstr(m.group(1)), t)
                text += t
            else:
                text += line

    with path.open(mode='w', encoding='utf-8') as f:
        f.write(text)


for p in pathlib.Path('./src').rglob('*.cpp'):
    migrate_includes(p)
    p.rename(p.parent.joinpath(
        ''.join([i.capitalize() for i in p.name.split('_')])))

for p in pathlib.Path('./src').rglob('*.hpp'):
    migrate_includes(p)
    p.rename(p.parent.joinpath(
        ''.join([i.capitalize() for i in p.name.split('_')])))

for p in pathlib.Path('./include/cru').rglob('*.hpp'):
    migrate_includes(p)
    p.rename(p.parent.joinpath(
        ''.join([i.capitalize() for i in p.name.split('_')])))
