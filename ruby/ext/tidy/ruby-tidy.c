#include <ruby.h>
#include "tidy.h"
#include "buffio.h"

static uint rb_tidyGetByte( TidyInputSource* source );
static void rb_tidyUngetByte( TidyInputSource* source, uint byteValue );
static Bool rb_tidyIsEOF( TidyInputSource* source );

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
  VALUE tdata = Data_Wrap_Struct(class, 0, rb_tidy_free, tdoc);
  argv[0] = hash;
  //rb_obj_call_init(tdata, 1, argv);
  return tdata;
}

/* parse the given input and return the tidy output, and the errors */
static VALUE rb_tidy_parse(VALUE self, VALUE input)
{
  VALUE array;

  TidyDoc tdoc;
  TidyBuffer output;
  TidyBuffer errbuf;
  TidyInputSource source;
  int status          = 0;

  Data_Get_Struct(self, TidyDoc, tdoc);

  tidyBufInit( &output );
  tidyBufInit( &errbuf );

  array = rb_ary_new();

  status = tidySetErrorBuffer( tdoc, &errbuf );

  if (status >= 0) {
    if (rb_respond_to(input, rb_intern("getc"))) {
      tidyInitSource(&source, (void *)&input, rb_tidyGetByte, rb_tidyUngetByte, rb_tidyIsEOF);
      status = tidyParseSource(tdoc, &source);
    } else {
      status = tidyParseString(tdoc, StringValuePtr(input));
    }
  }

  if (status >= 0)
    status = tidyCleanAndRepair( tdoc );
  if (status >= 0)
    status = tidyRunDiagnostics( tdoc );
  if (status >= 0)
    status = tidySaveBuffer( tdoc, &output );

  rb_ary_store(array, 0, rb_str_split(rb_str_new2(errbuf.bp), "\n"));
  rb_ary_store(array, 1, rb_str_new2(output.bp));

  tidyBufFree( &output );
  tidyBufFree( &errbuf );

  return array;
}

void
Init_tidy()
{
  cTidy = rb_define_class("Tidy", rb_cObject);
  rb_define_singleton_method(cTidy, "new", rb_tidy_new, 0);
  rb_define_method(cTidy, "parse", rb_tidy_parse, 1);
}

static uint rb_tidyGetByte( TidyInputSource* source )
{
  VALUE *data = (VALUE *)source->sourceData;
  VALUE value = rb_funcall(data, rb_intern("getc"), 0, NULL);
  return NUM2INT(value);
}

static void rb_tidyUngetByte( TidyInputSource* source, uint byteValue )
{
  VALUE *data = (VALUE *)source->sourceData;
  VALUE value = rb_funcall(data, rb_intern("ungetc"), 1, INT2NUM(byteValue));
}

static Bool rb_tidyIsEOF( TidyInputSource* source )
{
  VALUE *data = (VALUE *)source->sourceData;
  VALUE value = rb_funcall(data, rb_intern("eof"), 0, NULL);
  return value == 0 ? no : yes;
}
