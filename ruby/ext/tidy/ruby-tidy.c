#include <ruby.h>
#include "tidy.h"
#include "buffio.h"

static VALUE cTidy;

static void rb_tidy_free(void *ptr)
{
  tidyRelease(ptr);
}

static VALUE rb_tidy_new(VALUE class, VALUE hash)
{
  VALUE argv[1];
  TidyDoc tdoc = tidyCreate();
  VALUE tdata = Data_Wrap_Struct(class, 0, rb_tidy_free, tdoc);
  argv[0] = hash;
  //rb_obj_call_init(tdata, 1, argv);
  return tdata;
}

static VALUE rb_tidy_parse(VALUE self, VALUE input)
{
  VALUE array;

  TidyDoc tdoc;
  TidyBuffer output;
  TidyBuffer errbuf;

  int status          = 0;
  int contentErrors   = 0;
  int contentWarnings = 0;
  int accessWarnings  = 0;

  Data_Get_Struct(self, TidyDoc, tdoc);

  tidyBufInit( &output );
  tidyBufInit( &errbuf );

  // array = rb_ary_new();

  status = tidySetErrorBuffer( tdoc, &errbuf );

  if (rb_respond_to(input, rb_intern("read"))) {
    // TODO
  } else {
    status = tidyParseString(tdoc, StringValue(input));
  }

  contentErrors   += tidyErrorCount( tdoc );
  contentWarnings += tidyWarningCount( tdoc );
  accessWarnings  += tidyAccessWarningCount( tdoc );

  // if (contentErrors + contentWarnings > 0)
  //     tidyErrorSummary(tdoc);

  status = tidyCleanAndRepair( tdoc );

  status = tidyRunDiagnostics( tdoc );

  // rb_ary_push(array, );

  array = rb_str_split(rb_str_new2(errbuf.bp), "\n");

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

