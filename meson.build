project('alsa-utils-completions', 'c',  version: '0.0.3', license: 'Unlicense')

prefix = get_option('prefix')
assert(prefix.startswith('/'), 'Prefix is not absolute: "@0@"'.format(prefix))

bindir = join_paths(prefix, get_option('bindir'))
datadir = join_paths(prefix, get_option('datadir'))

cc = meson.get_compiler('c')

alsadep = dependency('alsa', required : true)
bashcompdep = dependency('bash-completion', required : true)

if bashcompdep.found()
    completionsdir = bashcompdep.get_variable(pkgconfig : 'completionsdir')
else
    completionsdir = join_paths(datadir, 'bash-completion', 'completions')
endif

version_conf = configuration_data()
version_conf.set_quoted('VERSION_ID', meson.project_version())
version_h = configure_file(output: 'version.h', configuration: version_conf)

executable(
    'alsa-list',
    [ version_h, 'list.c' ],
    dependencies : alsadep,
    install : true,
    install_dir : bindir,
)

utils_aliases = [
	'alsactl',
    'alsaloop',
    'alsamixer',
    'amidi',
    'amixer',
    'aplay',
    'arecord',
    'axfer',
	'speaker-test',
	'alsa-list',
	'iecset',
]

install_data('alsa-utils', install_dir: completionsdir)
foreach alias : utils_aliases
    dst = join_paths(completionsdir, alias)
    cmd = 'ln -fs @0@ $DESTDIR@1@'.format('alsa-utils', dst)
    meson.add_install_script('sh', '-c', cmd)
endforeach

utils_seq_aliases = [
	'aconnect',
	'aplaymidi',
	'aseqdump',
	'aseqnet',
]

install_data('alsa-utils-seq', install_dir: completionsdir)
foreach alias : utils_seq_aliases
    dst = join_paths(completionsdir, alias)
    cmd = 'ln -fs @0@ $DESTDIR@1@'.format('alsa-utils-seq', dst)
    meson.add_install_script('sh', '-c', cmd)
endforeach
