/*
 * stringBuilderTest.c
 *
 *  Created on: May 18, 2017
 *      Author: koldar
 */

#include <assert.h>
#include "stringBuilder.h"
#include "CuTest.h"
#include "log.h"
#include <string.h>

void testStringBuilder01(CuTest* tc) {
	string_builder* sb = cuStringBulilderNew(NULL, 3);
	cuStringBuilderAppendString(sb, "hello");
	cuStringBuilderAppendString(sb, " ");
	cuStringBuilderAppendString(sb, "world");

	info("string is \"%s\"", cuStringBuilderGetString(sb));
	assert(strcmp(cuStringBuilderGetString(sb), "hello world") == 0);
	cuStringBulilderDestroy(sb, NULL);
}

void testStringBuilder02(CuTest* tc) {
	string_builder* sb = cuStringBulilderNew(NULL, 3);
	cuStringBuilderAppendString(sb, "hello");
	cuStringBuilderAppendString(sb, " ");
	cuStringBuilderAppendString(sb, "world");

	cuStringBuilderClear(sb);
	assert(strcmp(cuStringBuilderGetString(sb), "") == 0);
	cuStringBulilderDestroy(sb, NULL);
}

void testStringBuilder03(CuTest* tc) {
	string_builder* sb = cuStringBulilderNew(NULL, 3);
	cuStringBuilderAppendString(sb, "hello");
	cuStringBuilderAppendString(sb, " ");
	cuStringBuilderAppendString(sb, "world");

	cuStringBuilderClear(sb);
	cuStringBuilderAppendString(sb, "bum");
	assert(strcmp(cuStringBuilderGetString(sb), "bum") == 0);
	cuStringBulilderDestroy(sb, NULL);
}

void testStringBuilder04(CuTest* tc) {
	string_builder* sb = cuStringBulilderNew(NULL, 100);
	cuStringBuilderAppendString(sb, "hello");
	cuStringBuilderAppendString(sb, " ");
	cuStringBuilderAppendString(sb, "world");

	assert(strcmp(cuStringBuilderGetString(sb), "hello world") == 0);
	cuStringBulilderDestroy(sb, NULL);
}

void testStringBuilder05(CuTest* tc) {
	string_builder* sb = cuStringBulilderNew(NULL, 100);
	cuStringBuilderAppendTemplate(sb, "hello world %s!", "john");

	assert(strcmp(cuStringBuilderGetString(sb), "hello world john!") == 0);
	cuStringBulilderDestroy(sb, NULL);
}

CuSuite* CuStringBuilderSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testStringBuilder01);
	SUITE_ADD_TEST(suite, testStringBuilder02);
	SUITE_ADD_TEST(suite, testStringBuilder03);
	SUITE_ADD_TEST(suite, testStringBuilder04);
	SUITE_ADD_TEST(suite, testStringBuilder05);

	return suite;
}
