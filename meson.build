project('tup', 'c',
    version : '0.1.0',
    meson_version : '>= 0.38.0',
    default_options : [
      'warning_level=2'
      ])

add_project_arguments([
    # warnings flags
    '-Wno-unused-parameter',
    '-Wformat-nonliteral',
    '-Wformat-security',
    '-Winit-self',
    '-Wswitch-default',
    '-Wuninitialized',
    '-Wmaybe-uninitialized',
    '-Wshadow',
    '-Wframe-larger-than=4096',
    '-Wpointer-arith',
    '-Wundef',
    '-Wcast-qual',
    '-Wjump-misses-init',
    '-Wmissing-prototypes',
    '-Wno-type-limits',
    '-fstack-usage',
    ], language : 'c')

libsmp_dep = subproject('libsmp').get_variable('libsmp_dep')

libtup_src = [
    'src/context.c',
    'src/message.c',
    ]

libtup_incdir = include_directories(['include'])

libtup = library('tup', libtup_src,
    include_directories : [libtup_incdir],
    dependencies : [libsmp_dep])

libtup_dep = declare_dependency(link_with : libtup,
    include_directories : [libtup_incdir],
    dependencies : [libsmp_dep])

subdir('examples')
