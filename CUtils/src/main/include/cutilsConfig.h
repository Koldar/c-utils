/**
 * @file
 *
 * A file containing all the macros used to configure CUtils project
 *
 * @author koldar
 * @date Sep 26, 2018
 */

#ifndef CUTILSCONFIG_H_
#define CUTILSCONFIG_H_

/**
 * @defgroup config Configuration Macros
 * All the macros used to configure CUtils
 * @{
 */

/**
 * The number ofentries in the stack to print in macros like ::CU_PRINT_STACKTRACE
 */
#ifndef CU_MAX_STACK_ENTRIES
#define CU_MAX_STACK_ENTRIES 128
#endif

/**
 * The default size of a buffer in the project
 *
 * It might happen that you need some temporary buffer where to store something (a s tring, some integers and so on).
 *
 * This macro allows you to speciofy a default size for that buffer without having to worry about specifying magic numbers.
 */
#ifndef BUFFER_SIZE
#	define BUFFER_SIZE 300
#endif

/**
 *
 * Represents a size of a buffer much longer than ::BUFFER_SIZE. It usually
 * is 5 times bigger than ::BUFFER_SIZE
 */
#ifndef LONG_BUFFER_SIZE
#	define LONG_BUFFER_SIZE (5*BUFFER_SIZE)
#endif

/**
 * CUtils contains several static array used to store temporary information.
 * If the size of such arrays is too small for your application, you can increase their size by teaking with this macro
 */
#ifndef CUTILS_ARRAY_SIZE
#	define CUTILS_ARRAY_SIZE 1000
#endif

///@}

#endif /* CUTILSCONFIG_H_ */
