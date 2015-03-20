#ifndef PHP_OBABEL
#define PHP_OBABEL

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#pragma GCC diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"

#define PHP_OBABEL_VERSION "0.0.1"
#define PHP_OBABEL_EXTNAME "obabel"
#define PHP_OBABEL_LIB_VERSION "4.0.2"

#include <iostream>
#include <php.h>
#include <php_ini.h>
#include <Zend/zend_exceptions.h>
#include "ext/standard/info.h"

PHP_FUNCTION(obabel_version);
PHP_FUNCTION(obabel_convert);
PHP_FUNCTION(obabel_mol);

static PHP_MINIT_FUNCTION(obabel);
static PHP_MINFO_FUNCTION(obabel);
static PHP_MSHUTDOWN_FUNCTION(obabel);

PHP_INI_BEGIN()
PHP_INI_ENTRY("obabel.author", "Jack", PHP_INI_ALL, NULL)
PHP_INI_END()

#define phpext_obabel_ptr &obabel_module_entry

#endif
