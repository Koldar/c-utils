/*
 * random_utils.h
 *
 *  Created on: Jul 18, 2017
 *      Author: koldar
 */

#include "random_utils.h"
#include <stdlib.h>
#include <limits.h>
#include "macros.h"
#include "log.h"
#include "errors.h"

static int rand_lim(int limit);

/* return a random number between 0 and limit inclusive.
 *
 * <a href="https://stackoverflow.com/a/2999130/1887602">Stack Overflow answer</a>
 */
static int rand_lim(int limit) {
	int divisor;

	if (limit < RAND_MAX) {
		divisor = RAND_MAX/(limit+1);
	} else if (limit == RAND_MAX) {
		divisor = 1;
	} else {
		ERROR_ON_APPLICATION("random", "%s", "rand_lim", "number", "%d", limit);
	}
    int retval;

    do {
        retval = random() / divisor;
    } while (retval > limit);

    return retval;
}

int get_random(int min, int max, bool minIncluded, bool maxIncluded) {
	min += minIncluded ? 0 : 1;
	max -= maxIncluded ? 0 : 1;

	return min + rand_lim(max - min);
}


int cuRandomUtilsGetInRange(int min, int max, bool minIncluded, bool maxIncluded) {
	min += minIncluded ? 0 : 1;
	max += maxIncluded ? 1 : 0;

	//now min is always included and max is always excluded
	return min + rand_lim(max - min - 1);
	//return min + (rand() % (max - min));
}

int cuRandomUtilsGetInRange2(struct cu_int_range intRange) {
	return cuRandomUtilsGetInRange(intRange.a, intRange.b, intRange.aIncluded, intRange.bIncluded);
}

int cuRandomUtilsGetInArray(int n, const int array[]) {
	return array[cuRandomUtilsGetInRange(0, n, true, false)];
}

void cuRandomUtilsSubsetArray(int k, int n, bool subset[]) {
	int i=0;

	//set all the elements of the array to false
	memset(subset, false, n);

	while (true) {
		if (i >= n) {
			break;
		}

		int val = cuRandomUtilsGetInRange(0, n, true, false);
		//if the value has not been set yet, we set it
		if (!subset[val]) {
			subset[val] = true;
			i++;
		}
	}

	return;
}

bool cuRandomUtilsGetInProbability(double probability) {
	int p = (int)(probability * ((double)RAND_MAX));
	return cuRandomUtilsGetInRange(0, RAND_MAX, true, false) < p;
}

//TODO remove. This function is deprecated
int getRandom(const unsigned int a, const unsigned int b, const bool includeb) {
	return a + (rand() % (b - a + (includeb ? 1 : 0)));
}

//TODO deprecated and replaced by cuRandomUtilsGetInArray
int getRandomFrom(int n, const int array[]) {
	int place = getRandom(0, n, false);
	return array[place];
}
