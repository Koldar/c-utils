/**
 * @file
 *
 * @author koldar
 * @date May 24, 2018
 */

#include "CuTest.h"
#include <assert.h>
#include "math_utils.h"

void test_cuMathUtilsClamp_01(CuTest* tc) {

	assert(cuMathUtilsClamp(13, 10, 20) == 13);
	assert(cuMathUtilsClamp(20, 10, 20) == 20);
	assert(cuMathUtilsClamp(10, 10, 20) == 10);
	assert(cuMathUtilsClamp(7, 10, 20) == 10);
	assert(cuMathUtilsClamp(23, 10, 20) == 20);

}

void test_cuMathUtilsClamp_02(CuTest* tc) {

	assert(cuMathUtilsClamp2(13.5, 10.5, 20.5) == 13.5);
	assert(cuMathUtilsClamp2(20.5, 10.5, 20.5) == 20.5);
	assert(cuMathUtilsClamp2(10.5, 10.5, 20.5) == 10.5);
	assert(cuMathUtilsClamp2(7.5, 10.5, 20.5) == 10.5);
	assert(cuMathUtilsClamp2(23.5, 10.5, 20.5) == 20.5);

}


CuSuite* CuMathUtilsSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_cuMathUtilsClamp_01);
	SUITE_ADD_TEST(suite, test_cuMathUtilsClamp_02);


	return suite;
}
