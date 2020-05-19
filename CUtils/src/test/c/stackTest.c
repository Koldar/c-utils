/*
 * stackTest.c
 *
 *  Created on: May 18, 2017
 *      Author: koldar
 */

#include <assert.h>
#include "static_stack.h"
#include "CuTest.h"
#include <stdbool.h>
#include "log.h"

///Test push, size of stack
void testStack01(CuTest* tc) {
	static_stack* stack = cuStaticStackNew(3);

	int values[] = {1,2,3,4,5,6,7,8,9,10};

	assert(cuStaticStackIsEmpty(stack) == true);
	assert(cuStaticStackIsFull(stack) == false);
	assert(cuStaticStackSize(stack) == 0);

	cuStaticStackPush(stack, &values[0]);

	assert(cuStaticStackIsEmpty(stack) == false);
	assert(cuStaticStackIsFull(stack) == false);
	assert(cuStaticStackSize(stack) == 1);

	cuStaticStackPush(stack, &values[1]);

	assert(cuStaticStackIsEmpty(stack) == false);
	assert(cuStaticStackIsFull(stack) == false);
	assert(cuStaticStackSize(stack) == 2);

	cuStaticStackPush(stack, &values[2]);

	assert(cuStaticStackIsEmpty(stack) == false);
	assert(cuStaticStackIsFull(stack) == true);
	assert(cuStaticStackSize(stack) == 3);

	cuStaticStackDestroy(stack, NULL);
}

void testStack02(CuTest* tc) {
	static_stack* stack = cuStaticStackNew(3);

	int values[] = {1,2,3,4,5,6,7,8,9,10};

	assert(cuStaticStackIsEmpty(stack) == true);
	assert(cuStaticStackIsFull(stack) == false);
	assert(cuStaticStackSize(stack) == 0);

	cuStaticStackPush(stack, &values[0]);
	cuStaticStackPush(stack, &values[1]);

	assert(cuStaticStackIsEmpty(stack) == false);
	assert(cuStaticStackIsFull(stack) == false);
	assert(cuStaticStackSize(stack) == 2);

	int* i = (int*)cuStaticStackPeek(stack);
	assert(*i == 2);
	assert(cuStaticStackSize(stack) == 2);

	i = cuStaticStackPop(stack);
	assert(*i == 2);
	assert(cuStaticStackSize(stack) == 1);

	i = cuStaticStackPop(stack);
	assert(*i == 1);
	assert(cuStaticStackSize(stack) == 0);

	cuStaticStackDestroy(stack, NULL);
}

void testStack03(CuTest* tc) {
	static_stack* stack = cuStaticStackNew(8);

	int values[] = {1,2,3,4,5,6,7,8,9,10};

	assert(cuStaticStackIsEmpty(stack) == true);
	assert(cuStaticStackIsFull(stack) == false);
	assert(cuStaticStackSize(stack) == 0);

	cuStaticStackPush(stack, &values[0]);
	cuStaticStackPush(stack, &values[1]);
	cuStaticStackPush(stack, &values[2]);
	cuStaticStackPush(stack, &values[3]);
	cuStaticStackPush(stack, &values[4]);

	int sum = 0;
	CU_ITERATE_OVER_STATIC_STACK(stack, n, int*) {
		sum += *n;
	}

	critical("sum is %d", sum);
	assert(sum == 15);

	cuStaticStackDestroy(stack, NULL);
}

void testStack04(CuTest* tc) {
	static_stack* stack = cuStaticStackNew(8);


	int sum = 0;
	CU_ITERATE_OVER_STATIC_STACK(stack, n, int*) {
		sum += *n;
	}

	assert(sum == 0);

	cuStaticStackDestroy(stack, NULL);
}

CuSuite* CuStackSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testStack01);
	SUITE_ADD_TEST(suite, testStack02);
	SUITE_ADD_TEST(suite, testStack03);
	SUITE_ADD_TEST(suite, testStack04);

	return suite;
}

