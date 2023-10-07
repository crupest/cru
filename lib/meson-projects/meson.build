project('cru-deps', 'c', 'cpp')

hello_word_test = '''
#include <stdio.h>

int main() {
  printf("Hello world!");
  return 0;
}
'''

cc = meson.get_compiler('c')
hello_world_result = cc.run(hello_word_test, name: 'hello world')

if hello_world_result.returncode() != 0
  error('Failed to hello world!')
endif

if get_option('cairo').enabled()
  subproject('glib')
  subproject('cairo')
endif

if get_option('pango').enabled()
  subproject('glib')
  subproject('pango')
endif

