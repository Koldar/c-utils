/*
 * dynamic_stackTest.c
 *
 *  Created on: Oct 11, 2017
 *      Author: koldar
 */

#include "CuTest.h"
#include "dynamic_stack.h"
#include <assert.h>

void test01(CuTest* tc) {
	dynamic_stack* s = cuDynamicStackNew(5, 3, cuPayloadFunctionsIntValue());

	assert(cuDynamicStackIsEmpty(s));
	assert(!cuDynamicStackIsFull(s));
	assert(cuDynamicStackGetSize(s) == 0);

	cuDynamicStackDestroyWithElements(s, NULL);
}

void test02(CuTest* tc) {
	dynamic_stack* s = cuDynamicStackNew(5, 3, cuPayloadFunctionsIntValue());

	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(5));
	assert(!cuDynamicStackIsEmpty(s));
	assert(!cuDynamicStackIsFull(s));
	assert(cuDynamicStackGetSize(s) == 1);
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(3));
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(2));
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(4));
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(7));
	assert(!cuDynamicStackIsEmpty(s));
	assert(cuDynamicStackIsFull(s));
	assert(cuDynamicStackGetSize(s) == 5);

	cuDynamicStackDestroyWithElements(s, NULL);
}

void test03(CuTest* tc) {
	dynamic_stack* s = cuDynamicStackNew(5, 3, cuPayloadFunctionsIntValue());

	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(5));
	assert(!cuDynamicStackIsEmpty(s));
	assert(!cuDynamicStackIsFull(s));
	assert(cuDynamicStackGetSize(s) == 1);
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(3));
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(2));
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(4));
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(7));
	assert(!cuDynamicStackIsEmpty(s));
	assert(cuDynamicStackIsFull(s));
	assert(cuDynamicStackGetSize(s) == 5);

	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(8));
	assert(!cuDynamicStackIsEmpty(s));
	assert(!cuDynamicStackIsFull(s));
	assert(cuDynamicStackGetSize(s) == 6);

	cuDynamicStackDestroyWithElements(s, NULL);
}

void test04(CuTest* tc) {
	dynamic_stack* s = cuDynamicStackNew(5, 3, cuPayloadFunctionsIntValue());

	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(5));
	assert(!cuDynamicStackIsEmpty(s));
	assert(!cuDynamicStackIsFull(s));
	assert(cuDynamicStackGetSize(s) == 1);
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(3));
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(2));
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(4));
	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(7));
	assert(!cuDynamicStackIsEmpty(s));
	assert(cuDynamicStackIsFull(s));
	assert(cuDynamicStackGetSize(s) == 5);

	cuDynamicStackPushItem(s, CU_CAST_INT2PTR(8));
	assert(!cuDynamicStackIsEmpty(s));
	assert(!cuDynamicStackIsFull(s));
	assert(cuDynamicStackGetSize(s) == 6);

	assert(CU_CAST_PTR2INT(cuDynamicStackPeekItem(s)) == 8);
	assert(CU_CAST_PTR2INT(cuDynamicStackPopItem(s)) == 8);
	assert(!cuDynamicStackIsEmpty(s));
	assert(!cuDynamicStackIsFull(s));
	assert(cuDynamicStackGetSize(s) == 5);

	cuDynamicStackDestroyWithElements(s, NULL);
}

CuSuite* CuDynamicStackSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test01);
	SUITE_ADD_TEST(suite, test02);
	SUITE_ADD_TEST(suite, test03);
	SUITE_ADD_TEST(suite, test04);

	return suite;
}
