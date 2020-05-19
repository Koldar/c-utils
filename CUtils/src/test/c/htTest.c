/*
 * htTest.c
 *
 *  Created on: May 18, 2017
 *      Author: koldar
 */


#include "CuTest.h"
#include "hashtable.h"
#include <string.h>
#include <assert.h>
#include "macros.h"

///test cuHTNew
void testHT01(CuTest* tc) {
	HT* ht = cuHTNew();
	cuHTDestroy(ht, NULL);
}

///test cuHTAddItem
void testHT02(CuTest* tc) {
	HT* ht = cuHTNew();

	assert(cuHTGetSize(ht) == 0);
	assert(cuHTIsEmpty(ht) == true);
	int a = 3;
	cuHTAddItem(ht, 5, &a);

	assert(cuHTGetSize(ht) == 1);
	assert(cuHTIsEmpty(ht) == false);

	cuHTRemoveItem(ht, 5);

	assert(cuHTGetSize(ht) == 0);
	assert(cuHTIsEmpty(ht) == true);

	cuHTDestroy(ht, NULL);
}

///test CU_ITERATE_OVER_HT
void testHT03(CuTest* tc) {
	HT* ht = cuHTNew();

	int a = 5;
	int b = 6;
	int c = 7;
	int d = 10;

	assert(cuHTGetSize(ht) == 0);
	assert(cuHTIsEmpty(ht) == true);

	cuHTAddItem(ht, 5, &a);
	cuHTAddItem(ht, 6, &b);
	cuHTAddItem(ht, 7, &c);
	cuHTAddItem(ht, 10, &d);

	assert(cuHTGetSize(ht) == 4);
	assert(cuHTIsEmpty(ht) == false);

	int sum = 0;
	CU_ITERATE_OVER_HT(ht, pair) {
		sum += *((int*)(pair->data));
	}

	assert(sum == 28);

	cuHTDestroy(ht, NULL);
}

///test cuHTGetItem
void testHT04(CuTest* tc) {
	HT* ht = cuHTNew();

	int a = 5;
	int b = 6;
	int c = 7;
	int d = 10;

	assert(cuHTGetSize(ht) == 0);
	assert(cuHTIsEmpty(ht) == true);

	cuHTAddItem(ht, 5, &a);
	cuHTAddItem(ht, 6, &b);
	cuHTAddItem(ht, 7, &c);
	cuHTAddItem(ht, 10, &d);

	assert (cuHTGetItem(ht, 11) == NULL);
	assert (*(int*)cuHTGetItem(ht, 7) == 7);
	assert (*(int*)cuHTGetItem(ht, 5) == 5);
	assert (*(int*)cuHTGetItem(ht, 10) == 10);

	cuHTDestroy(ht, NULL);
}

///test CU_ITERATE_OVER_HT_VALUES
void testHT05(CuTest* tc) {
	HT* ht = cuHTNew();

	int a = 5;
	int b = 6;
	int c = 7;
	int d = 10;

	assert(cuHTGetSize(ht) == 0);
	assert(cuHTIsEmpty(ht) == true);

	cuHTAddItem(ht, 5, &a);
	cuHTAddItem(ht, 6, &b);
	cuHTAddItem(ht, 7, &c);
	cuHTAddItem(ht, 10, &d);

	assert(cuHTGetSize(ht) == 4);
	assert(cuHTIsEmpty(ht) == false);

	int sum = 0;
	CU_ITERATE_OVER_HT_VALUES(ht, value, int*) {
		sum += *value;
	}

	assert(sum == 28);

	cuHTDestroy(ht, NULL);
}

void testHT_sameid(CuTest* tc) {
	CU_WITH(int_ht* ht = cuHTNew())(cuHTDestroyWithElements2(ht, NULL)) {
		int a = 5;
		int b = 6;

		cuHTAddItem(ht, 5, &a);
		cuHTAddItem(ht, 5, &b);

		assert(cuHTGetSize(ht) == 2);
	}

}

void test_CU_ITERATE_OVER_HASHTABLE_01(CuTest* tc) {
	int_ht* ht = cuHTNew(cuPayloadFunctionsIntValue());

	cuHTAddItem(ht, 5, CU_CAST_INT2PTR(10));
	cuHTAddItem(ht, 7, CU_CAST_INT2PTR(14));
	cuHTAddItem(ht, 9, CU_CAST_INT2PTR(27));

	int sum = 0;
	CU_ITERATE_OVER_HASHTABLE(ht, key, value, int_ptr) {
		info("summing %d", CU_CAST_PTR2INT(value));
		sum += CU_CAST_PTR2INT(value);
	}

	info("obtained %d", sum);
	assert(sum == 51);

	cuHTDestroyWithElements2(ht, NULL);
}

void test_CU_ITERATE_OVER_HASHTABLE_02(CuTest* tc) {
	int_ht* ht = cuHTNew(cuPayloadFunctionsIntValue());



	int sum = 0;
	CU_ITERATE_OVER_HASHTABLE(ht, key, value, int_ptr) {
		info("summing %d", CU_CAST_PTR2INT(value));
		sum += CU_CAST_PTR2INT(value);
	}

	info("obtained %d", sum);
	assert(sum == 0);

	cuHTDestroyWithElements2(ht, NULL);
}

void test_CU_ITERATE_OVER_HASHTABLE_03(CuTest* tc) {
	int_ht* ht = cuHTNew(cuPayloadFunctionsIntValue());

	cuHTAddItem(ht, 7, CU_CAST_INT2PTR(14));

	int sum = 0;
	CU_ITERATE_OVER_HASHTABLE(ht, key, value, int_ptr) {
		info("summing %d", CU_CAST_PTR2INT(value));
		sum += CU_CAST_PTR2INT(value);
	}

	info("obtained %d", sum);
	assert(sum == 14);

	cuHTDestroyWithElements2(ht, NULL);
}

void test_cuHTStoreInFile_01(CuTest* tc) {
	int_ht* ht = cuHTNew(cuPayloadFunctionsIntValue());

	cuHTAddItem(ht, 7, CU_CAST_INT2PTR(10));
	cuHTAddItem(ht, 5, CU_CAST_INT2PTR(15));

	FILE* f = fopen(__func__, "wb");

	cuHTStoreInFile2(f, ht);

	fclose(f);
	cuHTDestroyWithElements2(ht, NULL);

	f = fopen(__func__, "rb");

	ht = cuHTLoadFromFile(f, cuPayloadFunctionsIntValue());

	assert(cuHTGetSize(ht) == 2);
	//TODO rename cuHTContainsItem in cuHTContainsKey
	assert(cuHTContainsItem(ht, 7));
	assert(cuHTContainsItem(ht, 5));

	intptr_t a = CU_CAST_PTR2INT(cuHTGetItem(ht, 5));
	info("getitem of 5 is %d", a);
	info("15 is equal to 15? %d", a - 15);
	info("15 is equal to 15? %s", a == 15 ? "yes" : "no");
	assert(CU_CAST_PTR2INT(cuHTGetItem(ht, 5)) == 15);
	assert(CU_CAST_PTR2INT(cuHTGetItem(ht, 7)) == 10);

	cuHTDestroyWithElements2(ht, NULL);
}

void test_CU_VARIABLE_ITERATE_OVER_HASHTABLE_01(CuTest* tc) {
	pint_ht* ht = cuHTNew(cuPayloadFunctionsIntValue());

	cuHTAddItem(ht, 7, CU_CAST_INT2PTR(10));
	cuHTAddItem(ht, 5, CU_CAST_INT2PTR(15));

	int sum = 0;
	assert(cuHTGetSize(ht) == 2);

	sum = 0;
	CU_VARIABLE_ITERATE_OVER_HASHTABLE(ht, cell, key, value, int_ptr) {
		cuHTDestroyCell(ht, cell);
		sum += CU_CAST_PTR2INT(value);
	}

	assert(sum == 25);
	assert(cuHTIsEmpty(ht));

	sum = 0;
	CU_ITERATE_OVER_HASHTABLE(ht, key, value, int_ptr) {
		sum += CU_CAST_PTR2INT(value);
	}
	assert(sum == 0);

	cuHTDestroyWithElements2(ht, NULL);
}

void test_CU_VARIABLE_ITERATE_OVER_HASHTABLE_02(CuTest* tc) {
	pint_ht* ht = cuHTNew(cuPayloadFunctionsIntValue());

	cuHTAddItem(ht, 7, CU_CAST_INT2PTR(10));

	int sum = 0;
	assert(cuHTGetSize(ht) == 1);

	sum = 0;
	CU_VARIABLE_ITERATE_OVER_HASHTABLE(ht, cell, key, value, int_ptr) {
		cuHTDestroyCell(ht, cell);
		sum += CU_CAST_PTR2INT(value);
	}

	assert(sum == 10);
	assert(cuHTIsEmpty(ht));

	sum = 0;
	CU_ITERATE_OVER_HASHTABLE(ht, key, value, int_ptr) {
		sum += CU_CAST_PTR2INT(value);
	}
	assert(sum == 0);

	cuHTDestroyWithElements2(ht, NULL);
}

void test_CU_VARIABLE_ITERATE_OVER_HASHTABLE_03(CuTest* tc) {
	pint_ht* ht = cuHTNew(cuPayloadFunctionsIntValue());


	int sum = 0;
	assert(cuHTGetSize(ht) == 0);

	sum = 0;
	CU_VARIABLE_ITERATE_OVER_HASHTABLE(ht, cell, key, value, int_ptr) {
		cuHTDestroyCell(ht, cell);
		sum += CU_CAST_PTR2INT(value);
	}

	assert(sum == 0);
	assert(cuHTIsEmpty(ht));

	sum = 0;
	CU_ITERATE_OVER_HASHTABLE(ht, key, value, int_ptr) {
		sum += CU_CAST_PTR2INT(value);
	}
	assert(sum == 0);

	cuHTDestroyWithElements2(ht, NULL);
}

CuSuite* CuHTSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testHT01);
	SUITE_ADD_TEST(suite, testHT02);
	SUITE_ADD_TEST(suite, testHT03);
	SUITE_ADD_TEST(suite, testHT04);
	SUITE_ADD_TEST(suite, testHT05);

	SUITE_ADD_TEST(suite, test_CU_ITERATE_OVER_HASHTABLE_01);
	SUITE_ADD_TEST(suite, test_CU_ITERATE_OVER_HASHTABLE_02);
	SUITE_ADD_TEST(suite, test_CU_ITERATE_OVER_HASHTABLE_03);

	SUITE_ADD_TEST(suite, test_cuHTStoreInFile_01);

	SUITE_ADD_TEST(suite, test_CU_VARIABLE_ITERATE_OVER_HASHTABLE_01);
	SUITE_ADD_TEST(suite, test_CU_VARIABLE_ITERATE_OVER_HASHTABLE_02);
	SUITE_ADD_TEST(suite, test_CU_VARIABLE_ITERATE_OVER_HASHTABLE_03);


	return suite;
}


