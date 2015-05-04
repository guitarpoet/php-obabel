#include "functions.h"
#include "fragment.h"
#include <string.h>
#include <vector>
#include <set>

bool obabel_function_fragment(OpenBabel::OBMol* mol, zval* pzv_arr) {
	Fragmentor* pf = new Fragmentor();
	Fset* pf_set = pf->fragment(mol);
	SetItr itr;
	for(itr=pf_set->begin(); itr!=pf_set->end();++itr) {
		// The fragment vector
		std::vector<int> v = (std::vector<int>) *itr;

		// Create the vector
		zval* pzv_vec = NULL;
		MAKE_STD_ZVAL(pzv_vec);
		array_init(pzv_vec);

		std::vector<int>::iterator i;

		for(i=v.begin(); i!=v.end(); ++i) {
			add_next_index_long(pzv_vec, *i);
		}

		add_next_index_zval(pzv_arr, pzv_vec);
	}
	
	return true;
}

void obabel_function_set_conversion_opts(OpenBabel::OBConversion* conv, zval* pzv_options, OpenBabel::OBConversion::Option_type ot_type) {
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
					conv->AddOption((const char*) Z_STRVAL_P(*data), ot_type);
				}
			 } 
			 else {
				if(Z_TYPE_P(*data) == IS_STRING) {
					conv->AddOption(strdup(key), ot_type, strdup(Z_STRVAL_P(*data)));
				}
			 }
		}
	}
}

bool obabel_function_convert(std::istream* ps_input, const char* s_input_format, std::ostream* ps_output, const char* s_output_format, zval* pzv_gen_options, zval* pzv_in_options, zval* pzv_out_options) {
	OpenBabel::OBConversion conv(ps_input, ps_output);
	obabel_function_set_conversion_opts(&conv, pzv_gen_options, OpenBabel::OBConversion::GENOPTIONS);
	obabel_function_set_conversion_opts(&conv, pzv_in_options, OpenBabel::OBConversion::INOPTIONS);
	obabel_function_set_conversion_opts(&conv, pzv_out_options, OpenBabel::OBConversion::OUTOPTIONS);
	if(!conv.SetInAndOutFormats(s_input_format, s_output_format)) {
		return false;
	}
	conv.Convert();
	return true;
}
