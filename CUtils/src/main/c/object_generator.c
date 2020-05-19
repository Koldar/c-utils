/**
 * @file
 *
 * @author koldar
 * @date Jun 27, 2018
 */


#include "object_generator.h"
#include "errors.h"

enum coroutine_status {
	CS_INITIALIZED,
	CS_STARTED,
	CS_FINISHED
};

struct cu_generator {
	enum coroutine_status state;
	int label;
	union generic_t locals[_CU_LOCAL_SIZE];
	const var_args* init;
	generator_t generatorFunction;
};

cu_generator* cuGeneratorNew(generator_t gen, CU_NULLABLE const var_args* va) {
	cu_generator* result = CU_MALLOC(cu_generator);

	result->state = CS_INITIALIZED;
	//iterator on values
	result->init = va;
	result->label = 0;
	result->generatorFunction = gen;

	return result;
}

void cuGeneratorDestroy(CU_NOTNULL const cu_generator* generator, CU_NULLABLE const struct var_args* context) {
	CU_FREE(generator);
}


void _cuGeneratorStoreLocal(CU_NOTNULL cu_generator* generator, int index, union generic_t val) {
	generator->locals[index] = val;
}

void* _cuGeneratorLoadLocal(CU_NOTNULL const cu_generator* generator, int index) {
	return (void*)(&generator->locals[index]);
}

bool cuGeneratorIsFinished(CU_NOTNULL const cu_generator* generator) {
	return generator->state == CS_FINISHED;
}

bool cuGeneratorIsStarted(CU_NOTNULL const cu_generator* generator) {
	return generator->state == CS_STARTED;
}

bool cuGeneratorIsInitialized(CU_NOTNULL const cu_generator* generator) {
	return generator->state == CS_INITIALIZED;
}

void _cuGeneratorMarkAsFinished(CU_NOTNULL cu_generator* generator) {
	generator->state = CS_FINISHED;
}

void _cuGeneratorMarkAsStarted(CU_NOTNULL cu_generator* generator) {
	generator->state = CS_STARTED;
}

int _cuGeneratorGetLabel(CU_NOTNULL const cu_generator* generator) {
	return generator->label;
}

void _cuGeneratorSetLabel(CU_NOTNULL cu_generator* generator, int label) {
	generator->label = label;
}

CU_NULLABLE const var_args* cuGeneratorGetVarArgs(CU_NOTNULL const cu_generator* generator) {
	return generator->init;
}
