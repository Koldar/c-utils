/*
 * queueTest.c
 *
 *  Created on: Aug 4, 2017
 *      Author: koldar
 */



#include "CuTest.h"
#include <assert.h>
#include "priority_queue.h"
#include "defaultFunctions.h"
#include <stdint.h>
#include "string_utils.h"


void testQueue01(CuTest* tc) {

	priority_queue* q = cuPriorityQueueNew(cuPayloadFunctionsIntValue());

	cuPriorityQueueDestroy(q, NULL);
}

void testQueue02(CuTest* tc) {
	priority_queue* q = cuPriorityQueueNew(cuPayloadFunctionsIntValue());

	assert(cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 0);
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(4), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(4), NULL));
	assert(!cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 1);

	cuPriorityQueueDestroy(q, NULL);
}

///popping and peeking a single element queue
void testQueue03(CuTest* tc) {
	priority_queue* q = cuPriorityQueueNew(cuPayloadFunctionsIntValue());

	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(4), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(4), NULL));

	assert(cuPriorityQueuePeekItem(q, intptr_t) == 4);
	assert(!cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 1);
	assert(cuPriorityQueuePopItem(q, intptr_t) == 4);

	assert(cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 0);
	assert(cuPriorityQueuePeekItem(q, void*) == NULL);

	cuPriorityQueueDestroy(q, NULL);
}

///Adding several values in a queue
void testQueue04(CuTest* tc) {
	priority_queue* q = cuPriorityQueueNew(cuPayloadFunctionsIntValue());

	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(4), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(4), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(3), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(3), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(6), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(6), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(10), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(10), NULL));

	assert(cuPriorityQueuePeekItem(q, intptr_t) == 3);
	assert(!cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 4);

	cuPriorityQueueDestroy(q, NULL);
}

///popping and peeking queue with more values
void testQueue05(CuTest* tc) {
	priority_queue* q = cuPriorityQueueNew(cuPayloadFunctionsIntValue());

	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(4), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(4), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(3), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(3), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(6), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(6), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(10), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(10), NULL));

	cuPriorityQueueSavePNG(q, __func__);

	assert(cuPriorityQueuePeekItem(q, intptr_t) == 3);
	assert(!cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 4);
	assert(cuPriorityQueuePopItem(q, intptr_t) == 3);
	cuPriorityQueueSavePNG(q, "testQueue05_01");
	assert(cuPriorityQueuePeekItem(q, intptr_t) == 4);
	assert(!cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 3);

	cuPriorityQueueDestroy(q, NULL);
}

///popping and peeking queue with more values
void testQueue06(CuTest* tc) {
	priority_queue* q = cuPriorityQueueNew(cuPayloadFunctionsIntValue());

	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(4), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(4), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(3), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(3), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(10), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(10), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(6), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(6), NULL));

	assert(cuPriorityQueuePeekItem(q, intptr_t) == 3);
	assert(!cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 4);
	assert(cuPriorityQueuePopItem(q, intptr_t) == 3);
	assert(cuPriorityQueuePopItem(q, intptr_t) == 4);
	assert(!cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 2);

	//add elements that won't alter min
	assert(cuPriorityQueuePeekItem(q, intptr_t) == 6);
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(13), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(13), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(11), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(11), NULL));
	assert(!cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 4);

	assert(cuPriorityQueuePeekItem(q, intptr_t) == 6);
	assert(cuPriorityQueuePopItem(q, intptr_t) == 6);
	assert(cuPriorityQueuePeekItem(q, intptr_t) == 10);
	assert(!cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 3);

	//add element that will alter min
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(5), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(5), NULL));
	cuPriorityQueueAddItem(q, CU_CAST_INT2PTR(3), cuDefaultFunctionsEvaluatorObject(CU_CAST_INT2PTR(3), NULL));
	assert(cuPriorityQueuePeekItem(q, intptr_t) == 3);
	assert(!cuPriorityQueueIsEmpty(q));
	assert(cuPriorityQueueGetSize(q) == 5);

	cuPriorityQueueDestroy(q, NULL);
}

struct string_data {
	struct priority_queue_cell* inQueue;
	const char* str;
};

static struct string_data* stringDataNew(CU_NOTNULL const char* s) {
	struct string_data* result = CU_MALLOC(struct string_data);
	if (result == NULL) {
		ERROR_MALLOC();
	}
	result->inQueue = NULL;
	result->str = s;

	return result;
}

#define CU_FUNCTION_POINTER_destructor_void_stringDataDestroy__string_dataConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID
static void stringDataDestroy(CU_NOTNULL const struct string_data* sd, CU_NULLABLE const struct var_args* context) {
	CU_FREE(sd);
}

#define CU_FUNCTION_POINTER_queue_findItem_priority_queue_cell_stringDataGetQueueCell_priority_queueConstPtr_voidConstPtr CU_QUEUE_FINDITEM_ID
static struct priority_queue_cell* stringDataGetQueueCell(priority_queue* q, CU_NOTNULL struct string_data* sd) {
	return sd->inQueue;
}

#define CU_FUNCTION_POINTER_queue_addItem_void_stringDataAddItem_priority_queuePtr_voidPtr_priority_queue_cellPtr CU_QUEUE_ADDITEM_ID
static void stringDataAddItem(CU_NOTNULL priority_queue* q, CU_NOTNULL struct string_data* sd, CU_NOTNULL struct priority_queue_cell* qc) {
	sd->inQueue = qc;
}

static int stringDataEvaluate(CU_NOTNULL const struct string_data* sd, CU_NULLABLE const struct var_args* context) {
	return (int)sd->str[0];
}

static bool stringDataCompare(CU_NOTNULL const struct string_data* a, CU_NOTNULL const struct string_data* b) {
	if (a == b) {
		return true;
	}
	return isStrEqual(a->str, b->str);
}

void test_find_01(CuTest* tc) {

	CU_WITH(priority_queue* q = cuPriorityQueueNew(cuPayloadFunctionsDefault()))(cuPriorityQueueDestroy(q, NULL))
			CU_WITH(struct string_data* sd1 = stringDataNew("a"))(stringDataDestroy(sd1, NULL))
			CU_WITH(struct string_data* sd2 = stringDataNew("b"))(stringDataDestroy(sd2, NULL))
			CU_WITH(struct string_data* sd3 = stringDataNew("c"))(stringDataDestroy(sd3, NULL)) {
		cuPriorityQueueEnableFastContainOperation(q, CU_QUEUE_AS_FINDITEM(stringDataGetQueueCell), CU_QUEUE_AS_ADDITEM(stringDataAddItem));

		cuPriorityQueueAddItem(q, (void*)sd1, 40);
		cuPriorityQueueAddItem(q, (void*)sd2, 4);

		assert(cuPriorityQueueContainsItem1(q, sd1) == true);
		assert(cuPriorityQueueContainsItem1(q, sd2) == true);
		assert(cuPriorityQueueContainsItem1(q, sd3) == false);
	}
}

void test_cuQueueAddItem1_01(CuTest* tc) {
	CU_WITH(priority_queue* q = cuPriorityQueueNew(cuPayloadFunctionsDefault()))(cuPriorityQueueDestroy(q, NULL))
			CU_WITH(struct string_data* sd1 = stringDataNew("a"))(stringDataDestroy(sd1, NULL))
			CU_WITH(struct string_data* sd2 = stringDataNew("b"))(stringDataDestroy(sd2, NULL))
			CU_WITH(struct string_data* sd3 = stringDataNew("c"))(stringDataDestroy(sd3, NULL)) {
		cuPriorityQueueEnableFastAddOperation(q, CU_AS_EVALUATOR(stringDataEvaluate));

		cuPriorityQueueAddItem1(q, (void*)sd1, NULL);
		cuPriorityQueueAddItem1(q, (void*)sd2, NULL);

		int i = 0;
		while (!cuPriorityQueueIsEmpty(q)) {
			struct string_data* s = cuPriorityQueuePopItem(q, struct string_data*);
			switch(i) {
			case 0: {
				assert(isStrEqual(s->str,"a"));
				break;
			}
			case 1: {
				assert(isStrEqual(s->str,"b"));
				break;
			}
			}
			i += 1;
		}

	}
}

void test_cuQueueChangePriority_01(CuTest* tc) {
	CU_WITH(priority_queue* q = cuPriorityQueueNew(cuPayloadFunctionsDefault()))(cuPriorityQueueDestroy(q, NULL))
			CU_WITH(struct string_data* sd1 = stringDataNew("a"))(stringDataDestroy(sd1, NULL))
			CU_WITH(struct string_data* sd2 = stringDataNew("b"))(stringDataDestroy(sd2, NULL))
			CU_WITH(struct string_data* sd3 = stringDataNew("c"))(stringDataDestroy(sd3, NULL))
			CU_WITH(struct string_data* sd4 = stringDataNew("d"))(stringDataDestroy(sd4, NULL))
			CU_WITH(struct string_data* sd5 = stringDataNew("e"))(stringDataDestroy(sd5, NULL))
			CU_WITH(struct string_data* sd6 = stringDataNew("f"))(stringDataDestroy(sd6, NULL)) {
		cuPriorityQueueEnableFastContainOperation(q, CU_QUEUE_AS_FINDITEM(stringDataGetQueueCell), CU_QUEUE_AS_ADDITEM(stringDataAddItem));

		cuPriorityQueueAddItem(q, (void*)sd1, 50);
		cuPriorityQueueAddItem(q, (void*)sd2, 3);
		assert(isStrEqual(cuPriorityQueuePeekItem(q, struct string_data*)->str, "b"));

		//decrease priority (which change order)
		cuPriorityQueueChangePriority(q, sd1, 2);
		assert(isStrEqual(cuPriorityQueuePeekItem(q, struct string_data*)->str, "a"));

		//increase priority (which change order)
		cuPriorityQueueChangePriority(q, sd1, 7);
		assert(isStrEqual(cuPriorityQueuePeekItem(q, struct string_data*)->str, "b"));

		//decrease priority (whichout change order)
		cuPriorityQueueChangePriority(q, sd1, 6);
		assert(isStrEqual(cuPriorityQueuePeekItem(q, struct string_data*)->str, "b"));

		//increase priority (whichout change order)
		cuPriorityQueueChangePriority(q, sd1, 8);
		assert(isStrEqual(cuPriorityQueuePeekItem(q, struct string_data*)->str, "b"));

		//add more items (sd1=8 sd2=3)
		cuPriorityQueueAddItem(q, (void*)sd3, 10);
		cuPriorityQueueAddItem(q, (void*)sd4, 15);
		cuPriorityQueueAddItem(q, (void*)sd5, 20);
		cuPriorityQueueAddItem(q, (void*)sd6, 25);

		//increase priority (with change order)
		cuPriorityQueueChangePriority(q, sd1, 24);
		CU_WITH(priority_queue* clone = cuPriorityQueueClone(q, NULL))(cuPriorityQueueDestroy(clone, NULL)) {
			int i = 0;
			while (!cuPriorityQueueIsEmpty(clone)) {
				struct string_data* sd = cuPriorityQueuePopItem(clone, struct string_data*);
				switch(i) {
				case 0: assert(isStrEqual(sd->str, "b")); break;
				case 1: assert(isStrEqual(sd->str, "c")); break;
				case 2: assert(isStrEqual(sd->str, "d")); break;
				case 3: assert(isStrEqual(sd->str, "e")); break;
				case 4: assert(isStrEqual(sd->str, "a")); break;
				case 5: assert(isStrEqual(sd->str, "f")); break;
				}
				i += 1;
			}
		}

		//decrease priority (WITHOUT CHANGE ORDER)
		cuPriorityQueueChangePriority(q, sd1, 23);
		CU_WITH(priority_queue* clone = cuPriorityQueueClone(q, NULL))(cuPriorityQueueDestroy(clone, NULL)) {
			int i = 0;
			while (!cuPriorityQueueIsEmpty(clone)) {
				struct string_data* sd = cuPriorityQueuePopItem(clone, struct string_data*);
				switch(i) {
				case 0: assert(isStrEqual(sd->str, "b")); break;
				case 1: assert(isStrEqual(sd->str, "c")); break;
				case 2: assert(isStrEqual(sd->str, "d")); break;
				case 3: assert(isStrEqual(sd->str, "e")); break;
				case 4: assert(isStrEqual(sd->str, "a")); break;
				case 5: assert(isStrEqual(sd->str, "f")); break;
				}
				i += 1;
			}
		}

		//decrease priority (WITH CHANGE ORDER)
		cuPriorityQueueChangePriority(q, sd1, 2);
		CU_WITH(priority_queue* clone = cuPriorityQueueClone(q, NULL))(cuPriorityQueueDestroy(clone, NULL)) {
			int i = 0;
			while (!cuPriorityQueueIsEmpty(clone)) {
				struct string_data* sd = cuPriorityQueuePopItem(clone, struct string_data*);
				switch(i) {
				case 0: assert(isStrEqual(sd->str, "a")); break;
				case 1: assert(isStrEqual(sd->str, "b")); break;
				case 2: assert(isStrEqual(sd->str, "c")); break;
				case 3: assert(isStrEqual(sd->str, "d")); break;
				case 4: assert(isStrEqual(sd->str, "e")); break;
				case 5: assert(isStrEqual(sd->str, "f")); break;
				}
				i += 1;
			}
		}
	}
}


CuSuite* CuQueueSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testQueue01);
	SUITE_ADD_TEST(suite, testQueue02);
	SUITE_ADD_TEST(suite, testQueue03);
	SUITE_ADD_TEST(suite, testQueue04);
	SUITE_ADD_TEST(suite, testQueue05);
	SUITE_ADD_TEST(suite, testQueue06);

	SUITE_ADD_TEST(suite, test_find_01);
	SUITE_ADD_TEST(suite, test_cuQueueAddItem1_01);
	SUITE_ADD_TEST(suite, test_cuQueueChangePriority_01);



	return suite;
}
