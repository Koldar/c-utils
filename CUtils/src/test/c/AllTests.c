#include <stdio.h>
#include "CuTest.h"
#include "log.h"
#include <stdlib.h>

CuSuite* CuProjectSuite();
CuSuite* CuHTSuite();
CuSuite* CuGraphSuite();
CuSuite* CuStackSuite();
CuSuite* CuSimpleLoopComputerSuite();
CuSuite* CuStringBuilderSuite();
CuSuite* CuRegexSuite();
CuSuite* CuCSVSuite();
CuSuite* CuPlotProducerSuite();
CuSuite* CuTopologicalOrderSuite();
CuSuite* CuHeapSuite();
CuSuite* CuBinarySearchTreeSuite();
CuSuite* CuRedBlackTreeSuite();
CuSuite* CuSCCSuite();
CuSuite* CuLogSuite();
CuSuite* CuMacroSuite();
CuSuite* CuDynamicArraySuite();
CuSuite* CuQueueSuite();
CuSuite* CuArgParserSuite();
CuSuite* CuDynamicStackSuite();
CuSuite* CuSpaceMeasurementSuite();
CuSuite* CuHashSetSuite();
CuSuite* CuVarArgSuite();
CuSuite* CuNaiveQueueSuite();
CuSuite* CuOnlineStatisticPoolSuite();
CuSuite* CuColorsSuite();
CuSuite* CuMultiTrheadingSuite();
CuSuite* CuUtilitySuite();
CuSuite* CuMathUtilsSuite();
CuSuite* CuFileUtilsSuite();
CuSuite* CuDefaultFunctionsSuite();
CuSuite* CuObjectGeneratorSuite();
CuSuite* CuPathSuite();
CuSuite* CuStackTraceSuite();
CuSuite* CuStringUtilsSuite();

static CuSuite* suites[100];
static char run[100];
static int length = 0;

CuSuite* addSuite(CuSuite* suite) {
	suites[length] = suite;
	run[length] = 'y';
	length++;
	return suite;
}

void skip(CuSuite* s) {
	for (int i=0; i<length; i++) {
		if (suites[i] == s) {
			run[i] = 'n';
		}
	}
}

void runOnly(CuSuite* s) {
	for (int i=0; i<length; i++) {
		if (suites[i] == s) {
			run[i] = 'y';
		} else {
			run[i] = 'n';
		}
	}
}

void freeSuites() {
	for (int i=0; i<length; i++) {
		free(suites[length]);
	}
	length = 0;
}


void RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	addSuite(CuProjectSuite());
	addSuite(CuHTSuite());
	addSuite(CuGraphSuite());
	addSuite(CuStackSuite());
	addSuite(CuStringBuilderSuite());
	CuSuite* regex = addSuite(CuRegexSuite());
	addSuite(CuCSVSuite());
	addSuite(CuPlotProducerSuite());
	addSuite(CuTopologicalOrderSuite());
	addSuite(CuHeapSuite());
	addSuite(CuBinarySearchTreeSuite());
	addSuite(CuRedBlackTreeSuite());
	addSuite(CuSCCSuite());
	addSuite(CuLogSuite());
	CuSuite* macro = addSuite(CuMacroSuite());
	addSuite(CuDynamicArraySuite());
	addSuite(CuSimpleLoopComputerSuite());
	addSuite(CuQueueSuite());
	//addSuite(CuArgParserSuite());
	addSuite(CuDynamicStackSuite());
//	addSuite(CuSpaceMeasurementSuite());
	addSuite(CuHashSetSuite());
	addSuite(CuVarArgSuite());
	addSuite(CuNaiveQueueSuite());
	CuSuite* osp = addSuite(CuOnlineStatisticPoolSuite());
	CuSuite* colors = addSuite(CuColorsSuite());
	addSuite(CuStackTraceSuite());
	addSuite(CuMultiTrheadingSuite());
	addSuite(CuUtilitySuite());
	addSuite(CuMathUtilsSuite());
	addSuite(CuFileUtilsSuite());
	addSuite(CuDefaultFunctionsSuite());
	addSuite(CuObjectGeneratorSuite());
	addSuite(CuPathSuite());
	addSuite(CuStringUtilsSuite());


//	runOnly(osp);
//	runOnly(aStar);
	//runOnly(macro);
//	runOnly(colors);
	//skip(aStar);

	for (int i=0; i<length; i++) {
		if (run[i] == 'y') {
			CuSuiteAddSuite(suite, suites[i]);
		}
	}

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);

	freeSuites();

	CuStringDelete(output);

	CuSuiteDelete(suite);
	clearExcludedLoggers();
}

int main(void)
{
	RunAllTests();
}
