require 'mkmf'

$CFLAGS ||= ''
$LDFLAGS ||= ''

$CFLAGS << ' -fprofile-arcs -ftest-coverage '
$LDFLAGS << ' -fprofile-arcs -ftest-coverage '

dir_config("tidy")
create_makefile("tidy")

