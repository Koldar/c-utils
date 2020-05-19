/*
 * tuple.c
 *
 *  Created on: Jan 22, 2018
 *      Author: koldar
 */

#include "tuple.h"
#include "macros.h"
#include <stdlib.h>
#include "errors.h"

static int cuPointerPairBufferStringer(CU_NOTNULL const struct pointer_pair_t* pair, char* buffer);
static struct pointer_pair_t* cuPointerPairCloner(CU_NOTNULL const struct pointer_pair_t* pair);
#define CU_FUNCTION_POINTER_cloner_voidPtr_cuPointerPairCloner_voidConstPtr CU_CLONER_ID
static bool cuPointerPairComparator(CU_NOTNULL const struct pointer_pair_t* pair1, CU_NOTNULL const struct pointer_pair_t* pair2);

struct pointer_pair_t* cuPointerPairNew(const void* first, const void* second, payload_functions firstFunctions, payload_functions secondFunctions) {
	struct pointer_pair_t* retVal = CU_MALLOC(struct pointer_pair_t);
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	*retVal = cuPointerPairSetup(first, second, firstFunctions, secondFunctions);

	return retVal;
}

struct pointer_pair_t cuPointerPairSetup(CU_NULLABLE const void* first, CU_NULLABLE const void* second, payload_functions firstFunctions, payload_functions secondFunctions) {
	struct pointer_pair_t result;

	result.first = (void*)first;
	result.second = (void*) second;
	result.firstFunctions = firstFunctions;
	result.secondFunctions = secondFunctions;

	return result;
}

void cuPointerPairDestroy(const struct pointer_pair_t* p, CU_NULLABLE const struct var_args* context) {
	CU_FREE(p);
}

void cuPointerPairDestroyWithElements(const struct pointer_pair_t* p, CU_NULLABLE const struct var_args* context) {
	p->firstFunctions.destroy(p->first, context);
	p->secondFunctions.destroy(p->second, context);
	cuPointerPairDestroy(p, context);
}

payload_functions cuPointerPairGetPayloadFunctions() {
	payload_functions result = cuPayloadFunctionsError();

	result.bufferString = CU_AS_BUFFERSTRINGER(cuPointerPairBufferStringer);
	result.clone = CU_AS_CLONER(cuPointerPairCloner);
	result.compare = CU_AS_COMPARER(cuPointerPairComparator);
	result.destroy = CU_AS_DESTRUCTOR(cuPointerPairDestroyWithElements);

	return result;
}

static int cuPointerPairBufferStringer(CU_NOTNULL const struct pointer_pair_t* pair, char* buffer) {
	int i=0;

	i += sprintf(&buffer[i], "(");
	pair->firstFunctions.bufferString(pair->first, &buffer[i]);
	i += sprintf(&buffer[i], ", ");
	pair->secondFunctions.bufferString(pair->second, &buffer[i]);
	i += sprintf(&buffer[i], ")");

	return i;
}

static struct pointer_pair_t* cuPointerPairCloner(CU_NOTNULL const struct pointer_pair_t* pair) {
	return cuPointerPairNew(pair->firstFunctions.clone(pair->first), pair->secondFunctions.clone(pair->second), pair->firstFunctions, pair->secondFunctions);
}

static bool cuPointerPairComparator(CU_NOTNULL const struct pointer_pair_t* pair1, CU_NOTNULL const struct pointer_pair_t* pair2) {
	CU_REQUIRE(pair1->firstFunctions.compare == pair2->firstFunctions.compare);
	CU_REQUIRE(pair1->secondFunctions.compare == pair2->secondFunctions.compare);

	if (!pair1->firstFunctions.compare(pair1->first, pair2->first)) {
		return false;
	}
	if (!pair1->secondFunctions.compare(pair1->second, pair2->second)) {
		return false;
	}

	return true;
}

