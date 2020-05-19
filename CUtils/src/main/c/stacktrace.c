/**
 * @file
 *
 * @author koldar
 * @date Sep 26, 2018
 */

#include "stacktrace.h"
#include "errors.h"
#include <stdio.h>

#ifdef HAS_LIBBACKTRACE
#ifdef DEBUG

void _cuStackTraceErrorCallback(void *data, const char *msg, int errnum) {
	struct cu_bt_ctx *ctx = data;
	fprintf(stderr, "ERROR: %s (%d)", msg, errnum);
	ctx->error = 1;
}

#endif
#endif
