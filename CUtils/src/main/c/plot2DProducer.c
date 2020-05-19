/*
 * plot2DProducer.c
 *
 *  Created on: Jan 16, 2017
 *      Author: koldar
 */

#include "plot2DProducer.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include "utility.h"
#include "log.h"
#include "list.h"
#include "macros.h"
#include "dynamic_array.h"
#include "errors.h"
#include "string_utils.h"

typedef struct Series {
	const char* name;
	enum plot_style style;
} Series;

typedef list SeriesList;

/**
 * The main structure used to store support data for this module
 */
struct plot_2d_helper {
	///the name where the output will be created. No extension
	const char* fileNameTemplate;
	///a file containing the data gnuplot will use
	FILE* datFile;

	///a list containing all the series of the graph
	SeriesList* series;
	/**
	 * list of points previously added by ::cuPlot2DHelperAddPoint
	 *
	 * If you're usig multiple series but you want to add a single series per time,
	 * the API needs to memorize somewhere the point previously added. This is the memory storage.
	 *
	 * @code
	 * cuPlot2DHelperAddSeries(helper, "sin", PS_LINES);
	 * cuPlot2DHelperAddSeries(helper, "cos", PS_LINES);
	 * cuPlot2DHelperAddSeries(helper, "tan", PS_LINES);
	 *
	 * cuPlot2DHelperAddPoint(helper, -1, sin(-1));
	 * cuPlot2DHelperAddPoint(helper, -1, cos(-1));
	 * cuPlot2DHelperAddPoint(helper, -1, tan(-1));
	 * cuPlot2DHelperAddPoint(helper, 0, sin(0));
	 * cuPlot2DHelperAddPoint(helper, 0, cos(0));
	 * cuPlot2DHelperAddPoint(helper, 0, tan(0));
	 * cuPlot2DHelperAddPoint(helper, +1, sin(+1));
	 * cuPlot2DHelperAddPoint(helper, +1, cos(+1));
	 * cuPlot2DHelperAddPoint(helper, +1, tan(+1));
	 *
	 * cuPlot2DHelperPlot(helper);
	 * @endcode
	 *
	 * \note
	 * This field will be initialized only if the series number is strictly greater than 1: if not, allocate new space would be a totally useless
	 * operation
	 */
	double* temporaryValues;
	/**
	 * the series we're analylzing with ::cuPlot2DHelperAddPoint
	 *
	 * If ::plot_2d_helper::temporaryValues is  used, we also need to store the series the user is adding points into.
	 * In the first call of ::cuPlot2DHelperAddPoint the first series is considered, then in the second series we need to use the second one and so on.
	 * The field contain such information.
	 */
	int currentSeries;

	/**
	 * Determine which character should be treated as comment line inside the input file
	 */
	const char* commentCharacters;
	/**
	 * Determine whether or not we should print a grid on the generated image
	 */
	bool enableGrid;
	/**
	 * Determine how we plot the x axis
	 */
	enum axis_type xAxisType;
	/**
	 * Determine how we plot the y axis
	 */
	enum axis_type yAxisType;
	/**
	 * Determine if we want to have a cumulative x axis (cumulative sum on the y values)
	 */
	bool enableCumulativeXAxis;
	/**
	 * The X label
	 */
	const char* xLabel;
	/**
	 * the Y label
	 */
	const char* yLabel;
	/**
	 * the title of the plot
	 */
	const char* title;
	/**
	 * The (optional) subtitle of the graph
	 *
	 * this field can be NULL
	 */
	const char* subtitle;
	/**
	 * Determine how the subtitle will be rendered
	 *
	 * True if you want to write the subtitle with a smaller font; False if you want to use the same
	 * font of the title but you want to wrap subtitle inside parenthesis
	 */
	bool reduceSubtitleFont;
	/**
	 * If you when we create the actual png plot we will create a CSV containing all the data plotted.
	 */
	bool createDataCSVFile;
};

static const char* plotStyleToString[] = {
		"lines",
		"points",
		"linespoints",
		"impulses",
		"dots",
		"steps",
		"yerrorbars",
		"xerrorbars",
		"xyerrorbars",
		"boxes",
		"boxerrorbars",
		"boxxyerrorbars"
};


static void generateGnuPlotScript(plot_2d_helper* helper, const char* suffixName);
static Series* newSeries(const char* name, enum plot_style ps);
static void destroySeries(const Series* _s, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_destroySeries_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID
static const char* getStringOfPlotStyle(const enum plot_style ps);
static double* generateTemporaryValues(const int seriesNumber);
static void processAxisType(FILE* f, enum axis_type at, char axis);
static void processTitleAndSubtitle(FILE* f, const plot_2d_helper* helper);
static void generateDataCSV(const plot_2d_helper* helper, const char* format, ...);
static payload_functions payloadFunctionsOfSeries();

plot_2d_helper* cuPlot2DHelperNew(const char* output)  {
	plot_2d_helper* result = CU_MALLOC(plot_2d_helper);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	char datFileName[BUFFER_SIZE];
	result->fileNameTemplate = strdup(output);

	sprintf(datFileName, "%s.dat", result->fileNameTemplate);
	result->datFile = fopen(datFileName, "w");
	if (result->datFile == NULL) {
		ERROR_FILE(datFileName);
	}

	result->series = cuListNew(payloadFunctionsOfSeries());
	result->temporaryValues = NULL;
	result->currentSeries = 0;

	result->enableGrid = false;
	result->commentCharacters = NULL;
	result->xLabel = NULL;
	result->yLabel = NULL;
	result->xAxisType = AT_STANDARD;
	result->yAxisType = AT_STANDARD;
	result->enableCumulativeXAxis = false;
	result->title = NULL;
	result->subtitle = NULL;
	result->reduceSubtitleFont = false;
	result->createDataCSVFile = false;

	return result;
}

void cuPlot2DHelperAddPoint(plot_2d_helper* helper, double x, double y) {
	if (cuListGetSize(helper->series) == 1) {
		return cuPlot2DHelperAddPoints(helper, x, (double[]){y});
	}

	if ((cuListGetSize(helper->series) > 1) && (helper->temporaryValues == NULL)) {
		helper->temporaryValues = generateTemporaryValues(cuListGetSize(helper->series));
	}

	helper->temporaryValues[helper->currentSeries] = y;
	helper->currentSeries++;
	if (helper->currentSeries == cuListGetSize(helper->series)) {
		cuPlot2DHelperAddPoints(helper, x, helper->temporaryValues);
		helper->currentSeries = 0;
	}
}

void cuPlot2DHelperAddPoints(plot_2d_helper* helper, double x, double* ys) {
	fprintf(helper->datFile, "%.3f ", x);

	for (int i=0; i<cuListGetSize(helper->series); i++) {
		fprintf(helper->datFile, "%.3f", ys[i]);
		if ((i+1) < cuListGetSize(helper->series)) {
			fprintf(helper->datFile, " ");
		}
	}
	fprintf(helper->datFile, "\n");
}

void cuPlot2DHelperSetXAxisType(plot_2d_helper* helper, enum axis_type type) {
	helper->xAxisType = type;
}

void cuPlot2DHelperSetYAxisType(plot_2d_helper* helper, enum axis_type type) {
	helper->yAxisType = type;
}

void cuPlot2DHelperAddSeries(plot_2d_helper* helper, const char* name, enum plot_style ps) {
	Series* s = newSeries(name, ps);

	cuListAddTail(helper->series, s);
}

void cuPlot2DHelperPlot(plot_2d_helper* helper) {
	cuPlot2DHelperPlotWith(helper, false);
}

void cuPlot2DHelperPlotWith(plot_2d_helper* helper, bool drawAlsoLogarithmScale) {
	fclose(helper->datFile);

	//create the csv file
	if (helper->createDataCSVFile) {
		generateDataCSV(helper, helper->fileNameTemplate);
	}

	generateGnuPlotScript(helper, "");

	char buffer[BUFFER_SIZE];
	sprintf(buffer, "gnuplot %s.gp", helper->fileNameTemplate);
	int normalExitCode = system(buffer);
	//remove gnuplot script
	sprintf(buffer, "%s.gp", helper->fileNameTemplate);
	remove(buffer);

	if (drawAlsoLogarithmScale) {
		cuPlot2DHelperSetYAxisType(helper, AT_LOGARITMIC);
		generateGnuPlotScript(helper, "_log10");

		sprintf(buffer, "gnuplot %s.gp", helper->fileNameTemplate);
		int logExitCode = system(buffer);
		sprintf(buffer, "%s.gp", helper->fileNameTemplate);
		remove(buffer);
	}
	//remove dat file
	sprintf(buffer, "%s.dat", helper->fileNameTemplate);
	remove(buffer);
}

void cuPlot2DHelperSetGrid(plot_2d_helper* helper, bool grid) {
	helper->enableGrid = grid;
}

void cuPlot2DHelperCreateDataCSV(plot_2d_helper* helper, bool create) {
	helper->createDataCSVFile = create;
}

void setCommentStartCharacters(plot_2d_helper* helper, const char* strings) {
	helper->commentCharacters = strings;
}

void cuPlot2DHelperSetXLabelName(plot_2d_helper* helper, const char* label, ...) {
	if (helper->xLabel != NULL) {
		CU_FREE(helper->xLabel);
	}
	char buffer[BUFFER_SIZE];
	va_list va;
	va_start(va, label);
	vsnprintf(buffer, BUFFER_SIZE, label, va);
	va_end(va);

	helper->xLabel = strdup(buffer);
}

void cuPlot2DHelperSetYLabelName(plot_2d_helper* helper, const char* label, ...) {
	if (helper->yLabel != NULL) {
		CU_FREE(helper->yLabel);
	}
	char buffer[BUFFER_SIZE];
	va_list va;
	va_start(va, label);
	vsnprintf(buffer, BUFFER_SIZE, label, va);
	va_end(va);

	helper->yLabel = strdup(buffer);
}

void cuPlot2DHelperSetTitle(plot_2d_helper* helper, const char* title, ...) {
	if (helper->title != NULL) {
		CU_FREE(helper->title);
	}
	char buffer[BUFFER_SIZE];
	va_list va;
	va_start(va, title);
	vsnprintf(buffer, BUFFER_SIZE, title, va);
	va_end(va);

	helper->title = strdup(buffer);
}

void cuPlot2DHelperSetSubtitle(plot_2d_helper* helper, const char* subtitle, ...) {
	if (helper->subtitle != NULL) {
		CU_FREE(helper->subtitle);
	}
	char buffer[BUFFER_SIZE];
	va_list va;
	va_start(va, subtitle);
	vsnprintf(buffer, BUFFER_SIZE, subtitle, va);
	va_end(va);

	helper->subtitle = strdup(buffer);
}

void cuPlot2DHelperSetXCumulative(CU_NOTNULL plot_2d_helper* helper, bool cumulativeX) {
	helper->enableCumulativeXAxis = cumulativeX;
}

void cuPlot2DHelperDestroy(const plot_2d_helper* plotHelper, CU_NULLABLE const struct var_args* context) {
	cuListDestroyWithElement(plotHelper->series, context);
	if (plotHelper->temporaryValues != NULL) {
		free(plotHelper->temporaryValues);
	}
	if (plotHelper->title != NULL) {
		CU_FREE(plotHelper->title);
	}
	if (plotHelper->subtitle != NULL) {
		CU_FREE(plotHelper->subtitle);
	}
	if (plotHelper->xLabel != NULL) {
		CU_FREE(plotHelper->xLabel);
	}
	if (plotHelper->yLabel != NULL) {
		CU_FREE(plotHelper->yLabel);
	}
	CU_FREE(plotHelper->fileNameTemplate);
	CU_FREE(plotHelper);
}

static payload_functions payloadFunctionsOfSeries() {
	payload_functions result;

	result.destroy = CU_AS_DESTRUCTOR(destroySeries);

	return result;
}

static Series* newSeries(const char* name, enum plot_style ps) {
	Series* retVal = malloc(sizeof(Series));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->name = strdup(name);
	retVal->style = ps;

	return retVal;
}

static void destroySeries(const Series* s, CU_NULLABLE const struct var_args* context) {
	CU_FREE(s->name);
	CU_FREE(s);
}

static const char* getStringOfPlotStyle(enum plot_style ps) {
	return plotStyleToString[ps];
}

/**
 * Generates a perfectly empty array containing temporary values
 *
 * You need to perform this operation as soon as you know the exact number of series in the graph
 *
 * @param[in] seriesNumber the number of series in the graph
 * @return a pointer to the first cell of the array containing temporary numbers
 */
static double* generateTemporaryValues(const int seriesNumber) {
	double* retVal = calloc(seriesNumber, sizeof(double));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}
	return retVal;
}

static void processAxisType(FILE* f, enum axis_type at, char axis) {
	switch (at) {
	case AT_STANDARD : {
		fprintf(f,"unset logscale %c\n", axis);
		break;
	}
	case AT_LOGARITMIC: {
		fprintf(f, "set logscale %c 10\n", axis);
		//see format specifier of gnu plot manual
		fprintf(f, "set format %c \"%%.1lE%%+3L\"\n", axis);
		break;
	}
	default: {
		ERROR_UNHANDLED_CASE("axis type", at);
	}
	}
}

/**
 * Print on \c f the title and the subtitle of the graph
 *
 * \note
 * The implementation is based upon the following <a href="http://stackoverflow.com/a/13645897/1887602">link</a>
 *
 * The function does \b nothing if ::plot_2d_helper::title is NULL
 *
 * @param[inout] f the file where we need to write on
 * @param[in] helper the data structure with the title/subtitle information
 */
static void processTitleAndSubtitle(FILE* f, const plot_2d_helper* helper) {

	//set title "Big Title\n{/*0.5 subtitle}"
	if (helper->title != NULL) {
		fprintf(f, "set title \"%s", helper->title);
		if (helper->subtitle != NULL) {
			if (helper->reduceSubtitleFont) {
				fprintf(f, "\\n{/*0.8 %s}", helper->subtitle);
			} else {
				fprintf(f, "\\n(%s)", helper->subtitle);
			}

		}
		fprintf(f, "\"\n");
	}
}

/**
 * Generates a CSV containing all the data inside the DAT file.
 * It's basically a conversion between dat file and csv file.
 *
 * @param[in] helper the helper to use
 * @param[in] format the string template to use as csv file name. Such template will then be appended with ".plotdata.csv" string
 * @param[in] ... the variadic arguments for \c format string
 */
static void generateDataCSV(const plot_2d_helper* helper, const char* format, ...) {
	char templateString[BUFFER_SIZE];
	char csvFilename[BUFFER_SIZE];
	va_list va;
	va_start(va, format);
	vsnprintf(templateString, BUFFER_SIZE, format, va);
	va_end(va);

	cuSnprintf(csvFilename, BUFFER_SIZE, "%s.plotdata.csv", templateString);

	//we don't use csvhelper because we don't know how many columns we have. Or at least, the API of CsvHelper are not so easily flexible in this scenario!
	char datFileName[BUFFER_SIZE];
	cuSnprintf(datFileName, BUFFER_SIZE, "%s.dat", helper->fileNameTemplate);
	FILE* fin = fopen(datFileName, "r");
	if (fin == NULL) {
		ERROR_FILE(datFileName);
	}
	FILE* fout = fopen(csvFilename, "w");
	if (fout == NULL) {
		ERROR_FILE(csvFilename);
	}

	dynamic_1D_array* temporaryCumulativeValues = NULL;
	if (helper->enableCumulativeXAxis) {
		temporaryCumulativeValues = cuDynamicArrayNew(float, cuListGetSize(helper->series));
		cuDynamicArrayClearTo(temporaryCumulativeValues, float, 0.0);
	}

	// *************** WRITE CSV HEADER ****************
	fprintf(fout, "sep=,\n");
	fprintf(fout, "X, ");
	int i = 0;
	CU_ITERATE_OVER_LIST(helper->series, cell, s, Series*) {
		fprintf(fout, "%s", s->name);
		fprintf(fout, "%s", ((i+1) == cuListGetSize(helper->series)) ? "\n" : ", ");
		i++;
	}

	// *************** WRITE CSV DATA ******************
	double value = -1;
	int elementsRead = 0;
	while(true) {
		//read x
		elementsRead = fscanf(fin, "%lf%*[ \n\t]", &value);
		if (elementsRead == EOF) {
			//reached the end of the input
			break;
		}
		if (elementsRead != 1) {
			ERROR_IMPOSSIBLE_SCENARIO("file dat is not valid! Read %d elements", elementsRead);
		}
		fprintf(fout, "%f, ", value);

		for (int series_id=0;  series_id<cuListGetSize(helper->series); series_id++) {
			//read y value
			if ((elementsRead = fscanf(fin, "%lf%*[ \n\t]", &value)) != 1) {
				ERROR_IMPOSSIBLE_SCENARIO("file dat is not valid! Read %d elements", elementsRead);
			}
			if (helper->enableCumulativeXAxis) {
				float oldValue = cuDynamicArrayGetCellValue(temporaryCumulativeValues, series_id, float);
				float newValue = oldValue + value;
				cuDynamicArraySetCellTo(temporaryCumulativeValues, series_id, float, newValue);
				fprintf(fout, "%f", newValue);
			} else {
				fprintf(fout, "%f", value);
			}
			fprintf(fout, "%s", ((series_id+1) == cuListGetSize(helper->series)) ? "\n" : ", ");
		}
	}

	if (helper->enableCumulativeXAxis) {
		cuDynamicArrayDestroy(temporaryCumulativeValues, NULL); //TODO context null
	}

	fclose(fout);
}

/**
 * <a href="http://www.math.utk.edu/~vasili/refs/How-to/gnuplot.print.html">how to save gnuplot image</a>
 *
 * @param[in] helper the structure containing all information about the plotting
 * @param[in] a string appended at the end of output image
 */
static void generateGnuPlotScript(plot_2d_helper* helper, const char* suffixName) {
	char buffer[BUFFER_SIZE];
	sprintf(buffer,"%s.gp", helper->fileNameTemplate);
	FILE* gnuPlotFile = fopen(buffer, "w");

	fprintf(gnuPlotFile, "reset\n");

	if (helper->commentCharacters != NULL) {
		fprintf(gnuPlotFile, "set datafile commentschars \"%s\"\n", helper->commentCharacters);
	}

	fprintf(gnuPlotFile, "set term png\n");
	fprintf(gnuPlotFile, "set output \"%s%s.png\"\n", helper->fileNameTemplate, suffixName);

	processTitleAndSubtitle(gnuPlotFile, helper);

	if (helper->xLabel != NULL) {
		fprintf(gnuPlotFile, "set xlabel \"%s\"\n", helper->xLabel);
	}

	if (helper->yLabel != NULL) {
		fprintf(gnuPlotFile, "set ylabel \"%s\"\n", helper->yLabel);
	}

	processAxisType(gnuPlotFile, helper->xAxisType, 'x');
	fprintf(gnuPlotFile, "set xtic rotate\n");
	processAxisType(gnuPlotFile, helper->yAxisType, 'y');

	if (helper->enableGrid) {
		fprintf(gnuPlotFile, "set grid\n");
	}

	fprintf(gnuPlotFile, "plot ");

	int i = 0;
	CU_ITERATE_OVER_LIST(helper->series, cell, s, Series*) {
		fprintf(gnuPlotFile, "\"%s.dat\" using 1:%d title \"%s\" with %s", helper->fileNameTemplate, (i+2), s->name, getStringOfPlotStyle(s->style));
		if ((i+1) < cuListGetSize(helper->series)) {
			fprintf(gnuPlotFile, ", ");
		} else {
			fprintf(gnuPlotFile, "\n");
		}
		i++;
	}

	fclose(gnuPlotFile);
}
