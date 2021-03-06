#ifndef PHP_OBABEL_FUNCTIONS
#define PHP_OBABEL_FUNCTIONS

#include <php.h>
#include <iostream>
#include <openbabel/obconversion.h>
#include "fragment.h"
#pragma GCC diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"

bool obabel_function_fragment(OpenBabel::OBMol* mol, zval* pzv_arr);
bool obabel_function_convert(std::istream* ps_input, const char* s_input_format, std::ostream* ps_output, const char* s_output_format, zval* pzv_gen_options, zval* pzv_in_options, zval* pzv_out_options);

#endif
