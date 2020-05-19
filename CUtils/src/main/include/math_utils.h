/**
 * @file
 *
 * Contains several useful math functions
 *
 * @date Sep 19, 2017
 * @author koldar
 */

#ifndef MATH_UTILS_H_
#define MATH_UTILS_H_

#include "macros.h"


/**
 * Compute \f${{n}\choose{k}}\f$
 *
 * @param[in] n the number of element choosable
 * @param[in] k the size each group chosen must have. Default to 2
 * @return the binomial coefficient
 */
int cuMathUtilsBinomialCoefficient(int n, int k);
CU_DECLARE_FUNCTION_WITH_DEFAULTS(int, cuMathUtilsBinomialCoefficient, int, int);
#define cuMathUtilsBinomialCoefficient(...) CU_CALL_FUNCTION_WITH_DEFAULTS(cuMathUtilsBinomialCoefficient, 2, __VA_ARGS__)
CU_DECLARE_DEFAULT_VALUES(cuMathUtilsBinomialCoefficient,
        ,
		2
);

/**
 * COmputes the factorial of a number
 *
 * @param[in] n the number whose factorial we need to compute
 * @return the factorial of the number
 */
int cuMathUtilsFactorial(int n);

/**
 * Clamp a number between a certain interval
 *
 * @param[in] n the number to clamp
 * @param[in] a lower bound of the interval
 * @param[in] b upper bound of the interval
 * @return
 *  @li n if \f$ a \leq n leq b\f$;
 *  @li a if \f$ n < a \f$;
 *  @li b if \f$ n > b \f$;
 */
int cuMathUtilsClamp(int n, int a, int b);

/**
 * Clamp a number between a certain interval
 *
 * @param[in] n the number to clamp
 * @param[in] a lower bound of the interval
 * @param[in] b upper bound of the interval
 * @return
 *  @li n if \f$ a \leq n leq b\f$;
 *  @li a if \f$ n < a \f$;
 *  @li b if \f$ n > b \f$;
 */
float cuMathUtilsClamp2(float n, float a, float b);

/**
 * get minimum between 2 integers
 *
 * @param[in] a the first number
 * @param[in] b the second number
 * @return the minimum between \c a and \c b
 */
int getMinimumBetweenInt(const int a, const int b);

#endif /* MATH_UTILS_H_ */
