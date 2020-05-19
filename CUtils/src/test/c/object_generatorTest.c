/**
 * @file
 *
 * @author koldar
 * @date Jun 27, 2018
 */


#include "CuTest.h"
#include <assert.h>
#include "object_generator.h"
#include "var_args.h"
#include "log.h"

int range(cu_generator* g) {
	int start = cuVarArgsGetItem(cuGeneratorGetVarArgs(g), 0, int);
	int step = cuVarArgsGetItem(cuGeneratorGetVarArgs(g), 1, int);
	int end = cuVarArgsGetItem(cuGeneratorGetVarArgs(g), 2, int);
	CU_GENERATOR(g, int, i)
		for (i=start; i<end; i+=step) {
			info("returning i=%d", i);

			CU_YIELD(g, i, int, i);
			info("loaded i=%d", i);
		}
	CU_GENERATOR_END(g);
}

void test_objectGenerator_01(CuTest* tc) {
	cuInitVarArgsOnStack(va, 10, 20, 100);
	cu_generator* gen = cuGeneratorNew(CU_AS_GENERATOR(range), va);

	assert(range(gen) == 10);
	assert(!cuGeneratorIsFinished(gen));
	assert(range(gen) == 30);
	assert(!cuGeneratorIsFinished(gen));
	assert(range(gen) == 50);
	assert(!cuGeneratorIsFinished(gen));
	assert(range(gen) == 70);
	assert(!cuGeneratorIsFinished(gen));
	assert(range(gen) == 90);
	assert(!cuGeneratorIsFinished(gen));

	cuGeneratorDestroy(gen, NULL);
}

void test_objectGenerator_02(CuTest* tc) {
	cuInitVarArgsOnStack(va, 10, 20, 100);
	cu_generator* gen = cuGeneratorNew(CU_AS_GENERATOR(range), va);

	assert(range(gen) == 10);
	assert(!cuGeneratorIsFinished(gen));
	assert(range(gen) == 30);
	assert(!cuGeneratorIsFinished(gen));
	assert(range(gen) == 50);
	assert(!cuGeneratorIsFinished(gen));
	assert(range(gen) == 70);
	assert(!cuGeneratorIsFinished(gen));
	assert(range(gen) == 90);
	assert(!cuGeneratorIsFinished(gen));
	range(gen);
	assert(cuGeneratorIsFinished(gen));

	cuGeneratorDestroy(gen, NULL);
}

CuSuite* CuObjectGeneratorSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_objectGenerator_01);
	SUITE_ADD_TEST(suite, test_objectGenerator_02);

	return suite;
}
