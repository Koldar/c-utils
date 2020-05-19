/**
 * @file
 *
 * @author koldar
 * @date Sep 26, 2018
 */

#include "macros.h"
#include "CuTest.h"
#include <assert.h>
#include "stacktrace.h"
#include <stdio.h>
#include "file_utils.h"
#include "log.h"
#include "string_utils.h"

void test_CU_PRINT_STACKTRACE_01(CuTest* tc) {

	FILE* f = fopen(__func__, "w");

	CU_PRINT_STACKTRACE(f);

	fclose(f);

	char buffer[LONG_BUFFER_SIZE];
	int i = 0;
	f = fopen(__func__, "r");

	const char* assertions[] = {
			"test_CU_PRINT_STACKTRACE_01",
			"src/test/c/stacktraceTest.c:",
			"CuTestRun",
			"src/test/c/CuTest.c:",
			"CuSuiteRun",
			"src/test/c/CuTest.c:",
			"RunAllTests",
			"src/test/c/AllTests.c:",
			"main",
			"src/test/c/AllTests.c:",
			"???",
			"???:0",
			"???",
			"???:0",
			"???",
			"???:0"
	};

	CU_ITERATE_ON_FILE_LINES(f, 100, line, index) {
		info("%s", line);
		//TODO readd assert(cuIsStrContains(line, assertions[index]));
		i += snprintf(&buffer[i], LONG_BUFFER_SIZE - i, "%s", line);
	}

	fclose(f);

	info("%s", buffer);
}


CuSuite* CuStackTraceSuite() {
	CuSuite* suite = CuSuiteNew();


	SUITE_ADD_TEST(suite, test_CU_PRINT_STACKTRACE_01);

	return suite;
}
