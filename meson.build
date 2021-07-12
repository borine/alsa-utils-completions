project('alsa-utils-completions', 'c',  version: '0.0.1', license: 'Unlicense')

prefix = get_option('prefix')
assert(prefix.startswith('/'), 'Prefix is not absolute: "@0@"'.format(prefix))

bindir = join_paths(prefix, get_option('bindir'))
datadir = join_paths(prefix, get_option('datadir'))

cc = meson.get_compiler('c')

alsadep = dependency('alsa', required : true)
bashcompdep = dependency('bash-completion', required : true)

if bashcompdep.found()
    completionsdir = bashcompdep.get_pkgconfig_variable('completionsdir')
else
    completionsdir = join_paths(datadir, 'bash-completion', 'completions')
endif

executable(
    'alsa-list-ctls',
    'alsa-list-ctls.c',
    dependencies : alsadep,
    install : true,
    install_dir : bindir,
)

aliases = [
    'alsaloop',
    'alsamixer',
    'amidi',
    'amixer',
    'aplay',
    'arecord',
    'axfer',
]

install_data('alsa-utils', install_dir: completionsdir)

foreach alias : aliases
    dst = join_paths(completionsdir, alias)
    cmd = 'ln -fs @0@ $DESTDIR@1@'.format('alsa-utils', dst)
    meson.add_install_script('sh', '-c', cmd)
endforeach

