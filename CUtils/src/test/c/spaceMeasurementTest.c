/*
 * spaceMeasurementTest.c
 *
 *  Created on: Nov 30, 2017
 *      Author: koldar
 */


#include <assert.h>
#include "CuTest.h"
#include "space_measurement.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>

void testSpaceMeasurement01(CuTest* tc) {
	double csci;

	csci = cuSpaceConsumptionToDouble(1000*33, SUE_KB);
	critical("%2.3f", csci);
	assert(csci >= 32.9f && csci <=33.1f);
	csci = cuSpaceConsumptionToDouble((int)(1000*2.5f), SUE_KB);
	critical("%2.3f", csci);
	assert(csci >= 2.4f && csci <= 2.6f);
}

void testSpaceMeasurement02(CuTest* tc) {
	char* p = NULL;
	unsigned long size = 100000000;
	CU_SPACE_PROFILE_CODE(consumed, SME_USED_VIRTUAL, SUE_BYTE) {
		p = malloc(size * sizeof(int));
		memset(p, 0xFF, size);
	}
	if (p != NULL) {
		free(p);
	}

	mem_size_t lowerBound = (mem_size_t)(sizeof(int)*size*0.9);
	mem_size_t upperBound = (mem_size_t)(sizeof(int)*size*1.1);
	critical("%llu %llu %llu", lowerBound, (mem_size_t)consumed, upperBound);
	assert(((mem_size_t)consumed) > 0);
	assert(((mem_size_t)consumed) >= lowerBound);
	assert(((mem_size_t)consumed) <= upperBound);
}

void testSpaceMeasurement03(CuTest* tc) {
	char* p = NULL;
	int size = 100000000;
	CU_SPACE_PROFILE_CODE(consumed, SME_PROCESS_USED_VIRTUAL, SUE_BYTE) {
		p = malloc(size * sizeof(int));
		memset(p, 0xFF, size);
	}
	if (p != NULL) {
		free(p);
	}

	mem_size_t lowerBound = (mem_size_t)(sizeof(int)*size*0.9);
	mem_size_t upperBound = (mem_size_t)(sizeof(int)*size*1.1);
	critical("%llu %llu %llu", lowerBound, (mem_size_t)consumed, upperBound);
	assert(((mem_size_t)consumed) > 0);
	assert(((mem_size_t)consumed) >= lowerBound);
	assert(((mem_size_t)consumed) <= upperBound);
}

void testSpaceMeasurement04(CuTest* tc) {
	char* p = NULL;
	int size = 100000000;
	CU_SPACE_PROFILE_CODE(consumed, SME_USED_PHYSICAL, SUE_BYTE) {
		p = malloc(size * sizeof(int));
		memset(p, 0xFF, size);
	}
	if (p != NULL) {
		free(p);
	}
	critical("%d", (int)consumed);
}

void testSpaceMeasurement05(CuTest* tc) {
	int* p = NULL;
	int size = 1000000;
	CU_SPACE_PROFILE_CODE(consumed, SME_PROCESS_USED_PHYSICAL, SUE_BYTE) {
		p = malloc(size * sizeof(int));
		memset(p, 0xFF, size);
	}
	if (p != NULL) {
		free(p);
	}
	critical("%d", (int)consumed);
}

CuSuite* CuSpaceMeasurementSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testSpaceMeasurement01);
	SUITE_ADD_TEST(suite, testSpaceMeasurement03);

	return suite;
}
