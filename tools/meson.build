compiler = meson.get_compiler('c')

tupctl_cflags = []

has_poll = compiler.has_function('poll')
is_windows = host_machine.system() == 'windows'

if has_poll or is_windows
  if has_poll
    tupctl_cflags += ['-DHAVE_POLL']
  endif

  executable('tupctl', 'tupctl.c',
      c_args : tupctl_cflags,
      dependencies : libtup_dep)
endif
