/**
 * @file
 *
 * @author koldar
 * @date May 14, 2018
 */

#include "multithreading.h"
#include <pthread.h>
#include <stdlib.h>
#include "log.h"
#include "dynamic_array.h"
#include "errors.h"

enum thread_state {
	TS_TOSTART,
	TS_RUNNING,
	TS_STOPPING,
	TS_DEAD
};

struct cu_thread {
	pthread_t thread;
	pthread_attr_t attrs;

	cu_runnable runnable;
	bool shouldStop;
	int loopNumber;
	const struct var_args* arguments;
	enum thread_state state;
};

struct cu_mutex {
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;
};

static void* cuAbstractRunnable(void* arguments);
static void setThreadState(CU_NOTNULL cu_thread* thread, enum thread_state state);
static cu_mutex cuSetupMutex(bool recursive);

cu_thread* cuThreadNew(CU_NOTNULL cu_runnable runnable, const struct var_args* varargs) {
	cu_thread* result = CU_MALLOC(struct cu_thread);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	result->runnable = runnable;
	result->shouldStop = false;
	result->arguments = varargs;
	result->state = TS_TOSTART;
	result->loopNumber = 0;

	return result;
}

void cuThreadDestroy(CU_NOTNULL const cu_thread* thread, CU_NULLABLE const struct var_args* context) {
	pthread_attr_destroy((pthread_attr_t*)&thread->attrs);
	CU_FREE(thread);
}

void cuThreadRequestStart(CU_NOTNULL cu_thread* thread) {
	//set the current thread as joinable
	pthread_attr_init(&thread->attrs);
	pthread_attr_setdetachstate(&thread->attrs, PTHREAD_CREATE_JOINABLE);

	pthread_create(&thread->thread, &thread->attrs, cuAbstractRunnable, (void*)thread);
}

void cuThreadRequestStop(CU_NOTNULL cu_thread* thread) {
	thread->shouldStop = true;
	thread->state = TS_STOPPING;
}

bool cuThreadIsStopHasBeenRequest(CU_NOTNULL const cu_thread* thread) {
	return thread->shouldStop == true;
}

bool cuThreadIsTobeStart(CU_NOTNULL const cu_thread* thread) {
	return thread->state == TS_TOSTART;
}

bool cuThreadIsRunning(CU_NOTNULL const cu_thread* thread) {
	return thread->state == TS_RUNNING;
}

bool cuThreadIsDead(CU_NOTNULL const cu_thread* thread) {
	return thread->state == TS_DEAD;
}

int cuThreadGetLoopCounter(CU_NOTNULL const cu_thread* thread) {
	return thread->loopNumber;
}

void cuThreadWaitForCompletition(CU_NOTNULL const cu_thread* anotherThread) {
	void* status = NULL;
	pthread_join(anotherThread->thread, &status);
}

cu_mutex* cuMutexNew(bool recursive) {
	cu_mutex* result = CU_MALLOC(cu_mutex);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	*result = cuSetupMutex(recursive);

	return result;
}

void cuMutexLock(CU_NOTNULL cu_mutex* mutex) {
	pthread_mutex_lock(&mutex->mutex);
}

void cuMutexUnlock(CU_NOTNULL cu_mutex* mutex) {
	pthread_mutex_unlock(&mutex->mutex);
}

bool cuMutexTryLock(CU_NOTNULL cu_mutex* mutex) {
	return pthread_mutex_trylock(&mutex->mutex) == 0;
}

void cuMutexDestroy(CU_NOTNULL const cu_mutex* mutex, CU_NULLABLE const struct var_args* context) {
	pthread_mutex_destroy((pthread_mutex_t*)&mutex->mutex);
	pthread_mutexattr_destroy((pthread_mutexattr_t*)&mutex->attr);
	CU_FREE(mutex);
}

struct cu_condition {
	/**
	 * The actual pthread condition
	 */
	pthread_cond_t condition;
	/**
	 * The mutex associated with ::cu_condition::condition
	 */
	pthread_mutex_t mutex;
	/**
	 * Attributes of mutex ::cu_condition::mutex
	 */
	pthread_mutexattr_t mutexAttr;

	/**
	 * True if the condition is satisfied, false otheriwse
	 */
	bool checker;
	int threadWaiting;
};

cu_condition* cuConditionNew() {
	cu_condition* result = CU_MALLOC(cu_condition);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	pthread_cond_init(&result->condition, NULL);
	pthread_mutexattr_init(&result->mutexAttr);
	pthread_mutexattr_settype(&result->mutexAttr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&result->mutex, &result->mutexAttr);

	result->checker = false;
	result->threadWaiting = 0;

	return result;
}

void cuConditionDestroy(CU_NOTNULL const cu_condition* cond, CU_NULLABLE const struct var_args* context) {
	pthread_cond_destroy((pthread_cond_t*)&cond->condition);
	pthread_mutex_destroy((pthread_mutex_t*)&cond->mutex);
	CU_FREE(cond);
}

void cuConditionLockUntilVerified(CU_NOTNULL cu_condition* cond) {
	//lock the associated mutex. Required in order for condition to work
	pthread_mutex_lock(&cond->mutex);
	//if we got the lock, a new thread is waiting
	cond->threadWaiting += 1;
	while (!cond->checker) {
		//by standard, the function will automatically RELEASE the lock while it's waiting.
		pthread_cond_wait(&cond->condition, &cond->mutex);
		//by standard the function will automatically LOCK the mutex when it is woken up
	}
	//release the lock: every thread will go here anddecrease the value of the number
	cond->threadWaiting -= 1;
	pthread_mutex_unlock(&cond->mutex);
}

void cuConditionLockVerifySingleThread(CU_NOTNULL cu_condition* cond) {
	//lock the mutex. Required by standard
	pthread_mutex_lock(&cond->mutex);
	//if only one thread is waiting for this condition, ise pthread_cond_signal. Other wise use pthread_cond_broadcast
	cond->checker = true;
	if (cond->threadWaiting > 1) {
		pthread_cond_broadcast(&cond->condition);
	} else if (cond->threadWaiting == 1) {
		pthread_cond_signal(&cond->condition);
	} else {
		ERROR_IMPOSSIBLE_SCENARIO("At least one thread should be waiting!");
	}
	//we need to unlock the mutex since pthread_cond_wait will automatically relock the mutex as soon as it wakes up
	pthread_mutex_unlock(&cond->mutex);
}

static void setThreadState(CU_NOTNULL cu_thread* thread, enum thread_state state) {
	thread->state = state;
}

/**
 * @param[in] arguments a pointer pointing to the ::cu_thread instance which generated this new thread
 */
static void* cuAbstractRunnable(void* arguments) {
	cu_thread* thread = (cu_thread*) arguments;

	setThreadState(thread, TS_RUNNING);
	thread->loopNumber = 0;

	//infinite loop
	while(true) {
		//check if we need to stop
		if (cuThreadIsStopHasBeenRequest(thread)) {
			goto exit;
		}

		//run the custom code
		enum thread_loop_state result = thread->runnable(thread, thread->arguments);
		if (result == TLS_STOP) {
			goto exit;
		}
		//increment loop number
		thread->loopNumber += 1;
	}

	exit:;
	setThreadState(thread, TS_DEAD);
	pthread_exit(NULL);
}

typedef dynamic_1D_array cu_thread_dynamic_1d_array;

struct cu_parallel_thread_pool {
	cu_mutex foundMutex;
	cu_thread_dynamic_1d_array* threads;
	cu_runnable runnable;
};

cu_parallel_thread_pool* cuNewParallelThreadPool(int totalThreads, CU_NOTNULL cu_runnable runnable, const var_args* va) {
	struct cu_parallel_thread_pool* result = CU_MALLOC(cu_parallel_thread_pool);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	pthread_mutex_init(&result->foundMutex.mutex, NULL);
	//the last thread is the timing one
	result->threads = cuDynamicArrayNew(cu_thread*, totalThreads + 1);
	for (int i=0; i<totalThreads; i++) {
		cuDynamicArraySetCellTo(result->threads, i, cu_thread*, cuThreadNew(runnable, va));
	}
	result->runnable = runnable;

	return result;
}

void cuParallelThreadPoolRequestStart(CU_NOTNULL cu_parallel_thread_pool* executor) {
	CU_ITERATE_THROUGH_DYNAMIC_ARRAY(executor->threads, i, t, cu_thread*) {
		cuThreadRequestStart(t);
	}
	ERROR_NOT_IMPLEMENTED();
}

void* cuParallelThreadPoolWaitResult(CU_NOTNULL cu_parallel_thread_pool* executor, long msToWait, void* defaultValue) {
	ERROR_NOT_IMPLEMENTED();
}

void* cuDestroyParallelThreadPool(CU_NOTNULL cu_parallel_thread_pool* executor) {
	ERROR_NOT_IMPLEMENTED();
}

/**
 * Setup a mutex on the stack
 *
 * @private
 *
 * @param[in] recursive true if you want to have a recursive lock
 * @return a value representing the mutex setupped
 */
static cu_mutex cuSetupMutex(bool recursive) {
	cu_mutex result;
	cu_mutex* presult = &result;

	pthread_mutexattr_init(&presult->attr);
	if (recursive) {
		pthread_mutexattr_settype(&presult->attr, PTHREAD_MUTEX_RECURSIVE);
	} else {
		pthread_mutexattr_settype(&presult->attr, PTHREAD_MUTEX_NORMAL);
	}
	pthread_mutex_init(&presult->mutex, &presult->attr);

	return result;
}
