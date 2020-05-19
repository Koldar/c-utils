/*
 * hashsetTest.c
 *
 *  Created on: Dec 21, 2017
 *      Author: koldar
 */

#include <assert.h>
#include "CuTest.h"
#include "payload_functions.h"
#include "hash_set.h"
#include "log.h"

void testHashSet01(CuTest* tc) {
	pint_hash_set* intSet = cuHashSetNew(cuPayloadFunctionsIntValue());
	cuHashSetDestroyWithElements(intSet, NULL);
}

void testHashSet02(CuTest* tc) {
	pint_hash_set* intSet = cuHashSetNew(cuPayloadFunctionsIntValue());

	assert(cuHashSetIsEmpty(intSet));
	cuHashSetAddItem(intSet, CU_CAST_INT2PTR(5));
	assert(!cuHashSetIsEmpty(intSet));
	assert(cuHashSetGetSize(intSet) == 1);

	cuHashSetDestroyWithElements(intSet, NULL);
}

void testHashSet03(CuTest* tc) {
	pint_hash_set* intSet = cuHashSetNew(cuPayloadFunctionsIntValue());

	assert(cuHashSetIsEmpty(intSet));
	cuHashSetAddItem(intSet, CU_CAST_INT2PTR(5));
	cuHashSetAddItem(intSet, CU_CAST_INT2PTR(5));
	assert(!cuHashSetIsEmpty(intSet));
	assert(cuHashSetGetSize(intSet) == 1);

	cuHashSetDestroyWithElements(intSet, NULL);
}

void testHashSet04(CuTest* tc) {
	pint_hash_set* intSet = cuHashSetNew(cuPayloadFunctionsIntValue());

	assert(cuHashSetIsEmpty(intSet));
	cuHashSetAddItem(intSet, CU_CAST_INT2PTR(5));
	cuHashSetAddItem(intSet, CU_CAST_INT2PTR(3));
	assert(!cuHashSetIsEmpty(intSet));
	assert(cuHashSetGetSize(intSet) == 2);

	cuHashSetDestroyWithElements(intSet, NULL);
}

void testHashSet05(CuTest* tc) {
	pint_hash_set* intSet = cuHashSetNew(cuPayloadFunctionsIntValue());

	assert(cuHashSetIsEmpty(intSet));

	cuHashSetAddItem(intSet, CU_CAST_INT2PTR(5));
	cuHashSetAddItem(intSet, CU_CAST_INT2PTR(3));
	cuHashSetRemoveItem(intSet, CU_CAST_INT2PTR(3));

	assert(!cuHashSetIsEmpty(intSet));
	assert(cuHashSetGetSize(intSet) == 1);
	assert(cuHashSetContainsItem(intSet, CU_CAST_INT2PTR(5)));
	assert(!cuHashSetContainsItem(intSet, CU_CAST_INT2PTR(3)));

	cuHashSetDestroyWithElements(intSet, NULL);
}

void testHashSet06(CuTest* tc) {
	pint_hash_set* intSet = cuHashSetNew(cuPayloadFunctionsIntValue());
	int sum;

	assert(cuHashSetIsEmpty(intSet));

	cuHashSetAddItem(intSet, CU_CAST_INT2PTR(5));
	cuHashSetAddItem(intSet, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(intSet, CU_CAST_INT2PTR(4));

	assert(!cuHashSetIsEmpty(intSet));
	assert(cuHashSetGetSize(intSet) == 3);

	assert(cuHashSetContainsItem(intSet, CU_CAST_INT2PTR(3)));
	assert(cuHashSetContainsItem(intSet, CU_CAST_INT2PTR(4)));
	assert(cuHashSetContainsItem(intSet, CU_CAST_INT2PTR(5)));

	sum = 0;
	CU_ITERATE_OVER_HASHSET(intSet, var, intptr_t) {
		sum += var;
	}
	assert(sum == 12);

	cuHashSetDestroyWithElements(intSet, NULL);
}

void testHashSet07(CuTest* tc) {
	pint_hash_set* a = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* b = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* the_union;

	cuHashSetAddItem(a, CU_CAST_INT2PTR(1));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(3));

	cuHashSetAddItem(b, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(4));

	the_union = cuHashSetGetUnionOfHashSets(a, b, false);

	assert(the_union != a);
	assert(the_union != b);
	assert(cuHashSetGetSize(a) == 3);
	assert(cuHashSetGetSize(b) == 3);

	assert(cuHashSetGetSize(the_union) == 4);
	assert(cuHashSetContainsItem(the_union, CU_CAST_INT2PTR(1)));
	assert(cuHashSetContainsItem(the_union, CU_CAST_INT2PTR(2)));
	assert(cuHashSetContainsItem(the_union, CU_CAST_INT2PTR(3)));
	assert(cuHashSetContainsItem(the_union, CU_CAST_INT2PTR(4)));

	cuHashSetDestroyWithElements(a, NULL);
	cuHashSetDestroyWithElements(b, NULL);
	cuHashSetDestroyWithElements(the_union, NULL);
}

void testHashSet08(CuTest* tc) {
	pint_hash_set* a = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* b = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* the_intersection;

	cuHashSetAddItem(a, CU_CAST_INT2PTR(1));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(3));

	cuHashSetAddItem(b, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(4));

	bcritical("set1 is: %s", (cuHashSetGetBufferString(a, log_buffer), log_buffer));
	bcritical("set2 is: %s", (cuHashSetGetBufferString(b, log_buffer), log_buffer));
	the_intersection = cuHashSetGetIntersectionOfHashSets(a, b, false);

	assert(the_intersection != a);
	assert(the_intersection != b);
	assert(cuHashSetGetSize(a) == 3);
	assert(cuHashSetGetSize(b) == 3);

	assert(cuHashSetGetSize(the_intersection) == 2);
	assert(cuHashSetContainsItem(the_intersection, CU_CAST_INT2PTR(2)));
	assert(cuHashSetContainsItem(the_intersection, CU_CAST_INT2PTR(3)));

	cuHashSetDestroyWithElements(a, NULL);
	cuHashSetDestroyWithElements(b, NULL);
	cuHashSetDestroyWithElements(the_intersection, NULL);
}

void testHashSet09(CuTest* tc) {
	pint_hash_set* a = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* b = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* the_difference;

	cuHashSetAddItem(a, CU_CAST_INT2PTR(1));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(3));

	cuHashSetAddItem(b, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(4));

	the_difference = cuHashSetGetDifferenceOfHashSets(a, b, false);

	assert(the_difference != a);
	assert(the_difference != b);
	assert(cuHashSetGetSize(a) == 3);
	assert(cuHashSetGetSize(b) == 3);

	assert(cuHashSetGetSize(the_difference) == 1);
	assert(cuHashSetContainsItem(the_difference, CU_CAST_INT2PTR(1)));

	cuHashSetDestroyWithElements(a, NULL);
	cuHashSetDestroyWithElements(b, NULL);
	cuHashSetDestroyWithElements(the_difference, NULL);
}

void testHashSet10(CuTest* tc) {
	pint_hash_set* a = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* b = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* the_union;

	cuHashSetAddItem(a, CU_CAST_INT2PTR(1));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(3));

	cuHashSetAddItem(b, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(4));

	the_union = cuHashSetGetUnionOfHashSets(a, b, true);

	assert(the_union == a);
	assert(the_union != b);
	assert(cuHashSetGetSize(a) == 4);
	assert(cuHashSetGetSize(b) == 3);

	assert(cuHashSetGetSize(the_union) == 4);
	assert(cuHashSetContainsItem(the_union, CU_CAST_INT2PTR(1)));
	assert(cuHashSetContainsItem(the_union, CU_CAST_INT2PTR(2)));
	assert(cuHashSetContainsItem(the_union, CU_CAST_INT2PTR(3)));
	assert(cuHashSetContainsItem(the_union, CU_CAST_INT2PTR(4)));

	cuHashSetDestroyWithElements(a, NULL);
	cuHashSetDestroyWithElements(b, NULL);
}

void testHashSet11(CuTest* tc) {
	pint_hash_set* a = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* b = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* the_intersection;

	cuHashSetAddItem(a, CU_CAST_INT2PTR(1));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(3));

	cuHashSetAddItem(b, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(4));

	the_intersection = cuHashSetGetIntersectionOfHashSets(a, b, true);

	assert(the_intersection == a);
	assert(the_intersection != b);
	assert(cuHashSetGetSize(a) == 2);
	assert(cuHashSetGetSize(b) == 3);

	assert(cuHashSetGetSize(the_intersection) == 2);
	assert(cuHashSetContainsItem(the_intersection, CU_CAST_INT2PTR(2)));
	assert(cuHashSetContainsItem(the_intersection, CU_CAST_INT2PTR(3)));

	cuHashSetDestroyWithElements(a, NULL);
	cuHashSetDestroyWithElements(b, NULL);
}

void testHashSet12(CuTest* tc) {
	pint_hash_set* a = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* b = cuHashSetNew(cuPayloadFunctionsIntValue());
	pint_hash_set* the_difference;

	cuHashSetAddItem(a, CU_CAST_INT2PTR(1));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(3));

	cuHashSetAddItem(b, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(b, CU_CAST_INT2PTR(4));

	the_difference = cuHashSetGetDifferenceOfHashSets(a, b, true);

	assert(the_difference == a);
	assert(the_difference != b);
	assert(cuHashSetGetSize(a) == 1);
	assert(cuHashSetGetSize(b) == 3);

	assert(cuHashSetGetSize(the_difference) == 1);
	assert(cuHashSetContainsItem(the_difference, CU_CAST_INT2PTR(1)));

	cuHashSetDestroyWithElements(a, NULL);
	cuHashSetDestroyWithElements(b, NULL);
}

void test_cuHashSetGetAnItem_01(CuTest* tc) {
	pint_hash_set* a = cuHashSetNew(cuPayloadFunctionsIntValue());

	cuHashSetAddItem(a, CU_CAST_INT2PTR(1));

	assert(CU_CAST_PTR2INT(cuHashSetGetAnItem(a)) == 1);

	cuHashSetDestroyWithElements(a, NULL);
}

void test_cuHashSetGetAnItem_02(CuTest* tc) {
	pint_hash_set* a = cuHashSetNew(cuPayloadFunctionsIntValue());

	assert(cuHashSetGetAnItem(a) == NULL);

	cuHashSetDestroyWithElements(a, NULL);
}

void test_cuHashSetGetAnItem_03(CuTest* tc) {
	pint_hash_set* a = cuHashSetNew(cuPayloadFunctionsIntValue());

	cuHashSetAddItem(a, CU_CAST_INT2PTR(1));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(a, CU_CAST_INT2PTR(3));

	int val = CU_CAST_PTR2INT(cuHashSetGetAnItem(a));
	assert(val == 1 || val == 2 || val ==3);

	cuHashSetDestroyWithElements(a, NULL);
}

CuSuite* CuHashSetSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testHashSet01);
	SUITE_ADD_TEST(suite, testHashSet02);
	SUITE_ADD_TEST(suite, testHashSet03);
	SUITE_ADD_TEST(suite, testHashSet04);
	SUITE_ADD_TEST(suite, testHashSet05);
	SUITE_ADD_TEST(suite, testHashSet06);
	SUITE_ADD_TEST(suite, testHashSet07);
	SUITE_ADD_TEST(suite, testHashSet08);
	SUITE_ADD_TEST(suite, testHashSet09);
	SUITE_ADD_TEST(suite, testHashSet10);
	SUITE_ADD_TEST(suite, testHashSet11);
	SUITE_ADD_TEST(suite, testHashSet12);

	SUITE_ADD_TEST(suite, test_cuHashSetGetAnItem_01);
	SUITE_ADD_TEST(suite, test_cuHashSetGetAnItem_02);
	SUITE_ADD_TEST(suite, test_cuHashSetGetAnItem_03);



	return suite;
}
