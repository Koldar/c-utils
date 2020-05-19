/*
 * defaultFunctions.c
 *
 *  Created on: May 18, 2017
 *      Author: koldar
 */

#ifndef _GNU_SOURCE
#	define _GNU_SOURCE
#endif

#include "defaultFunctions.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "macros.h"
#include "log.h"
#include "errors.h"

// *************************** COMPARATORS *********************************

bool cuDefaultFunctionsComparatorObject(const void* a, const void* b) {
	return a == b;
}

bool cuDefaultFunctionsComparatorIntValue(const int _a, const int _b) {
	return _a == _b;
}

bool cuDefaultFunctionsComparatorIntPtr(CU_NOTNULL const int* a, CU_NOTNULL const int* b) {
	return (*a) == (*b);
}

bool cuDefaultFunctionsComparatorNullObject(const void* a, const void* b) {
	ERROR_OBJECT_NOT_FOUND("Comparator function", "%p", NULL);
	UNREACHABLE_RETURN(true);
}

bool cuDefaultFunctionsComparatorString(const char* a, const char* b) {
	return strcmp(a, b) == 0;
}

// **************************** DESTRUCTORS ****************************

void cuDefaultFunctionsDestructorObject(CU_NULLABLE const void* payload, CU_NULLABLE const struct var_args* context) {
	if (payload != NULL) {
		free((void*)payload);
	}
}

void cuDefaultFunctionsDestructorNullObject(CU_NULLABLE const void* obj, CU_NULLABLE const struct var_args* context) {
	ERROR_OBJECT_NOT_FOUND("Destructor function", "%p", NULL);
}

void cuDefaultFunctionDestructorNOP(CU_NULLABLE const void* payload, CU_NULLABLE const struct var_args* context) {
	return;
}

void cuDefaultFunctionsDesturctorString(CU_NULLABLE const char* str, CU_NULLABLE const struct var_args* context) {
	if (str != NULL) {
		CU_FREE(str);
	}
}

// ****************************** ORDERER *********************************

int cuDefaultFunctionsOrdererIntPtr(const int* a, const int* b) {
	return *a - *b;
}

int cuDefaultFunctionsOrdererIntValue(const int a, const int b) {
	//be careful of the size! pointer algebra deals with the size of the pointed type.
	//if sizeof(int)=2 0x7 - 0x1 will compute 0x05!
	return (a) - (b);
}

int cuDefaultFunctionsOrdererObject(const void* a, const void* b) {
	return (int)(a - b);
}

int cuDefaultFunctionsOrdererNullObject(const void* a, const void* b) {
	ERROR_OBJECT_NOT_FOUND("Orderer function", "%p", NULL);
	UNREACHABLE_RETURN(0);
}

int cuDefaultFunctionsOrdererString(CU_NOTNULL const char* a, CU_NOTNULL const char* b) {
	return strcmp(a, b);
}

// *********************** CLONER ******************************

void* cuDefaultFunctionsClonerObject(const void* payload) {
	if (payload == NULL) {
		return NULL;
	}
	return (void*)payload;
}

int* cuDefaultFunctionClonerIntPtr(CU_NOTNULL const int* i) {
	int* clone = CU_MALLOC(int);
	if (clone == NULL) {
		ERROR_MALLOC();
	}
	*clone = *i;

	return clone;
}

void* cuDefaultFunctionsClonerNullObject(const void* payload) {
	ERROR_OBJECT_NOT_FOUND("Cloner function", "%p", NULL);
	UNREACHABLE_RETURN(NULL);
}

char* cuDefaultFunctionsClonerString(CU_NOTNULL const char* str) {
	return strdup(str);
}

// ******************* COLORER ******************************

int cuDefaultFunctionsColorerObject(void* payload) {
	return 0x00000000;
}

// ******************** BUFFER STRING ***************************

int cuDefaultFunctionsBufferStringerObject(const void* payload, char* buffer) {
	int i = 0;
	i = sprintf(buffer, "%p", payload);
	return i;
}

int cuDefaultFunctionsBufferStringerIntValue(const int j, char* buffer) {
	int i = 0;
	i += sprintf(&buffer[i], "%d", j);
	return i;
}

int cuDefaultFunctionsBufferStringerIntPtr(CU_NOTNULL const int* j, CU_NOTNULL char* buffer) {
	int i = 0;
	i += sprintf(&buffer[i], "%d", *j);
	return i;
}

int cuDefaultFunctionsBufferStringerNullObject(CU_NULLABLE const void* payload, CU_NOTNULL char* buffer) {
	ERROR_OBJECT_NOT_FOUND("Buffer String function", "%p", NULL);
	UNREACHABLE_RETURN(0);
}

int cuDefaultFunctionsBufferStringerString(CU_NULLABLE const char* str, CU_NOTNULL char* buffer) {
	int i = 0;

	i += sprintf(&buffer[i], "%s", str);

	return i;
}


int cuDefaultFunctionsBufferStringerBoolValue(const bool j, CU_NOTNULL char* buffer) {
	int i = 0;
	i += sprintf(&buffer[i], "%s", j ? "true" : "false");
	return i;
}

// ********************** SERIALIZER ****************************

int cuDefaultFunctionsSerializerObject(FILE* f, const void* payload) {
	fwrite(&payload, sizeof(payload), 1, f);
	return sizeof(payload);
}

int cuDefaultFunctionsSerializerNullObject(CU_NOTNULL FILE* f, CU_NULLABLE const void* payload) {
	ERROR_OBJECT_NOT_FOUND("Serializer function", "%p", NULL);
	UNREACHABLE_RETURN(0);
}

int cuDefaultFunctionsSerializerIntValue(FILE* f, const int n) {
	fwrite(&n, sizeof(n), 1, f);
	return sizeof(n);
}

int cuDefaultFunctionsSerializerIntPtr(FILE* f, CU_NOTNULL const int* n) {
	return cuDefaultFunctionsSerializerIntValue(f, *n);
}

int cuDefaultFunctionsSerializerString(FILE* f, CU_NOTNULL const char* str) {
	int bytes = 0;
	//store the length of the string (this return the number of bytes of the string without the null terminator character)
	int strLength = strlen(str);
	bytes += sizeof(int) * fwrite(&strLength, sizeof(int), 1, f);
	//store the actual string (without the null terminator)
	bytes += sizeof(char) * fwrite(str, sizeof(char), strLength, f);
	return bytes;
}

// ********************** DESERIALIZER ******************************

int cuDefaultFunctionsDeserializerObject(FILE* f, void* _payload) {
	void** payload = (void**) _payload;
	void* p;
	int objectsRead = fread(&p, sizeof(p), 1, f);
	*payload = p;
	return sizeof(p);
}

int cuDefaultFunctionsDeserializerNullObject(CU_NOTNULL FILE* f, CU_NULLABLE const void* payload) {
	ERROR_OBJECT_NOT_FOUND("Deserializer function", "%p", NULL);
	UNREACHABLE_RETURN(0);
}

int cuDefaultFunctionsDeserializerIntValue(FILE* f, int* n) {
	//make all the bits in the int initialized
	*n = 0;
	int intRead = fread(n, sizeof(int), 1, f);
	return intRead;
}

int cuDefaultFunctionsDeserializerIntPtr(FILE* f, int** n) {
	*n = CU_MALLOC(int);
	if (*n == NULL) {
		ERROR_MALLOC();
	}

	return cuDefaultFunctionsDeserializerIntValue(f, *n);
}

int cuDefaultFunctionsDeserializerString(CU_NOTNULL FILE* f, CU_NOTNULL char** pstr) {
	//read the number of character the string has
	int bytes = 0;

	//read the length of the string
	int strLength;
	bytes += sizeof(int) * fread(&strLength, sizeof(int), 1, f);
	//malloc the string
	*pstr = (char*) malloc(sizeof(char) * (strLength + 1));
	if ((*pstr) == NULL) {
		ERROR_MALLOC();
	}
	//read the actual string
	bytes += sizeof(char) * fread(*pstr, sizeof(char), strLength, f);
	//set the null terminator
	(*pstr)[strLength] = '\0';

	return bytes;
}

// ********************** EVALUATOR ***************************

int cuDefaultFunctionsEvaluatorObject(const void* p, const struct var_args* va) {
	return CU_CAST_PTR2INT(p);
}

// ******************************* HASH FUNCTIONS **********************************

unsigned long hashString(const char* str) {
		//djb2 hash function
		unsigned long hash = 5381;
		int c;
		while (c = *str++)
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

		return hash;
}


