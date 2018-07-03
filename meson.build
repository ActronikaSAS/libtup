project('tup', 'c',
    version : '0.3.0',
    meson_version : '>= 0.38.0',
    default_options : [
      'warning_level=2'
      ])

if meson.get_compiler('c').get_id() == 'gcc'
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
endif

libsmp_dep = subproject('libsmp').get_variable('libsmp_dep')

libtup_src = [
    'src/context.c',
    'src/message.c',
    ]

## Public config file
cdata = configuration_data()

cdata.set('TUP_CONTEXT_BUFFER_SIZE',
    get_option('tup-context-buffer-size').to_int(),
    description : 'The maximum context size in bytes')

configure_file(output : 'libtup-config.h',
    configuration : cdata)

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
