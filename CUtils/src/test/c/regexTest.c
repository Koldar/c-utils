/*
 * regexTest.c
 *
 *  Created on: May 18, 2017
 *      Author: koldar
 */

#include <assert.h>
#include "regularExpression.h"
#include "CuTest.h"
#include <stdbool.h>
#include <string.h>
#include "string_utils.h"
#include "log.h"

///Test simple regex
void testRegex01(CuTest* tc) {
	char* string = "Hello wo123rld!";
	char** groups = NULL;

	bool r = cuRegularExpressionApply(string, "o[0-9][0-9]*r", 0, NULL);
	assert(r == true);


}

void testRegex02(CuTest* tc) {
	char* string = "Hello wo123rld!";
	char** groups = NULL;

	bool r = cuRegularExpressionApply(string, "o[[:digit:]][[:digit:]]*r", 0, NULL);
	assert(r == true);
}

void testRegex03(CuTest* tc) {
	char* string = "Hello wo123rld!";
	const char *const * groups = NULL;

	bool r = cuRegularExpressionApply(string, "o\\([[:digit:]][[:digit:]]*\\)r", 1, &groups);
	assert(r == true);

	info("group 0 \"%s\"", (groups[0]));
	info("group 1 \"%s\"", (groups[1]));
	assert(strcmp(groups[0], "o123r") == 0);
	assert(strcmp(groups[1], "123") == 0);

	cuRegularExpressionDestroyGroupInfo(1, &groups);
}

//doesn't match
void testRegex04(CuTest* tc) {
	char* string = "Hello world!";
	char** groups = NULL;

	bool r = cuRegularExpressionApply(string, "o[[:digit:]][[:digit:]]*r", 0, NULL);
	assert(r == false);
}

//regex syntax error
void testRegex05(CuTest* tc) {
	char* string = "Hello wo123rld!";
	char** groups = NULL;

	bool r = cuRegularExpressionApply(string, "o[:digit:][:digit:]*r", 0, NULL);
	assert(r == false);
}

//test capturing group
void testRegex06(CuTest* tc) {
	char* string = "Hello wo123rld!";
	const char *const * groups = NULL;

	bool r = cuRegularExpressionApply(string, "o" CU_RE_CG("[[:digit:]][[:digit:]]*") "r", 1, &groups);
	assert(r == true);

	info("group 0 \"%s\"", (groups[0]));
	info("group 1 \"%s\"", (groups[1]));
	assert(isStrEqual(groups[0], "o123r"));
	assert(isStrEqual(groups[1], "123"));

	cuRegularExpressionDestroyGroupInfo(1, &groups);
}

//test pipe
void testRegex07(CuTest* tc) {
	char* string = "Hello wo123rld!";
	const char *const * groups = NULL;

	bool r = cuRegularExpressionApply(string, "o" CU_RE_PIPE(CU_RE_DIGIT, "r")"*" "l", 0, &groups);
	assert(r == true);
}

CuSuite* CuRegexSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testRegex01);
	SUITE_ADD_TEST(suite, testRegex02);
	SUITE_ADD_TEST(suite, testRegex03);
	SUITE_ADD_TEST(suite, testRegex04);
	SUITE_ADD_TEST(suite, testRegex05);
	SUITE_ADD_TEST(suite, testRegex06);
	SUITE_ADD_TEST(suite, testRegex07);

	return suite;
}

