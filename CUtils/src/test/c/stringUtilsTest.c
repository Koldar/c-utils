/*
 * stringUtilsTest.c
 *
 *  Created on: Apr 5, 2019
 *      Author: koldar
 */

#include <assert.h>
#include "CuTest.h"
#include "log.h"
#include "string_utils.h"

void test_cuIsStrEndsWith_01(CuTest* tc) {
	assert(cuIsStrEndsWith("hello", "lo"));
}

void test_cuIsStrEndsWith_02(CuTest* tc) {
	assert(!cuIsStrEndsWith("hello!", "lo"));
}

void test_cuIsStrEndsWith_03(CuTest* tc) {
	assert(!cuIsStrEndsWith("hello", "lo!"));
}

void test_cuIsStrEndsWith_04(CuTest* tc) {
	assert(cuIsStrEndsWith("h", "h"));
}

void test_cuIsStrEndsWith_05(CuTest* tc) {
	assert(!cuIsStrEndsWith("h", "e"));
}

void test_cuIsStrEndsWith_06(CuTest* tc) {
	assert(!cuIsStrEndsWith("h", "he"));
}

void test_cuIsStrContains_01(CuTest* tc) {
	assert(cuIsStrContains("hello world!", "l"));
	assert(!cuIsStrContains("hello world!", "k"));
	assert(cuIsStrContains("hello world!", "lo"));
	assert(!cuIsStrContains("hello world!", "lk"));
	assert(cuIsStrContains("hello world!", ""));
	assert(cuIsStrContains("hello world!", "hello world!"));

	assert(!cuIsStrContains("hello world!", "L"));
	assert(!cuIsStrContains("hello world!", "K"));
	assert(!cuIsStrContains("hello world!", "LO"));
	assert(!cuIsStrContains("hello world!", "LK"));
	assert(cuIsStrContains("hello world!", ""));
	assert(!cuIsStrContains("hello world!", "HELLO WORLD!"));
}

void test_cuIsStrCaseContains_01(CuTest* tc) {
	assert(cuIsStrCaseContains("hello world!", "L"));
	assert(!cuIsStrCaseContains("hello world!", "K"));
	assert(cuIsStrCaseContains("hello world!", "LO"));
	assert(!cuIsStrCaseContains("hello world!", "LK"));
	assert(cuIsStrCaseContains("hello world!", ""));
	assert(cuIsStrCaseContains("hello world!", "HELLO WORLD!"));
}

void test_cuStrToLowerCase_01(CuTest* tc) {
	char* output;

	cuStrToLowerCase("hElLo", &output);
	assert(isStrEqual(output, "hello")); free(output);

	cuStrToLowerCase("H", &output);
	assert(isStrEqual(output, "h")); free(output);

	cuStrToLowerCase("", &output);
	assert(isStrEqual(output, "")); free(output);

	cuStrToLowerCase("hello", &output);
	assert(isStrEqual(output, "hello")); free(output);
}

void test_cuStrToUpperCase_01(CuTest* tc) {
	char* output;

	cuStrToUpperCase("hElLo", &output);
	assert(isStrEqual(output, "HELLO")); free(output);

	cuStrToUpperCase("h", &output);
	assert(isStrEqual(output, "H")); free(output);

	cuStrToUpperCase("", &output);
	assert(isStrEqual(output, "")); free(output);

	cuStrToUpperCase("hello", &output);
	assert(isStrEqual(output, "HELLO")); free(output);
}

CuSuite* CuStringUtilsSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_cuIsStrEndsWith_01);
	SUITE_ADD_TEST(suite, test_cuIsStrEndsWith_02);
	SUITE_ADD_TEST(suite, test_cuIsStrEndsWith_03);
	SUITE_ADD_TEST(suite, test_cuIsStrEndsWith_04);
	SUITE_ADD_TEST(suite, test_cuIsStrEndsWith_05);
	SUITE_ADD_TEST(suite, test_cuIsStrEndsWith_06);

	SUITE_ADD_TEST(suite, test_cuIsStrContains_01);
	SUITE_ADD_TEST(suite, test_cuIsStrCaseContains_01);

	SUITE_ADD_TEST(suite, test_cuStrToLowerCase_01);
	SUITE_ADD_TEST(suite, test_cuStrToUpperCase_01);

	return suite;
}

