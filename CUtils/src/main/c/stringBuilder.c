/*
 * stringBuilder.c
 *
 *  Created on: Feb 27, 2017
 *      Author: koldar
 */


#include "stringBuilder.h"
#include "utility.h"
#include "log.h"
#include "errors.h"
#include "errors.h"

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct string_builder {
	/**
	 * A pointer to the memory area where we're building the string
	 */
	CU_NULLABLE char* output;
	/**
	 * Length of the building string
	 *
	 * Does **not** count the '\0' string terminator
	 */
	int length;
	/**
	 * size of the memory area pointed by ::string_builder::output
	 */
	int bufferSize;
	/**
	 * number of bytes we increase the area ::string_builder::output when it is filled
	 *
	 * Always positive not 0
	 */
	int resizeFactor;
};

static void resizeOutput(CU_NOTNULL string_builder* sb);



string_builder* cuStringBulilderNew(CU_NULLABLE const char* startString, int resizeFactor) {
	string_builder* sb = malloc(sizeof(string_builder));
	if (sb == NULL) {
		ERROR_MALLOC();
	}

	sb->output = NULL;
	sb->length = 0;
	sb->bufferSize = 0;
	sb->resizeFactor = 100;

	if (startString != NULL) {
		cuStringBuilderAppendString(sb, startString);
	}

	return sb;
}

CU_DEFINE_DEFAULT_VALUES(cuStringBulilderNew,
        NULL,
        100
);

void cuStringBuilderAppendString(CU_NOTNULL string_builder* sb, CU_NOTNULL const char* str) {
	int strLength = strlen(str);
	while (sb->bufferSize < (sb->length + strLength + 1)) {
		resizeOutput(sb);
	}

	strcpy(sb->output + sb->length, str);
	sb->length += strLength;
}

void cuStringBuilderAppendTemplate(CU_NOTNULL string_builder* sb, CU_NOTNULL const char* template, ...) {
	char buffer[BUFFER_SIZE];

	va_list vaList;
	va_start(vaList, template);
	int bytes = vsnprintf(buffer, BUFFER_SIZE, template, vaList);
	va_end(vaList);

	cuStringBuilderAppendString(sb, buffer);
}

void appendStringToStringBuilder(CU_NOTNULL string_builder* sb, CU_NOTNULL const char* string) {
	cuStringBuilderAppendString(sb, string);
}

void cuStringBuilderAppendChar(CU_NOTNULL string_builder* sb, char ch) {
	cuStringBuilderAppendTemplate(sb, "%c", ch);
}

void cuStringBuilderAppendInt(CU_NOTNULL string_builder* sb, int i) {
	cuStringBuilderAppendTemplate(sb, "%d", i);
}

void cuStringBuilderAppendFloat(CU_NOTNULL string_builder* sb, float f) {
	cuStringBuilderAppendTemplate(sb, "%f", f);
}

void cuStringBuilderAppendDouble(CU_NOTNULL string_builder* sb, double d) {
	cuStringBuilderAppendTemplate(sb, "%f", d);
}

void cuStringBuilderAppendLong(CU_NOTNULL string_builder* sb, long l) {
	cuStringBuilderAppendTemplate(sb, "%ld", l);
}

char* cuStringBuilderGetString(CU_NOTNULL const string_builder* sb) {
	return sb->length > 0 ? sb->output : "";
}

void cuStringBuilderClear(CU_NOTNULL string_builder* sb) {
	if (sb->output != NULL) {
		free(sb->output);
	}
	sb->output = NULL;
	sb->length = 0;
	sb->bufferSize = 0;
}

void cuStringBulilderDestroy(CU_NOTNULL const string_builder* sb, CU_NULLABLE const struct var_args* context) {
	if (sb->output != NULL) {
		CU_FREE(sb->output);
	}
	CU_FREE(sb);
}

/**
 * Enalrge the memory pointed by ::string_builder::output
 *
 * Such pointer may change
 *
 * @param[inout] sb the strinb builder whose burffer we need to enlarge
 */
static void resizeOutput(CU_NOTNULL string_builder* sb) {
	char* newStr = calloc((sb->bufferSize + sb->resizeFactor), sizeof(char));
	if (newStr == NULL) {
		ERROR_MALLOC();
	}

	debug("output is \"%p\"", sb->output);
	if (sb->output != NULL) {
		debug("copying\"%s\" into newStr", sb->output);
		strcpy(newStr, sb->output);
		free(sb->output);
	}
	sb->output = newStr;
	sb->bufferSize += sb->resizeFactor;
}

