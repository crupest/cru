project('cru-deps', 'c', 'cpp')

if get_option('cairo').enabled()
  subproject('glib')
  subproject('cairo')
endif

if get_option('pango').enabled()
  subproject('glib')
  subproject('pango')
endif

