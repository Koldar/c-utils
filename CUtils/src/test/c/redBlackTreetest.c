/*
 * redBlackTreetest.c
 *
 *  Created on: Jun 9, 2017
 *      Author: koldar
 */

#include <stdlib.h>

#include "log.h"
#include "CuTest.h"
#include <assert.h>
#include "redBlackTree.h"
#include "defaultFunctions.h"

void testRedBlackTree01(CuTest* tc) {
	rb_tree* tree = cuRedBlackTreeNew(cuPayloadFunctionsIntValue());
	cuRedBlackTreeDestroy(tree, NULL);
}

void testRedBlackTree02(CuTest* tc) {
	rb_tree* tree = cuRedBlackTreeNew(cuPayloadFunctionsIntValue());

	assert(cuRedBlackTreeGetSize(tree) == 0);
	assert(cuRedBlackTreeIsEmpty(tree));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(5)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(3)) == false);

	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(5));

	assert(cuRedBlackTreeGetSize(tree) == 1);
	assert(!cuRedBlackTreeIsEmpty(tree));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(5)) == true);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(3)) == false);

	cuRedBlackTreeDestroyWithElements(tree, NULL);
}

///test additional adds
void testRedBlackTree03(CuTest* tc) {
	rb_tree* tree = cuRedBlackTreeNew(cuPayloadFunctionsIntValue());

	assert(cuRedBlackTreeGetSize(tree) == 0);
	assert(cuRedBlackTreeIsEmpty(tree));

	int memory[10];
	int t;
	for (int i=0; i<100; i++) {
		t = rand()%50;
		if ((i % 10) == 0) {
			memory[i/10] = t;
		}
		cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(t));
	}

	assert(cuRedBlackTreeGetSize(tree) == 100);
	assert(!cuRedBlackTreeIsEmpty(tree));
	for (int i=0; i<10; i++) {
		assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(memory[i])));
	}

	cuRedBlackTreeDestroyWithElements(tree, NULL);
}

///test minimum
void testRedBlackTree04(CuTest* tc) {
	rb_tree* tree = cuRedBlackTreeNew(cuPayloadFunctionsIntValue());

	assert(cuRedBlackTreeGetSize(tree) == 0);
	assert(cuRedBlackTreeIsEmpty(tree));

	int t;
	for (int i=0; i<100; i++) {
		t = rand()%50 + 5;
		cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(t));
	}
	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(3));

	assert(cuRedBlackTreeGetSize(tree) == 101);
	assert(!cuRedBlackTreeIsEmpty(tree));
	int min = CU_CAST_PTR2INT(cuRedBlackTreeGetMinimum(tree));
	info("the minimum is %d", min);
	assert(min == 3);

	cuRedBlackTreeDestroyWithElements(tree, NULL);
}

///test minimum in empty tree
void testRedBlackTree05(CuTest* tc) {
	rb_tree* tree = cuRedBlackTreeNew(cuPayloadFunctionsIntValue());

	assert(cuRedBlackTreeGetSize(tree) == 0);
	assert(cuRedBlackTreeIsEmpty(tree));
	assert(cuRedBlackTreeGetMinimum(tree) == NULL);

	cuRedBlackTreeDestroyWithElements(tree, NULL);
}

///test removal
void testRedBlackTree06(CuTest* tc) {
	rb_tree* tree = cuRedBlackTreeNew(cuPayloadFunctionsIntValue());

	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(5));
	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(4));
	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(7));
	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(2));
	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(8));

	assert(cuRedBlackTreeGetSize(tree) == 5);

	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(1)) == false);
	assert(cuRedBlackTreeGetSize(tree) == 5);

	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(2)) == true);
	assert(cuRedBlackTreeGetSize(tree) == 4);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(2)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(4)));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(5)));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(7)));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(8)));

	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(5)) == true);
	assert(cuRedBlackTreeGetSize(tree) == 3);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(2)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(4)));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(5)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(7)));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(8)));

	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(8)) == true);
	assert(cuRedBlackTreeGetSize(tree) == 2);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(2)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(4)));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(5)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(7)));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(8)) == false);

	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(4)) == true);
	assert(cuRedBlackTreeGetSize(tree) == 1);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(2)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(4)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(5)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(7)));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(8)) == false);

	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(7)) == true);
	assert(cuRedBlackTreeGetSize(tree) == 0);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(2)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(4)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(5)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(7)) == false);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(8)) == false);

	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(7)) == false);

	cuRedBlackTreeDestroyWithElements(tree, NULL);
}

//some insertion and removal in the red black tree
void testRedBlackTree07(CuTest* tc) {
	rb_tree* tree = cuRedBlackTreeNew(cuPayloadFunctionsIntValue());

	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(5));
	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(4));
	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(7));

	assert(!cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(3)));
	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(4)));

	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(4)) == false);
	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(4));
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(4)) == true);

	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(7)));
	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(5)));
	assert(cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(4)));

	assert(!cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(4)));
	assert(!cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(5)));
	assert(!cuRedBlackTreeRemoveItem(tree, CU_CAST_INT2PTR(7)));


	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(2));
	cuRedBlackTreeAddItem(tree, CU_CAST_INT2PTR(8));

	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(2)) == true);
	assert(cuRedBlackTreeContainsItem(tree, CU_CAST_INT2PTR(8)) == true);

	cuRedBlackTreeDestroyWithElements(tree, NULL);
}


CuSuite* CuRedBlackTreeSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testRedBlackTree01);
	SUITE_ADD_TEST(suite, testRedBlackTree02);
	SUITE_ADD_TEST(suite, testRedBlackTree03);
	SUITE_ADD_TEST(suite, testRedBlackTree04);
	SUITE_ADD_TEST(suite, testRedBlackTree05);
	SUITE_ADD_TEST(suite, testRedBlackTree06);
	SUITE_ADD_TEST(suite, testRedBlackTree07);

	return suite;
}

