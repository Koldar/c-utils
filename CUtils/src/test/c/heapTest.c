/*
 * heapTest.c
 *
 *  Created on: Jun 5, 2017
 *      Author: koldar
 */

#include <assert.h>
#include <stdlib.h>
#include "CuTest.h"
#include "heap.h"
#include "defaultFunctions.h"
#include "log.h"
#include <string.h>

///test init and destroy functions
void testnewHeap01(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntValue());

	assert(cuHeapIsEmpty(h));
	assert(cuHeapGetSize(h) == 0);
	assert(cuHeapGetMaxSize(h) == 10);

	cuHeapDestroy(h, NULL);
}

///test if adding item will increase the size of heap
void testinsertItemInHeap01(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntValue());

	cuHeapInsertItem(h, CU_CAST_INT2PTR(5));

	assert(!cuHeapIsEmpty(h));
	assert(cuHeapGetSize(h) == 1);
	assert(cuHeapGetMaxSize(h) == 10);

	cuHeapInsertItem(h, CU_CAST_INT2PTR(6));

	assert(!cuHeapIsEmpty(h));
	assert(cuHeapGetSize(h) == 2);
	assert(cuHeapGetMaxSize(h) == 10);

	cuHeapDestroyWithElements(h, NULL);
}

///test destruction with elements
void testinsertItemInHeap02(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntPtr());

	int* a = malloc(sizeof(int));
	*a = 5;
	cuHeapInsertItem(h, a);

	assert(!cuHeapIsEmpty(h));
	assert(cuHeapGetSize(h) == 1);
	assert(cuHeapGetMaxSize(h) == 10);

	int* b = malloc(sizeof(int));
	*b = 6;
	cuHeapInsertItem(h, b);

	assert(!cuHeapIsEmpty(h));
	assert(cuHeapGetSize(h) == 2);
	assert(cuHeapGetMaxSize(h) == 10);

	cuHeapDestroyWithElements(h, NULL);
}

///test cuHeapRemoveMinItem
void testinsertItemInHeap03(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntValue());

	cuHeapInsertItem(h, CU_CAST_INT2PTR(5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(6));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(7));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(8));

	assert(!cuHeapIsEmpty(h));
	assert(cuHeapGetSize(h) == 4);
	assert(cuHeapGetMaxSize(h) == 10);

	info("peek: %d", cuHeapPeekMinItem(h));
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 5);

	info("peek: %d", cuHeapPeekMinItem(h));
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 6);

	info("peek: %d", cuHeapPeekMinItem(h));
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 7);

	info("peek: %d", cuHeapPeekMinItem(h));
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 8);

	cuHeapDestroyWithElements(h, NULL);
}

///like ::testinsertItemInHeap03 with unonrdered input
void testinsertItemInHeap04(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntValue());

	cuHeapInsertItem(h, CU_CAST_INT2PTR(7));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(8));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(6));

	assert(!cuHeapIsEmpty(h));
	assert(cuHeapGetSize(h) == 4);
	assert(cuHeapGetMaxSize(h) == 10);

	info("peek: %d", cuHeapPeekMinItem(h));
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 5);

	info("peek: %d", cuHeapPeekMinItem(h));
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 6);

	info("peek: %d", cuHeapPeekMinItem(h));
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 7);

	info("peek: %d", cuHeapPeekMinItem(h));
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 8);

	cuHeapDestroyWithElements(h, NULL);
}

///like maximum capacity
void testinsertItemInHeap05(CuTest* tc) {
	heap* h = cuHeapNew(5, cuPayloadFunctionsIntValue());

	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(1)) == true);
	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(2)) == true);
	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(3)) == true);
	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(4)) == true);
	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(5)) == true);

	assert(cuHeapGetSize(h) == 5);
	assert(cuHeapGetMaxSize(h) == 5);
	assert(cuHeapIsFull(h));

	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(6)) == false);

	assert(cuHeapGetSize(h) == 5);
	assert(cuHeapGetMaxSize(h) == 5);
	assert(cuHeapIsFull(h));

	cuHeapDestroyWithElements(h, NULL);
}

void testClear(CuTest* tc) {
	heap* h = cuHeapNew(5, cuPayloadFunctionsIntValue());

	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(1)) == true);
	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(2)) == true);
	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(3)) == true);
	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(4)) == true);
	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(5)) == true);

	cuHeapClear(h);

	assert(cuHeapGetSize(h) == 0);
	assert(cuHeapGetMaxSize(h) == 5);
	assert(!cuHeapIsFull(h));
	assert(cuHeapIsEmpty(h));

	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(4)) == true);
	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(3)) == true);
	assert(cuHeapInsertItem(h, CU_CAST_INT2PTR(5)) == true);

	assert(cuHeapGetSize(h) == 3);
	assert(cuHeapGetMaxSize(h) == 5);
	assert(!cuHeapIsFull(h));
	assert(!cuHeapIsEmpty(h));

	assert(CU_CAST_PTR2INT(cuHeapPeekMinItem(h)) == 3);
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 3);
	assert(CU_CAST_PTR2INT(cuHeapPeekMinItem(h)) == 4);
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 4);
	assert(CU_CAST_PTR2INT(cuHeapPeekMinItem(h)) == 5);
	assert(CU_CAST_PTR2INT(cuHeapRemoveMinItem(h)) == 5);

	cuHeapDestroyWithElements(h, NULL);
}

void testcontainsItemInHeap01(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntValue());

	cuHeapInsertItem(h, CU_CAST_INT2PTR(0));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(7));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(-5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(3));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(2));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(4));

	//check if root is present
	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(-5)));
	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(7)));
	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(0)));
	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(-5)));
	assert(!cuHeapContainsItem(h, CU_CAST_INT2PTR(-4)));

	cuHeapDestroyWithElements(h, NULL);
}

void testIterationOnHeap(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntValue());

	cuHeapInsertItem(h, CU_CAST_INT2PTR(0));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(7));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(-5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(3));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(2));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(4));

	int sum = 0;
	CU_ITERATE_THROUGH_HEAP(h, num1, intptr_t) {
		sum += num1;
	}
	assert(sum == 16);

	cuHeapClear(h);

	int flag = 0;
	CU_ITERATE_THROUGH_HEAP(h, num2, intptr_t) {
		flag++;
	}
	assert(flag == 0);

	cuHeapDestroyWithElements(h, NULL);
}

void testBufferStringOfHeap(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntValue());

	cuHeapInsertItem(h, CU_CAST_INT2PTR(0));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(7));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(-5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(3));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(2));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(4));

	char buffer[BUFFER_SIZE];
	char expected[BUFFER_SIZE] = "{ -5 0 2 5 3 7 4 }";
	cuHeapGetBufferString(h, buffer);
	info("actual is %s", buffer);
	assert(strcmp(buffer, expected) == 0);

	cuHeapClear(h);

	char expected2[BUFFER_SIZE] = "{ }";
	cuHeapGetBufferString(h, buffer);
	info("actual is %s", buffer);
	assert(strcmp(buffer, expected2) == 0);

	cuHeapDestroyWithElements(h, NULL);
}

///dst is empty
void testcloneHeap01(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntValue());

	cuHeapInsertItem(h, CU_CAST_INT2PTR(0));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(7));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(-5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(3));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(2));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(4));

	heap* cloned = cuHeapClone(h);

	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(0)));
	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(5)));
	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(7)));
	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(-5)));
	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(3)));
	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(2)));
	assert(cuHeapContainsItem(h, CU_CAST_INT2PTR(4)));

	debug("clone has %d elements", cuHeapGetSize(cloned));
	assert(cuHeapGetSize(cloned) == 7);
	assert(cuHeapContainsItem(cloned, CU_CAST_INT2PTR(0)));
	assert(cuHeapContainsItem(cloned, CU_CAST_INT2PTR(5)));
	assert(cuHeapContainsItem(cloned, CU_CAST_INT2PTR(7)));
	assert(cuHeapContainsItem(cloned, CU_CAST_INT2PTR(-5)));
	assert(cuHeapContainsItem(cloned, CU_CAST_INT2PTR(3)));
	assert(cuHeapContainsItem(cloned, CU_CAST_INT2PTR(2)));
	assert(cuHeapContainsItem(cloned, CU_CAST_INT2PTR(4)));

	cuHeapDestroy(h, NULL);
	cuHeapDestroy(cloned, NULL);
}

///dst is empty
void testmoveHeapElements01(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntValue());
	heap* dst = cuHeapNew(10, cuPayloadFunctionsIntValue());

	cuHeapInsertItem(h, CU_CAST_INT2PTR(0));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(7));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(-5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(3));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(2));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(4));

	assert(cuHeapIsEmpty(dst));
	assert(cuHeapGetSize(h) == 7);

	cuHeapMoveItems(dst, h);

	assert(cuHeapIsEmpty(h));
	assert(cuHeapGetSize(dst) == 7);

	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(0)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(5)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(7)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(-5)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(3)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(2)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(4)));

	cuHeapDestroy(h, NULL);
	cuHeapDestroy(dst, NULL);
}

///dst is not empty
void testmoveHeapElements02(CuTest* tc) {
	heap* h = cuHeapNew(10, cuPayloadFunctionsIntValue());
	heap* dst = cuHeapNew(10, cuPayloadFunctionsIntValue());

	cuHeapInsertItem(dst, CU_CAST_INT2PTR(10));
	cuHeapInsertItem(dst, CU_CAST_INT2PTR(12));

	cuHeapInsertItem(h, CU_CAST_INT2PTR(0));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(7));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(-5));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(3));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(2));
	cuHeapInsertItem(h, CU_CAST_INT2PTR(4));

	assert(cuHeapGetSize(dst) == 2);
	assert(cuHeapGetSize(h) == 7);

	cuHeapMoveItems(dst, h);

	assert(cuHeapIsEmpty(h));
	assert(cuHeapGetSize(dst) == 9);

	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(0)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(5)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(7)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(-5)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(3)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(2)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(4)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(10)));
	assert(cuHeapContainsItem(dst, CU_CAST_INT2PTR(12)));

	cuHeapDestroy(h, NULL);
	cuHeapDestroy(dst, NULL);
}

CuSuite* CuHeapSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testnewHeap01);
	SUITE_ADD_TEST(suite, testinsertItemInHeap01);
	SUITE_ADD_TEST(suite, testinsertItemInHeap02);
	SUITE_ADD_TEST(suite, testinsertItemInHeap03);
	SUITE_ADD_TEST(suite, testinsertItemInHeap04);
	SUITE_ADD_TEST(suite, testinsertItemInHeap05);
	SUITE_ADD_TEST(suite, testClear);
	SUITE_ADD_TEST(suite, testcontainsItemInHeap01);
	SUITE_ADD_TEST(suite, testBufferStringOfHeap);
	SUITE_ADD_TEST(suite, testcloneHeap01);
	SUITE_ADD_TEST(suite, testmoveHeapElements01);
	SUITE_ADD_TEST(suite, testmoveHeapElements02);


	return suite;
}

