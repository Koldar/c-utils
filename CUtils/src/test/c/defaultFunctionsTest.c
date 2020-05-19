/**
 * @file
 *
 * @author koldar
 * @date Jun 8, 2018
 */


#include "CuTest.h"
#include <assert.h>
#include "defaultFunctions.h"
#include "string_utils.h"
#include "log.h"

void test_cuDefaultFunctionsSerializerString_01(CuTest* tc) {
	char str[BUFFER_SIZE] = "Hello World!";
	FILE* fout = fopen("cuDefaultFunctionsSerializerString_01", "wb");

	assert(cuDefaultFunctionsSerializerString(fout, str) == (4 + 12));

	fclose(fout);
}

void test_cuDefaultFunctionsDeserializerString_02(CuTest* tc) {
	char* str;
	FILE* fout = fopen("cuDefaultFunctionsSerializerString_01", "rb");

	assert(cuDefaultFunctionsDeserializerString(fout, &str) == (4 +12));
	info("got from file \"%s\"!", str);
	assert(isStrEqual(str, "Hello World!"));

	CU_FREE(str);

	fclose(fout);
}

CuSuite* CuDefaultFunctionsSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_cuDefaultFunctionsSerializerString_01);
	SUITE_ADD_TEST(suite, test_cuDefaultFunctionsDeserializerString_02);



	return suite;
}
