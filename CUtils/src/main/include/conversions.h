/**
 * @file
 *
 * A module containing all the function used to convert from and to basic C data.
 *
 * Think of this module as a easy frontend to "strtoX" functions available in C.
 *
 * All the utilities are the same and can be used as follows:
 *
 * @code
 *
 *  int output;
 *  //this conversion doesn't work. output is unchanged
 * 	if (!cuConvertString2Int("ciao", &output)) {
 * 		printf(stderr, "error!\n");
 * 	}
 * 	//this conversion works. output now is 4
 * 	if (!cuConvertString2Int("4", &output)) {
 * 		printf(stderr, "error!\n");
 * 	}
 * @endcode
 *
 *
 * @date Sep 15, 2017
 * @author koldar
 */

#ifndef CONVERSIONS_H_
#define CONVERSIONS_H_

#include <stdbool.h>
#include "macros.h"

/**
 * Convert a string to a float
 *
 * @param[in] str the string to convert
 * @param[out] output if the function returns \c true, the pointer is populated with the converted value
 * @return
 * 	\li true if we were able to convert the string;
 * 	\li false otherwise
 */
bool cuConvertString2Float(CU_NOTNULL const char* str, CU_NOTNULL float* output);

/**
 * Convert a string to a double
 *
 * @param[in] str the string to convert
 * @param[out] output if the function returns \c true, the pointer is populated with the converted value
 * @return
 * 	\li true if we were able to convert the string;
 * 	\li false otherwise
 */
bool cuConvertString2Double(CU_NOTNULL const char* str, CU_NOTNULL double* output);

/**
 * Convert a string to a int
 *
 * @param[in] str the string to convert
 * @param[out] output if the function returns \c true, the pointer is populated with the converted value
 * @return
 * 	\li true if we were able to convert the string;
 * 	\li false otherwise
 */
bool cuConvertString2Int(CU_NOTNULL const char* str, CU_NOTNULL int* output);

/**
 * Convert a string to a long
 *
 * @param[in] str the string to convert
 * @param[out] output if the function returns \c true, the pointer is populated with the converted value
 * @return
 * 	\li true if we were able to convert the string;
 * 	\li false otherwise
 */
bool cuConvertString2Long(CU_NOTNULL const char* str, CU_NOTNULL long* output);

/**
 * Convert a string to a boolean
 *
 * A string is considered to be "true" if is equal (case insensitive) to either one of those:
 * \li true;
 * \li yes;
 * \li ok;
 * \li 1;
 *
 * A string is considered to be "false" if is equal (case insensitive) to either one of those:
 * \li false;
 * \li no;
 * \li ko;
 * \li 0;
 *
 * @param[in] str the string to convert
 * @param[out] output if the function returns \c true, the pointer is populated with the converted value
 * @return
 * 	\li true if we were able to convert the string;
 * 	\li false otherwise
 */
bool cuConvertString2Bool(CU_NOTNULL const char* str, CU_NOTNULL bool* output);


#endif /* CONVERSIONS_H_ */
