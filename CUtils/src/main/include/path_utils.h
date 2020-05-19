/**
 * @file
 *
 * A module allowing you to abstractally handle paths.
 *
 * Paths are just strings
 *
 * @author koldar
 * @date Sep 11, 2018
 */

#ifndef PATH_UTILS_H_
#define PATH_UTILS_H_

#include <stdbool.h>
#include "macros.h"

/**
 * a path representing the current working directory
 */
#define CU_PATH_CWD "."

/**
 * a path representing the top of the file system
 */
#define CU_PATH_ROOT "/"

/**
 * a path representing how to separate 2 directories in the path
 *
 * @code
 * /home/koldar/git
 * @endcode
 */
#define CU_PATH_DIR_SEPARATOR "/"

/**
 * a string representing how to seaprate 2 different paths
 *
 * @code
 * /usr/bin:/usr
 * @endcode
 */
#define CU_PATH_PATH_SEPARATOR ":"


/**
 * Creates a path from the directories and files fetched as input
 *
 * @private
 *
 * @pre
 *  @li @c pathBuffer is totally empty
 *
 * @code
 *  cuPathGenerate(buffer, 300, "home", "koldar", "git", "hello.dat");
 * @endcode
 *
 * @param[inout] pathBuffer a buffer where we will put the path
 * @param[in] pathSize the size of the buffer
 * @param[in] argsSize the number of variadic arguments in <tt>...</tt>
 * @param[in] ... the name of the directories composing the path
 * @return
 * 	@li true if we have generated the whole file;
 * 	@li false if the buffer wasn't enough
 */
bool _cuPathGenerate(CU_NOTNULL char* pathBuffer, int pathSize, int argsSize, ...);

/**
 * Creates a path from the directories and files fetched as input
 *
 *
 * @pre
 *  @li @c pathBuffer is totally empty
 *
 * @code
 *  cuPathGenerate(buffer, 300, "home", "koldar", "git", "hello.dat");
 * @endcode
 *
 * @param[inout] pathBuffer a buffer where we will put the path
 * @param[in] pathSize the size of the buffer
 * @param[in] ... the name of the directories composing the path
 * @return
 * 	@li true if we have generated the whole file;
 * 	@li false if the buffer wasn't enough
 */
#define CU_PATH_GENERATE(pathBuffer, pathSize, ...) \
	_cuPathGenerate(pathBuffer, pathSize, CU_COUNT_ARGS(__VA_ARGS__), ## __VA_ARGS__)

/**
 * Appends a path from the directories and files fetched as input
 *
 * @private
 *
 * @code
 *  cuPathGenerate(buffer, 300, "home", "koldar", "git", "hello.dat");
 * @endcode
 *
 * @param[inout] pathBuffer a buffer where we will put the path. It contains inside it a portion of the previous path
 * @param[in] pathSize the size of the buffer (including the part already filled by the other path
 * @param[in] argsSize the number of variadic arguments in <tt>...</tt>
 * @param[in] ... the name of the directories composing the path
 * @return
 * 	@li true if we have generated the whole file;
 * 	@li false if the buffer wasn't enough
 */
bool _cuPathConcatenate(CU_NOTNULL char* pathBuffer, int pathSize, int argsSize, ...);

/**
 * Appends a path from the directories and files fetched as input
 *
 * @private
 *
 * @code
 *  cuPathGenerate(buffer, 300, "home", "koldar", "git", "hello.dat");
 * @endcode
 *
 * @param[inout] pathBuffer a buffer where we will put the path. It contains inside it a portion of the previous path
 * @param[in] pathSize the size of the buffer (including the part already filled by the other path
 * @param[in] ... the name of the directories composing the path
 * @return
 * 	@li true if we have generated the whole file;
 * 	@li false if the buffer wasn't enough
 */
#define CU_PATH_CONCATENATE(pathBuffer, pathSize, ...) \
	_cuPathConcatenate(pathBuffer, pathSize, CU_COUNT_ARGS(__VA_ARGS__), ## __VA_ARGS__)


bool cuPathMkDirs(const char* path);

/**
 * Compute the basename string of a path
 *
 * \note
 * Don't eliminate \c supportBuffer from memory and use the return value of this fuction since this will lead to
 * an invalid read!
 *
 * @param[in] path the path you want to compute the basename of
 * @param[inout] supportBuffer a pointer to the first cell of a char buffer where the return value will be located
 * @return a pointer referring to a cell of \c supportBuffer representing the basename of \c path
 */
char* cu_computeFileBaseName(const char* restrict path, char* restrict supportBuffer);

/**
 * Generate the specified path if it does not exist
 *
 * The function does nothing if the path already exists
 *
 * \post
 * 	\li \c path is present inside the filesystem
 *
 * @param[in] path the path to generate
 * @param[in] isFile false if \c path points to a directory, true if it points to a file
 */
void createPathIfNotExist(const char* path, bool isFile);

#endif /* PATH_UTILS_H_ */
