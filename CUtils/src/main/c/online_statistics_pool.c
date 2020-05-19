/*
 * online_statistics_pool.c
 *
 *  Created on: Jan 4, 2018
 *      Author: koldar
 */

#include "online_statistics_pool.h"
#include "defaultFunctions.h"
#include <stdlib.h>
#include "hashtable.h"
#include "macros.h"
#include <limits.h>
#include "errors.h"

struct online_statistics_pool {
	/**
	 * Hashtable keyed with strings and online_statistics as values
	 *
	 * keys are assumed to be in the data sector of the application while values are assumed to be in the heap
	 */
	HT* statistics;
};

static CU_NULLABLE online_statistics* cuOSPGetItem(CU_NULLABLE const online_statistics_pool* pool, CU_NOTNULL const char* name);
static bool _isOSPEnabled(CU_NULLABLE const online_statistics_pool* pool);

CU_NULLABLE online_statistics_pool* cuOSPNew(bool enable) {
	if (!enable) {
		return NULL;
	}

	online_statistics_pool* retVal = malloc(sizeof(online_statistics_pool));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->statistics = cuHTNew();

	return retVal;
}

void cuOSPDestroy(const online_statistics_pool* pool, CU_NULLABLE const struct var_args* context) {
	if (!_isOSPEnabled(pool)) {
		return;
	}

	CU_ITERATE_OVER_HT_VALUES(pool->statistics, pool, online_statistics*) {
		cuOnlineStatisticsDestroy(pool, context);
	}
	cuHTDestroy(pool->statistics, context);
	CU_FREE(pool);
}


void cuOSPUpdateItem(online_statistics_pool* stat, const char* name, double newValue) {
	if (!_isOSPEnabled(stat)) {
		return;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);
	cuOnlineStatisticsUpdate(retVal, newValue);
}

void cuOSPAddOne(CU_NULLABLE online_statistics_pool* stat, CU_NOTNULL const char* name) {
	cuOSPUpdateItem(stat, name, (double)1.0);
}

double cuOSPGetAverage(const online_statistics_pool* stat, const char* name) {
	if (!_isOSPEnabled(stat)) {
		return 0;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);
	if (cuOnlineStatisticsIsEmpty(retVal)) {
		ERROR_OBJECT_IS_EMPTY("online statistic", "%s", name);
	}
	return cuOnlineStatisticsGetAverage(retVal);
}

double cuOSPGetVariance(const online_statistics_pool* stat, const char* name) {
	if (!_isOSPEnabled(stat)) {
		return 0;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);
	if (cuOnlineStatisticsIsEmpty(retVal)) {
		ERROR_OBJECT_IS_EMPTY("online statistic", "%s", name);
	}
	return cuOnlineStatisticsGetVariance(retVal);
}

double cuOSPGetStandardDeviation(const online_statistics_pool* stat, const char* name) {
	if (!_isOSPEnabled(stat)) {
		return +INFINITY;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);
	if (cuOnlineStatisticsIsEmpty(retVal)) {
		ERROR_OBJECT_IS_EMPTY("online statistic", "%s", name);
	}
	return cuOnlineStatisticsGetStandardDeviation(retVal);
}

double cuOSPGetMin(const online_statistics_pool* stat, const char* name) {
	if (!_isOSPEnabled(stat)) {
		return +INFINITY;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);
	if (cuOnlineStatisticsIsEmpty(retVal)) {
		ERROR_OBJECT_IS_EMPTY("online statistic", "%s", name);
	}
	return cuOnlineStatisticsGetMin(retVal);
}

double cuOSPGetMax(const online_statistics_pool* stat, const char* name) {
	if (!_isOSPEnabled(stat)) {
		return -INFINITY;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);
	if (cuOnlineStatisticsIsEmpty(retVal)) {
		ERROR_OBJECT_IS_EMPTY("online statistic", "%s", name);
	}
	return cuOnlineStatisticsGetMax(retVal);
}

double cuOSPGetMaxOrDefault(const online_statistics_pool* stat, const char* name, double defaultValue) {
	if (!_isOSPEnabled(stat)) {
		return -INFINITY;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);

	return cuOnlineStatisticsGetMaxOrDefault(retVal, defaultValue);
}

long cuOSPGetNumber(const online_statistics_pool* stat, const char* name) {
	if (!_isOSPEnabled(stat)) {
		return 0;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);
	return cuOnlineStatisticsGetN(retVal);
}

double cuOSPGetLastValue(const online_statistics_pool* stat, const char* name) {
	if (!_isOSPEnabled(stat)) {
		return true;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);
	if (cuOnlineStatisticsIsEmpty(retVal)) {
		ERROR_OBJECT_IS_EMPTY("online statistic", "%s", name);
	}

	return cuOnlineStatisticsGetLastValue(retVal);
}

double cuOSPGetLastValueOrDefault(const online_statistics_pool* stat, const char* name, double defaultValue) {
	if (!_isOSPEnabled(stat)) {
		return defaultValue;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);
	return cuOnlineStatisticsGetLastValueOrDefault(retVal, defaultValue);
}

void cuOSPClear(online_statistics_pool* stat, const char* name) {
	if (!_isOSPEnabled(stat)) {
		return;
	}

	online_statistics* retVal = cuOSPGetItem(stat, name);
	cuOnlineStatisticsClear(retVal);
}

void cuOSPClearPool(online_statistics_pool* stat) {
	if (!_isOSPEnabled(stat)) {
		return;
	}

	CU_ITERATE_OVER_HT_VALUES(stat->statistics, stat, online_statistics*) {
		cuOnlineStatisticsClear(stat);
	}
}

bool cuOSPIsEmpty(const online_statistics_pool* stat, const char* name) {
	if (!_isOSPEnabled(stat)) {
		return true;
	}
	online_statistics* retVal = cuOSPGetItem(stat, name);
	return cuOnlineStatisticsIsEmpty(retVal);
}

bool cuOSPIsPoolEmpty(const online_statistics_pool* pool) {
	if (!_isOSPEnabled(pool)) {
		return true;
	}
	return cuHTIsEmpty(pool->statistics);
}

int cuOSPGetPoolSize(const online_statistics_pool* pool) {
	if (!_isOSPEnabled(pool)) {
		return 0;
	}
	return cuHTGetSize(pool->statistics);
}

/**
 * Return an online_statistics in the pool with the given name
 *
 * \note
 * If the online_statistics is not present in the pool, it will be automatically be created
 *
 * @param[in] pool the pool to consider
 * @param[in] name the name of the online_statistics to get;
 * @return the associated online_statistics you wanted.
 */
static online_statistics* cuOSPGetItem(const online_statistics_pool* pool, const char* name) {
	online_statistics* retVal = cuHTGetItem(pool->statistics, hashString(name));
	if (retVal == NULL) {
		retVal = cuOnlineStatisticsNew();
		cuHTAddItem(pool->statistics, hashString(name), retVal);
	}
	return retVal;
}

/**
 * Check if a pool is valid or not
 *
 * @param[in] pool the pool to check
 * @return
 * @li true if the pool is valid, aka the user wants to register data
 */
static bool _isOSPEnabled(CU_NULLABLE const online_statistics_pool* pool) {
	return pool != NULL;
}
