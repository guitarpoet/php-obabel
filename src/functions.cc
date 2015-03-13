#include "functions.h"

void obabel_function_set_conversion_opts(OpenBabel::OBConversion* conv, zval* pzv_options) {
	if(pzv_options) {
		HashTable* ht = Z_ARRVAL_P(pzv_options);
		HashPosition position;
		zval **data = NULL;

		// Iterating all the key and values in the context
		for (zend_hash_internal_pointer_reset_ex(ht, &position);
			 zend_hash_get_current_data_ex(ht, (void**) &data, &position) == SUCCESS;
			 zend_hash_move_forward_ex(ht, &position)) {

			 char *key = NULL;
			 uint  klen;
			 ulong index;

			 if (zend_hash_get_current_key_ex(ht, &key, &klen, &index, 0, &position) == HASH_KEY_IS_LONG) {
				// This is only the value
				if(Z_TYPE_P(*data) == IS_STRING) {
					conv->AddOption((const char*) Z_STRVAL_P(*data), OpenBabel::OBConversion::GENOPTIONS);
				}
			 } 
			 else {
				if(Z_TYPE_P(*data) == IS_STRING) {
					conv->AddOption(key, OpenBabel::OBConversion::GENOPTIONS, Z_STRVAL_P(*data));
				}
			 }
		}
	}
}

bool obabel_function_convert(std::istream* ps_input, const char* s_input_format, std::ostream* ps_output, const char* s_output_format, zval* pzv_options) {
	OpenBabel::OBConversion conv(ps_input, ps_output);
	obabel_function_set_conversion_opts(&conv, pzv_options);
	if(!conv.SetInAndOutFormats(s_input_format, s_output_format)) {
		return false;
	}
	conv.Convert();
	return true;
}
