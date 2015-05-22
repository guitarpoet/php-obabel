#include "obabel.h"
#include "functions.h"
#include <string.h>
#include <strstream>
#include <openbabel/descriptor.h>

/*******************************************************************************
 *
 *  Function obabel_version
 *
 *  This function will return the version of the open babel
 *
 *  @version 1.0
 *
 *******************************************************************************/
PHP_FUNCTION(obabel_version) {
#ifdef HAVE_OBABEL_RELEASE_VERSION
	RETURN_STRING(OpenBabel::OBReleaseVersion().c_str(), true);
#else
	RETURN_STRING("1.0", true);
#endif
}

/*******************************************************************************
 *
 *  Function obabel_format_exists
 *
 *  This function will test if the format is exists in obabel
 *
 *  @version 1.0
 *
 *******************************************************************************/
PHP_FUNCTION(obabel_format_exists) {
	char* s_format = NULL;
	int format_length = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &s_format, &format_length) == SUCCESS) {
		OpenBabel::OBConversion conv;
		if(conv.FindFormat(s_format)) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

/*******************************************************************************
 *
 *  Function obabel_convert
 *
 *  This function will convert the input string to output format
 *
 *  @version 1.0
 *
 *******************************************************************************/
PHP_FUNCTION(obabel_convert) {
	char* s_input = NULL;
	int input_length = 0;
	char* s_input_format = NULL;
	int input_format_length = 0;
	char* s_output_format = NULL;
	int output_format_length = 0;
	zval* pzv_gen_options;
	zval* pzv_in_options;
	zval* pzv_out_options;


	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssaaa", &s_input, &input_length,
		&s_input_format, &input_format_length, 
		&s_output_format, &output_format_length, &pzv_gen_options, &pzv_in_options, &pzv_out_options) == SUCCESS) {

		std::ostringstream output;
		std::istringstream input(s_input);
		obabel_function_convert(&input, s_input_format, &output, s_output_format, pzv_gen_options,
				pzv_in_options, pzv_out_options);

		std::string s_output = output.str();
		RETURN_STRING(s_output.c_str(), true);
	}
}

/*******************************************************************************
 *
 *  Function obabel_mol
 *
 *  This function will convert the input string to output format
 *
 *  @version 1.0
 *
 *******************************************************************************/
PHP_FUNCTION(obabel_mol) {
	char* s_input = NULL;
	int input_length = 0;
	zval* pzv_desc = NULL;
	zval* pzv_out = NULL;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "saa", &s_input, &input_length,
		&pzv_desc, &pzv_out) == SUCCESS) {

		std::istringstream input(s_input);
		std::ostringstream output;

		OpenBabel::OBConversion conv(&input, &output);
		conv.SetInFormat("mol");

		OpenBabel::OBMol mol;

		if(conv.Read(&mol)) {
			// Add the descriptors
			HashTable* ht = Z_ARRVAL_P(pzv_desc);
			HashPosition position;
			zval **data = NULL;

			for (zend_hash_internal_pointer_reset_ex(ht, &position);
				 zend_hash_get_current_data_ex(ht, (void**) &data, &position) == SUCCESS;
				 zend_hash_move_forward_ex(ht, &position)) {

				 char *key = NULL;
				 uint  klen;
				 ulong index;

				 if (zend_hash_get_current_key_ex(ht, &key, &klen, &index, 0, &position) == HASH_KEY_IS_LONG) {
					// This is only the value
					if(Z_TYPE_P(*data) == IS_STRING) {
						const char* s_desc = (const char*) Z_STRVAL_P(*data);
						OpenBabel::OBDescriptor* p_descr = OpenBabel::OBDescriptor::FindType(s_desc);
						if(p_descr) {
							p_descr->PredictAndSave(&mol, NULL);
						}
					}
				 } 
			}
			
			// Add basic properties
			add_assoc_string(pzv_out, "title", (char*) mol.GetTitle(), true);
			add_assoc_long(pzv_out, "atoms", mol.NumAtoms());
			add_assoc_long(pzv_out, "bonds", mol.NumBonds());
			add_assoc_long(pzv_out, "heavy atoms", mol.NumHvyAtoms());
			add_assoc_long(pzv_out, "residues", mol.NumResidues());
			add_assoc_long(pzv_out, "rotors", mol.NumRotors());
			add_assoc_long(pzv_out, "total charge", mol.GetTotalCharge());
			add_assoc_long(pzv_out, "total spin multiplicity", mol.GetTotalSpinMultiplicity());
			add_assoc_double(pzv_out, "energy", mol.GetEnergy());
			add_assoc_double(pzv_out, "mol weight", mol.GetMolWt());
			add_assoc_double(pzv_out, "exact mass", mol.GetExactMass());
			add_assoc_string(pzv_out, "formula", (char*) mol.GetFormula().c_str(), true);
			add_assoc_string(pzv_out, "formula spaced", (char*) mol.GetSpacedFormula().c_str(), true);

			// Add all the propertymaps
			std::vector<OpenBabel::OBGenericData*> vdata = mol.GetData();
			std::vector<OpenBabel::OBGenericData*>::iterator k;
			for (k = vdata.begin();k != vdata.end();++k) {
				std::string s_k = (*k)->GetAttribute();
				std::string s_v =((OBPairData*)(*k))->GetValue();
				if ((*k)->GetDataType() == OpenBabel::OBGenericDataType::PairData) {
					add_assoc_string(pzv_out, s_k.c_str(), (char *)s_v.c_str(), true);
				}
			}

			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}

/*******************************************************************************
 *
 *  Function obabel_fragment
 *
 *  This function will fragment the smiles into fragments
 *
 *  @version 1.0
 *
 *******************************************************************************/
PHP_FUNCTION(obabel_fragment) {
	char* s_mol = NULL;
	int mol_length = 0;
	zval* pzv_out;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &s_mol, &mol_length,
		&pzv_out) == SUCCESS) {
		std::istringstream input(s_mol);
		std::ostringstream output;

		OpenBabel::OBConversion conv(&input, &output);
		conv.SetInFormat("smiles");

		OpenBabel::OBMol mol;

		if(conv.Read(&mol)) {
			obabel_function_fragment(&mol, pzv_out);
		}
	}

	RETURN_TRUE;
}


/*******************************************************************************
 *
 *  Open SDF function
 *
 *  This function will try to open an sdf file to read mol data
 *
 *  @version 1.0
 *
 *******************************************************************************/
PHP_FUNCTION(obabel_read_sdf) {
	char* s_filename = NULL;
	int filename_length = 0;
	zval* pzv_out = NULL;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &s_filename, &filename_length, &pzv_out) == SUCCESS) {
		// Setup the input and output stream
		std::ifstream s_in(s_filename);
		std::ostringstream s_out;
		OpenBabel::OBConversion conv(&s_in, &s_out);
		conv.SetInAndOutFormats("SDF", "MOL");

		OpenBabel::OBMol mol; 
		while(conv.Read(&mol)) {
			// Make the array variable
			zval* pzv_item = NULL;
			MAKE_STD_ZVAL(pzv_item);
			array_init(pzv_item);

			conv.Write(&mol);

			// Get the mol output
			std::string s = s_out.str();

			// Add the mol to the output
			add_assoc_string(pzv_item, "mol", (char*) s.c_str(), true);

			// Getting all the data into PHP
			std::vector<OpenBabel::OBGenericData*> datas = mol.GetAllData(1);

            for(std::vector<OpenBabel::OBGenericData*>::iterator i = datas.begin(); i != datas.end(); i++) {
				OpenBabel::OBGenericData* data = *i;
				add_assoc_string(pzv_item, (char*) strdup(data->GetAttribute().c_str()), (char*) data->GetValue().c_str(), true);
			}

			// Clear the string output stream
			s_out.str("");

			// Add this item to the result array
			add_next_index_zval(pzv_out, pzv_item);
		}
		RETURN_TRUE;
	}

    RETURN_FALSE;
}


/*******************************************************************************
 *
 *  Init Function obabel
 *
 *  This function will init the open babel
 *
 *  @version 1.0
 *
 *******************************************************************************/
static PHP_MINIT_FUNCTION(obabel) {
    REGISTER_INI_ENTRIES();
    return SUCCESS;
}

/*******************************************************************************
 *
 *  Info Function
 *
 *  This function will show the plugins information
 *
 *  @version 1.0
 *
 *******************************************************************************/
static PHP_MINFO_FUNCTION(obabel) {
    php_info_print_table_start();
    php_info_print_table_row(2, "Revision", PHP_OBABEL_VERSION);
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();    
}

static PHP_MSHUTDOWN_FUNCTION(obabel) {
    UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

static zend_function_entry obabel_functions[] = {
    PHP_FE(obabel_version, NULL)   
    PHP_FE(obabel_convert, NULL)   
    PHP_FE(obabel_fragment, NULL)   
    PHP_FE(obabel_format_exists, NULL)   
    PHP_FE(obabel_read_sdf, NULL)
    PHP_FE(obabel_mol, NULL)
	NULL, NULL, NULL
};
  
zend_module_entry obabel_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,    
#endif
    PHP_OBABEL_EXTNAME,         
    obabel_functions,           
    PHP_MINIT(obabel),
	PHP_MSHUTDOWN(obabel),	
    NULL,
    NULL,
	PHP_MINFO(obabel),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_OBABEL_VERSION,         
#endif
    STANDARD_MODULE_PROPERTIES 
};
  
#ifdef COMPILE_DL_OBABEL
ZEND_GET_MODULE(obabel)
#endif
