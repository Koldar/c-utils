/**
 * @file
 *
 * Represents an header which is an interface for `libbacktrace` library
 *
 * You can use it as follows
 *
 * @code
 * @endcode
 *
 * @author koldar
 * @date Sep 26, 2018
 */

#ifndef STACKTRACE_H_
#define STACKTRACE_H_

#define HAS_LIBBACKTRACE
#ifdef HAS_LIBBACKTRACE
#	include <backtrace.h>
#	include <backtrace-supported.h>

#	ifdef DEBUG


		struct cu_bt_ctx {
			struct backtrace_state *state;
			int error;
		};

		/**
		 *
		 */
		void _cuStackTraceErrorCallback(void *data, const char *msg, int errnum);

#		define CU_PRINT_STACKTRACE(f) { \
			struct backtrace_state* UV(state) = backtrace_create_state("program", BACKTRACE_SUPPORTS_THREADS, _cuStackTraceErrorCallback, NULL); \
			struct cu_bt_ctx UV(ctx) = {UV(state), 0}; \
			backtrace_print(UV(state), 0, f); \
		}

#	else
#		define CU_PRINT_STACKTRACE(f) \
			fprintf(f, "In order to show the stack trace, you need to be in debug mode (with the compiler enable -g and make sure -DDEBUG is present).")
#	endif


#else

#define CU_PRINT_STACKTRACE(f) \
	fprintf(f, \
			"Couldn't load libbacktrace, hence we couldn't generate the stack trace of the call." \
			"Install on your system libbacktrace trace to show the stacktrace. Make it available in the system directory path" \
			"\"libbacktrace/backtrace.h\".\n" \
);

#endif

#endif /* STACKTRACE_H_ */
