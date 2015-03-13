#include "obabel.h"
#include "functions.h"

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
	RETURN_STRING(PHP_OBABEL_LIB_VERSION, true);
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

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &s_input, &input_length,
		&s_input_format, &input_format_length, 
		&s_output_format, &output_format_length) == SUCCESS) {

		char buffer[1024];

		obabel_function_convert(s_input, s_input_format, buffer, s_output_format);

		RETURN_STRING(buffer, true);
	}
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
	PHP_FE_END
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
