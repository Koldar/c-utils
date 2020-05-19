/**
 * @file
 *
 * @author koldar
 * @date Apr 6, 2018
 */

#include <assert.h>
#include "CuTest.h"
#include "colors.h"
#include <stdio.h>

void testColors01(CuTest* tc) {

	printf(CU_RED("red\n"));
}

CuSuite* CuColorsSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testColors01);

	return suite;
}
