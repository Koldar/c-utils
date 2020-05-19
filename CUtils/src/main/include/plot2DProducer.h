/**
 * @file
 *
 * Represents a module allowing you to create plot graphs.
 *
 * The module is just a easy front end for <b>gnuplot</b> plot utilities.
 * In order to use this module, you need to have <b>gnuplot</b> installed on your system.
 *
 * <h1>Use</h1>
 *
 * The most simple example for the module is the following one:
 *
 * @code
 * 	//initialize
 * 	plot_2d_helper* helper = cuPlot2DHelperNew("output");
 * 	//add some points
 * 	addPointInSeries(helper, 1, 1);
 * 	addPointInSeries(helper, 2, 4);
 * 	addPointInSeries(helper, 3, 9);
 * 	addPointInSeries(helper, 4, 16);
 * 	//actually generate the plot
 * 	cuPlot2DHelperPlot(helper);
 * 	//free the memory
 * 	cuPlot2DHelperDestroy(helper);
 * @endcode
 *
 * @date Jan 16, 2017
 * @author koldar
 */

#ifndef PLOT2DPRODUCER_H_
#define PLOT2DPRODUCER_H_

#include <stdio.h>
#include <stdbool.h>
#include "list.h"
#include "var_args.h"

enum plot_style {
	PS_LINES,
	PS_POINTS,
	PS_LINESPOINTS,
	PS_IMPULSES,
	PS_DOTS,
	PS_STEPS,
	PS_YERRORBARS,
	PS_XERRORBARS,
	PS_XYERRORBARS,
	PS_BOXES,
	PS_BOXERRORBARS,
	PS_BOXXYERRORBARS,
};

/**
 * Determine how we plot an axis
 */
enum axis_type {
	///default one. The axis is perfectly normal
	AT_STANDARD,
	///the axis is using a logaritmic step
	AT_LOGARITMIC
};

typedef struct plot_2d_helper plot_2d_helper;

/**
 * Creates a support structure for printing plots
 *
 * @param[in] output name of output plot you want to have. No extension needed
 */
plot_2d_helper* cuPlot2DHelperNew(CU_NOTNULL const char* output);

/**
 * set the grid for the plot
 *
 * @param[inout] helper the support structure involved
 * @param[in] grid true if you want to display the grid, false otherwise
 */
void cuPlot2DHelperSetGrid(CU_NOTNULL plot_2d_helper* helper, bool grid);

/**
 * Allows you to create a csv containing all the data plotted inside the graph
 *
 * \note
 * This function is useful if you have plots whose data is the output of some sort of heavy computation.
 * Usually this is not the case though.
 *
 * @param[inout] helper the helper involved
 * @param[in] create true if you want to generate a csv file containing the plotted data before the actual plotting;
 */
void cuPlot2DHelperCreateDataCSV(CU_NOTNULL plot_2d_helper* helper, bool create);

/**
 * set label for x axis
 *
 * @param[inout] helper the support sturcture involved
 * @param[in] label the label to set. It is @c fprintf format compliant
 * @param[in] ... actual values for @c label template
 */
void cuPlot2DHelperSetXLabelName(CU_NOTNULL plot_2d_helper* helper, CU_NOTNULL const char* label, ...);
/**
 * set label for y axis
 *
 * @param[inout] helper the support sturcture involved
 * @param[in] label the label to set. It is @c fprintf format compliant
 * @param[in] ... actual values for @c label template
 */
void cuPlot2DHelperSetYLabelName(CU_NOTNULL plot_2d_helper* helper, CU_NOTNULL const char* label, ...);
/**
 * set title for the plot
 *
 * \attention
 * the title is set by \b value
 *
 * @param[inout] helper the support sturcture involved
 * @param[in] title the title of the plot to set. It is @c fprintf compliant
 * @param[in] ... actual values for @c title template
 */
void cuPlot2DHelperSetTitle(CU_NOTNULL plot_2d_helper* helper, CU_NOTNULL const char* title, ...);

/**
 * set the subtitle for the given plot
 *
 * \attention
 * the subtitle is set by \b value
 *
 * @param[inout] helper the support sturcture involved
 * @param[in] subtitle the subtitle of the plot to set. It is @c fprintf compliant
 * @param[in] ... actual values for @c subtitle template
 */
void cuPlot2DHelperSetSubtitle(CU_NOTNULL plot_2d_helper* helper, CU_NOTNULL const char* subtitle, ...);

/**
 * Set whether the plot should be cumulative
 *
 * @param[in] helper the suypport structure involved
 * @param[in] cumulativeX true if we want to plot cumulative plot. False otherwise
 */
void cuPlot2DHelperSetXCumulative(CU_NOTNULL plot_2d_helper* helper, bool cumulativeX);

/**
 * Set the type of the x axis
 *
 * @param[inout] helper the structure to alter
 * @param[in] type the new type
 */
void cuPlot2DHelperSetXAxisType(CU_NOTNULL plot_2d_helper* helper, enum axis_type type);

/**
 * Set the type of the y axis
 *
 * @param[inout] helper the structure to alter
 * @param[in] type the new type
 */
void cuPlot2DHelperSetYAxisType(CU_NOTNULL plot_2d_helper* helper, enum axis_type type);

/**
 * Add a new plotline to draw inside the plot
 *
 * \note
 * This should be called before adding any points in the graph!
 *
 * @param[inout] helper the helper to use
 * @param[in] name the name of the series
 * @param[in] ps how should we render the plot line
 */
void cuPlot2DHelperAddSeries(CU_NOTNULL plot_2d_helper* helper, CU_NOTNULL const char* name, enum plot_style ps);

/**
 * Add a single point inside the the plot
 *
 * This function **heavily** depends on the order you use to call ::cuPlot2DHelperAddSeries ! Assuming the followig:
 * @code
 * 	cuPlot2DHelperAddSeries(helper, "foo", PS_LINES);
 * 	cuPlot2DHelperAddSeries(helper, "bar", PS_LINES);
 *
 * 	cuPlot2DHelperAddPoint(helper, 0, 3); cuPlot2DHelperAddPoint(helper, 0, 2);
 * 	cuPlot2DHelperAddPoint(helper, 0, 6); cuPlot2DHelperAddPoint(helper, 0, 7);
 * @endcode
 *
 * point \f$<0,3>\f$ will be added in "foo" series while \f$<0,2>\f$ to "bar". Then \f$<0,6>\f$ will be the second point belonging to "foo" and
 * \f$<0,7>\f$ will set to "bar".
 *
 * @param[inout] helper the helper to use
 * @param[in] x the x of the point
 * @param[in] y the y of the point
 */
void cuPlot2DHelperAddPoint(CU_NOTNULL plot_2d_helper* helper, double x, double y);

/**
 * Like ::cuPlot2DHelperAddPoint but you assign in one sweep all the series values for a particular "x" value. Assuming
 * you have 6 series \c ys will be an array of 6 elements, one per plot.
 *
 * Doing:
 * @code
 * cuPlot2DHelperAddPoints(helper, 0, (double[]){1,2,3,4});
 * @encode
 * It's identical as doing:
 * @code
 * cuPlot2DHelperAddPoint(helper, 0, 1);
 * cuPlot2DHelperAddPoint(helper, 0, 2);
 * cuPlot2DHelperAddPoint(helper, 0, 3);
 * cuPlot2DHelperAddPoint(helper, 0, 4);
 * @endcode
 *
 * @param[inout] helper the helper to use
 * @param[in] x the x value;
 * @param[in] ys an array of values containing as many doubles as there are series in the \c helper
 */
void cuPlot2DHelperAddPoints(CU_NOTNULL plot_2d_helper* helper, double x, CU_NOTNULL double* ys);

/**
 * Generates the image of the plot
 *
 * \attention
 * After calling this function, it won't be possible to add points anymore. Doing so will create
 * <b>undefined behaviours</b>
 *
 * @param[inout] helper the helper structure involved
 */
void cuPlot2DHelperPlot(CU_NOTNULL plot_2d_helper* helper);

/**
 * Generates the image of the plot
 *
 * \attention
 * After calling this function, it won't be possible to add points anymore. Doing so will create
 * <b>undefined behaviours</b>
 *
 * @param[inout] helper the helper structure involved
 * @param[in] drawAlsoLogarithmScale if true, the function will generate 2 plots: the seocnd one will have the y-scale with log-10 step
 */
void cuPlot2DHelperPlotWith(CU_NOTNULL plot_2d_helper* helper, bool drawAlsoLogarithmScale);
/**
 * destroy the structure created by ::cuPlot2DHelperNew
 *
 * \attention
 * The function won't remove the *.m nor *.png created by ::cuPlot2DHelperNew
 *
 * @param[in] plotHelper the structure to free from the memory
 */
void cuPlot2DHelperDestroy(CU_NOTNULL const plot_2d_helper* plotHelper, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuPlot2DHelperDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID



#endif /* PLOT2DPRODUCER_H_ */
