/**
 * @file
 *
 * @author koldar
 * @date May 14, 2018
 */

#include "CuTest.h"
#include "multithreading.h"
#include "log.h"
#include "string_utils.h"
#include <unistd.h>

enum thread_loop_state run01(CU_NOTNULL const cu_thread* thread, const struct var_args* va) {
	int* a = cuVarArgsGetItem(va, 0, int*);
	*a = 5;
	critical("inside thread");

	return TLS_STOP;
}

//test a simple thread
void test_multithreading_01(CuTest* tc) {
	int a = 0;
	cuInitVarArgsOnStack(va, &a);

	cu_thread* thread = cuThreadNew(run01, va);
	critical("start thread...");
	cuThreadRequestStart(thread);

	cuThreadWaitForCompletition(thread);

	assert(a == 5);

	cuThreadDestroy(thread, NULL);
}

enum thread_loop_state run02(CU_NOTNULL const cu_thread* thread, const struct var_args* va) {
	int* a = cuVarArgsGetItem(va, 0, int*);
	char* str = cuVarArgsGetItem(va, 1, char*);
	float* b = cuVarArgsGetItem(va, 2, float*);

	*a = 5;
	*b += *a;
	str[3] = '*';

	return TLS_STOP;
}

//test a simple thread with multiple arguments
void test_multithreading_02(CuTest* tc) {
	int a = 0;
	char* str = strdup("hello");
	float b = 0.0;

	cuInitVarArgsOnStack(va, &a, str, &b);
	cu_thread* thread = cuThreadNew(run02, va);
	cuThreadRequestStart(thread);

	cuThreadWaitForCompletition(thread);

	assert(a == 5);
	assert(isStrEqual(str, "hel*o"));
	assert(b == 5.0);

	cuThreadDestroy(thread, NULL);
	CU_FREE(str);
}

enum thread_loop_state run03(CU_NOTNULL const cu_thread* thread, const struct var_args* va) {
	int* a = cuVarArgsGetItem(va, 0, int*);
	int inc = cuVarArgsGetItem(va, 1, int);
	cu_mutex* mutex = cuVarArgsGetItem(va, 2, cu_mutex*);

	CU_LOCK(mutex) {
		*a += inc;
	}

	return cuThreadGetLoopCounter(thread) < 3 ? TLS_CONTINUE : TLS_STOP;
}

//test mutex control
void test_multithreading_03(CuTest* tc) {
	int a = 0;
	cu_mutex* mutex = cuMutexNew(false);

	cuInitVarArgsOnStack(va1, &a, 1, mutex);
	cuInitVarArgsOnStack(va2, &a, 3, mutex);

	cu_thread* thread1 = cuThreadNew(run03, va1);
	cu_thread* thread2 = cuThreadNew(run03, va2);

	cuThreadRequestStart(thread1);
	cuThreadRequestStart(thread2);

	//we execute each thread 4 times in total
	cuThreadWaitForCompletition(thread1);
	cuThreadWaitForCompletition(thread2);

	critical("a is %d", a);
	assert(a == (1+1+1+1+3+3+3+3));

	cuThreadDestroy(thread1, NULL);
	cuThreadDestroy(thread2, NULL);

	cuMutexDestroy(mutex, NULL);
}

#define TCOUNT 10
#define COUNT_LIMIT 12

int     count = 0;
pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

enum thread_loop_state inc_count(CU_NOTNULL const cu_thread* thread, CU_NOTNULL const var_args* va) {
	int my_id = cuVarArgsGetItem(va, 0, int);
	cu_condition* cond = cuVarArgsGetItem(va, 1, cu_condition*);
	cu_mutex* counterMutex = cuVarArgsGetItem(va, 2, cu_mutex*);

	for (int i=0; i<TCOUNT; i++) {
		info("thread %d: trying to obtain counterMutex...", my_id);
		CU_LOCK(counterMutex) {
			info("thread %d: counterMutex obtained!", my_id);
			count++;
			if (count == COUNT_LIMIT) {
				//wake up threads waiting for this condition
				cuConditionLockVerifySingleThread(cond);
				info("inc_count(): thread %d, count = %d  Threshold reached.",	my_id, count);
			}
			info("inc_count(): thread %d, count = %d, unlocking mutex", my_id, count);
		}

		/* Do some "work" so threads can alternate on mutex lock */
		sleep(1);
	}

	return TLS_STOP;
}

enum thread_loop_state watch_count(CU_NOTNULL const cu_thread* thread, CU_NOTNULL const var_args* va) {
	int my_id = cuVarArgsGetItem(va, 0, int);
	cu_condition* cond = cuVarArgsGetItem(va, 1, cu_condition*);
	cu_mutex* counterMutex = cuVarArgsGetItem(va, 2, cu_mutex*);

	info("Starting watch_count(): thread %d", my_id);

	cuConditionLockUntilVerified(cond);
	//we have been woken up! Increase count variable!
	CU_LOCK(counterMutex) {
		count += 125;
		info("watch_count(): thread %d count now = %d.", my_id, count);
	}

	return TLS_STOP;
}

//test conditions
void test_multithreading_04(CuTest* tc) {
	int i, rc;
	long t1=1, t2=2, t3=3;
	pthread_t threads[3];
	pthread_attr_t attr;

	CU_WITH(cu_mutex* counterLock = cuMutexNew(false))(cuMutexDestroy(counterLock, NULL))
	CU_WITH(cu_condition* cond = cuConditionNew())(cuConditionDestroy(cond, NULL)) {

		cuInitVarArgsOnStack(va0, 0, cond, counterLock);
		cuInitVarArgsOnStack(va1, 1, cond, counterLock);
		cuInitVarArgsOnStack(va2, 2, cond, counterLock);

		info("va0: %p item 0: %p", va0, _cuVarArgsGetItem(va0, 0));
		info("va1: %p item 0: %p", va1, _cuVarArgsGetItem(va1, 0));

		CU_WITH(cu_thread* thread0 = cuThreadNew(inc_count, va0))(cuThreadDestroy(thread0, NULL))
		CU_WITH(cu_thread* thread1 = cuThreadNew(inc_count, va1))(cuThreadDestroy(thread1, NULL))
		CU_WITH(cu_thread* thread2 = cuThreadNew(watch_count, va2))(cuThreadDestroy(thread2, NULL)) {

			cuThreadRequestStart(thread0);
			cuThreadRequestStart(thread1);
			cuThreadRequestStart(thread2);

			cuThreadWaitForCompletition(thread0);
			cuThreadWaitForCompletition(thread1);
			cuThreadWaitForCompletition(thread2);

			assert(count == (TCOUNT + TCOUNT  + 125));
		}
	}

}

int factorial(int n, cu_mutex* recursiveLock) {
	int result;

	CU_LOCK(recursiveLock) {
		if (n == 0) {
			result = 1;
		} else {
			result = n * factorial(n-1, recursiveLock);
		}
	}

	return result;
}

///test recursive locks
void test_multithreading_05(CuTest* tc) {

	CU_WITH(cu_mutex* recursiveLock = cuMutexNew(true))(cuMutexDestroy(recursiveLock, NULL)) {
		assert(factorial(5, recursiveLock) == 120);
	}
}

CuSuite* CuMultiTrheadingSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_multithreading_01);
	SUITE_ADD_TEST(suite, test_multithreading_02);
	SUITE_ADD_TEST(suite, test_multithreading_03);
	SUITE_ADD_TEST(suite, test_multithreading_04);
	SUITE_ADD_TEST(suite, test_multithreading_05);

	return suite;
}
