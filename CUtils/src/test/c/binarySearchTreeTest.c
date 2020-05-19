/*
 * binarySearchTreeTest.c
 *
 *  Created on: Jun 6, 2017
 *      Author: koldar
 */

#include "CuTest.h"
#include "defaultFunctions.h"
#include "binaryTree.h"
#include "log.h"
#include <assert.h>

void testBinarySearchTree01(CuTest* tc) {
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());
	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree02(CuTest* tc) {
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());

	assert(cuBinaryTreeGetSize(bt) == 0);
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(4));
	assert(cuBinaryTreeGetSize(bt) == 1);

	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree03(CuTest* tc) {
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());

	assert(cuBinaryTreeGetSize(bt) == 0);
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(4));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(3));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(5));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(1));
	assert(cuBinaryTreeGetSize(bt) == 4);

	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree04(CuTest* tc) {
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());

	assert(cuBinaryTreeGetSize(bt) == 0);
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(4));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(3));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(5));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(1));
	assert(cuBinaryTreeGetSize(bt) == 4);

	assert(cuBinaryTreeContainsItem(bt, CU_CAST_INT2PTR(6)) == false);
	assert(cuBinaryTreeContainsItem(bt, CU_CAST_INT2PTR(0)) == false);
	assert(cuBinaryTreeContainsItem(bt, CU_CAST_INT2PTR(1)) == true);
	assert(cuBinaryTreeContainsItem(bt, CU_CAST_INT2PTR(3)) == true);
	assert(cuBinaryTreeContainsItem(bt, CU_CAST_INT2PTR(4)) == true);
	assert(cuBinaryTreeContainsItem(bt, CU_CAST_INT2PTR(5)) == true);

	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree05(CuTest* tc) {
	char* str;
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());

	assert(cuBinaryTreeGetSize(bt) == 0);
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(4));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(3));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(5));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(1));
	assert(cuBinaryTreeGetSize(bt) == 4);

	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(7)) == false);
	assert(cuBinaryTreeGetSize(bt) == 4);
	//remove the root itself
	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(4)) == true);
	assert(cuBinaryTreeGetSize(bt) == 3);
	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(4)) == false);
	assert(cuBinaryTreeGetSize(bt) == 3);

	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(3)) == true);
	assert(cuBinaryTreeGetSize(bt) == 2);
	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(1)) == true);
	assert(cuBinaryTreeGetSize(bt) == 1);
	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(5)) == true);
	assert(cuBinaryTreeGetSize(bt) == 0);

	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree06(CuTest* tc) {
	char* str;
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());

	assert(cuBinaryTreeGetSize(bt) == 0);
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(4));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(3));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(5));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(1));
	assert(cuBinaryTreeGetSize(bt) == 4);

	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(7)) == false);
	assert(cuBinaryTreeGetSize(bt) == 4);
	//remove the root itself
	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(4)) == true);
	assert(cuBinaryTreeGetSize(bt) == 3);
	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(4)) == false);
	assert(cuBinaryTreeGetSize(bt) == 3);

	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(3)) == true);
	assert(cuBinaryTreeGetSize(bt) == 2);

	//adds new elements
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(7));
	assert(cuBinaryTreeGetSize(bt) == 3);

	cuBinaryTreeClear(bt);
	assert(cuBinaryTreeGetSize(bt) == 0);

	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(1)) == false);
	assert(cuBinaryTreeGetSize(bt) == 0);
	assert(cuBinaryTreeRemoveItem(bt, CU_CAST_INT2PTR(5)) == false);
	assert(cuBinaryTreeGetSize(bt) == 0);

	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree07(CuTest* tc) {
	char* str;
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());
	int sum = 0;

	assert(cuBinaryTreeGetSize(bt) == 0);

	sum = 0;
	ITERATE_THROUGH_BST(bt, n, intptr_t) {
		sum += n;
	}
	assert(sum == 0);

	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree08(CuTest* tc) {
	char* str;
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());
	int sum = 0;

	assert(cuBinaryTreeGetSize(bt) == 0);
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(4));

	sum = 0;
	ITERATE_THROUGH_BST(bt, n, intptr_t) {
		sum += n;
	}
	critical("sum is %d", sum);
	assert(sum == 4);

	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree09(CuTest* tc) {
	char* str;
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());
	int sum = 0;

	assert(cuBinaryTreeGetSize(bt) == 0);
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(4));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(3));

	sum = 0;
	ITERATE_THROUGH_BST(bt, n, intptr_t) {
		info("adding %d to %d", n, sum);
		sum += n;
	}
	assert(sum == 7);

	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree10(CuTest* tc) {
	char* str;
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());
	int sum = 0;

	assert(cuBinaryTreeGetSize(bt) == 0);
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(4));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(5));

	sum = 0;
	ITERATE_THROUGH_BST(bt, n, intptr_t) {
		sum += n;
	}
	assert(sum == 9);

	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree11(CuTest* tc) {
	char* str;
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());
	int sum = 0;

	assert(cuBinaryTreeGetSize(bt) == 0);
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(4));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(3));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(5));

	sum = 0;
	ITERATE_THROUGH_BST(bt, n, intptr_t) {
		sum += n;
	}
	assert(sum == 12);

	cuBinaryTreeDestroy(bt, NULL);
}

void testBinarySearchTree12(CuTest* tc) {
	char* str;
	binary_tree* bt = cuBinaryTreeNew(cuPayloadFunctionsIntValue());
	int sum = 0;

	assert(cuBinaryTreeGetSize(bt) == 0);
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(4));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(3));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(5));
	cuBinaryTreeAddItem(bt, CU_CAST_INT2PTR(1));

	sum = 0;
	ITERATE_THROUGH_BST(bt, n, intptr_t) {
		sum += n;
	}
	assert(sum == 13);

	cuBinaryTreeDestroy(bt, NULL);
}

CuSuite* CuBinarySearchTreeSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testBinarySearchTree01);
	SUITE_ADD_TEST(suite, testBinarySearchTree02);
	SUITE_ADD_TEST(suite, testBinarySearchTree03);
	SUITE_ADD_TEST(suite, testBinarySearchTree04);
	SUITE_ADD_TEST(suite, testBinarySearchTree05);
	SUITE_ADD_TEST(suite, testBinarySearchTree06);
	SUITE_ADD_TEST(suite, testBinarySearchTree07);
	SUITE_ADD_TEST(suite, testBinarySearchTree08);
	SUITE_ADD_TEST(suite, testBinarySearchTree09);
	SUITE_ADD_TEST(suite, testBinarySearchTree10);
	SUITE_ADD_TEST(suite, testBinarySearchTree11);
	SUITE_ADD_TEST(suite, testBinarySearchTree12);

	return suite;
}


