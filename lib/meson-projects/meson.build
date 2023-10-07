project('cru-deps', 'c', 'cpp')

alias_target('all', 
  dependency('cairo', fallback: ['cairo', 'cairo_dep'], required: get_option('cairo')),
  dependency('pango', fallback: ['pango', 'pango_deps'], required: get_option('pango'))
)

