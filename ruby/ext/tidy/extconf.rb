require 'mkmf'

if arg_config('--with-coverage') || ENV['COV']
  $CFLAGS += ' -fprofile-arcs -ftest-coverage '
  $LDFLAGS += ' -fprofile-arcs -ftest-coverage '
end

dir_config("tidy")
create_makefile("tidy")

