/*
 * conversions.c
 *
 *  Created on: Sep 15, 2017
 *      Author: koldar
 */

#include "conversions.h"
#include <errno.h>
#include <stdlib.h>
#include "string_utils.h"
#include <limits.h>
#include <math.h>
#include <errno.h>

bool cuConvertString2Float(CU_NOTNULL const char* str, CU_NOTNULL float* output) {
	char* endPtr;
	float val;

	errno = 0;
	val = strtof(str, &endPtr);

	//code inspired from "man 3 strtod"
	if ((errno == ERANGE && (val == HUGE_VALF || val == HUGE_VALL)) || (errno != 0 && val == 0)) {
		return false;
	}

	if (endPtr == str) {
		return false;
	}

	*output = val;
	return true;
}

bool cuConvertString2Double(CU_NOTNULL const char* str, CU_NOTNULL double* output) {
	char* endPtr;
	double val;

	errno = 0;
	val = strtod(str, &endPtr);

	//code inspired from "man 3 strtod"
	if ((errno == ERANGE && (val == HUGE_VALF || val == HUGE_VALL)) || (errno != 0 && val == 0)) {
		return false;
	}

	if (endPtr == str) {
		return false;
	}

	*output = val;
	return true;
}

bool cuConvertString2Int(CU_NOTNULL const char* str, CU_NOTNULL int* output) {
	long val;
	if (cuConvertString2Long(str, &val)) {
		*output = val;
		return true;
	}
	return false;
}

bool cuConvertString2Long(CU_NOTNULL const char* str, CU_NOTNULL long* output) {
	char* endPtr;
	long val;

	errno = 0;
	val = strtol(str, &endPtr, 10);

	//code copied from "man 3 strtol"
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
		return false;
	}

	if (endPtr == str) {
		return false;
	}

	*output = val;
	return true;
}

bool cuConvertString2Bool(CU_NOTNULL const char* str, CU_NOTNULL bool* output) {
	if (isStrCaseEqual(str, "true") || isStrCaseEqual(str, "yes") || isStrCaseEqual(str, "ok") || isStrEqual(str, "1")) {
		*output = true;
		return true;
	}

	if (isStrCaseEqual(str, "false") || isStrCaseEqual(str, "no") || isStrCaseEqual(str, "ko") || isStrEqual(str, "0")) {
		*output = false;
		return true;
	}

	return false;
}
