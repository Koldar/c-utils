/**
 * @file
 *
 * @author koldar
 * @date May 28, 2018
 */

#include "CuTest.h"
#include "file_utils.h"
#include <stdio.h>
#include <assert.h>
#include "log.h"
#include "string_utils.h"

#define TEST_RESOURCE_FOLDER(x) "../../src/test/resources/" x

//read empty file
void test_cuFileUtilsReadLine_01(CuTest* tc) {
	FILE* f = fopen(TEST_RESOURCE_FOLDER("emptyFile.dat"), "r");
	char buffer[100];

	enum file_outcome outcome = cuFileUtilsReadLine(f, 100, buffer);
	assert(outcome == FO_EOF);

	fclose(f);
}

//read non empty file
void test_cuFileUtilsReadLine_02(CuTest* tc) {
	FILE* f = fopen(TEST_RESOURCE_FOLDER("nonEmptyFile.dat"), "r");
	char buffer[100];
	enum file_outcome outcome;

	outcome = cuFileUtilsReadLine(f, 100, buffer);
	assert(outcome == FO_READLINE);
	info("string read is \"%s\"", buffer);
	assert(isStrEqual(buffer, "1"));

	outcome = cuFileUtilsReadLine(f, 100, buffer);
	assert(outcome == FO_READLINE);
	assert(isStrEqual(buffer, "2"));

	outcome = cuFileUtilsReadLine(f, 100, buffer);
	assert(outcome == FO_READLINE);
	assert(isStrEqual(buffer, "123"));

	outcome = cuFileUtilsReadLine(f, 100, buffer);
	assert(outcome == FO_READLINE);
	assert(isStrEqual(buffer, "456"));

	outcome = cuFileUtilsReadLine(f, 100, buffer);
	assert(outcome == FO_EOF);

	fclose(f);
}

//read single line file
void test_cuFileUtilsReadLine_03(CuTest* tc) {
	FILE* f = fopen(TEST_RESOURCE_FOLDER("oneLineFile.dat"), "r");
	char buffer[100];
	enum file_outcome outcome;

	outcome = cuFileUtilsReadLine(f, 100, buffer);
	assert(outcome == FO_READLINE);
	assert(isStrEqual(buffer, "123"));

	outcome = cuFileUtilsReadLine(f, 100, buffer);
	assert(outcome == FO_EOF);

	fclose(f);
}

void test_CU_ITERATE_ON_FILE_LINES_01(CuTest* tc) {
	FILE* f = fopen(TEST_RESOURCE_FOLDER("nonEmptyFile.dat"), "r");

	CU_ITERATE_ON_FILE_LINES(f, 100, line, i) {
		switch (i) {
		case 0: {
			assert(isStrEqual(line, "1"));
			break;
		}
		case 1: {
			assert(isStrEqual(line, "2"));
			break;
		}
		case 2: {
			assert(isStrEqual(line, "123"));
			break;
		}
		case 3: {
			assert(isStrEqual(line, "456"));
			break;
		}
		default: {
			assert(false);
		}
		}
	}

	fclose(f);
}

void test_cuFileUtilsJoin_01(CuTest* tc) {
	char buffer[300];
	assert(isStrEqual((cuFileUtilsJoin(buffer, 300, "foo"), buffer), "foo"));
	assert(isStrEqual((cuFileUtilsJoin(buffer, 300, "foo", "bar"), buffer), "foo/bar"));
	assert(isStrEqual((cuFileUtilsJoin(buffer, 300, "foo/", "bar"), buffer), "foo/bar"));
	assert(isStrEqual((cuFileUtilsJoin(buffer, 300, "foo/", "bar/"), buffer), "foo/bar/"));
	assert(isStrEqual((cuFileUtilsJoin(buffer, 300, "foo/", "/bar/"), buffer), "foo/bar/"));
	assert(isStrEqual((cuFileUtilsJoin(buffer, 300, "/foo/", "/bar/"), buffer), "/foo/bar/"));
	assert(isStrEqual((cuFileUtilsJoin(buffer, 300, "foo", "bar", ".."), buffer), "foo/bar/.."));
	assert(isStrEqual((cuFileUtilsJoin(buffer, 300, "foo", "bar/", ".."), buffer), "foo/bar/.."));
}

CuSuite* CuFileUtilsSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_cuFileUtilsReadLine_01);
	SUITE_ADD_TEST(suite, test_cuFileUtilsReadLine_02);
	SUITE_ADD_TEST(suite, test_cuFileUtilsReadLine_03);
	SUITE_ADD_TEST(suite, test_CU_ITERATE_ON_FILE_LINES_01);
	SUITE_ADD_TEST(suite, test_cuFileUtilsJoin_01);

	return suite;
}
