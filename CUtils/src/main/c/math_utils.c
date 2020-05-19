/*
 * math_utils.c
 *
 *  Created on: Sep 19, 2017
 *      Author: koldar
 */

#include "math_utils.h"
#include "macros.h"
#include "errors.h"

int cuMathUtilsBinomialCoefficient(int n, int k) {
	if (k == 2) {
		//we use a faster implementation if k=2 since we can compute it in one sweep
		return (n*(n-1))/2;
	} else {
		int nFact = cuMathUtilsFactorial(n);
		int kFact = cuMathUtilsFactorial(k);
		int n_kFact = cuMathUtilsFactorial(n - k);

		//this is a really naive implementation. A more accurate implementation would be to delete sahred factorials sectors
		return nFact / (kFact * n_kFact);
	}
}

CU_DEFINE_DEFAULT_VALUES(cuMathUtilsBinomialCoefficient,
        ,
		2
);

int cuMathUtilsFactorial(int n) {
	//we do no use recursion to avoid unecessary stacks/unstacks
	int retVal = 1;
	for (int i=n; i>1; i--) {
		retVal *= i;
	}
	return retVal;
}

int cuMathUtilsClamp(int n, int a, int b) {
	if (n < a) {
		return a;
	}
	if (n > b) {
		return b;
	}
	return n;
}

float cuMathUtilsClamp2(float n, float a, float b) {
	if (n < a) {
		return a;
	}
	if (n > b) {
		return b;
	}
	return n;
}

int getMinimumBetweenInt(const int a, const int b) {
	return a > b ? b : a;
}
