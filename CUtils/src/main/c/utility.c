/*
 * utility.c
 *
 *  Created on: Dec 22, 2016
 *      Author: koldar
 */


#include "utility.h"

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <ctype.h>
#include <unistd.h>
#include "regularExpression.h"
#include "conversions.h"
#include "log.h"
#include "errors.h"

FILE* finput = NULL;
FILE* foutput = NULL;

#define RANGE_REGEX "^\\(" "\\[\\|\\]\\|(" "\\)" "[[:blank:]]*" "\\(" "[0-9][0-9]*" "\\)" "[[:blank:]]*" ";" "[[:blank:]]*" "\\(" "[0-9][0-9]*" "\\)" "[[:blank:]]*" "\\(" "\\[\\|\\]\\|)" "\\)"

void cuUtilsCallSystem(CU_NOTNULL const char* template, ...) {
	char buffer[BUFFER_SIZE];

	va_list va;
	va_start(va, template);
	vsnprintf(buffer, BUFFER_SIZE, template, va);
	va_end(va);

	int exitCode = system(buffer);
	if (exitCode != 0) {
		ERROR_IMPOSSIBLE_SCENARIO("Command \"%s\" failed.", buffer);
	}
}


bool cuUtilsRangeInt(CU_NOTNULL const char* rangeStr, CU_NOTNULL int* restrict a, CU_NOTNULL int* restrict b, CU_NOTNULL bool* restrict aIncluded, CU_NOTNULL bool* restrict bIncluded) {
	const char *const * groups = NULL;

	bool r = cuRegularExpressionApply(rangeStr, RANGE_REGEX, 4, &groups);
	if (r == false) {
		return false;
	}
	//fetch the first
	const char* lowerBoundInclusionStr = groups[1];
	const char* lowerBoundStr = groups[2];
	const char* upperBoundStr = groups[3];
	const char* upperBoundInclusionStr = groups[4];

	//lower bound
	if (!cuConvertString2Int(lowerBoundStr, a)) {
		ERROR_ON_CONSTRUCTION("lower bound", "%s", lowerBoundStr);
	}

	//upper bound
	if (!cuConvertString2Int(upperBoundStr, b)) {
		ERROR_ON_CONSTRUCTION("lower bound", "%s", upperBoundStr);
	}

	//lower bound inclusion
	debug("lower bound inclusion str is \"%s\"", lowerBoundInclusionStr);
	debug("upper bound inclusion str is \"%s\"", upperBoundInclusionStr);
	switch (lowerBoundInclusionStr[0]) {
	case '[': {
		*aIncluded = true;
		break;
	}
	case ']':
	case '(': {
		*aIncluded = false;
		break;
	}
	default: {
		ERROR_INVALID_SWITCH_CASE("lower bound inclusion", "%c", lowerBoundInclusionStr[0]);
	}
	}

	//upper bound inclusion
	switch (upperBoundInclusionStr[0]) {
	case ']': {
		*bIncluded = true;
		break;
	}
	case '[':
	case ')': {
		*bIncluded = false;
		break;
	}
	default: {
		ERROR_INVALID_SWITCH_CASE("lower bound inclusion", "%c", lowerBoundInclusionStr[0]);
	}
	}

	//destroy all the groups
	cuRegularExpressionDestroyGroupInfo(4, &groups);

	return true;
}

bool cuUtilsRangeInt2(CU_NOTNULL const char* rangeStr, CU_NOTNULL struct cu_int_range* range) {
	return cuUtilsRangeInt(rangeStr, &range->a, &range->b, &range->aIncluded, &range->bIncluded);
}
