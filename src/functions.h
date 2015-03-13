#ifndef PHP_OBABEL_FUNCTIONS
#define PHP_OBABEL_FUNCTIONS

#include <openbabel/obconversion.h>

bool obabel_function_convert(const char* s_input, const char* s_input_format, char* s_output, const char* s_output_format);

#endif
