#include "functions.h"

bool obabel_function_convert(const char* s_input, const char* s_input_format, char* s_output, const char* s_output_format) {
	sprintf(s_output, "input %s, format %s, output format %s", s_input, s_input_format, s_output_format);
	return true;
}
