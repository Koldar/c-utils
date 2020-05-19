/**
 * @file
 *
 * Specifies a set of functions useful to generate random numbers
 *
 * @date Jul 18, 2017
 * @author koldar
 */

#ifndef RANDOM_UTILS_H_
#define RANDOM_UTILS_H_

#include <stdbool.h>

/**
 * A structure representing an range in the integer field
 */
struct cu_int_range {
	int a;
	int b;
	bool aIncluded;
	bool bIncluded;
};

/**
 * Get a random integer number inside the range \f$ \[ min; max \]\f$
 *
 * \pre
 * 	\li \f$ max > min \f$;
 *
 * @param[in] min the range lowerbound
 * @param[in] max the range upper bound
 * @param[in] minIncluded true if min might be choosen in the random, false otherwise
 * @param[in] maxIncluded true if max might be choosen in the random, false otherwise
 * @return the number choosen
 */
int cuRandomUtilsGetInRange(int min, int max, bool minIncluded, bool maxIncluded);

/**
 * Like ::cuRandomUtilsGetInRange but accepts a range of int
 *
 * @param[in] intRange a struct representing a range
 * @return the number choosen
 */
int cuRandomUtilsGetInRange2(struct cu_int_range intRange);

/**
 * Choose a value among the one specified in an array
 *
 * @param[in] n the size of the array
 * @param[in] array an array containing the number we want to choose from
 * @return a random value in @c array
 */
int cuRandomUtilsGetInArray(int n, const int array[]);

/**
 * Pick a subset of elements in a given array
 *
 * @code
 * cuRandomUtilsSubsetArray(3, 10, {false,false,false,false,false,false,false,false,false,false});
 * //we pick 3 elements in the array. sbuset will be (for example):
 * {false, false, true, false, false, false, true, true, false, false}
 * //if the array is the mark of an array {1,2,3,4,5,6,7,8,9,10}, we should pick 3,7,8
 * @endcode
 *
 * @param[in] k the number of elements to pick in @c array
 * @param[in] n the size of the array @c array and @c subset
 * @param[inout] subset after the function it contains true if the i-th element is picked, false otherwise
 */
void cuRandomUtilsSubsetArray(int k, int n, bool subset[]);

/**
 * Often you need to check a condition depending on the outcome of a probability.
 * For example do something with probability 0.7. This method returns true if such probability is checked
 *
 * @param[in] probability a number between 0.0 and 1.0
 * @return
 * 	\li true if the random succeed in the probability;
 * 	\li false otherwise
 */
bool cuRandomUtilsGetInProbability(double probability);

/**
 * get a random number
 *
 * @param[in] a the lower bound of the random number (it may be returned by the function)
 * @param[in] b the upper bound of the random numer;
 * @param[in] includeb true if want that the function may return b, false otherwise
 * @return an integer random nunber in \f$ [lowInclusive; highExclusive[ \f$
 */
int getRandom(const unsigned int a, const unsigned int b, const bool includeb);

/**
 * get a random number among those inside the array
 *
 * @param[in] n the size of the array \c array
 * @param[in] array the container of the numbers we need to choose from
 * @return one of the number inside \c array
 */
int getRandomFrom(int n, const int array[]);

#endif /* RANDOM_UTILS_H_ */
