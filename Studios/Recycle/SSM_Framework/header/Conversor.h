#ifndef CONVERSOR_DE_STRINGS
#define CONVERSOR_DE_STRINGS

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

using namespace std;

/*	convertToInt e convertToFloat ---> baseados no exemplo de 
 *	http://www.parashift.com/c++-faq-lite/misc-technical-issues.html#faq-39.1
 *	acessado em 09/09/2008
 */

inline int convertStringToInt( const char *string_valor_inteiro ){
	istringstream inteiro( string_valor_inteiro );
	int return_int = 0;
	if( !( inteiro >> return_int )){
		cout << "OBJ_Reader ERRO: não foi possível converter a string " << string_valor_inteiro
			<< " para um valor inteiro. ";
	}
	return return_int;
};

inline float convertStringToFloat( const char *string_valor_float ){
	istringstream value( string_valor_float );
	float return_float = 0.f;
	if( !( value >> return_float ) ){
		cout << "OBJ_Reader ERRO: não foi possível converter a string " << string_valor_float
			<< " para um valor float. ";
	}
	return return_float;
};

#endif

