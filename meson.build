project('tup', 'c',
    version : '0.5.0',
    meson_version : '>= 0.43.0',
    default_options : [
      'warning_level=2'
      ])

c_compiler = meson.get_compiler('c')
warning_flags = [
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
    ]

add_project_arguments(c_compiler.get_supported_arguments(warning_flags),
    language: 'c')

libsmp_dep = dependency('libsmp', fallback: [ 'libsmp', 'libsmp_dep'])

libtup_src = [
    'src/context.c',
    'src/message.c',
    ]

libtup_incdir = include_directories(['include'])

libtup_flags = []
if get_option('default_library') == 'shared'
  libtup_flags += '-DTUP_EXPORT_API'
endif

libtup = library('tup', libtup_src,
    include_directories : [libtup_incdir],
    c_args : libtup_flags,
    cpp_args : libtup_flags,
    dependencies : [libsmp_dep])

libtup_dep = declare_dependency(link_with : libtup,
    include_directories : [libtup_incdir],
    dependencies : [libsmp_dep])

subdir('tools')
