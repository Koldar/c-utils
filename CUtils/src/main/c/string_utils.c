/*
 * string_utils.c
 *
 *  Created on: Apr 2, 2019
 *      Author: koldar
 */

#include <stdarg.h>
#include "errors.h"

#include "string_utils.h"

#include "errors.h"

int cuSnprintf(CU_NOTNULL char* buffer, int maxSize, CU_NOTNULL const char* format, ...) {

	va_list list;
	va_start(list, format);
	int result = vsnprintf(buffer, maxSize, format, list);
	va_end(list);

	if (result < 0) {
		CU_ERROR_PRINTF();
	}
	if (result >= maxSize) {
		CU_ERROR_PRINTF_BUFFEROVERFLOW();
	}

	return result;
}

void strReplace(char* restrict dest, const char* restrict src, char toReplace, char replacement) {
	while(*src != '\0') {
		*dest = (*src == toReplace) ? replacement : *src;
		dest++;
		src++;
	}
	//copy null character
	*dest = *src;
}

bool cuIsStrEndsWith(CU_NOTNULL const char* stringToConsider, CU_NOTNULL const char* endString) {
	if (stringToConsider == endString) {
		return true;
	}

	int len1 = strlen(stringToConsider);
	int len2 = strlen(endString);

	if (len2 > len1) {
		return false;
	}

	for (int i=(len2-1); i>=0; i--) {
		if (stringToConsider[len1-len2+i] != endString[i]) {
			return false;
		}
	}

	return true;
}

bool cuIsStrContains(CU_NOTNULL const char* str, CU_NOTNULL const char* substring) {
	return strstr(str, substring) != NULL;
}

void cuStrToLowerCase(CU_NOTNULL const char* restrict str, CU_NOTNULL char** restrict output) {
	*output = strdup(str);
	for (int i=0; i<strlen(str); i++) {
		(*output)[i] = tolower(str[i]);
	}
}

void cuStrToUpperCase(CU_NOTNULL const char* restrict str, CU_NOTNULL char** restrict output) {
	*output = strdup(str);
	for (int i=0; i<strlen(str); i++) {
		(*output)[i] = toupper(str[i]);
	}
}

bool cuIsStrCaseContains(CU_NOTNULL const char* str, CU_NOTNULL const char* substring) {
	char* str2;
	char* substring2;

	cuStrToLowerCase(str, &str2);
	cuStrToLowerCase(substring, &substring2);

	bool result = cuIsStrContains(str2, substring2);

	CU_FREE(str2);
	CU_FREE(substring2);

	return result;
}

bool isStrEqual(CU_NOTNULL const char* str1, CU_NOTNULL const char* str2) {
	if (str1 == str2) {
		return true;
	}

	while (true) {
		if (*str1 == 0 && *str2 == 0) {
			return true;
		}

		if (*str1 != *str2) {
			return false;
		}
		str1++;
		str2++;
	}
}

bool isStrCaseEqual(CU_NOTNULL const char* str1, CU_NOTNULL const char* str2) {
	if (str1 == str2) {
		return true;
	}

	while (true) {
		if (*str1 == 0 && *str2 == 0) {
			return true;
		}

		if (tolower(*str1) != tolower(*str2)) {
			return false;
		}
		str1++;
		str2++;
	}
}
