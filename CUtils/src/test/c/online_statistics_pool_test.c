/**
 * @file
 *
 * @author koldar
 * @date Apr 6, 2018
 */

#include "CuTest.h"
#include <assert.h>
#include "online_statistics_pool.h"

#define TEST1 "test1"
#define TEST2 "test2"

void test_osp_00(CuTest* tc) {
	online_statistics_pool* osp = cuOSPNew(false);
	cuOSPDestroy(osp, NULL);
}

void test_osp_01(CuTest* tc) {
	online_statistics_pool* osp = cuOSPNew(true);
	cuOSPDestroy(osp, NULL);
}

void test_osp_02(CuTest* tc) {
	online_statistics_pool* osp = cuOSPNew(true);

	assert(cuOSPIsPoolEmpty(osp));

	cuOSPUpdateItem(osp, TEST1, 4);

	assert(!cuOSPIsPoolEmpty(osp));
	assert(cuOSPGetPoolSize(osp) == 1);
	assert(!cuOSPIsEmpty(osp, TEST1));
	assert(cuOSPIsEmpty(osp, TEST2));

	assert(cuOSPGetLastValue(osp, TEST1) == 4);

	cuOSPDestroy(osp, NULL);
}

void test_osp_03(CuTest* tc) {
	online_statistics_pool* osp = cuOSPNew(false);

	assert(cuOSPIsPoolEmpty(osp));

	cuOSPUpdateItem(osp, TEST1, 4);

	assert(cuOSPIsPoolEmpty(osp));
	assert(cuOSPGetPoolSize(osp) == 0);
	assert(cuOSPIsEmpty(osp, TEST1));
	assert(cuOSPIsEmpty(osp, TEST2));

	cuOSPDestroy(osp, NULL);
}

void test_cuOSPAddOne_01(CuTest* tc) {
	CU_WITH(online_statistics_pool* osp = cuOSPNew(true))(cuOSPDestroy(osp, NULL)) {
		assert(cuOSPIsEmpty(osp, TEST1));

		cuOSPAddOne(osp, TEST1);
		assert(!cuOSPIsEmpty(osp, TEST1));
		assert(cuOSPGetNumber(osp, TEST1) == 1);
		assert(((int)cuOSPGetLastValue(osp, TEST1)) == 1);

		cuOSPAddOne(osp, TEST1);
		assert(!cuOSPIsEmpty(osp, TEST1));
		assert(cuOSPGetNumber(osp, TEST1) == 2);
		assert(((int)cuOSPGetLastValue(osp, TEST1)) == 1);
	}
}

CuSuite* CuOnlineStatisticPoolSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_osp_00);
	SUITE_ADD_TEST(suite, test_osp_01);
	SUITE_ADD_TEST(suite, test_osp_02);
	SUITE_ADD_TEST(suite, test_osp_03);

	SUITE_ADD_TEST(suite, test_cuOSPAddOne_01);


	return suite;
}
