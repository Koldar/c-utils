/*
 * online_statistics.c
 *
 *  Created on: Aug 16, 2017
 *      Author: koldar
 */


#include "online_statistics.h"
#include <stdlib.h>
#include <math.h>
#include "macros.h"
#include "errors.h"

struct online_statistics {
	double average;
	double variance;
	double min;
	double max;
	long n;
	double lastValue;
};

online_statistics* cuOnlineStatisticsNew() {
	online_statistics* retVal = malloc(sizeof(online_statistics));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	cuOnlineStatisticsClear(retVal);

	return retVal;
}

void cuOnlineStatisticsDestroy(online_statistics* stat, CU_NULLABLE const struct var_args* context) {
	free(stat);
}

online_statistics* cuOnlineStatisticsUpdate(online_statistics* stat, double newValue) {
	stat->lastValue = newValue;
	//number
	stat->n++;
	//average
	double delta1 = newValue - stat->average;
	stat->average += delta1/stat->n;
	//variance
	double delta2 = newValue - stat->average;
	double m2 = delta1 * delta2;
	if (stat->n < 2) {
		stat->variance = 0;
	} else {
		stat->variance = (((stat->n - 1) * stat->variance) + m2)/(stat->n);
	}
	//min
	if (newValue < stat->min) {
		stat->min = newValue;
	}
	//max
	if (newValue > stat->max) {
		stat->max = newValue;
	}

	return stat;
}

double cuOnlineStatisticsGetAverage(const online_statistics* stat) {
	if (stat->n == 0) {
		ERROR_OBJECT_NOT_FOUND("mean value of statistic", "%p", stat);
	}

	return stat->average;
}

double cuOnlineStatisticsGetVariance(const online_statistics* stat) {
	if (stat->n == 0) {
		ERROR_OBJECT_NOT_FOUND("variance value of statistic", "%p", stat);
	}

	return stat->variance;
}

double cuOnlineStatisticsGetStandardDeviation(const online_statistics* stat) {
	return sqrt(stat->variance);
}

double cuOnlineStatisticsGetMin(const online_statistics* stat) {
	if (stat->n == 0) {
		ERROR_OBJECT_NOT_FOUND("min value of statistic", "%p", stat);
	}

	return stat->min;
}

double cuOnlineStatisticsGetMax(const online_statistics* stat) {
	if (stat->n == 0) {
		ERROR_OBJECT_NOT_FOUND("max value of statistic", "%p", stat);
	}

	return stat->max;
}

double cuOnlineStatisticsGetMaxOrDefault(const online_statistics* stat, double defaultValue) {
	return cuOnlineStatisticsIsEmpty(stat) ? defaultValue : stat->max;
}

long cuOnlineStatisticsGetN(const online_statistics* stat) {
	return stat->n;
}

double cuOnlineStatisticsGetLastValue(const online_statistics* stat) {
	if (stat->n == 0) {
		ERROR_OBJECT_NOT_FOUND("last value of statistic", "%p", stat);
	}
	return stat->lastValue;
}

double cuOnlineStatisticsGetLastValueOrDefault(const online_statistics* stat, double defaultValue) {
	return cuOnlineStatisticsIsEmpty(stat) ? defaultValue : stat->lastValue;
}

void cuOnlineStatisticsClear(online_statistics* stat) {
	stat->average = 0;
	stat->max = -INFINITY;
	stat->min = +INFINITY;
	stat->n = 0;
	stat->variance = 0;
	stat->lastValue = 0;
}

bool cuOnlineStatisticsIsEmpty(const online_statistics* stat) {
	return stat->n == 0;
}
