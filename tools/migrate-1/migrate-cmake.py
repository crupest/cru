import re
from pathlib import Path

regex = re.compile(r'\s*(.*\/)*(.*)(\.[hc]pp)$')

def xstr(s):
    if s is None:
        return ''
    return str(s)

for p in Path('./src').rglob('CMakeLists.txt'):
    text = ''
    with p.open(mode='r', encoding='utf-8') as f:
        for line in f.readlines():
            m = regex.match(line)
            if m:
                t = m.group(2)
                t = ''.join(([i.capitalize() for i in t.split('_')]))
                t = '\t{}{}{}\n'.format(xstr(m.group(1)), t, m.group(3))
                text += t
            else:
                text += line
    with p.open(mode='w', encoding='utf-8') as f:
        f.write(text)


