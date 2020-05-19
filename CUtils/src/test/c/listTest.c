/*
 * RegexTest.c
 *
 *  Created on: Dec 23, 2016
 *      Author: koldar
 */

#include "CuTest.h"
#include "list.h"
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "macros.h"
#include "defaultFunctions.h"
#include "log.h"


static bool testFindNumberLambda(const CU_PDOC(int*) _c, const var_args* va) {
	int* c = (int*) _c;
	return *c == cuVarArgsGetItem(va, 0, int);
}

void testList01(CuTest* suite) {
	list* l = cuListNew();
	cuListDestroy(l, NULL);
}

void testList02(CuTest* suite) {
	list* l = cuListNew();

	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	assert(cuListIsEmpty(l) == true);
	assert(cuListGetSize(l) == 0);
	cuListAddHead(l, &a);
	assert(cuListIsEmpty(l) == false);
	assert(cuListGetSize(l) == 1);
	cuListAddHead(l, &b);
	assert(cuListIsEmpty(l) == false);
	assert(cuListGetSize(l) == 2);
	cuListAddHead(l, &c);
	assert(cuListIsEmpty(l) == false);
	assert(cuListGetSize(l) == 3);
	cuListAddHead(l, &d);
	assert(cuListIsEmpty(l) == false);
	assert(cuListGetSize(l) == 4);

	cuListDestroy(l, NULL);
}

void testList03(CuTest* suite) {
	list* l = cuListNew();

	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	assert(cuListIsEmpty(l) == true);
	assert(cuListGetSize(l) == 0);
	cuListAddTail(l, &a);
	assert(cuListIsEmpty(l) == false);
	assert(cuListGetSize(l) == 1);
	cuListAddTail(l, &b);
	assert(cuListIsEmpty(l) == false);
	assert(cuListGetSize(l) == 2);
	cuListAddTail(l, &c);
	assert(cuListIsEmpty(l) == false);
	assert(cuListGetSize(l) == 3);
	cuListAddTail(l, &d);
	assert(cuListIsEmpty(l) == false);
	assert(cuListGetSize(l) == 4);

	cuListDestroy(l, NULL);
}

void testList04(CuTest* suite) {
	list* lst = cuListNew();

	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);

	int sum = 0;
	CU_ITERATE_OVER_LIST(lst, cell, payload, int*) {
		sum += *payload;
	}
	assert(sum == 10);

	cuListDestroy(lst, NULL);
}

void testList05(CuTest* suite) {
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);

	popped = cuListPopFrom(lst);
	assert(cuListGetSize(lst) == 3);
	assert(*popped == 1);
	popped = cuListPopFrom(lst);
	assert(cuListGetSize(lst) == 2);
	assert(*popped == 2);
	popped = cuListPopFrom(lst);
	assert(cuListGetSize(lst) == 1);
	assert(*popped == 3);
	popped = cuListPopFrom(lst);
	assert(cuListGetSize(lst) == 0);
	assert(*popped == 4);
	popped = cuListPopFrom(lst);
	assert(cuListGetSize(lst) == 0);
	assert(popped == NULL);

	cuListDestroy(lst, NULL);
}

void testList06(CuTest* suite) {
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);

	int* value = NULL;
	cuInitVarArgsOnStack(va1, 3);
	value = cuListfindItem(lst, testFindNumberLambda, va1);
	assert((*value) == 3);

	cuInitVarArgsOnStack(va2, 5);
	value = cuListfindItem(lst, testFindNumberLambda, va2);
	printf("value is %p\n", value);
	assert(value == NULL);

	cuListDestroy(lst, NULL);
}

void testList07(CuTest* suite) {
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);

	cuInitVarArgsOnStack(va1, 5);
	cuListRemoveItem(lst, testFindNumberLambda, va1);
	assert(cuListGetSize(lst) == 4);

	cuListDestroy(lst, NULL);
}

void testList08(CuTest* suite) {
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);

	cuInitVarArgsOnStack(va1, 1);
	cuListRemoveItem(lst, testFindNumberLambda, va1);
	assert(cuListGetSize(lst) == 3);
	int sum = 0;
	CU_ITERATE_OVER_LIST(lst, cell, value, int*) {
		sum += *value;
	}
	assert(sum == 9);

	cuListDestroy(lst, NULL);
}

void testList09(CuTest* suite) {
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);

	cuInitVarArgsOnStack(va1, 4);
	cuListRemoveItem(lst, testFindNumberLambda, va1);
	assert(cuListGetSize(lst) == 3);
	int sum = 0;
	CU_ITERATE_OVER_LIST(lst, cell, value, int*) {
		sum += *value;
	}
	assert(sum == 6);

	cuListDestroy(lst, NULL);
}

void testList10(CuTest* suite) {
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);


	cuInitVarArgsOnStack(va1, 3);
	cuListRemoveItem(lst, testFindNumberLambda, va1);
	assert(cuListGetSize(lst) == 3);
	int sum = 0;
	CU_ITERATE_OVER_LIST(lst, cell, value, int*) {
		sum += *value;
	}
	assert(sum == 7);

	cuListDestroy(lst, NULL);
}

//test cuListClear
void testList11(CuTest* suite) {
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	assert(cuListGetSize(lst) == 0);
	cuListClear(lst);
	assert(cuListGetSize(lst) == 0);

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);


	assert(cuListGetSize(lst) == 4);
	cuListClear(lst);
	assert(cuListGetSize(lst) == 0);

	cuListAddHead(lst, &a);
	cuListAddHead(lst, &b);
	cuListAddHead(lst, &c);
	cuListAddHead(lst, &d);

	assert(cuListGetSize(lst) == 4);

	cuListClear(lst);
	assert(cuListGetSize(lst) == 0);

	cuListDestroy(lst, NULL);
}

static bool lambdaNumToRemove(const CU_PDOC(int*) _a, const var_args* va) {
	int* a = (int*) _a;
	return *a == cuVarArgsGetItem(va, 0, int);
}

///test remove element from list on head
void testList12(CuTest* tc) {
	int sum = 0;
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);

	assert(cuListGetSize(lst) == 4);
	sum = 0;
	CU_ITERATE_OVER_LIST(lst, cell, n, int*) {
		sum += *n;
	}
	assert(sum == 10);

	//remove head
	cuInitVarArgsOnStack(va1, 1);
	cuListRemoveItem(lst, lambdaNumToRemove, va1);

	assert(cuListGetSize(lst) == 3);
	sum = 0;
	CU_ITERATE_OVER_LIST(lst, cell1, n1, int*) {
		sum += *n1;
	}
	assert(sum == 9);

	cuListDestroy(lst, NULL);
}

///test remove element from list on tail
void testList13(CuTest* tc) {
	int sum = 0;
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);

	assert(cuListGetSize(lst) == 4);
	sum = 0;
	CU_ITERATE_OVER_LIST(lst, cell, n, int*) {
		sum += (int)*n;
	}
	assert(sum == 10);

	//remove head
	cuInitVarArgsOnStack(va2, 4);
	cuListRemoveItem(lst, lambdaNumToRemove, va2);

	assert(cuListGetSize(lst) == 3);
	sum = 0;
	CU_ITERATE_OVER_LIST(lst, cell1, n1, int*) {
		sum += (int)*n1;
	}
	assert(sum == 6);

	cuListDestroy(lst, NULL);
}

///test remove element from list in middle
void testList14(CuTest* tc) {
	int sum = 0;
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	cuListAddTail(lst, &a);
	cuListAddTail(lst, &b);
	cuListAddTail(lst, &c);
	cuListAddTail(lst, &d);

	assert(cuListGetSize(lst) == 4);
	sum = 0;
	CU_ITERATE_OVER_LIST(lst, cell, n, int*) {
		sum += (int)*n;
	}
	assert(sum == 10);

	//remove head
	cuInitVarArgsOnStack(va3, 3);
	cuListRemoveItem(lst, lambdaNumToRemove, va3);

	assert(cuListGetSize(lst) == 3);
	sum = 0;
	CU_ITERATE_OVER_LIST(lst, cell1, n1, int*) {
		sum += (int)*n1;
	}
	assert(sum == 7);

	cuListDestroy(lst, NULL);
}

//test ::cuListGetLastElement
void testGetLastElementOfList(CuTest* suite) {
	list* lst = cuListNew();

	int* popped = 0;
	int a = 1;
	int b = 2;
	int c = 3;
	int d = 4;

	assert(cuListGetLastElement(lst) == NULL);

	cuListAddTail(lst, &a);
	assert(cuListGetLastElement(lst) == &a);
	cuListAddHead(lst, &b);
	assert(cuListGetLastElement(lst) == &a);
	cuListAddTail(lst, &c);
	assert(cuListGetLastElement(lst) == &c);

	cuListClear(lst);

	assert(cuListGetLastElement(lst) == NULL);

	cuListDestroy(lst, NULL);
}

void testRemoval(CuTest* suite) {
	list* lst = cuListNew(cuPayloadFunctionsIntPtr());

	int* a = malloc(sizeof(int)); *a = 1;
	int* b = malloc(sizeof(int)); *b = 2;
	int* c = malloc(sizeof(int)); *c = 3;
	int* d = malloc(sizeof(int)); *d = 4;

	cuListAddTail(lst, a);
	cuListAddTail(lst, b);
	cuListAddTail(lst, c);
	cuListAddTail(lst, d);


	cuInitVarArgsOnStack(va1, 3);
	assert(cuListRemoveAndDestroyItem(lst,testFindNumberLambda, va1) == true);
	assert(cuListGetSize(lst) == 3);
	cuInitVarArgsOnStack(va2, 3);
	assert(cuListRemoveAndDestroyItem(lst,testFindNumberLambda, va2) == false);

	cuListDestroyWithElement(lst, NULL);
}

void testgetNthElementOfList01(CuTest* tc) {
	list* lst = cuListNew(cuPayloadFunctionsIntPtr());

	int* a = malloc(sizeof(int)); *a = 1;
	int* b = malloc(sizeof(int)); *b = 2;
	int* c = malloc(sizeof(int)); *c = 3;
	int* d = malloc(sizeof(int)); *d = 4;

	cuListAddTail(lst, a);
	cuListAddTail(lst, b);
	cuListAddTail(lst, c);
	cuListAddTail(lst, d);

	assert(cuListGetNthItem(lst, 0, int*) == a);
	assert(cuListGetNthItem(lst, 1, int*) == b);
	assert(cuListGetNthItem(lst, 2, int*) == c);
	assert(cuListGetNthItem(lst, 3, int*) == d);
	assert(cuListGetNthItem(lst, -1, int*) == NULL);
	assert(cuListGetNthItem(lst, 4, int*) == NULL);

	cuListDestroyWithElement(lst, NULL);
}

void testremoveElementInListCell01(CuTest* tc) {
	list* lst = cuListNew(cuPayloadFunctionsIntPtr());

	int* a = malloc(sizeof(int)); *a = 1;
	int* b = malloc(sizeof(int)); *b = 2;
	int* c = malloc(sizeof(int)); *c = 3;
	int* d = malloc(sizeof(int)); *d = 4;

	cuListAddTail(lst, a);
	cuListAddTail(lst, b);
	cuListAddTail(lst, c);
	cuListAddTail(lst, d);

	int sum = 0;
	CU_VARIABLE_ITERATE_OVER_LIST(lst, prevCell, nowCell, number1, int*) {
		sum += *number1;
	}
	assert(sum == 10);
	assert(cuListGetSize(lst) == 4);

	sum = 0;
	CU_VARIABLE_ITERATE_OVER_LIST(lst, prevCell, nowCell, number2, int*) {
		info("prevcell: %p; nowCell: %p", prevCell, nowCell);
		if (*number2 <= 2) {
			cuListRemoveItemInListCell(lst, &prevCell, nowCell);
		} else {
			sum += *number2;
		}
	}
	assert(sum == 7);
	assert(cuListGetSize(lst) == 2);

	sum = 0;
	CU_VARIABLE_ITERATE_OVER_LIST(lst, prevCell, nowCell, number3, int*) {
		if (*number3 > 2) {
			cuListRemoveItemInListCell(lst, &prevCell, nowCell);
		} else {
			sum += *number3;
		}
	}
	assert(sum == 0);
	assert(cuListGetSize(lst) == 0);

	cuListDestroy(lst, NULL);
	free(a);
	free(b);
	free(c);
	free(d);
}

void testmoveListContents(CuTest* tc) {
	list* a = cuListNew();
	list* b = cuListNew();

	cuListAddTail(a, CU_CAST_INT2PTR(1));
	cuListAddTail(a, CU_CAST_INT2PTR(2));
	cuListAddTail(a, CU_CAST_INT2PTR(3));
	cuListAddTail(a, CU_CAST_INT2PTR(4));

	cuListAddTail(b, CU_CAST_INT2PTR(5));
	cuListAddTail(b, CU_CAST_INT2PTR(6));
	cuListAddTail(b, CU_CAST_INT2PTR(7));

	assert(cuListGetSize(a) == 4);
	assert(cuListGetSize(b) == 3);

	cuListMoveContent(a, b);

	assert(cuListGetSize(a) == 7);
	assert(cuListGetSize(b) == 0);

	int sum;

	sum = 0;
	CU_ITERATE_OVER_LIST(a, cell1, num1, intptr_t) {
		sum += num1;
	}
	info("sum is %d", sum);
	assert(sum == 28);

	sum = 0;
	CU_ITERATE_OVER_LIST(b, cell2, num1, intptr_t) {
		sum += num1;
	}
	assert(sum == 0);

	cuListAddTail(b, CU_CAST_INT2PTR(8));
	cuListAddTail(b, CU_CAST_INT2PTR(9));
	cuListAddTail(b, CU_CAST_INT2PTR(10));

	cuListMoveContent(a, b);

	assert(cuListGetSize(a) == 10);
	assert(cuListGetSize(b) == 0);

	sum = 0;
	CU_ITERATE_OVER_LIST(a, cell3, num, intptr_t) {
		sum += num;
	}
	assert(sum == 55);

	sum = 0;
	CU_ITERATE_OVER_LIST(b, cell4, num, intptr_t) {
		sum += num;
	}
	assert(sum == 0);

	cuListMoveContent(a, b);


	assert(cuListGetSize(a) == 10);
	assert(cuListGetSize(b) == 0);

	cuListMoveContent(b, a);

	assert(cuListGetSize(a) == 0);
	assert(cuListGetSize(b) == 10);

	cuListMoveContent(a, b);

	assert(cuListGetSize(a) == 10);
	assert(cuListGetSize(b) == 0);


	cuListDestroy(a, NULL);
	cuListDestroy(b, NULL);
}

void testVariableIterateOnList(CuTest* tc) {
	list* l = cuListNew();
	int sum = 0;

	for (int i=0; i<9; i++) {
		cuListAddTail(l, CU_CAST_INT2PTR(i+1));
	}

	int j=0;
	{
		CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, j, intptr_t) {
			if (j == 1) {
				assert(prevCell == NULL);
			} else {
				assert(prevCell != NULL);
				assert(CU_CAST_PTR2INT(_cuListGetPayloadOfCell(prevCell)) == (j-1));
			}
		}
	}

	sum = 0;
	{
		CU_ITERATE_OVER_LIST(l, cell, num1, intptr_t) {
			sum += num1;
		}
	}
	assert(sum == 45);

	//remove head
	{
		CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, j, intptr_t) {
			if (j == 1) {
				cuListRemoveItemInListCell(l, &prevCell, cell);
			}
		}
	}

	sum = 0;
	{
		CU_ITERATE_OVER_LIST(l, cell, num3, intptr_t) {
			sum += num3;
		}
	}
	assert(sum == 44);

	//remove tail
	{
		CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, j, intptr_t) {
			if (j == 9) {
				cuListRemoveItemInListCell(l, &prevCell, cell);
			}
		}
	}

	sum = 0;
	{
		CU_ITERATE_OVER_LIST(l, cell, num5, intptr_t) {
			sum += num5;
		}
		assert(sum == 35);
	}

	//remove something in between
	{
		CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, j, intptr_t) {
			if (j == 5) {
				cuListRemoveItemInListCell(l, &prevCell, cell);
			}
		}
	}

	sum = 0;
	{
		CU_ITERATE_OVER_LIST(l, cell, num7, intptr_t) {
			sum += num7;
		}
	}
	assert(sum == 30);

	//remove the second element
	{
		CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, j, intptr_t) {
			if (j == 3) {
				cuListRemoveItemInListCell(l, &prevCell, cell);
			}
		}
	}

	sum = 0;
	{
		CU_ITERATE_OVER_LIST(l, cell, num8, intptr_t) {
			sum += num8;
		}
	}
	assert(sum == 27);

	cuListDestroy(l, NULL);
}

void test_addInListItemAfter_01(CuTest* tc) {
	int_list* l = cuListNew(cuPayloadFunctionsIntValue());

	cuListAddTail(l, CU_CAST_INT2PTR(1));
	cuListAddTail(l, CU_CAST_INT2PTR(2));
	cuListAddTail(l, CU_CAST_INT2PTR(3));
	cuListAddTail(l, CU_CAST_INT2PTR(4));
	cuListAddTail(l, CU_CAST_INT2PTR(5));

	//fetch the desired cell
	list_cell* interestingCell = NULL;
	CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, p, intptr_t) {
		if (p == 1) {
			interestingCell = cell;
		}
	}
	cuListAddItemAfter(l, interestingCell, CU_CAST_INT2PTR(10));

	assert(cuListGetSize(l) == 6);
	assert(CU_CAST_PTR2INT(cuListGetHead0(l)) == 1);
	assert(CU_CAST_PTR2INT(cuListGetTail0(l)) == 5);
	assert(cuListGetNthItem(l, 0, intptr_t) == 1);
	assert(cuListGetNthItem(l, 1, intptr_t) == 10);
	assert(cuListGetNthItem(l, 2, intptr_t) == 2);

	cuListDestroyWithElement(l, NULL);
}

void test_addInListItemAfter_02(CuTest* tc) {
	int_list* l = cuListNew(cuPayloadFunctionsIntValue());

	cuListAddTail(l, CU_CAST_INT2PTR(1));
	cuListAddTail(l, CU_CAST_INT2PTR(2));
	cuListAddTail(l, CU_CAST_INT2PTR(3));
	cuListAddTail(l, CU_CAST_INT2PTR(4));
	cuListAddTail(l, CU_CAST_INT2PTR(5));

	//fetch the desired cell
	list_cell* interestingCell = NULL;
	CU_VARIABLE_ITERATE_OVER_LIST(l, prevCell, cell, p, intptr_t) {
		if (p == 5) {
			interestingCell = cell;
		}
	}
	cuListAddItemAfter(l, interestingCell, CU_CAST_INT2PTR(10));

	assert(cuListGetSize(l) == 6);
	assert(CU_CAST_PTR2INT(cuListGetHead0(l)) == 1);
	assert(CU_CAST_PTR2INT(cuListGetTail0(l)) == 10);
	assert(cuListGetNthItem(l, 0, intptr_t) == 1);
	assert(cuListGetNthItem(l, 1, intptr_t) == 2);
	assert(cuListGetNthItem(l, 2, intptr_t) == 3);
	assert(cuListGetNthItem(l, 3, intptr_t) == 4);
	assert(cuListGetNthItem(l, 4, intptr_t) == 5);
	assert(cuListGetNthItem(l, 5, intptr_t) == 10);

	cuListDestroyWithElement(l, NULL);
}

//insert on head
void test_addInListItemAfter2_01(CuTest* tc) {
	int_list* l = cuListNew(cuPayloadFunctionsIntValue());
	int sum = 0;
	int position = 0;

	cuListAddTail(l, CU_CAST_INT2PTR(1));
	cuListAddTail(l, CU_CAST_INT2PTR(2));
	cuListAddTail(l, CU_CAST_INT2PTR(3));
	cuListAddTail(l, CU_CAST_INT2PTR(4));
	cuListAddTail(l, CU_CAST_INT2PTR(5));

	cuListAddItemAt(l, 0, CU_CAST_INT2PTR(100));
	assert(cuListGetSize(l) == 6);

	position = 0;
	CU_ITERATE_OVER_LIST(l, cell, _i, intptr_t) {
		sum += (_i * position);
		position++;
	}
	assert(sum == (0*100 + 1*1 + 2*2 + 3*3 + 4*4 + 5*5));

	cuListDestroyWithElement(l, NULL);
}

//insert on tail
void test_addInListItemAfter2_02(CuTest* tc) {
	int_list* l = cuListNew(cuPayloadFunctionsIntValue());
	int sum = 0;
	int position = 0;

	cuListAddTail(l, CU_CAST_INT2PTR(1));
	cuListAddTail(l, CU_CAST_INT2PTR(2));
	cuListAddTail(l, CU_CAST_INT2PTR(3));
	cuListAddTail(l, CU_CAST_INT2PTR(4));
	cuListAddTail(l, CU_CAST_INT2PTR(5));

	cuListAddItemAt(l, 4, CU_CAST_INT2PTR(100));
	assert(cuListGetSize(l) == 6);

	position = 0;
	CU_ITERATE_OVER_LIST(l, cell, _i, intptr_t) {
		sum += (_i * position);
		position++;
	}
	assert(sum == (0*1 + 1*2 + 2*3 + 3*4 + 4*5 + 5*100));

	cuListDestroyWithElement(l, NULL);
}

//insert in middle
void test_addInListItemAfter2_03(CuTest* tc) {
	int_list* l = cuListNew(cuPayloadFunctionsIntValue());
	int sum = 0;
	int position = 0;

	cuListAddTail(l, CU_CAST_INT2PTR(1));
	cuListAddTail(l, CU_CAST_INT2PTR(2));
	cuListAddTail(l, CU_CAST_INT2PTR(3));
	cuListAddTail(l, CU_CAST_INT2PTR(4));
	cuListAddTail(l, CU_CAST_INT2PTR(5));

	cuListAddItemAt(l, 3, CU_CAST_INT2PTR(100));
	assert(cuListGetSize(l) == 6);

	position = 0;
	CU_ITERATE_OVER_LIST(l, cell, _i, intptr_t) {
		sum += (_i * position);
		position++;
	}
	assert(sum == (0*1 + 1*2 + 2*3 + 3*100 + 4*4 + 5*5));

	cuListDestroyWithElement(l, NULL);
}

//set head
void test_setInListItemAt_01(CuTest* tc) {
	int_list* l = cuListNew(cuPayloadFunctionsIntValue());
	int sum = 0;
	int position = 0;

	cuListAddTail(l, CU_CAST_INT2PTR(1));
	cuListAddTail(l, CU_CAST_INT2PTR(2));
	cuListAddTail(l, CU_CAST_INT2PTR(3));
	cuListAddTail(l, CU_CAST_INT2PTR(4));
	cuListAddTail(l, CU_CAST_INT2PTR(5));

	cuListSetItemAt(l, 0, CU_CAST_INT2PTR(100));
	assert(cuListGetSize(l) == 5);

	position = 0;
	CU_ITERATE_OVER_LIST(l, cell, _i, intptr_t) {
		sum += (_i * position);
		position++;
	}
	assert(sum == (0*100 + 1*2 + 2*3 + 3*4 + 4*5));

	cuListDestroyWithElement(l, NULL);
}

//set in tail
void test_setInListItemAt_02(CuTest* tc) {
	int_list* l = cuListNew(cuPayloadFunctionsIntValue());
	int sum = 0;
	int position = 0;

	cuListAddTail(l, CU_CAST_INT2PTR(1));
	cuListAddTail(l, CU_CAST_INT2PTR(2));
	cuListAddTail(l, CU_CAST_INT2PTR(3));
	cuListAddTail(l, CU_CAST_INT2PTR(4));
	cuListAddTail(l, CU_CAST_INT2PTR(5));

	cuListSetItemAt(l, 4, CU_CAST_INT2PTR(100));
	assert(cuListGetSize(l) == 5);

	position = 0;
	CU_ITERATE_OVER_LIST(l, cell, i, intptr_t) {
		sum += (i * position);
		position++;
	}
	assert(sum == (0*1 + 1*2 + 2*3 + 3*4 + 4*100));

	cuListDestroyWithElement(l, NULL);
}

//set in middle
void test_setInListItemAt_03(CuTest* tc) {
	int_list* l = cuListNew(cuPayloadFunctionsIntValue());
	int sum = 0;
	int position = 0;

	cuListAddTail(l, CU_CAST_INT2PTR(1));
	cuListAddTail(l, CU_CAST_INT2PTR(2));
	cuListAddTail(l, CU_CAST_INT2PTR(3));
	cuListAddTail(l, CU_CAST_INT2PTR(4));
	cuListAddTail(l, CU_CAST_INT2PTR(5));

	cuListSetItemAt(l, 3, CU_CAST_INT2PTR(100));
	assert(cuListGetSize(l) == 5);

	position = 0;
	CU_ITERATE_OVER_LIST(l, cell, _i, intptr_t) {
		sum += (_i * position);
		position++;
	}
	assert(sum == (0*1 + 1*2 + 2*3 + 3*100 + 4*5));

	cuListDestroyWithElement(l, NULL);
}

///remove and destroy head on empty list
void test_cuListRemoveHeadAndDestroyItem_01(CuTest* tc) {
	CU_WITH(int_list* l = cuListNew(cuPayloadFunctionsIntValue()))(cuListDestroyWithElement(l, NULL)) {
		cuListRemoveHeadAndDestroyItem(l, NULL);
		assert(cuListIsEmpty(l));
	}
}

///remove and destroy head on empty list
void test_cuListRemoveHeadAndDestroyItem_02(CuTest* tc) {
	CU_WITH(int_list* l = cuListNew(cuPayloadFunctionsIntValue()))(cuListDestroyWithElement(l, NULL)) {
		cuListAddTail(l, CU_CAST_INT2PTR(5));
		cuListRemoveHeadAndDestroyItem(l, NULL);
		assert(cuListIsEmpty(l));
	}
}

///remove and destroy head on list with 2 elements
void test_cuListRemoveHeadAndDestroyItem_03(CuTest* tc) {
	CU_WITH(int_list* l = cuListNew(cuPayloadFunctionsIntValue()))(cuListDestroyWithElement(l, NULL)) {
		cuListAddTail(l, CU_CAST_INT2PTR(5));
		cuListAddTail(l, CU_CAST_INT2PTR(4));
		cuListRemoveHeadAndDestroyItem(l, NULL);
		assert(!cuListIsEmpty(l));
		assert(CU_CAST_PTR2INT(cuListGetHead0(l)) == 4);
		assert(cuListGetSize(l) == 1);
	}
}

///remove and destroy head on list with 3 elements
void test_cuListRemoveHeadAndDestroyItem_04(CuTest* tc) {
	CU_WITH(int_list* l = cuListNew(cuPayloadFunctionsIntValue()))(cuListDestroyWithElement(l, NULL)) {
		cuListAddTail(l, CU_CAST_INT2PTR(5));
		cuListAddTail(l, CU_CAST_INT2PTR(4));
		cuListAddTail(l, CU_CAST_INT2PTR(3));
		cuListRemoveHeadAndDestroyItem(l, NULL);
		assert(!cuListIsEmpty(l));
		assert(CU_CAST_PTR2INT(cuListGetHead0(l)) == 4);
		assert(CU_CAST_PTR2INT(cuListGetTail0(l)) == 3);
		assert(cuListGetSize(l) == 2);
	}
}

CuSuite* CuProjectSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testList01);
	SUITE_ADD_TEST(suite, testList02);
	SUITE_ADD_TEST(suite, testList03);
	SUITE_ADD_TEST(suite, testList04);
	SUITE_ADD_TEST(suite, testList05);
	SUITE_ADD_TEST(suite, testList06);
	SUITE_ADD_TEST(suite, testList07);
	SUITE_ADD_TEST(suite, testList08);
	SUITE_ADD_TEST(suite, testList09);
	SUITE_ADD_TEST(suite, testList10);
	SUITE_ADD_TEST(suite, testList11);
	SUITE_ADD_TEST(suite, testList12);
	SUITE_ADD_TEST(suite, testList13);
	SUITE_ADD_TEST(suite, testList14);
	SUITE_ADD_TEST(suite, testGetLastElementOfList);
	SUITE_ADD_TEST(suite, testRemoval);
	SUITE_ADD_TEST(suite, testgetNthElementOfList01);
	SUITE_ADD_TEST(suite, testremoveElementInListCell01);
	SUITE_ADD_TEST(suite, testmoveListContents);
	SUITE_ADD_TEST(suite, testVariableIterateOnList);
	SUITE_ADD_TEST(suite, test_addInListItemAfter_01);
	SUITE_ADD_TEST(suite, test_addInListItemAfter_02);

	SUITE_ADD_TEST(suite, test_addInListItemAfter2_01);
	SUITE_ADD_TEST(suite, test_addInListItemAfter2_02);
	SUITE_ADD_TEST(suite, test_addInListItemAfter2_03);

	SUITE_ADD_TEST(suite, test_setInListItemAt_01);
	SUITE_ADD_TEST(suite, test_setInListItemAt_02);
	SUITE_ADD_TEST(suite, test_setInListItemAt_03);

	SUITE_ADD_TEST(suite, test_cuListRemoveHeadAndDestroyItem_01);
	SUITE_ADD_TEST(suite, test_cuListRemoveHeadAndDestroyItem_02);
	SUITE_ADD_TEST(suite, test_cuListRemoveHeadAndDestroyItem_03);
	SUITE_ADD_TEST(suite, test_cuListRemoveHeadAndDestroyItem_04);



	return suite;
}
