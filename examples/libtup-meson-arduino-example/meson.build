project('libtup-adruino-example', 'c', 'cpp',
  default_options : [
  'buildtype=minsize',
  'default_library=static',
  'b_staticpic=false',
  'libsmp:avr-enable-serial0=true',
  'libsmp:avr-enable-serial1=true',
  'libsmp:avr-enable-serial2=false',
  'libsmp:avr-enable-serial3=false',
  'libsmp:message-max-values=8',
  'libsmp:serial-frame-max-size=128',
  'libsmp:use-arduino-lib=true',
  ],
)

assert(meson.is_cross_build(), 'AVR projects can only be built in a cross build environment.')

project_name = 'libtup-adruino-example'

# source and includes
src = [
  'main.cpp',
]

include = include_directories([
])

arduino_core_dep = subproject('arduino-core').get_variable('arduino_core_dep')
libtup_dep = subproject('libtup').get_variable('libtup_dep')

avrdude_mcu = host_machine.cpu()
objcopy = find_program(meson.get_cross_property('objcopy'))
avrdude = find_program(meson.get_cross_property('avrdude'))

# Get avr programmer
programmer = get_option('programmer')
baudrate = get_option('baudrate')
serial_port = get_option('serial-port')
if programmer == 'arduino'
  local_avrdude_flags = [
    '-cwiring',
    '-P' + serial_port,
    '-b' + baudrate,
    '-D'
  ]
elif programmer == 'isp-v2'
  local_avrdude_flags = [
    '-cavrispv2',
    '-Pusb'
  ]
elif programmer == 'usbasp'
  local_avrdude_flags = [
    '-cusbasp',
    '-Pusb'
  ]
elif programmer == 'usbtiny'
  local_avrdude_flags = [
    '-cusbtiny'
  ]
else
  error('Unsupported programmer')
endif

avrdude_flags = [
  '-Csubprojects/arduino-core/' + meson.get_cross_property('avrdude_conf'),
  local_avrdude_flags,
  '-p' + avrdude_mcu,
  '-v',
]

l_fuse = meson.get_cross_property('l_fuse')
h_fuse = meson.get_cross_property('h_fuse')
e_fuse = meson.get_cross_property('e_fuse')

# C++ flags
cppflags = [
  '-Wno-non-virtual-dtor'
]

exe = executable(project_name, src,
  dependencies : [arduino_core_dep, libtup_dep],
  include_directories : include,
  cpp_args : cppflags,
)

hex = custom_target(project_name + '.hex',
  input : exe,
  output : project_name + '.hex',
  command : [objcopy, '-O', 'ihex', '-R', '.eeprom', '@INPUT@', '@OUTPUT@'],
)

run_target('flash',
  command : [avrdude, avrdude_flags, '-Uflash:w:@0@:i'.format(hex.full_path())],
  depends : hex
)

run_target('reset',
  command : [avrdude, avrdude_flags,
    '-E reset',
  ]
)

run_target('fuse',
  command : [avrdude, avrdude_flags,
    '-Ulfuse:w:'+l_fuse+':m',
    '-Uhfuse:w:'+h_fuse+':m',
    '-Uefuse:w:'+e_fuse+':m'
  ]
)
