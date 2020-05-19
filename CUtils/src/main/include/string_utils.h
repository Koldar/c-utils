/*
 * string_utils.h
 *
 * @date Apr 2, 2019
 * @author koldar
 */

#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include "macros.h"

/**
 * Like ::snprintf but automatically checks for errors and/or for buffer overflow
 *
 * @note
 * This function has been created because starting from GCC 7.1 unhandled truncation
 * generates a warning (format-truncation)
 *
 * @param[inout]
 *
 * @see https://stackoverflow.com/a/51536663/1887602
 */
int cuSnprintf(CU_NOTNULL char* buffer, int maxSize, CU_NOTNULL const char* format, ...);

/**
 * check if a string contains another substring (case sensitive)
 *
 * @param[in] str the string where we need to look for @c substring
 * @param[in] substring the substring we're looking for in @c str
 * @return
 *  @li true if @c sbustring is inside @c str,
 *  @li false otherwise
 */
bool cuIsStrContains(CU_NOTNULL const char* str, CU_NOTNULL const char* substring);

/**
 * check if a string contains another substring (case unsensisitive)
 *
 * @param[in] str the string where we need to look for @c substring
 * @param[in] substring the substring we're looking for in @c str
 * @return
 *  @li true if @c sbustring is inside @c str,
 *  @li false otherwise
 */
bool cuIsStrCaseContains(CU_NOTNULL const char* str, CU_NOTNULL const char* substring);

/**
 * convert a string into lower case
 *
 * @code
 * cuStrToLowerCase("HeLlO WoRlD!"); //return hello world!
 * @endcode
 *
 * @param[in] str the string to convert
 * @param[inout] output a **new malloced** string containing the lowercase version of str
 */
void cuStrToLowerCase(CU_NOTNULL const char* restrict str, CU_NOTNULL char** restrict output);

/**
 * convert a string into upper case
 *
 * @code
 * cuStrToLowerCase("HeLlO WoRlD!"); //return HELLO WORLD!
 * @endcode
 *
 * @param[in] str the string to convert
 * @param[inout] output a **new malloced** string containing the uppercase version of str
 */
void cuStrToUpperCase(CU_NOTNULL const char* restrict str, CU_NOTNULL char** restrict output);

/**
 * @param[in] str1 the first string to check
 * @param[in] str2 the second string to check
 * @return
 * 	\li true if the 2 strings are the same;
 * 	\li false otherwise
 */
bool isStrEqual(CU_NOTNULL const char* str1, CU_NOTNULL const char* str2);

/**
 * @param[in] str1 the first string to check
 * @param[in] str2 the second string to check
 * @return
 * 	\li true if the 2 strings are the same without considering the case of the letters;
 * 	\li false otherwise
 */
bool isStrCaseEqual(CU_NOTNULL const char* str1, CU_NOTNULL const char* str2);

/**
 * replace all the occurences of a character with another one in a string
 *
 * For example the string "abcbdb" will be "azczdz" if you replace "b" with "z". The input string
 * won't be modified at all
 *
 * @param[out] dest the string that will be modified. Nothing is made to ensure \c dest is large enough to contain the \c src modified.
 * 					At the end of the function \c dest will contain the output
 * @param[in] src the string where we're looking into
 * @param[in] toReplace the character we need to replace in \c src
 * @param[in] replacement the character to substitute to \c toReplace
 */
void strReplace(char* restrict dest, const char* restrict src, char toReplace, char replacement);

/**
 * Check if a string ends with another one (case sensitive)
 *
 * @code
 * 	isStrEndsWith("hello", "lo"); //yes
 * 	isStrEndsWith("hello!", "lo"); //no
 * @endcode
 *
 * @param[in] stringToConsider th3e string we need to look at its end
 * @param[in] endString the string which should be present at the end of @c stringToConsider
 * @return
 *  @li true if @c stringToConsider ends with @c endString;
 *  @li false otherwise
 */
bool cuIsStrEndsWith(CU_NOTNULL const char* stringToConsider, CU_NOTNULL const char* endString);

#endif /* STRING_UTILS_H_ */
