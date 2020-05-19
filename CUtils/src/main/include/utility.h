/**
 * \file utility.h
 *
 * A file where lots of utility functions used throughout all the project are defined
 *
 *  Created on: Dec 22, 2016
 *      Author: koldar
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "macros.h"
#include "random_utils.h"

#ifndef GRAPHIMPL
#	define GRAPHIMPL PredSuccGraph
#endif

#ifndef GRAPHEMPTYCONSTRUCTOR
#	define GRAPHEMPTYCONSTRUCTOR cuPredSuccGraphNew
#endif

#ifndef GRAPHDESTRUCTOR
#	define GRAPHDESTRUCTOR cuPredSuccGraphDestroyWithElements
#endif

/**
 * Static variable representing where we need to retrieve user commands
 *
 * The variable is used whenever a scanf is required or in general whenever we need to fetch data from user keyboard
 *
 * \ingroup globalVariables
 */
extern FILE* finput;
/**
 * Static variable representing where we need to print output
 *
 * The variable is used whenever we need to print out data to the console
 *
 * \ingroup globalVariables
 */
extern FILE* foutput;

/**
 * Exercute a command from bash
 *
 * @post
 *  @li if the command execute fails, an error is raised
 *
 * @param[in] template printf-like string
 * @param[in] ... parameters for @c template string
 */
void cuUtilsCallSystem(CU_NOTNULL const char* template, ...);


/**
 * Parse a string representing a range
 *
 * Accepted ranges are:
 * ```
 * [ 2; 3 ]
 * ] 2; 3 [
 * ( 2; 3 )
 * //or mix of them (white space optionals)
 * ```
 *
 * The general pattern is:
 *
 * ```
 * range <- open number ; number close
 * open <- either [, ] or (
 * close <- either [, ] or )
 * ```
 *
 * @param[in] rangeStr a string representing a range.
 * @param[out] a the lower value of the range
 * @param[out] b the upper value of the range
 * @param[out] aIncluded true if the lower value is included in the range, false otherwise
 * @param[out] bIncluded true if the upper value is included in the range, false otherwise
 * @return true if we could parse the value, false otherwise
 */
bool cuUtilsRangeInt(CU_NOTNULL const char* rangeStr, CU_NOTNULL int* restrict a, CU_NOTNULL int* restrict b, CU_NOTNULL bool* restrict aIncluded, CU_NOTNULL bool* restrict bIncluded);

/**
 * Like ::cuUtilsRangeInt but using cu_int_range
 *
 * @param[in] rangeStr a string representing a range.
 * @param[out] range a structure which needs to be popoulated
 * @return true if we could parse the value, false otherwise
 */
bool cuUtilsRangeInt2(CU_NOTNULL const char* rangeStr, CU_NOTNULL struct cu_int_range* range);

#endif /* UTILITY_H_ */
