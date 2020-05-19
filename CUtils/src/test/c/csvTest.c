/*
 * csvTest.c
 *
 *  Created on: May 22, 2017
 *      Author: koldar
 */

#include <assert.h>
#include "CuTest.h"
#include "log.h"
#include "csvProducer.h"


void testcsvProducer01(CuTest* tc) {
	const char* header[] = {"foo", "bar", "hello"};

	debug("initialize csvHelper...");
	csv_helper* csvHelper = cuCSVHelperNew(__func__, ',', '\n', "%d %5.2f %s", header, "w");
	debug("done");

	assert(csvHelper->headerSize == 3);

	debug("printing data row...");
	cuCSVHelperprintDataRow(csvHelper, 5, 3.2, "string1");
	debug("done");

	cuCSVHelperprintDataRow(csvHelper, 3, 3.2565, "string2");
	cuCSVHelperprintDataRow(csvHelper, 2, 343.2, "string3");
	cuCSVHelperprintDataRow(csvHelper, 1, 35.2, "string4");

	cuCSVHelperDestroy(csvHelper, NULL);
}

void testcsvProducer02(CuTest* tc) {
	const char* header[] = {"foo", "bar", "hello"};

	debug("initialize csvHelper...");
	csv_helper* csvHelper = cuCSVHelperNew(__func__, ',', '\n', "%d %d %d", header, "w");
	debug("done");

	assert(csvHelper->headerSize == 3);

	debug("printing data row...");
	for (int i=0; i<3; i++) {
		cuCSVHelperPrintSingleDataInRow(csvHelper, i);
	}
	cuCSVHelperprintDataRow(csvHelper, 5, 5, 5);
	cuCSVHelperprintDataRow(csvHelper, 5, 5, 5);
	cuCSVHelperprintDataRow(csvHelper, 5, 5, 5);

	cuCSVHelperDestroy(csvHelper, NULL);
}

CuSuite* CuCSVSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testcsvProducer01);
	SUITE_ADD_TEST(suite, testcsvProducer02);

	return suite;
}
