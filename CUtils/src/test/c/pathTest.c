/**
 * @file
 *
 * @author koldar
 * @date Sep 11, 2018
 */

#include "CuTest.h"
#include <assert.h>
#include "path_utils.h"
#include "string_utils.h"
#include "log.h"

void test_path_01(CuTest* tc) {
	char path[BUFFER_SIZE];
	CU_PATH_GENERATE(path, BUFFER_SIZE, "home", "koldar", "file.dat");
	info("path generated is %s", path);
	assert(isStrCaseEqual("home/koldar/file.dat", path));
}

void test_path_02(CuTest* tc) {
	char path[BUFFER_SIZE];
	CU_PATH_GENERATE(path, BUFFER_SIZE, "home", "koldar", "file");
	info("path generated is %s", path);
	assert(isStrCaseEqual("home/koldar/file", path));
}

void test_path_03(CuTest* tc) {
	char path[BUFFER_SIZE];
	CU_PATH_GENERATE(path, BUFFER_SIZE, CU_PATH_ROOT, "home", "file.dat");
	info("path generated is %s", path);
	assert(isStrCaseEqual("/home/file.dat", path));
}

void test_path_04(CuTest* tc) {
	char path[BUFFER_SIZE];
	CU_PATH_GENERATE(path, BUFFER_SIZE, CU_PATH_CWD, "git", "file.dat");
	info("path generated is %s", path);
	assert(isStrCaseEqual("./git/file.dat", path));
}

void test_path_05(CuTest* tc) {
	char path[BUFFER_SIZE];
	CU_PATH_GENERATE(path, BUFFER_SIZE, CU_PATH_CWD, "git/", "file.dat");
	info("path generated is %s", path);
	assert(isStrCaseEqual("./git/file.dat", path));
}

CuSuite* CuPathSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_path_01);
	SUITE_ADD_TEST(suite, test_path_02);
	SUITE_ADD_TEST(suite, test_path_03);
	SUITE_ADD_TEST(suite, test_path_04);
	SUITE_ADD_TEST(suite, test_path_05);

	return suite;
}
