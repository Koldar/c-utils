/*
 * dynamic_arrayTest.c
 *
 *  Created on: Jul 12, 2017
 *      Author: koldar
 */

#include "dynamic_array.h"
#include "CuTest.h"
#include <assert.h>

///test dynamic array allocation and deallocation
void testDynamicArray01(CuTest* tc) {
	dynamic_1D_array* da = cuDynamicArrayNew(int, 10);

	cuDynamicArrayDestroy(da, NULL);
}

///test setting and getting a cell
void testDynamicArray02(CuTest* tc) {
	dynamic_1D_array* da = cuDynamicArrayNew(int, 10);

	cuDynamicArraySetCellTo(da, 0, int, 5);
	assert(cuDynamicArrayGetCellValue(da, 0, int) == 5);
	cuDynamicArraySetCellTo(da, 3, int, 5);
	assert(cuDynamicArrayGetCellValue(da, 3, int) == 5);
	cuDynamicArraySetCellTo(da, 3, int, 4);
	assert(cuDynamicArrayGetCellValue(da, 3, int) == 4);

	cuDynamicArrayDestroy(da, NULL);
}

///test clearing a dynamic array
void testDynamicArray03(CuTest* tc) {
	dynamic_1D_array* da = cuDynamicArrayNew(int, 10);

	cuDynamicArraySetCellTo(da, 0, int, 5);
	assert(cuDynamicArrayGetCellValue(da, 0, int) == 5);
	cuDynamicArraySetCellTo(da, 3, int, 5);
	assert(cuDynamicArrayGetCellValue(da, 3, int) == 5);
	cuDynamicArraySetCellTo(da, 3, int, 4);
	assert(cuDynamicArrayGetCellValue(da, 3, int) == 4);

	assert(cuDynamicArrayGetSize(da) == 10);

	cuDynamicArrayClearTo(da, int, 10);
	for (int i=0; i<cuDynamicArrayGetSize(da); i++) {
		assert(cuDynamicArrayGetCellValue(da, i, int) == 10);
	}

	cuDynamicArrayDestroy(da, NULL);
}

///test setting and getting the first and the last cell of the dynamic array
void testDynamicArray04(CuTest* tc) {
	dynamic_1D_array* da = cuDynamicArrayNew(int, 10);

	cuDynamicArraySetCellTo(da, 0, int, 5);
	assert(cuDynamicArrayGetCellValue(da, 0, int) == 5);
	cuDynamicArraySetCellTo(da, 0, int, 4);
	assert(cuDynamicArrayGetCellValue(da, 0, int) == 4);

	cuDynamicArraySetCellTo(da, 9, int, 5);
	assert(cuDynamicArrayGetCellValue(da, 9, int) == 5);
	cuDynamicArraySetCellTo(da, 9, int, 4);
	assert(cuDynamicArrayGetCellValue(da, 9, int) == 4);

	cuDynamicArrayDestroy(da, NULL);
}

///test cloning
void testDynamicArray05(CuTest* tc) {
	dynamic_1D_array* da = cuDynamicArrayNew(int, 4);

	cuDynamicArraySetCellTo(da, 0, int, 5);
	cuDynamicArraySetCellTo(da, 1, int, 6);
	cuDynamicArraySetCellTo(da, 2, int, 7);
	cuDynamicArraySetCellTo(da, 3, int, 8);

	dynamic_1D_array* clone = cuDynamicArrayClone(da);

	assert(cuDynamicArrayGetSize(da) == 4);
	assert(da != clone);

	assert(cuDynamicArrayGetCellValue(clone, 0, int) == 5);
	assert(cuDynamicArrayGetCellValue(clone, 1, int) == 6);
	assert(cuDynamicArrayGetCellValue(clone, 2, int) == 7);
	assert(cuDynamicArrayGetCellValue(clone, 3, int) == 8);

	cuDynamicArrayDestroy(da, NULL);
	cuDynamicArrayDestroy(clone, NULL);
}

///test dynamic 2d matrix allocation and deallocation
void testDynamic2DMatrix01(CuTest* tc) {
	dynamic_2D_matrix* d2m = cuDynamic2DMatrixNew(int, 5, 3);

	cuDynamic2DMatrixDestroy(d2m, NULL);
}

///test setting and getting a cell
void testDynamic2DMatrix02(CuTest* tc) {
	dynamic_2D_matrix* d2m = cuDynamic2DMatrixNew(int, 5, 3);

	cuDynamic2DMatrixSetCellTo(d2m, 3, 1, int, 5);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 3, 1, int) == 5);
	cuDynamic2DMatrixSetCellTo(d2m, 3, 2, int, 5);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 3, 2, int) == 5);
	cuDynamic2DMatrixSetCellTo(d2m, 3, 2, int, 4);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 3, 2, int) == 4);

	cuDynamic2DMatrixDestroy(d2m, NULL);
}

///test clearing a dynamic array
void testDynamic2DMatrix03(CuTest* tc) {
	dynamic_2D_matrix* d2m = cuDynamic2DMatrixNew(int, 5, 3);

	cuDynamic2DMatrixSetCellTo(d2m, 3, 1, int, 5);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 3, 1, int) == 5);
	cuDynamic2DMatrixSetCellTo(d2m, 3, 2, int, 5);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 3, 2, int) == 5);
	cuDynamic2DMatrixSetCellTo(d2m, 3, 2, int, 4);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 3, 2, int) == 4);

	assert(cuDynamic2DMatrixGetRows(d2m) == 5);
	assert(cuDynamic2DMatrixGetColumns(d2m) == 3);
	assert(cuDynamic2DMatrixGetSize(d2m) == 15);

	cuDynamic2DMatrixClearTo(d2m, int, 10);
	for (int i=0; i<cuDynamic2DMatrixGetRows(d2m); i++) {
		for (int j=0; j<cuDynamic2DMatrixGetColumns(d2m); j++) {
			assert(cuDynamic2DMatrixGetCellValue(d2m, i, j, int) == 10);
		}
	}

	cuDynamic2DMatrixDestroy(d2m, NULL);
}

///test setting and getting the first and the last cell of the dynamic array
void testDynamic2DMatrix04(CuTest* tc) {
	dynamic_2D_matrix* d2m = cuDynamic2DMatrixNew(int, 5, 3);

	cuDynamic2DMatrixSetCellTo(d2m, 0, 0, int, 5);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 0, 0, int) == 5);
	cuDynamic2DMatrixSetCellTo(d2m, 0, 0, int, 4);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 0, 0, int) == 4);

	cuDynamic2DMatrixSetCellTo(d2m, 4, 2, int, 5);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 4, 2, int) == 5);
	cuDynamic2DMatrixSetCellTo(d2m, 4, 2, int, 4);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 4, 2, int) == 4);

	cuDynamic2DMatrixDestroy(d2m, NULL);
}


void testDynamic2DMatrix05(CuTest* tc) {
	dynamic_2D_matrix* d2m = cuDynamic2DMatrixNew(int, 3, 3);

	for (int i=0; i<3; i++) {
		for (int j=0; j<3; j++) {
			cuDynamic2DMatrixSetCellTo(d2m, i, j, int, i+j);
		}
	}

	assert(cuDynamic2DMatrixGetCellValue(d2m, 0, 0, int) == 0);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 0, 1, int) == 1);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 0, 2, int) == 2);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 1, 0, int) == 1);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 1, 1, int) == 2);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 1, 2, int) == 3);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 2, 0, int) == 2);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 2, 1, int) == 3);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 2, 2, int) == 4);

	int* row1 = cuDynamic2DMatrixGetDirectPointerOnRow(d2m, 1, int);

	row1[0] = 5;
	row1[1] = 5;
	row1[2] = 5;

	assert(cuDynamic2DMatrixGetCellValue(d2m, 0, 0, int) == 0);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 0, 1, int) == 1);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 0, 2, int) == 2);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 1, 0, int) == 5);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 1, 1, int) == 5);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 1, 2, int) == 5);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 2, 0, int) == 2);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 2, 1, int) == 3);
	assert(cuDynamic2DMatrixGetCellValue(d2m, 2, 2, int) == 4);

	cuDynamic2DMatrixDestroy(d2m, NULL);
}

///test cloning
void testDynamic2DMatrix06(CuTest* tc) {
	dynamic_2D_matrix* da = cuDynamic2DMatrixNew(int, 3, 2);

	cuDynamic2DMatrixSetCellTo(da, 0, 0, int, 5);
	cuDynamic2DMatrixSetCellTo(da, 0, 1, int, 6);
	cuDynamic2DMatrixSetCellTo(da, 1, 0, int, 7);
	cuDynamic2DMatrixSetCellTo(da, 1, 1, int, 8);
	cuDynamic2DMatrixSetCellTo(da, 2, 0, int, 9);
	cuDynamic2DMatrixSetCellTo(da, 2, 1, int, 10);

	dynamic_2D_matrix* clone = cuDynamic2DMatrixClone(da);

	assert(cuDynamic2DMatrixGetSize(clone) == 6);
	assert(da != clone);

	assert(cuDynamic2DMatrixGetCellValue(clone, 0, 0, int) == 5);
	assert(cuDynamic2DMatrixGetCellValue(clone, 0, 1, int) == 6);
	assert(cuDynamic2DMatrixGetCellValue(clone, 1, 0, int) == 7);
	assert(cuDynamic2DMatrixGetCellValue(clone, 1, 1, int) == 8);
	assert(cuDynamic2DMatrixGetCellValue(clone, 2, 0, int) == 9);
	assert(cuDynamic2DMatrixGetCellValue(clone, 2, 1, int) == 10);

	cuDynamic2DMatrixDestroy(da, NULL);
	cuDynamic2DMatrixDestroy(clone, NULL);
}

CuSuite* CuDynamicArraySuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testDynamicArray01);
	SUITE_ADD_TEST(suite, testDynamicArray02);
	SUITE_ADD_TEST(suite, testDynamicArray03);
	SUITE_ADD_TEST(suite, testDynamicArray04);
	SUITE_ADD_TEST(suite, testDynamicArray05);

	SUITE_ADD_TEST(suite, testDynamic2DMatrix01);
	SUITE_ADD_TEST(suite, testDynamic2DMatrix02);
	SUITE_ADD_TEST(suite, testDynamic2DMatrix03);
	SUITE_ADD_TEST(suite, testDynamic2DMatrix04);
	SUITE_ADD_TEST(suite, testDynamic2DMatrix05);
	SUITE_ADD_TEST(suite, testDynamic2DMatrix06);


	return suite;
}
