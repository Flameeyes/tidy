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

/* called when iterating over options hash */
static VALUE rb_tidy_set_option(VALUE arg1, VALUE arg2)
{
  VALUE key, value;
  TidyDoc tdoc;
  TidyOption opt;
  Bool status = no;

  /* See platform.h, opaque_type for typedef convention */
  Data_Get_Struct(arg2, struct _TidyDoc, tdoc);

  key   = rb_ary_entry(arg1, 0);
  value = rb_ary_entry(arg1, 1);

  /* if the key is a symbol, convert it to a string
   * and replace the underscores '_' with minuses '-'
   */
  if (TYPE(key) == T_SYMBOL) {
    char *ckey, *ptr;

    ckey = (char *) rb_id2name(SYM2ID(key));

    for(ptr = ckey; *ptr != '\0'; ptr++) {
      if (*ptr == '_') {
        *ptr = '-';
      }
    }

    opt = tidyGetOptionByName(tdoc, ckey);
  } else {
    opt = tidyGetOptionByName(tdoc, StringValuePtr(key));
  }

  if ( opt != NULL ) {
    TidyOptionId optId = tidyOptGetId(opt);

    switch(TYPE(value)) {
      case T_FALSE:
        status = tidyOptSetBool(tdoc, optId, no);
        break;
      case T_TRUE:
        status = tidyOptSetBool(tdoc, optId, yes);
        break;
      case T_FIXNUM:
        status = tidyOptSetInt(tdoc, optId, NUM2ULONG(value));
        break;
      default:
        status = tidyOptSetValue(tdoc, optId, StringValuePtr(value));
        break;
    }
  }

  return Qnil;
}

/* create a new tidy doc */
static VALUE rb_tidy_new(int argc, VALUE *argv, VALUE class)
{
  TidyDoc tdoc = tidyCreate();
  VALUE options;
  VALUE access = INT2NUM(4);
  VALUE errors = rb_ary_new();

  VALUE self = Data_Wrap_Struct(class, 0, rb_tidy_free, (struct _TidyDoc *)tdoc);

  rb_scan_args(argc, argv, "01", &options);
  options = NIL_P(options) ? rb_hash_new() : options;

  rb_iv_set(self, "@options", options);
  rb_iv_set(self, "@access", access);
  rb_iv_set(self, "@errors", errors);

  rb_iterate(rb_each, options, rb_tidy_set_option, self);

  rb_obj_call_init(self, 0, NULL);

  return self;
}

/* parse the given input and return the tidy errors and output */
static VALUE rb_tidy_parse(VALUE self, VALUE input)
{
  VALUE array;
  VALUE access;
  VALUE errors;
  VALUE options;

  TidyDoc tdoc;
  TidyBuffer output;
  TidyBuffer errbuf;
  int status          = 0;

  int contentErrors   = 0;
  int contentWarnings = 0;
  int accessWarnings  = 0;

  /* See platform.h, opaque_type for typedef convention */
  Data_Get_Struct(self, struct _TidyDoc, tdoc);

  tidyBufInit( &output );
  tidyBufInit( &errbuf );

  array = rb_ary_new();

  status = tidySetErrorBuffer( tdoc, &errbuf );

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
      status = tidyParseString(tdoc, StringValuePtr(input));
    }
  }

  if (status >= 0)
    status = tidyCleanAndRepair( tdoc );
  if (status >= 0)
    status = tidyRunDiagnostics( tdoc );
  if (status >= 0)
    tidyErrorSummary( tdoc );
  if (status >= 0)
    tidyGeneralInfo( tdoc );

  if (status >= 0)
    status = tidySaveBuffer( tdoc, &output );

  contentErrors   = tidyErrorCount( tdoc );
  contentWarnings = tidyWarningCount( tdoc );
  accessWarnings  = tidyAccessWarningCount( tdoc );

  if (contentErrors > 0 || contentWarnings > 0) {
    errors = rb_str_new2(errbuf.bp);
  } else {
    errors = rb_str_new2("");
  }

  rb_iv_set(self, "@errors", errors);

  rb_ary_store(array, 0, errors);
  rb_ary_store(array, 1, rb_str_new2(output.bp));

  tidyBufFree( &output );
  tidyBufFree( &errbuf );

  return array;
}

static VALUE rb_tidy_init(VALUE self)
{
  return self;
}

/* Create a tidy object */
static VALUE rb_tidy_open(VALUE class, VALUE options)
{
  VALUE args[1];
  VALUE tidy;

  args[0] = options;
  tidy  = rb_tidy_new(1, args, class);

  if (rb_block_given_p()) {
    rb_yield(tidy);
  }

  return tidy;
}

/* Given a string, returns the string after tidying */
static VALUE rb_tidy_clean(VALUE self, VALUE input)
{
  VALUE array;

  array = rb_tidy_parse(self, input);

  return rb_ary_entry(array, 1);
}

/* For sideways compatibility */
static VALUE rb_tidy_path_get(VALUE self)
{
  VALUE path;
  path = rb_cv_get(self, "@@path");
  return path;
}

/* For sideways compatibility */
static VALUE rb_tidy_path_set(VALUE self, VALUE path)
{
  rb_cv_set(self, "@@path", path);
  return Qnil;
}

void Init_tidy()
{
  cTidy = rb_define_class("Tidy", rb_cObject);

  rb_define_class_variable(cTidy, "@@path", rb_str_new2("tidy-is-built-in"));

  rb_define_singleton_method(cTidy, "new", rb_tidy_new, -1);
  rb_define_singleton_method(cTidy, "open", rb_tidy_open, 1);
  rb_define_singleton_method(cTidy, "path", rb_tidy_path_get, 0);
  rb_define_singleton_method(cTidy, "path=", rb_tidy_path_set, 1);

  rb_define_method(cTidy, "initialize", rb_tidy_init, 0);
  rb_define_method(cTidy, "parse", rb_tidy_parse, 1);
  rb_define_method(cTidy, "clean", rb_tidy_clean, 1);

  rb_define_attr(cTidy, "access", 1, 1);
  rb_define_attr(cTidy, "errors", 1, 0);
}
