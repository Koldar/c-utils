/**
 * @file
 *
 * @author koldar
 * @date Sep 11, 2018
 */

#include "path_utils.h"
#include <stdarg.h>
#include "string_utils.h"
#include "errors.h"
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

bool _cuPathGenerate(CU_NOTNULL char* pathBuffer, int pathSize, int argsSize, ...) {
	va_list list;
	va_start(list, argsSize);
	bool result = true;

	int charConsumed = 0;
	for (int i=0; i<argsSize; i++) {
		charConsumed += snprintf(
			&pathBuffer[charConsumed],
			pathSize - charConsumed,
			"%s",
			va_arg(list, char*)
		);
		if (charConsumed > pathSize) {
			result = false;
			goto exit;
		}
		//we need to print the path separator only if this is not the last directory or the last character put is not a path separator
		if ((!CU_LASTLOOP(i, argsSize)) && (!cuIsStrEndsWith(pathBuffer, CU_PATH_DIR_SEPARATOR))) {
			charConsumed += snprintf(&pathBuffer[charConsumed], pathSize - charConsumed, "%s", CU_PATH_DIR_SEPARATOR);
		}
		if (charConsumed > pathSize) {
			result = false;
			goto exit;
		}
	}

	exit:;
	va_end(list);
	return result;
}

bool _cuPathConcatenate(CU_NOTNULL char* pathBuffer, int pathSize, int argsSize, ...) {
	va_list list;
	va_start(list, argsSize);
	bool result = true;

	int charConsumed = strlen(pathBuffer);
	for (int i=0; i<argsSize; i++) {
		charConsumed += snprintf(&pathBuffer[charConsumed], pathSize - charConsumed, "%s%s", va_arg(list, char*), CU_PATH_DIR_SEPARATOR);
		if (charConsumed > pathSize) {
			result = false;
			goto exit;
		}
	}

	exit:;
	va_end(list);
	return result;
}

char* cu_computeFileBaseName(const char* restrict path, char* restrict supportBuffer) {
	strcpy(supportBuffer, path);
	return basename(supportBuffer);
}

void createPathIfNotExist(const char* path, bool isFile) {
	char mkdirBuffer[BUFFER_SIZE];
	char* path2 = strdup(path);
	char* path3 = path2;
	struct stat statOutput;

	if (stat(path, &statOutput) == 0) {
		free(path2);
		return;
	}
	path3 = isFile ? dirname(path2) : path2;
	sprintf(mkdirBuffer, "mkdir -p \"%s\"", path3);
	int exitCode = system(mkdirBuffer);
	free(path2);
}
