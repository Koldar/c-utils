/**
 * @file
 *
 * Contains functions used to help you when managing files
 *
 * The modules provides some routine to easily read/write from/to files.
 *
 * Readingwise, this file utils is for reading file open with "r" flag. It is **not** suitable for reading binary files.
 * An example of reading a file with this module can be found here:
 *
 * @includedoc fileReading.dox
 *
 * @author koldar
 * @date May 28, 2018
 */

#ifndef FILE_UTILS_H_
#define FILE_UTILS_H_

#include <stdio.h>
#include <stdbool.h>
#include "macros.h"

/**
 * The output of the function ::cuFileUtilsReadLine
 */
enum file_outcome {
	/**
	 * a read error has occured
	 */
	FO_ERROR,
	/**
	 * the function has read a line (which is possibly empty)
	 */
	FO_READLINE,
	/**
	 * The function succeded but we reached the end of file (we read nothing)
	 */
	FO_EOF
};

/**
 * Join several paths into one
 *
 * @private
 *
 * @param[inout] buffer where to store the output of the function. Needs to be already initialized
 * @param[in] size length of array @c buffer
 * @param[in] pathsNumber number of elements in "..."
 * @param[in] ... set of paths to join
 * @return the number of characters filled in the buffer
 */
int _cuFileUtilsJoin(CU_NOTNULL char* buffer, int size, int pathsNumber, ...);

/**
 * An enhanced version of `fopen` function
 *
 * @param[in] mode how you need to open the file. Supported values are "w", "r", "a", "wb", "rb"
 * @param[in] format the format of the filename. Follos `printf` format
 * @param[in] ... additional values for @c format parameter
 * @return a FILE* instance. Close it with `fclose` as per usual
 */
FILE* cuFileUtilsOpen(const char* restrict mode, const char* restrict format, ...);

/**
 * An enhanced version of `fopen` function
 *
 * @param[in] mode how you need to open the file. Supported values are "w", "r", "a", "wb", "rb"
 * @param[in] format the format of the filename. Follos `printf` format
 * @param[in] va additional values for @c format parameter
 * @return a FILE* instance. Close it with `fclose` as per usual
 */
FILE* cuFileUtilsOpen1(CU_NOTNULL const char* restrict mode, CU_NOTNULL const char* restrict format, va_list va);

/**
 * Read a line from a file
 *
 * @code
 * while(cuFileUtilsReadLine(f, 100, buffer, &eofCheck) )
 * @endcode
 *
 * @pre
 *  @li f open with "r" flag;
 *
 * @param[inout] f the file to read the line from
 * @param[in] lineBufferSize length of the buffer @c lineBuffer
 * @param[out] lineBuffer the buffer where the lien read will be put
 * @param[in] truncateCarriageReturn if true, we will automatically truncate the carriage return from the line
 * @return
 * 	@li the outcome of the readline operation
 */
enum file_outcome cuFileUtilsReadLine(FILE* f, int lineBufferSize, char lineBuffer[], bool truncateCarriageReturn);
CU_DECLARE_FUNCTION_WITH_DEFAULTS(enum file_outcome, cuFileUtilsReadLine, FILE*, int, char*, bool);
#define cuFileUtilsReadLine(...) CU_CALL_FUNCTION_WITH_DEFAULTS(cuFileUtilsReadLine, 4, __VA_ARGS__)
CU_DECLARE_DEFAULT_VALUES(cuFileUtilsReadLine,
        ,
		,
		,
		true
);

/**
 * alias for @c fprintf
 *
 * @pre
 *  @li @c f open in write mode
 *
 * @param[inout] f the file to write on
 * @param[in] format a string <tt>printf-like</tt> containing formast specifiers
 * @param[in] ... values of format specifiers
 */
void cuFileUtilsWrite(CU_NOTNULL FILE* f, CU_NOTNULL const char* format, ...);

/**
 * Writes line inside an open file and adds a carriage return
 *
 * @pre
 *  @li @c f open in write mode
 *
 * @param[inout] f the file to write on
 * @param[in] format a string <tt>printf-like</tt> containing formast specifiers
 * @param[in] ... values of format specifiers
 */
void cuFileUtilsWriteln(CU_NOTNULL FILE* f, CU_NOTNULL const char* format, ...);

/**
 * Prints something inside the file prefixing it with \c tabbed number of tabs
 *
 * @param[in] fout the file to write on
 * @param[in] tabbed the number of tabs to write into the file
 * @param[in] format printf argument
 * @param[in] ... printf argument
 */
void cuFileUtilsTabbedWrite(FILE* fout, int tabbed, const char* format, ...);

/**
 * Prints something inside the file prefixing it with \c tabbed number of tabs
 *
 * @param[in] fout the file to write on
 * @param[in] tabbed the number of tabs to write into the file
 * @param[in] format printf argument
 * @param[in] ... printf argument
 */
void cuFileUtilsTabbedWriteln(FILE* fout, int tabbed, const char* format, ...);

/**
 * Writes a simple carriage return and nothing else
 *
 * @pre
 *  @li @c f open in write mode
 *
 * @param[inout] f the file to write one
 */
void cuFileUtilsWriteNewLine(CU_NOTNULL FILE* f);

/**
 * Removes a file from the file system
 *
 * @param[in] template a <tt>printf</tt> like format sting
 * @param[in] ... additional value to the @c template string
 * @return
 *  @li true if the file has been correctly removed;
 *  @li false otherwise;
 */
bool cuFileUtilsDeleteFile(CU_NOTNULL const char* template, ...);

//TODO deprecated use path_utils instead
/**
 * Join paths
 *
 * @code
 * char buffer[300];
 * cuFileUtils(buffer, 300, "foo", "bar/", "..");
 * //outputs foo/bar/..
 * @endcode
 *
 * @param[inout] buffer an already inizialized buffer where to store data
 * @param[in] size number of bytes in @c buffer
 * @param[in] ... subpaths to join into @c buffer
 * @return number of byts filled in @c buffer
 */
#define cuFileUtilsJoin(buffer, size, ...) _cuFileUtilsJoin((char*)(buffer), (int)(size), CU_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

/**
 * Iterate over the lines of a file
 *
 * @pre
 *  @li f open with "r" flag;
 *
 * For example:
 *
 * @includedoc fileReading.dox
 *
 * @param[in] fileVariableName an allocated <tt>FILE*</tt> variable name
 * @param[in] lineMaxSize an integer representing the size of the buffer @c line
 * @param[in] line a <tt>char*</tt> buffer that will contain the line just read at each iteration
 * @param[in] lineCountNoVarName name of a variable which will contain the lines number read up until now. At the first iteration it is equal to 0
 */
#define CU_ITERATE_ON_FILE_LINES(fileVariableName, lineMaxSize, line, lineCountNoVarName) \
		CU_WITH(char line[lineMaxSize])(CU_NOCODE) \
		for( \
			int lineCountNoVarName = 0 \
			; \
			cuFileUtilsReadLine(fileVariableName, lineMaxSize, line, true) == FO_READLINE \
			; \
			lineCountNoVarName++ \
		)

#endif /* FILE_UTILS_H_ */
