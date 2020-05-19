/**
 * @file
 *
 * @author koldar
 * @date May 28, 2018
 */


#include "file_utils.h"
#include <stdarg.h>
#include <unistd.h>
#include "errors.h"

static int _cuFileUtilsJoin2(CU_NOTNULL char* buffer, int nextToFill, int size, const char* other);

FILE* cuFileUtilsOpen(const char* restrict mode, const char* restrict format, ...) {
	char buffer[BUFFER_SIZE];

	va_list va;
	va_start(va, format);
	vsnprintf(buffer, BUFFER_SIZE, format, va);
	va_end(va);

	return fopen(buffer, mode);
}

FILE* cuFileUtilsOpen1(CU_NOTNULL const char* restrict mode, CU_NOTNULL const char* restrict format, va_list va) {
	char buffer[BUFFER_SIZE];
	vsnprintf(buffer, BUFFER_SIZE, format, va);
	return fopen(buffer, mode);
}

int _cuFileUtilsJoin(CU_NOTNULL char* buffer, int size, int pathsNumber, ...) {
	va_list va;
	int nextToFill = 0;

	va_start(va, pathsNumber);
	//initialize the buffer with the first path
	nextToFill += snprintf(buffer, size, "%s", va_arg(va, const char*));
	for (int i=1; i<pathsNumber; i++) {
		//append the remainder paths to the buffer
		nextToFill += _cuFileUtilsJoin2(buffer, nextToFill, size, va_arg(va, const char*));
	}

	return nextToFill;
}


/**
 * Append a path into another one
 *
 * @param[inout] buffer the place where to add @c other
 * @param[in] nextToFill the index where you need to append @c other in @buffer. It also points the '\0' of @c buffer
 * @param[in] size number of byts @c buffer has
 * @param[in] other the path to append
 * @return the number of characters filled in the buffer
 */
static int _cuFileUtilsJoin2(CU_NOTNULL char* buffer, int nextToFill, int size, const char* other) {
	if (nextToFill == size) {
		//the buffer is filled. We do nothing
		return 0;
	}

	//TODO linux only
	char last_character = buffer[nextToFill-1];
	if (last_character != '/') {
		//we need to append / to the path
		snprintf(&buffer[nextToFill], size - nextToFill, "/");
		return _cuFileUtilsJoin2(buffer, nextToFill+1, size, other) + 1;
	} else {
		//we can add the string other.
		//if we have come here it means that buffer ends with "/". If other starts with "/" as well we need to truncate it
		return snprintf(&buffer[nextToFill], size - nextToFill, "%s", other[0] == '/' ? &other[1] : other);
	}
}

enum file_outcome cuFileUtilsReadLine(FILE* f, int lineBufferSize, char lineBuffer[], bool truncateCarriageReturn) {
	char* fgetsOutput = fgets(lineBuffer, lineBufferSize, f);

	if (fgetsOutput == NULL) {
		if (ferror(f) != 0) {
			return FO_ERROR;
		}
		return FO_EOF;
	}

	if (truncateCarriageReturn) {
		//remove the first carriage return found
		int i=0;
		while(lineBuffer[i] != '\n') {
			i++;
		}
		lineBuffer[i] = '\0';
	}

	return FO_READLINE;
}

CU_DEFINE_DEFAULT_VALUES(cuFileUtilsReadLine,
		,
		,
		,
		true
);


void cuFileUtilsWrite(CU_NOTNULL FILE* f, CU_NOTNULL const char* format, ...) {
	va_list va;
	va_start(va, format);
	vfprintf(f, format, va);
	va_end(va);
}

void cuFileUtilsWriteln(CU_NOTNULL FILE* f, CU_NOTNULL const char* format, ...) {
	va_list va;
	va_start(va, format);
	vfprintf(f, format, va);
	fprintf(f, "\n");
	va_end(va);
}

void cuFileUtilsTabbedWrite(FILE* fout, int tabbed, const char* format, ...) {
	va_list vaList;

	for (int i=0; i<tabbed; i++) {
		fprintf(fout, "\t");
	}
	va_start(vaList, format);
	vfprintf(fout, format, vaList);
	va_end(vaList);
}

void cuFileUtilsTabbedWriteln(FILE* fout, int tabbed, const char* format, ...) {
	va_list vaList;

	for (int i=0; i<tabbed; i++) {
		fprintf(fout, "\t");
	}
	va_start(vaList, format);
	vfprintf(fout, format, vaList);
	va_end(vaList);
	fprintf(fout, "\n");
}

void cuFileUtilsWriteNewLine(CU_NOTNULL FILE* f) {
	fprintf(f, "\n");
}

bool cuFileUtilsDeleteFile(CU_NOTNULL const char* template, ...) {
	va_list va;
	va_start(va, template);
	char buffer[BUFFER_SIZE];
	vsnprintf(buffer, BUFFER_SIZE, template, va);
	va_end(va);

	return unlink(buffer) == 0;
}

