#include <ruby.h>

static VALUE mTidy;

static VALUE rb_test(VALUE obj)
{
  return rb_str_new2("hello world");
}

void
Init_tidy()
{
  mTidy = rb_define_module("Tidy");
  rb_define_module_function(mTidy, "test", rb_test, 0);
}


