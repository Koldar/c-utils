/**
 * @file
 *
 * @author koldar
 * @date Mar 14, 2018
 */

#include "CuTest.h"
#include "var_args.h"
#include <string.h>
#include "log.h"
#include <math.h>

struct p {
	int x;
	int y;
};

int foo(const var_args* va) {
	int sum = 0;
	info("count s %d", va->count);
	for (int i=0; i<va->count; i++) {
		info("fetchin %d: %d", i, cuVarArgsGetItem(va, i, int));
		sum += cuVarArgsGetItem(va, i, int);
	}
	return sum;
}


void testVarArgs01(CuTest* tc) {

	cuInitVarArgsOnStack(va, 1,2,3,4,5);
	assert(cuVarArgsGetSize(va) == 5);
	assert(cuVarArgsGetItem(va, 0, int) == 1);
	assert(cuVarArgsGetItem(va, 1, int) == 2);
	assert(cuVarArgsGetItem(va, 2, int) == 3);
	assert(cuVarArgsGetItem(va, 3, int) == 4);
	assert(cuVarArgsGetItem(va, 4, int) == 5);
	assert(foo(va) == 15);
}

void testVarArgs02(CuTest* tc) {

	struct p qwerty = {5,2};
	cuInitVarArgsOnStack(va, 5, qwerty, "ciao");
	assert(cuVarArgsGetItem(va, 0, int) == 5);
	assert(cuVarArgsGetItem(va, 1, struct p).x == 5);
	assert(cuVarArgsGetItem(va, 1, struct p).y == 2);
	assert(strcmp(cuVarArgsGetItemPtr(va, 2, char), "ciao") == 0);

}

void testVarArgs03(CuTest* tc) {
	cuInitVarArgsOnStack(va, 5, 2, 3);

	int power = 0;
	CU_ITERATE_ON_VARIADIC(va, i, v) {
		power += (int)pow((double)i+1, (double) (*(int*)v));
	}

	//1^5 + 2^2+ 3^3
	assert(power == (1+4+27));
}

void testVarArgs04(CuTest* tc) {
	cuInitVarArgsOnStack(va, 5);

	int power = 0;
	assert(cuVarArgsGetSize(va) == 1);
	CU_ITERATE_ON_VARIADIC(va, i, v) {
		power += (int)pow((double)i+1, (double) (*(int*)v));
	}

	//1^5 + 2^2+ 3^3
	assert(power == (1));
}

void testVarArgs05(CuTest* tc) {
	cuInitVarArgsOnStack(va);

	int power = 0;
	assert(cuVarArgsGetSize(va) == 0);
	CU_ITERATE_ON_VARIADIC(va, i, v) {
		power += (int)pow((double)i+1, (double) (*(int*)v));
	}

	assert(power == (0));
}

void testVarArgs06(CuTest* tc) {
	cuInitVarArgsOnStack(va1, 1, 2, 3);

	int sum = 0;
	assert(cuVarArgsGetSize(va1) == 3);
	CU_ITERATE_ON_VARIADIC(va1, i, v) {
		sum += *((int*)v);
	}
	assert(sum == 6);

	sum = 0;
	cuInitVarArgsOnStack(va2, 4, 5, 6);
	assert(cuVarArgsGetSize(va2) == 3);
	CU_ITERATE_ON_VARIADIC(va2, i, v) {
		sum += *((int*)v);
	}
	info("sum is %d", sum);
	assert(sum == 15);

}

void test_newVarArgsOnHeap_01(CuTest* tc) {
	cuNewVarArgsOnHeap(va, 1, 2, 3);

	int sum = 0;
	assert(cuVarArgsGetSize(va) == 3);
	CU_ITERATE_ON_VARIADIC(va, i, v) {
		sum += *((int*)v);
	}
	assert(sum == 6);

	cuDestroyVarArgs(va, NULL);
}

CuSuite* CuVarArgSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testVarArgs01);
	SUITE_ADD_TEST(suite, testVarArgs02);
	SUITE_ADD_TEST(suite, testVarArgs03);
	SUITE_ADD_TEST(suite, testVarArgs04);
	SUITE_ADD_TEST(suite, testVarArgs05);
	SUITE_ADD_TEST(suite, testVarArgs06);
	SUITE_ADD_TEST(suite, test_newVarArgsOnHeap_01);


	return suite;
}
