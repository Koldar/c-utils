/*
 * errors.h
 *
 *  Created on: May 17, 2017
 *      Author: koldar
 */

#ifndef _CUTILS_ERRORS_HEADER__
#define _CUTILS_ERRORS_HEADER__

#include "stacktrace.h"

/**
 * print a generic error
 *
 * @param[in] errCode integer. the error code specific to this error
 * @param[in] format const char*. a `printf`-like string format of the emssage to produce
 * @param[in] ... parameters of `format`
 */
#define _CU_ERROR_GENERIC(errCode, format, ...) \
	fprintf(stderr, "@ %s:%s LINE %d ERROR CODE %04d", __FILE__, __func__, __LINE__, errCode); \
	fprintf(stderr, "Message: "); \
	fprintf(stderr, format, ## __VA_ARGS__); \
	\
	exit(errCode)

/**
 * Error to use when a malloc return NULL
 */
#define ERROR_MALLOC() \
	_CU_ERROR_GENERIC(1, "Malloc returned NULL! It appears the memory is full")

/**
 * Error to use when fopen fails
 */
#define ERROR_FILE(file) \
	_CU_ERROR_GENERIC(2, "Bad permission while handling file \"%s\"", file)


/**
 * Error to use when you failed to add an element to something else because such something else is actually full.
 *
 * I usually use it on array, stacks and so on
 */
#define ERROR_OBJECT_IS_FULL(object, size) \
	_CU_ERROR_GENERIC(3, "Object \"%s\" has reached its maximum capacity %d!", object, size)

//TODO to remove


#define ERROR_NOT_SUCH_OBJECT(objectDsc, objectId) \
	_CU_ERROR_GENERIC(4, "Object %s %d has not been found!", objectDsc, objectId);

#define ERROR_OBJECT_NOT_FOUND(objectDsc, objectTemplate, object) \
		_CU_ERROR_GENERIC(4, "Object %s " objectTemplate " has not been found!", objectDsc, object);

//TODO to remove
#define ERROR_CONSTRUCTION_FAILED(objectDsc, objectId) \
		_CU_ERROR_GENERIC(5, "%s cannot be created by using \"%s\"!", objectDsc, objectId)

#define ERROR_ON_CONSTRUCTION(objectDsc, objectFormat, object) \
	_CU_ERROR_GENERIC(5, "%s cannot be created by using \""objectFormat "\"", objectDsc, object)

//TODO remove
/**
 * Error to use when you have tried (but failed) to apply something to something else.
 *
 * I usually use it for regex on string
 */
#define ERROR_APPLICATION_FAILED(applierDsc, applier, appliedDsc, applied) \
	_CU_ERROR_GENERIC(6, "%s \"%s\" cannot be applied to %s \"%s\"!", applierDsc, applier, appliedDsc, applied)

/**
 * Error to use when you have tried (but failed) to apply something to something else.
 *
 * I usually use it for regex on string
 *
 * @code
 * ERROR_ON_APPLICATION("Function", "%s", "strcmp", "value", "%d", 596)
 * @endcode
 *
 * @param[in] applierDsc a brief description of what the applier is
 * @param[in] applierFormat how you want to format the applied
 * @param[in] applier the applier itself
 * @param[in] appliedDsc a brief description of what the applied is
 * @param[in] appliedFormat how you want to format the applied
 * @param[in] applied the applied itself
 */
#define ERROR_ON_APPLICATION(applierDsc, applierFormat, applier, appliedDsc, appliedFormat, applied) \
	_CU_ERROR_GENERIC(6, "%s \"" applierFormat "\" is not applicable to %s \"" appliedFormat "\"!", applierDsc, applier, appliedDsc, applied)

#define ERROR_EQUALITY_FAILED(object1Dsc, object1, object2Dsc, object2) \
	_CU_ERROR_GENERIC(7, "%s \"%s\" isn't the same of %s \"%s\"!", object1Dsc, object1, object2Dsc, object2)

//TODO remove this
/**
 * Error to show when the code arrives to a case scenario that isn't supported
 *
 * I tend to exploit this in switch cases
 */
#define ERROR_UNHANDLED_CASE(scenarioDsc, scenario) \
	_CU_ERROR_GENERIC(8, "Case \"%d\" within \"%s\" not handled at all", scenario, scenarioDsc)

/**
 * Error to show when the code arrives to a case scenario that isn't supported
 *
 * I tend to exploit this in switch cases
 *
 * @code
 * switch(direction) {
 * 	case 0: //left
 * 	case 1: //up
 * 	case 2: //down
 * 	case 3: //right
 * 	default: {
 * 		ERROR_INVALID_SWITCH_CASE("direction", "%d", direction);
 * 	}
 * }
 *
 * @endcode
 *
 *
 * @param[in] scenarioDsc a description of what the switch-case is trying to achieve
 * @param[in] scenarioFormat the format of \c scenario. \c printf like template (e.g. <tt>%d</tt>)
 * @param[in] scenario the variable in the switch case
 *
 */
#define ERROR_INVALID_SWITCH_CASE(scenarioDsc, scenarioFormat, scenario) \
	_CU_ERROR_GENERIC(8, "Case \"" scenarioFormat "\" within \"%s\" not handled at all", scenario, scenarioDsc)

/**
 * Error to show when the code goes into a not implemented feature
 */
#define ERROR_NOT_IMPLEMENTED() \
	_CU_ERROR_GENERIC(9, "Not implemented yet error!")

#define ERROR_NULL(dsc) \
	_CU_ERROR_GENERIC(10, "%s is unexpectly NULL!", dsc)

#define ERROR_IS_ALREADY_PRESENT(object, container, format) \
	_CU_ERROR_GENERIC(11, "object \"" format "\" is already inside \"%s\"!", object, container)

/**
 * Error to show when you need to add an error but you know the program shouild neve go there
 */
#define ERROR_IMPOSSIBLE_SCENARIO(scenarioDsc, ...) \
	_CU_ERROR_GENERIC(12, "The software entered in a scenario that shouldn't happen at all! There is an high chance this is a BUG! Here's a description:\n" scenarioDsc, ## __VA_ARGS__)

/**
 * Error to show when an object that shouldn't be empty it is
 *
 *
 * @code
 * //assume s is a stack
 * ERROR_OBJECT_IS_EMPTY("stack", "%p", s);
 * @endcode
 *
 * @param[in] objectDsc a description of what is it the object
 * @param[in] objectTemplate how you want to print the object
 * @param[in] object the object to print
 */
#define ERROR_OBJECT_IS_EMPTY(objectDsc, objectTemplate, object) \
	_CU_ERROR_GENERIC(13, "The object %s \"" objectTemplate  "\" is empty!", objectDsc, object)

/**
 * Puts an unreachable return
 *
 * Sometime C warns us we didn't put a return value even in scenarios where the return cannot be reached. For example:
 *
 * @code
 * int foo(int a) {
 * 	if (a > 0) {
 * 	 	return +1;
 * 	} else if (a < 0) {
 * 		return -1;
 * 	} else {
 * 		exit(1);
 * 	}
 *
 * 	//we shouldn't put a return here
 * 	UNREACHABLE_RETURN(0);
 * }
 * @endcode
 *
 * This function puts a dummy return statement. However, before it also puts an error message: in this way you cannot possibly access to the return
 * statement itself. However, such unreachable return solves the warning from the compiler
 *
 * @param[in] impossibleValue the value to *return* on the unreachable return
 */
#define UNREACHABLE_RETURN(impossibleValue) \
	_CU_ERROR_GENERIC(13, "The software reached a return statement that should be impossible to reach. Something is very wrong!"); \
	return (impossibleValue)

#define CU_ERROR_PRINTF() \
	_CU_ERROR_GENERIC(14, "printf returned an error!")

#define CU_ERROR_PRINTF_BUFFEROVERFLOW() \
	_CU_ERROR_GENERIC(15, "printf buffer overflow!")

#define CU_ERROR_IMPOSSIBLE_OPERATION(description, ...) \
	_CU_ERROR_GENERIC(16, description, ## __VA_ARGS__)


#endif
