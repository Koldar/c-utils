/**
 * @file
 *
 * @author koldar
 * @date Mar 30, 2018
 */

#include "CuTest.h"
#include "naive_queue.h"
#include "log.h"

static int evaluateInteger(const int _x) {
	return 10 + CU_CAST_PTR2INT(_x);
}

static bool isNumber(const CU_PDOC(int) _x, const var_args* context) {
	return CU_CAST_PTR2INT(_x) == cuVarArgsGetItem(context, 0, int);
}

void test_01(CuTest* tc) {
	payload_functions p = cuPayloadFunctionsIntValue();
	int_naive_queue* q = cuNaiveQueueNew(p, CU_AS_EVALUATOR(evaluateInteger), NULL);

	cuNaiveQueueDestroyWithElements(q, NULL);
}

void test_02(CuTest* tc) {
	payload_functions p = cuPayloadFunctionsIntValue();
	int_naive_queue* q = cuNaiveQueueNew(p, CU_AS_EVALUATOR(evaluateInteger), NULL);

	assert(cuNaiveQueueGetSize(q) == 0);
	assert(cuNaiveQueueIsEmpty(q));

	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(1));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(2));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(3));

	assert(!cuNaiveQueueIsEmpty(q));
	assert(cuNaiveQueueGetSize(q) == 3);

	assert(cuNaiveQueuePeekItem(q, intptr_t) == 1);
	assert(cuNaiveQueueGetSize(q) == 3);

	assert(cuNaiveQueuePopItem(q, intptr_t) == 1);
	assert(cuNaiveQueueGetSize(q) == 2);
	assert(cuNaiveQueuePeekItem(q, intptr_t) == 2);

	assert(cuNaiveQueuePopItem(q, intptr_t) == 2);
	assert(cuNaiveQueuePopItem(q, intptr_t) == 3);

	assert(cuNaiveQueueGetSize(q) == 0);
	assert(cuNaiveQueueIsEmpty(q));
	assert(cuNaiveQueuePeekItem(q, void*) == NULL);
	assert(cuNaiveQueuePopItem(q, void*) == NULL);

	cuNaiveQueueDestroyWithElements(q, NULL);
}

void test_03(CuTest* tc) {
	payload_functions p = cuPayloadFunctionsIntValue();
	int_naive_queue* q = cuNaiveQueueNew(p, CU_AS_EVALUATOR(evaluateInteger), NULL);

	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(3));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(2));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(1));

	assert(cuNaiveQueuePopItem(q, intptr_t) == 1);
	assert(cuNaiveQueuePopItem(q, intptr_t) == 2);
	assert(cuNaiveQueuePopItem(q, intptr_t) == 3);

	cuNaiveQueueDestroyWithElements(q, NULL);
}

///test mixed insertions
void test_04(CuTest* tc) {
	payload_functions p = cuPayloadFunctionsIntValue();
	int_naive_queue* q = cuNaiveQueueNew(p, CU_AS_EVALUATOR(evaluateInteger), NULL);

	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(1));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(3));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(2));

	assert(cuNaiveQueuePopItem(q, intptr_t) == 1);
	assert(cuNaiveQueuePopItem(q, intptr_t) == 2);
	assert(cuNaiveQueuePopItem(q, intptr_t) == 3);

	cuNaiveQueueDestroyWithElements(q, NULL);
}

void test_05(CuTest* tc) {
	payload_functions p = cuPayloadFunctionsIntValue();
	int_naive_queue* q = cuNaiveQueueNew(p, CU_AS_EVALUATOR(evaluateInteger), NULL);

	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(5));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(4));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(3));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(2));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(1));

	//remove head
	{ cuInitVarArgsOnStack(va, 1); assert(cuNaiveQueueRemoveItemWithElements(q, isNumber, va)); }
	assert(cuNaiveQueueGetSize(q) == 4);
	//remove tail
	{ cuInitVarArgsOnStack(va, 5); assert(cuNaiveQueueRemoveItemWithElements(q, isNumber, va)); }
	assert(cuNaiveQueueGetSize(q) == 3);
	//remove middle
	{ cuInitVarArgsOnStack(va, 3); assert(cuNaiveQueueRemoveItemWithElements(q, isNumber, va)); }
	assert(cuNaiveQueueGetSize(q) == 2);
	//remove not existent
	{ cuInitVarArgsOnStack(va, 10); assert(!cuNaiveQueueRemoveItemWithElements(q, isNumber, va)); }
	assert(cuNaiveQueueGetSize(q) == 2);

	cuNaiveQueueDestroyWithElements(q, NULL);
}

///test iteration
void test_06(CuTest* tc) {
	payload_functions p = cuPayloadFunctionsIntValue();
	int_naive_queue* q = cuNaiveQueueNew(p, CU_AS_EVALUATOR(evaluateInteger), NULL);

	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(10));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(7));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(8));
	cuNaiveQueueAddItem(q, CU_CAST_INT2PTR(5));

	int sum = 0;
	int evalSum = 0;
	CU_ITERATE_OVER_NAIVE_QUEUE(q, i, evaluation, v, intptr_t) {
		info("fetch %d-th eval=%d; value=%d;", i, evaluation, v);
		sum += v;
		evalSum += evaluation;
	}
	info("sum is %d; evaluation sum is %d", sum, evalSum);
	assert(sum == 30);
	assert(evalSum == 70);

	cuNaiveQueueDestroyWithElements(q, NULL);
}

CuSuite* CuNaiveQueueSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_01);
	SUITE_ADD_TEST(suite, test_02);
	SUITE_ADD_TEST(suite, test_03);
	SUITE_ADD_TEST(suite, test_04);
	SUITE_ADD_TEST(suite, test_05);
	SUITE_ADD_TEST(suite, test_06);

	return suite;
}
