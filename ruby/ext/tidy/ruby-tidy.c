/*
  ruby-tidy.c - Ruby driver for HTML TidyLib
*/
#include <ruby.h>
#include "tidy.h"
#include "buffio.h"

int TIDY_CALL rb_tidyGetByte( TidyInputSource* source )
{
  VALUE data = (VALUE)source->sourceData;
  VALUE value = rb_funcall(data, rb_intern("getc"), 0, NULL);
  return NUM2INT(value);
}

void TIDY_CALL rb_tidyUngetByte( TidyInputSource* source, byte byteValue )
{
  VALUE data = (VALUE)source->sourceData;
  VALUE value = rb_funcall(data, rb_intern("ungetc"), 1, INT2NUM(byteValue));
}

Bool TIDY_CALL rb_tidyIsEOF( TidyInputSource* source )
{
  VALUE data = (VALUE)source->sourceData;
  VALUE value = rb_funcall(data, rb_intern("eof"), 0, NULL);
  return value == Qfalse ? no : yes;
}

static VALUE cTidy;

/* release tidy doc memory */
static void rb_tidy_free(void *ptr)
{
  tidyRelease(ptr);
}

/* create a new tidy doc */
static VALUE rb_tidy_new(VALUE class, VALUE hash)
{
  VALUE argv[1];
  TidyDoc tdoc = tidyCreate();
  VALUE tdata = Data_Wrap_Struct(class, 0, rb_tidy_free, (struct _TidyDoc *)tdoc);
  argv[0] = hash;
  rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

/* parse the given input and return the tidy errors and output */
static VALUE rb_tidy_parse(VALUE self, VALUE input)
{
  VALUE array;
  VALUE access;
  VALUE errors;

  TidyDoc tdoc;
  TidyBuffer output;
  TidyBuffer errbuf;
  int status          = 0;

  /* See platform.h, opaque_type for typedef convention */
  Data_Get_Struct(self, struct _TidyDoc, tdoc);

  tidyBufInit( &output );
  tidyBufInit( &errbuf );

  array = rb_ary_new();

  status = tidySetErrorBuffer( tdoc, &errbuf );

  access = rb_iv_get(self, "@access");
  tidyOptSetInt( tdoc, TidyAccessibilityCheckLevel, NUM2UINT(access));

  if (status >= 0) {

    int is_input_source = 0;

    is_input_source =
      rb_respond_to(input, rb_intern("eof")) == Qtrue &&
      rb_respond_to(input, rb_intern("getc")) == Qtrue &&
      rb_respond_to(input, rb_intern("ungetc")) == Qtrue;

    if (is_input_source != 0) {
      TidyInputSource source;

      tidyInitSource(&source, (void *)&input,
        (TidyGetByteFunc)rb_tidyGetByte,
        (TidyUngetByteFunc)rb_tidyUngetByte,
        (TidyEOFFunc)rb_tidyIsEOF);

      status = tidyParseSource(tdoc, &source);
    } else {
      status = tidyParseString(tdoc, StringValue(input));
    }
  }

  if (status >= 0)
    status = tidyCleanAndRepair( tdoc );
  if (status >= 0)
    status = tidyRunDiagnostics( tdoc );
  if (status >= 0)
    status = tidySaveBuffer( tdoc, &output );

  errors = rb_str_split(rb_str_new2(errbuf.bp), "\n");

  rb_iv_set(self, "@errors", errors);

  rb_ary_store(array, 0, errors);
  rb_ary_store(array, 1, rb_str_new2(output.bp));

  tidyBufFree( &output );
  tidyBufFree( &errbuf );

  return array;
}

static VALUE rb_tidy_init(VALUE self)
{
  VALUE access = INT2NUM(4);
  VALUE errors = rb_ary_new();

  rb_iv_set(self, "@access", access);
  rb_iv_set(self, "@errors", errors);

  return self;
}

static VALUE rb_tidy_open(VALUE class, VALUE hash)
{
  VALUE tidy = rb_tidy_new(class, hash);

  if (rb_block_given_p()) {
    rb_yield(tidy);
  }

  return tidy;
}

static VALUE rb_tidy_clean(VALUE self, VALUE input)
{
  VALUE array;

  array = rb_tidy_parse(self, input);

  return rb_ary_entry(array, 1);
}

void Init_tidy()
{
  cTidy = rb_define_class("Tidy", rb_cObject);
  rb_define_singleton_method(cTidy, "new", rb_tidy_new, 0);
  rb_define_singleton_method(cTidy, "open", rb_tidy_open, 1);

  rb_define_method(cTidy, "parse", rb_tidy_parse, 1);
  rb_define_method(cTidy, "initialize", rb_tidy_init, 0);
  rb_define_method(cTidy, "clean", rb_tidy_clean, 1);

  rb_define_attr(cTidy, "access", 1, 1);
  rb_define_attr(cTidy, "errors", 1, 0);
}
