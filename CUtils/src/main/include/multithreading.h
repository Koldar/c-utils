/**
 * @file
 *
 * A wrapper on pthread library to manage more easily the multithreading
 *
 * The module provides you several mutlithreading mechanisms:
 * @li an easy thread wrapper;
 * @li locking mehcanism;
 * @li conditions;
 *
 * <h2>Threads</h2>
 *
 * @code
 * enum thread_loop_state run01(CU_NOTNULL const cu_thread* thread, const struct var_args* va) {
 * 		int* a = cuVarArgsGetItem(va, 0, int*);
 * 		*a = 5;
 * 		critical("inside thread");
 * 		return TLS_STOP;
 * 	}
 *
 * 	void main() {
 * 		int a = 0;
 * 		cuInitVarArgsOnStack(va, &a);
 * 		cu_thread* thread = cuThreadNew(run01, va);
 * 		//start the thread
 * 		cuThreadRequestStart(thread);
 * 		//wait until the thread has terminated its work
 * 		cuThreadWaitForCompletition(thread);
 * 		assert(a == 5);
 * 		cuThreadDestroy(thread);
 * 	}
 * @endcode
 *
 * <h2>Locking</h2>
 *
 * The implementation is based from <a href="https://computing.llnl.gov/tutorials/pthreads/">here</a>
 *
 * <h2>Conditions</h2>
 *
 * Conditions are implemented via ::cu_condition. The implementation is based from <a href="https://computing.llnl.gov/tutorials/pthreads/">here</a>.
 *
 *
 * @author koldar
 * @date May 14, 2018
 */

#ifndef MULTITHREADING_H_
#define MULTITHREADING_H_

#include <stdbool.h>
#include "macros.h"
#include "var_args.h"

typedef struct cu_thread cu_thread;

typedef struct cu_mutex cu_mutex;
typedef struct cu_condition cu_condition;

/**
 * An enumeration for all the possible value a cu_runnable may return
 */
enum thread_loop_state {
	/**
	 * Tell the thread that another loop is required.
	 * This means that the cu_runnable will be called again one more time
	 */
	TLS_CONTINUE,
	/**
	 * Tell the thread that this is the last loop.
	 *
	 * The thread is likely to stop in the near future
	 */
	TLS_STOP
};

/**
 * A type representing a function which can be run via a thread
 */
typedef enum thread_loop_state (*cu_runnable)(CU_NOTNULL const cu_thread* thread, const struct var_args* varargs);

/**
 * Initialize a new thread
 *
 * @note
 * the thread is created, but it is not started
 *
 * @param[in] runnable a function pointer representing what si the callback to call in the thread
 * @param[in] varargs the variadic argument to call @c runnable with. **This variable must be still present whrn the thread is executed**; UB otherwise
 * @return a thread ready to be started
 */
cu_thread* cuThreadNew(CU_NOTNULL cu_runnable runnable, const struct var_args* varargs);

/**
 * Destroy the thread
 *
 * @param[in] thread the thread to destroy
 */
void cuThreadDestroy(CU_NOTNULL const cu_thread* thread, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuThreadDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Requests that a particular thread starts
 *
 * @param[in] thread the thread to start
 */
void cuThreadRequestStart(CU_NOTNULL cu_thread* thread);

/**
 * Request that a particular thread stop
 *
 * A thread is stop whenever it starts a new loop or when the develop thread code checks for the stop request
 *
 * @param[in] thread the thread to stop. Note that you're requesting to stop the thread, but if the code in the thread has not been coded properly
 * 	the thread may never stops
 */
void cuThreadRequestStop(CU_NOTNULL cu_thread* thread);

/**
 * Method used in cu_runnable to check if someone has requested the thread to be stopped
 *
 * @param[in] thread the thread we want to check if needs to be stopped
 * @return
 *  @li true if someone has called ::cuThreadRequestStop on @c thread;
 *  @li false otherwise
 */
bool cuThreadIsStopHasBeenRequest(CU_NOTNULL const cu_thread* thread);

/**
 * check if the thread is yet to be started
 *
 * @param[in] thread the thread to check
 * @return
 * 	@li true if we still needs to call ::cuThreadRequestStart on the thread;
 * 	@li false otherwise
 */
bool cuThreadIsTobeStart(CU_NOTNULL const cu_thread* thread);

/**
 * check if the thread is running
 *
 * @param[in] thread the thread to check
 * @return
 *  @li true if the thread is running;
 *  @li false otherwise
 */
bool cuThreadIsRunning(CU_NOTNULL const cu_thread* thread);

/**
 * check if the thread has stopped running
 *
 * @param[in] thread the thread to check
 * @return
 * 	@li true if the thread has terminated its job;
 * 	@li false otherwise
 */
bool cuThreadIsDead(CU_NOTNULL const cu_thread* thread);

/**
 * the number of times this thread has performed cu_runnable
 *
 * @param[in] thread the thread to check
 * @return times cu_runnable has run. The values starts from 0
 */
int cuThreadGetLoopCounter(CU_NOTNULL const cu_thread* thread);

/**
 * Put on pause the current thread and wait until @c anotherThread has terminated
 *
 * @note
 * this function is a **blocking operation**!
 *
 * @param[in] anotherThread the thread to wait
 */
void cuThreadWaitForCompletition(CU_NOTNULL const cu_thread* anotherThread);

/**
 * Initialize a new mutex, useful for trhead synchronization
 *
 * From <a href="http://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_mutex_lock.html">documentation</a>
 *
 * <h4>Normal Lock</h4>
 * <p>
 *  If the mutex type is PTHREAD_MUTEX_NORMAL, deadlock detection shall not be provided.
 *  Attempting to relock the mutex causes deadlock. If a thread attempts to unlock a mutex that it has not
 *  locked or a mutex which is unlocked, undefined behavior results.
 * <p>
 * <h4>Recursive Lock</h4>
 * <p>
 * If the mutex type is PTHREAD_MUTEX_RECURSIVE, then the mutex shall maintain the concept of a lock count.
 * When a thread successfully acquires a mutex for the first time, the lock count shall be set to one. Every
 * time a thread relocks this mutex, the lock count shall be incremented by one. Each time the thread unlocks the mutex,
 * the lock count shall be decremented by one. When the lock count reaches zero, the mutex shall become available for other
 * threads to acquire. If a thread attempts to unlock a mutex that it has not locked or a mutex which is unlocked, an
 * error shall be returned.
 * </p>
 *
 * @param[in] recursive if set, we will create a recursive lock
 * @return a new mutex;
 */
cu_mutex* cuMutexNew(bool recursive);

/**
 * Locks a mutex
 *
 * The thread will be blocked up until we can lock the mutex, so this is a **blocking operation**.
 *
 *
 * @param[inout] mutex the mutex to lock
 */
void cuMutexLock(CU_NOTNULL cu_mutex* mutex);

/**
 * Unlocks a mutex
 *
 * @param[inout] mutex the mutex to unlock
 */
void cuMutexUnlock(CU_NOTNULL cu_mutex* mutex);

/**
 * Try to lock a mutex. Does nothing otherwise
 *
 * @param[inout] mutex the mutex to lock
 * @return
 * 	@li true if we have locked the mutex;
 * 	@li false otherwise
 */
bool cuMutexTryLock(CU_NOTNULL cu_mutex* mutex);

/**
 * Destroy the given mutex
 *
 * @param[in] mutex the mutex to dispose from memoty
 */
void cuMutexDestroy(CU_NOTNULL const cu_mutex* mutex, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuMutexDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Creates a **thread-safe** block.
 *
 * Use this macro if you want to ensure that only one thread can access the statements per time.
 * For example:
 *
 * @code
 * mutex* m = newMutext();
 * int* a = malloc(sizeof(int));
 *
 * CU_LOCK(m) {
 * 	*a = 5;
 * }
 * @endcode
 *
 * @param[inout] mutexVariable a mutex which will be temporarly lock/unlocked
 */
#define CU_LOCK(mutexVariable) \
	CU_WITH \
	(cuMutexLock(mutexVariable))\
	(cuMutexUnlock(mutexVariable))

/**
 * Creates a new condition for multithreading purposes
 *
 * @return a new condition to use
 */
cu_condition* cuConditionNew();

/**
 * Destroy a initialized condition from memory
 *
 * @param[in] cond the condition to destroy
 */
void cuConditionDestroy(CU_NOTNULL const cu_condition* cond, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuConditionDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Block the current thread until a condition has been set
 *
 * The thread will stay block until someone calls ::cuConditionLockVerifySingleThread function
 *
 * @param[in] cond the condition to wait until it's true
 */
void cuConditionLockUntilVerified(CU_NOTNULL cu_condition* cond);

/**
 * Mark the condition as "set"
 *
 * This will wake up any thread waiting for the condition
 *
 * @param[in] cond mark a condition as "set2
 */
void cuConditionLockVerifySingleThread(CU_NOTNULL cu_condition* cond);

typedef struct cu_parallel_thread_pool cu_parallel_thread_pool;

//TODO doc
cu_parallel_thread_pool* cuNewParallelThreadPool(int totalThreads, CU_NOTNULL cu_runnable runnable, const var_args* va);

//TODO doc
void cuParallelThreadPoolRequestStart(CU_NOTNULL cu_parallel_thread_pool* executor);

//TODO doc
void* cuParallelThreadPoolWaitResult(CU_NOTNULL cu_parallel_thread_pool* executor, long msToWait, void* defaultValue);

//TODO doc
void* cuDestroyParallelThreadPool(CU_NOTNULL cu_parallel_thread_pool* executor);


#endif /* MULTITHREADING_H_ */
