/*
 * timeMeasurement.c
 *
 *  Created on: Jan 19, 2017
 *      Author: koldar
 */


#include "timeMeasurement.h"
#include "macros.h"
#include <limits.h>
#include <stdbool.h>
#include "errors.h"

static char* timeMeasurementString[] = {
		"ns", "us", "ms", "s"
};

const char* cuTimeMeasurementgetConstantString(enum time_unit_measurement tm) {
	return timeMeasurementString[tm];
}

enum time_unit_measurement cuTimeMeasurementParseFromString(CU_NOTNULL const char* s) {
	switch (*s) {
	case 's': return TM_SECONDS;
	case 'n': return TM_NANO;
	case 'u': return TM_MICRO;
	case 'm': return TM_MILLI;
	default: {
		ERROR_UNHANDLED_CASE("time measurement", *s);
	}
	}
	return TM_MICRO;
}

struct timespec cuTimeMeasurementGetCurrentTime() {
	struct timespec retVal;
	clock_gettime(CLOCK_MONOTONIC, &retVal);
	return retVal;
}

long cuTimeMeasurementComputeTimeGap(struct timespec start, struct timespec end, enum time_unit_measurement format) {
	long sec, nanoSec, retVal;

	sec = end.tv_sec - start.tv_sec;
	nanoSec = end.tv_nsec - start.tv_nsec;

	retVal = ((sec * 1E9) + nanoSec);
	for (int i=0; i<format; i++) {
		retVal = retVal/1000L;
	}
	return retVal;
}

struct tm cuTimeMeasurementNaiveCurrentDate() {
	time_t t = time(NULL);
	return *localtime(&t);
}

