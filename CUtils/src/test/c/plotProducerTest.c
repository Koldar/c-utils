/*
 * plotProducerTest.c
 *
 *  Created on: May 22, 2017
 *      Author: koldar
 */

#include <assert.h>
#include <stdbool.h>
#include "CuTest.h"
#include "plot2DProducer.h"
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


void testPlot2DProducer01(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "sin", PS_POINTS);

	for (double x=-10; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoint(helper, x, sin(x));
	}

	cuPlot2DHelperPlot(helper);
	cuPlot2DHelperDestroy(helper, NULL);
}

///like ::testPlot2DProducer01 but uses ::cuPlot2DHelperAddPoints with 2 series
void testPlot2DProducer02(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "sin", PS_POINTS);
	cuPlot2DHelperAddSeries(helper, "cos", PS_POINTS);

	for (double x=-10; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoints(helper, x, (double[]){sin(x), cos(x)});
	}

	cuPlot2DHelperPlot(helper);
	cuPlot2DHelperDestroy(helper, NULL);
}

///like ::testPlot2DProducer02 but set title, x axis and y axis
void testPlot2DProducer03(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "sin", PS_POINTS);
	cuPlot2DHelperAddSeries(helper, "cos", PS_POINTS);

	for (double x=-10; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoints(helper, x, (double[]){sin(x), cos(x)});
	}

	cuPlot2DHelperSetTitle(helper, __func__);
	cuPlot2DHelperSetXLabelName(helper, "x");
	cuPlot2DHelperSetYLabelName(helper, "f(x)");

	cuPlot2DHelperPlot(helper);
	cuPlot2DHelperDestroy(helper, NULL);
}

void testPlot2DProducer04(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "sin", PS_LINES);
	cuPlot2DHelperAddSeries(helper, "cos", PS_POINTS);

	for (double x=-10; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoint(helper, x, sin(x));
		cuPlot2DHelperAddPoint(helper, x, cos(x));
	}

	cuPlot2DHelperSetTitle(helper, __func__);
	cuPlot2DHelperSetXLabelName(helper, "x");
	cuPlot2DHelperSetYLabelName(helper, "f(x)");

	cuPlot2DHelperPlot(helper);
	cuPlot2DHelperDestroy(helper, NULL);
}

void testPlot2DProducer05(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "sin", PS_LINES);

	for (double x=-10; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoint(helper, x, sin(x));
	}

	cuPlot2DHelperSetTitle(helper, __func__);
	cuPlot2DHelperSetXLabelName(helper, "x");
	cuPlot2DHelperSetYLabelName(helper, "f(x)");

	cuPlot2DHelperPlot(helper);
	cuPlot2DHelperDestroy(helper, NULL);
}

void testPlot2DProducer06(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "sin", PS_LINES);

	for (double x=-10; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoint(helper, x, sin(x));
	}

	cuPlot2DHelperSetTitle(helper, __func__);
	cuPlot2DHelperSetXLabelName(helper, "x");
	cuPlot2DHelperSetYLabelName(helper, "f(x)");
	cuPlot2DHelperSetGrid(helper, true);

	cuPlot2DHelperPlot(helper);
	cuPlot2DHelperDestroy(helper, NULL);
}

///use the logaritmic scale on y
void testPlot2DProducer07(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "e^x", PS_LINES);

	for (double x=0; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoint(helper, x, exp(x));
	}

	cuPlot2DHelperSetTitle(helper, __func__);
	cuPlot2DHelperSetXLabelName(helper, "x");
	cuPlot2DHelperSetYLabelName(helper, "f(x)");
	cuPlot2DHelperSetGrid(helper, true);
	cuPlot2DHelperSetYAxisType(helper, AT_LOGARITMIC);

	cuPlot2DHelperPlot(helper);
	cuPlot2DHelperDestroy(helper, NULL);
}

///use the logaritmic scale on x
void testPlot2DProducer08(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "e^x", PS_LINES);

	for (double x=0; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoint(helper, x, exp(x));
	}

	cuPlot2DHelperSetTitle(helper, __func__);
	cuPlot2DHelperSetXLabelName(helper, "x");
	cuPlot2DHelperSetYLabelName(helper, "f(x)");
	cuPlot2DHelperSetGrid(helper, true);
	cuPlot2DHelperSetXAxisType(helper, AT_LOGARITMIC);

	cuPlot2DHelperPlot(helper);
	cuPlot2DHelperDestroy(helper, NULL);
}

///use the logaritmic scale on both x and y
void testPlot2DProducer09(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "e^x", PS_LINES);

	for (double x=0; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoint(helper, x, exp(x));
	}

	cuPlot2DHelperSetTitle(helper, __func__);
	cuPlot2DHelperSetXLabelName(helper, "x");
	cuPlot2DHelperSetYLabelName(helper, "f(x)");
	cuPlot2DHelperSetGrid(helper, true);
	cuPlot2DHelperSetXAxisType(helper, AT_LOGARITMIC);
	cuPlot2DHelperSetYAxisType(helper, AT_LOGARITMIC);

	cuPlot2DHelperPlot(helper);
	cuPlot2DHelperDestroy(helper, NULL);
}

///use subtitle
void testPlot2DProducer10(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "e^x", PS_LINES);

	for (double x=0; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoint(helper, x, exp(x));
	}

	cuPlot2DHelperSetTitle(helper, __func__);
	cuPlot2DHelperSetSubtitle(helper, "an interesting function");
	cuPlot2DHelperSetXLabelName(helper, "x");
	cuPlot2DHelperSetYLabelName(helper, "f(x)");
	cuPlot2DHelperSetGrid(helper, true);

	cuPlot2DHelperPlot(helper);
	cuPlot2DHelperDestroy(helper, NULL);
}

void testPlots2DProducer01(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "e^x", PS_LINES);

	for (double x=0; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoint(helper, x, exp(x));
	}

	cuPlot2DHelperSetTitle(helper, __func__);
	cuPlot2DHelperSetSubtitle(helper, "an interesting function");
	cuPlot2DHelperSetXLabelName(helper, "x");
	cuPlot2DHelperSetYLabelName(helper, "f(x)");
	cuPlot2DHelperSetGrid(helper, true);

	cuPlot2DHelperPlotWith(helper, true);
	cuPlot2DHelperDestroy(helper, NULL);
}

void testPlot2DProducer11(CuTest* tc) {
	plot_2d_helper* helper = cuPlot2DHelperNew(__func__);

	cuPlot2DHelperAddSeries(helper, "3*x", PS_LINES);
	cuPlot2DHelperAddSeries(helper, "2*x", PS_LINES);

	for (double x=0; x<+10; x += 0.1) {
		cuPlot2DHelperAddPoints(helper, x, (double[]){3*x, 2*x});
	}

	cuPlot2DHelperSetTitle(helper, __func__);
	cuPlot2DHelperSetSubtitle(helper, "an interesting functions");
	cuPlot2DHelperSetXLabelName(helper, "x");
	cuPlot2DHelperSetYLabelName(helper, "f(x)");
	cuPlot2DHelperSetGrid(helper, true);
	cuPlot2DHelperCreateDataCSV(helper, true);

	cuPlot2DHelperPlotWith(helper, true);
	cuPlot2DHelperDestroy(helper, NULL);

	//check if csv has been created
	char buffer[BUFFER_SIZE];
	sprintf(buffer, "%s.plotdata.csv", __func__);
	FILE* fcsv = fopen(buffer, "r");
	assert(fcsv != NULL);
	fclose(fcsv);

	//test if csv is of the desired length
	struct stat sb;
	if (stat(buffer, &sb) == -1) {
	    perror("stat");
	} else {
		printf("File size: %lld bytes\n", (long long) sb.st_size);
		assert(sb.st_size == 3066);
	}
}

CuSuite* CuPlotProducerSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testPlots2DProducer01);
	SUITE_ADD_TEST(suite, testPlot2DProducer02);
	SUITE_ADD_TEST(suite, testPlot2DProducer03);
	SUITE_ADD_TEST(suite, testPlot2DProducer04);
	SUITE_ADD_TEST(suite, testPlot2DProducer05);
	SUITE_ADD_TEST(suite, testPlot2DProducer06);
	SUITE_ADD_TEST(suite, testPlot2DProducer07);
	SUITE_ADD_TEST(suite, testPlot2DProducer08);
	SUITE_ADD_TEST(suite, testPlot2DProducer09);
	SUITE_ADD_TEST(suite, testPlot2DProducer10);
	SUITE_ADD_TEST(suite, testPlot2DProducer11);

	return suite;
}
