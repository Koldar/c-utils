/**
 * @file
 *
 * @author koldar
 * @date May 23, 2018
 */


#include <assert.h>
#include "CuTest.h"
#include "log.h"
#include "utility.h"

void test_parseRangeInt_01(CuTest* tc) {
	int a = 0;
	int b = 0;
	bool lbIncluded = false;
	bool ubIncluded = false;
	bool ret;

	ret = cuUtilsRangeInt("[2; 3]", &a, &b, &lbIncluded, &ubIncluded);
	assert(ret);
	assert(lbIncluded);
	assert(ubIncluded);
	assert(a == 2);
	assert(b == 3);
}

void test_parseRangeInt_02(CuTest* tc) {
	int a = 0;
	int b = 0;
	bool lbIncluded = false;
	bool ubIncluded = false;
	bool ret;

	ret = cuUtilsRangeInt("]2; 3]", &a, &b, &lbIncluded, &ubIncluded);
	assert(ret);
	assert(!lbIncluded);
	assert(ubIncluded);
	assert(a == 2);
	assert(b == 3);
}

void test_parseRangeInt_03(CuTest* tc) {
	int a = 0;
	int b = 0;
	bool lbIncluded = false;
	bool ubIncluded = false;
	bool ret;

	ret = cuUtilsRangeInt("(2; 3]", &a, &b, &lbIncluded, &ubIncluded);
	assert(ret);
	assert(!lbIncluded);
	assert(ubIncluded);
	assert(a == 2);
	assert(b == 3);
}

void test_parseRangeInt_04(CuTest* tc) {
	int a = 0;
	int b = 0;
	bool lbIncluded = false;
	bool ubIncluded = false;
	bool ret;

	ret = cuUtilsRangeInt("[2; 3[", &a, &b, &lbIncluded, &ubIncluded);
	assert(ret);
	assert(lbIncluded);
	assert(!ubIncluded);
	assert(a == 2);
	assert(b == 3);
}

void test_parseRangeInt_05(CuTest* tc) {
	int a = 0;
	int b = 0;
	bool lbIncluded = false;
	bool ubIncluded = false;
	bool ret;

	ret = cuUtilsRangeInt("[2; 3)", &a, &b, &lbIncluded, &ubIncluded);
	assert(ret);
	assert(lbIncluded);
	assert(!ubIncluded);
	assert(a == 2);
	assert(b == 3);
}

void test_parseRangeInt_06(CuTest* tc) {
	int a = 0;
	int b = 0;
	bool lbIncluded = false;
	bool ubIncluded = false;
	bool ret;

	ret = cuUtilsRangeInt("(2; 3)", &a, &b, &lbIncluded, &ubIncluded);
	assert(ret);
	assert(!lbIncluded);
	assert(!ubIncluded);
	assert(a == 2);
	assert(b == 3);
}

void test_parseRangeInt_07(CuTest* tc) {
	int a = 0;
	int b = 0;
	bool lbIncluded = false;
	bool ubIncluded = false;
	bool ret;

	ret = cuUtilsRangeInt("]2; 3[", &a, &b, &lbIncluded, &ubIncluded);
	assert(ret);
	assert(!lbIncluded);
	assert(!ubIncluded);
	assert(a == 2);
	assert(b == 3);
}



CuSuite* CuUtilitySuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_parseRangeInt_01);
	SUITE_ADD_TEST(suite, test_parseRangeInt_02);
	SUITE_ADD_TEST(suite, test_parseRangeInt_03);
	SUITE_ADD_TEST(suite, test_parseRangeInt_04);
	SUITE_ADD_TEST(suite, test_parseRangeInt_05);
	SUITE_ADD_TEST(suite, test_parseRangeInt_06);
	SUITE_ADD_TEST(suite, test_parseRangeInt_07);

	return suite;
}
