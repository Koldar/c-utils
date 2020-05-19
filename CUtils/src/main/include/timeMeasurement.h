/**
 * @file
 *
 * Perform time probings of C code.
 *
 * Typical use if the following:
 *
 * ```
 * CU_PROFILE_TIME_CODE(timing, TM_MILLI) {
 * 	sleep(5);
 * }
 * //it should output something like 5000
 * printf("timing is %l\n", timing);
 * ```
 *
 * @date Jan 19, 2017
 * @author koldar
 */

#ifndef TIMEMEASUREMENT_H_
#define TIMEMEASUREMENT_H_

#include <time.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include "macros.h"

/**
 * An enuemration representing a time unit
 */
enum time_unit_measurement {
	TM_NANO = 0,
	TM_MICRO = 1,
	TM_MILLI = 2,
	TM_SECONDS = 3
};

/**
 * Perform a timing of a piece of code
 *
 * You can use this macro like this:
 *
 * ```
 * CU_PROFILE_TIME_CODE(delta, TM_NANO) {
 * 		printf("Hello world!\n");
 * }
 * printf("time required to compute printf in nanoseconds=%ld\n", delta);
 * ```
 * @post
 * This will create a new variabile of type <tt>long</tt> called @c timegap
 *
 * @param[in] timegap the name of the variable that after the code has been execute will contain the number of seconds/milliseconds/microseconds/nanoseconds
 * 		tha tthe code took to be executed. The variable last after the macro call; You don't need to create it
 * @param[in] unit_measure a value of type ::time_unit_measurement. this will be the unit measure of \c timegap
 */
#define CU_PROFILE_TIME_CODE(timegap, unit_measure) \
	long timegap; \
	enum time_unit_measurement UV(um) = unit_measure; \
	struct timespec UV(start); \
	struct timespec UV(end); \
	UV(start) = cuTimeMeasurementGetCurrentTime(); \
	for ( \
			bool UV(condition)=true \
			; \
			UV(condition) \
			; \
			UV(end) = cuTimeMeasurementGetCurrentTime(),\
			UV(condition)=false, \
			timegap = cuTimeMeasurementComputeTimeGap(UV(start), UV(end), UV(um))\
	)

/**
 * get a string representastion of a time unit
 *
 * @param[in] tm the measurement involved
 * @return a string representation of \c tm
 */
const char* cuTimeMeasurementgetConstantString(enum time_unit_measurement tm);

/**
 * parse a string representation of a time unit
 *
 * @param[in] s a string representing a time measurement. Allowed values are "ns", "us", "ms" or "s"
 * @return the time measurement from a given string
 */
enum time_unit_measurement cuTimeMeasurementParseFromString(CU_NOTNULL const char* s);

/**
 * Retrieve a structure containing the current time
 *
 * @return the current time
 */
struct timespec cuTimeMeasurementGetCurrentTime();

/**
 * compute the time gap (using the unit @c format) between 2 instants
 *
 * @param[in] start the first instant
 * @param[in] end the second instant
 * @param[in] format the time unit we need to use to compute the result
 * @return <tt>end - start</tt>
 */
long cuTimeMeasurementComputeTimeGap(struct timespec start, struct timespec end, enum time_unit_measurement format);

/**
 * Fetch the current date
 *
 * Use this function if you want to approximately get todays date and other information. You can then use functions like @c strftime
 * to easily convert the date into string (see <a href="http://www.cplusplus.com/reference/ctime/strftime/">here</a> for the documentation of @c strftime):
 *
 * @code
 * #define BUFFER_SIZE 80
 * struct tm today = cuTimeMeasurementNaiveCurrentDate();
 * char buffer[BUFFER_SIZE];
 * strftime (buffer, BUFFER_SIZE, "Now it's %I:%M%p.", &today);
 * @endcode
 *
 * @return a structure containing timinig information
 */
struct tm cuTimeMeasurementNaiveCurrentDate();

#endif /* TIMEMEASUREMENT_H_ */
