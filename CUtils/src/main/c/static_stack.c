/*
 * stack.c
 *
 *  Created on: Mar 13, 2017
 *      Author: koldar
 */

#include "static_stack.h"
#include "utility.h"
#include <stdlib.h>
#include "errors.h"

struct static_stack {
	///the length of the array ::static_stack::elements
	int maxSize;
	///the number of elements inside ::static_stack::elements
	int size;
	///an array representing the stack itself
	void** elements;
};

static_stack* cuStaticStackNew(int maxSize) {
	static_stack* retVal = malloc(sizeof(static_stack));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->size = 0;
	retVal->maxSize = maxSize;
	retVal->elements = calloc(retVal->maxSize, sizeof(void*));
	if (retVal->elements == NULL) {
		ERROR_MALLOC();
	}

	return retVal;
}

void cuStaticStackDestroy(CU_NOTNULL const static_stack* stack, CU_NULLABLE const struct var_args* context) {
	CU_FREE(stack->elements);
	CU_FREE(stack);
}

int cuStaticStackSize(CU_NOTNULL const static_stack* stack) {
	return stack->size;
}

int cuStaticStackGetCapacity(CU_NOTNULL const static_stack* s) {
	return s->maxSize;
}

bool cuStaticStackIsEmpty(CU_NOTNULL const static_stack* stack) {
	return stack->size == 0;
}

bool cuStaticStackIsFull(CU_NOTNULL const static_stack* stack) {
	return stack->size == stack->maxSize;
}

void* cuStaticStackPop(CU_NOTNULL static_stack* stack) {
	if (cuStaticStackIsEmpty(stack)) {
		return NULL;
	}
	stack->size--;
	return stack->elements[stack->size];
}

void cuStaticStackPush(CU_NOTNULL static_stack* stack, CU_NULLABLE const void* data) {
	if (cuStaticStackIsFull(stack)) {
		ERROR_OBJECT_IS_FULL("stack", stack->size);
	}
	stack->elements[stack->size] = (void*)data;
	stack->size++;
}

void* cuStaticStackPeek(CU_NOTNULL const static_stack* stack) {
	if (cuStaticStackIsEmpty(stack)) {
		return NULL;
	}
	return stack->elements[stack->size - 1];
}

void cuStaticStackClear(CU_NOTNULL static_stack* stack) {
	stack->size = 0;
}

void* _cuStaticStackGetNthItem(CU_NOTNULL const static_stack* stack, int index) {
	return stack->elements[index];
}
