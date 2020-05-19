/**
 * @file
 *
 * @author koldar
 * @date Mar 13, 2018
 */

#include "var_args.h"
#include "macros.h"
#include "log.h"
#include <stdarg.h>
#include "errors.h"

var_args _cuInitVarArgsOnStack(int count, CU_NULLABLE const size_t* size_array, CU_NULLABLE const char* value_array) {
	struct var_args retVal;

	retVal.count = count;

	CU_SAFE_ASSIGN(retVal.sizes, size_array);
	CU_SAFE_ASSIGN(retVal.values, value_array);

	return retVal;
}

var_args* _cuNewVarArgsOnHeap(int count, CU_NULLABLE const size_t* size_array, CU_NULLABLE const char* value_array) {
	var_args* result = CU_MALLOC(var_args);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	result->count = count;
	result->sizes = (size_t*) size_array;
	result->values = (char*) value_array;

	return result;
}


void cuDestroyVarArgs(CU_NOTNULL const var_args* va, CU_NULLABLE const struct var_args* context) {
	CU_FREE(va->sizes);
	CU_FREE(va->values);
	CU_FREE(va);
}

void* _cuVarArgsGetItem(const var_args* va, int index) {
	int position = 0;
	for (int i=0; i<index; i++) {
		debug("reading block %d-th at pointer %p", i, va->sizes[i]);
		position += (va->sizes)[i];
	}
	return (void*)(((char*)va->values) + position);
}

int cuVarArgsGetSize(const var_args* va) {
	return va->count;
}
