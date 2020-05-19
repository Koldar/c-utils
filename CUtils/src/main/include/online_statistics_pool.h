/**
 * @file
 *
 * Represents a collection of online_statistics, nicely packed together
 *
 * This is useful because often you don't want to have just one statistic, but a batch of them. For example,
 * when testing the timings of an algorithm, it's likely you ant to test not only the total amount of time it needs, but also
 * the time of each of its subroutines, for profiling purposes.
 *
 * The APIs of this module closely relates to the ones offered for online_statistics.
 *
 * Note that, OSP acronym stands for **Online Statistics Pool**. Such acronym is used, for brevity in al lthe APIs o this module aside the
 * init and the disposer, just for clarity.
 *
 * @date Jan 4, 2018
 * @author koldar
 */

#ifndef ONLINE_STATISTICS_POOL_H_
#define ONLINE_STATISTICS_POOL_H_

#include <stdbool.h>
#include "online_statistics.h"
#include "macros.h"
#include "var_args.h"

typedef struct online_statistics_pool online_statistics_pool;

/**
 * Initialize a new online statistics pool
 *
 * @includedoc online_statistics_pool.dox
 *
 * @param[in] enable true if you want to enable statistics, false if you do not want them
 * @return
 *  @li the memory contains a new online statistic pool;
 *  @li NULL if the OSP has not been enabled at all
 */
CU_NULLABLE online_statistics_pool* cuOSPNew(bool enable);

/**
 * Remove from memory an online statistic pool
 *
 * @includedoc online_statistics_pool.dox
 *
 * @note
 * everything (::online_statistics as well) attached to it will be removed from memory as well
 *
 * @param[in] pool the pool to remove
 */
void cuOSPDestroy(CU_NULLABLE const online_statistics_pool* pool, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuOSPDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Add a new value in a statistic
 *
 * @includedoc online_statistics_pool.dox
 *
 * @param[inout] stat the pool to manage
 * @param[in] name the name of the statistic whose average we need to fetch
 * @param[in] newValue the new data to insert in the statistic
 */
void cuOSPUpdateItem(CU_NULLABLE online_statistics_pool* stat, CU_NOTNULL const char* name, double newValue);

/**
 * Inject a "1" inside the given statistic.
 *
 * This function is the same as  <code>cuOSPUpdateItem(stat, name, 1);</code>
 *
 * @includedoc online_statistics_pool.dox
 *
 * @param[inout] stat the pool to manage
 * @param[in] name the name of the statistic whose average we need to fetch
 */
void cuOSPAddOne(CU_NULLABLE online_statistics_pool* stat, CU_NOTNULL const char* name);

/**
 * get the average of a statistics
 *
 * @includedoc online_statistics_pool.dox
 *
 * @note
 * if no data is present in the statistic, an error will be thrown
 *
 * @param[in] stat the structure to analyze
 * @param[in] name the name of the statistic whose average we need to fetch
 * @return
 * the average of all the values put thanks to ::cuOnlineStatisticsUpdate into this structure
 */
double cuOSPGetAverage(CU_NULLABLE const online_statistics_pool* stat, CU_NOTNULL const char* name);

/**
 * get the variance of a statistics
 *
 * @includedoc online_statistics_pool.dox
 *
 * @note
 * if no data is present in the statistic, an error will be thrown
 *
 * @param[in] stat the structure to analyze
 * @param[in] name the name of the statistic whose variance we need to fetch
 * @return
 * the variance of all the values put thanks to ::cuOnlineStatisticsUpdate into this structure
 */
double cuOSPGetVariance(CU_NULLABLE const online_statistics_pool* stat, CU_NOTNULL const char* name);

/**
 * get the standard deviation of a statistic
 *
 * @includedoc online_statistics_pool.dox
 *
 * @note
 * if no data is present in the statistic, an error will be thrown
 *
 * @param[in] stat the structure to analyze
 * @param[in] name the name of the statistic whose standard deviation we need to fetch
 * @return
 * the standard deviation of all the values put thanks to ::cuOnlineStatisticsUpdate into this structure
 */
double cuOSPGetStandardDeviation(CU_NULLABLE const online_statistics_pool* stat, CU_NOTNULL const char* name);

/**
 * get the minimum value of a statistic
 *
 * @includedoc online_statistics_pool.dox
 *
 * @note
 * if no data is present in the statistic, an error will be thrown
 *
 * @param[in] stat the structure to analyze
 * @param[in] name the name of the statistic whose minimum we need to fetch
 * @return
 * the minimum of all the values put thanks to ::cuOnlineStatisticsUpdate into this structure
 */
double cuOSPGetMin(CU_NULLABLE const online_statistics_pool* stat, CU_NOTNULL const char* name);

/**
 * get the maximum of a statistics
 *
 * @includedoc online_statistics_pool.dox
 *
 * @note
 * if no data is present in the statistic, an error will be thrown
 *
 * @param[in] stat the structure to analyze
 * @param[in] name the name of the statistic whose maximum we need to fetch
 * @return
 * the maximum of all the values put thanks to ::cuOnlineStatisticsUpdate into this structure
 */
double cuOSPGetMax(CU_NULLABLE const online_statistics_pool* stat, CU_NOTNULL const char* name);

/**
 * like ::cuOnlineStatisticsGetMax but return a default value if the online_statistics is empty
 *
 * @includedoc online_statistics_pool.dox
 *
 * @param[in] stat the statistics to poll
 * @param[in] name the name of the statistic whose maximum we need to fetch
 * @param[in] defaultValue the value to return if \c stat is empty
 * @return the desired value
 */
double cuOSPGetMaxOrDefault(CU_NULLABLE const online_statistics_pool* stat, CU_NOTNULL const char* name, double defaultValue);

/**
 * get the number of data a statistic has detected
 *
 * @includedoc online_statistics_pool.dox
 *
 * @param[in] stat the structure to analyze
 * @param[in] name the name of the statistic whose number size we need to fetch
 * @return
 * the number of values put thanks to ::cuOnlineStatisticsUpdate into this structure
 */
long cuOSPGetNumber(CU_NULLABLE const online_statistics_pool* stat, CU_NOTNULL const char* name);

/**
 * get the last value a statistic has detected
 *
 * @includedoc online_statistics_pool.dox
 *
 * @note
 * if no data is present in the statistic, an error will be thrown
 *
 * @param[in] stat the structure to analyze
 * @param[in] name the name of the statistic whose last item we need to fetch
 * @return
 * the last value you have put with ::cuOnlineStatisticsUpdate into this structure.
 */
double cuOSPGetLastValue(CU_NULLABLE const online_statistics_pool* stat, CU_NOTNULL const char* name);

/**
 * like ::cuOnlineStatisticsGetLastValue but return a default value if the online_statistics is empty
 *
 * @includedoc online_statistics_pool.dox
 *
 * @param[in] stat the statistics to poll
 * @param[in] name the name of the statistic whose last value we need to fetch
 * @param[in] defaultValue the value to return if \c stat is empty
 * @return the desired value
 */
double cuOSPGetLastValueOrDefault(CU_NULLABLE const online_statistics_pool* stat, CU_NOTNULL const char* name, double defaultValue);

/**
 * Reset the statistics as if no data ever arrived
 *
 * @includedoc online_statistics_pool.dox
 *
 * \post
 * 	\li after this operation ::cuOnlineStatisticsGetN will return 0
 *
 * @param[inout] stat the pool to update
 * @param[in] name the name of the statistic to clear
 */
void cuOSPClear(CU_NULLABLE online_statistics_pool* stat, CU_NOTNULL const char* name);

/**
 * Reset every statistics in the pool as if no data were ever retrieved
 *
 * @includedoc online_statistics_pool.dox
 *
 * @param[inout] stat the pool to manage
 */
void cuOSPClearPool(CU_NULLABLE online_statistics_pool* stat);

/**
 * check if a statistic has no data
 *
 * @includedoc online_statistics_pool.dox
 *
 * @param[inout] stat the statistics to poll
 * @param[in] name the name of the statistic whose emptiness we need to fetch
 * @return
 * 	\li true if we didn't add even 1 data inside this statistics;
 * 	\li false otherwise
 */
bool cuOSPIsEmpty(CU_NULLABLE const online_statistics_pool* stat, CU_NOTNULL const char* name);

/**
 * Check if the pool has no statistics
 *
 * @includedoc online_statistics_pool.dox
 *
 * @param[in] pool the pool to check
 * @return
 * 	\li \c true if the pool has no statistics;
 * 	\li \c false otherwise
 */
bool cuOSPIsPoolEmpty(CU_NULLABLE const online_statistics_pool* pool);

/**
 * Count the number of statistics withint this poool
 *
 * @includedoc online_statistics_pool.dox
 *
 * @param[in] pool the pool to manage
 * @return the number of statistics within the pool
 */
int cuOSPGetPoolSize(CU_NULLABLE const online_statistics_pool* pool);

#endif /* ONLINE_STATISTICS_POOL_H_ */
