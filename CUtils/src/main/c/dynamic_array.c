/*
 * dynamic_array.c
 *
 *  Created on: Jul 12, 2017
 *      Author: koldar
 */


#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "dynamic_array.h"
#include "log.h"
#include "errors.h"


struct dynamic_1D_array {
	int size;
	int cellSize;
	char* array;
};

struct dynamic_2D_matrix {
	int rows;
	int columns;
	dynamic_1D_array matrix;
};

#define POINTER_OF_CELL(da, i) &da->array[(i)*(da->cellSize)]

static dynamic_1D_array* setupDynamicArray(CU_NOTNULL dynamic_1D_array* da, size_t cellSize, int cellsNumber);

dynamic_1D_array* _cuDynamicArrayNew(size_t cellSize, int cellsNumber) {
	dynamic_1D_array* retVal = malloc(sizeof(dynamic_1D_array));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	setupDynamicArray(retVal, cellSize, cellsNumber);

	return retVal;
}

void cuDynamicArrayDestroy(CU_NOTNULL const dynamic_1D_array* da, CU_NULLABLE const struct var_args* context) {
	free((void*)da->array);
	free((void*)da);
}

void _cuDynamicArraySetCellTo(CU_NOTNULL dynamic_1D_array* da, int cellNumber, CU_NOTNULL const void* value) {
#ifdef DEBUG
	if (cellNumber < 0 || cellNumber > da->size) {
		ERROR_NULL("index out of bounds");
	}
#endif
	memcpy(POINTER_OF_CELL(da, cellNumber), value, da->cellSize);
}

void* _cuDynamicArrayGetCellValue(CU_NOTNULL const dynamic_1D_array* da, int cellNumber) {
#ifdef DEBUG
	if (cellNumber < 0 || cellNumber > da->size) {
		ERROR_NULL("index out of bounds");
	}
#endif
	return POINTER_OF_CELL(da, cellNumber);
}

void _cuDynamicArrayClearTo(CU_NOTNULL dynamic_1D_array* da, CU_NOTNULL const void* value) {
	for (int i=0; i<da->size; i++) {
		_cuDynamicArraySetCellTo(da, i, value);
	}
}

int cuDynamicArrayGetSize(CU_NOTNULL const dynamic_1D_array* da) {
	return da->size;
}

dynamic_1D_array* cuDynamicArrayClone(CU_NOTNULL const dynamic_1D_array* original) {
	dynamic_1D_array* retVal = _cuDynamicArrayNew(original->cellSize, original->size);
	memcpy(retVal->array, original->array, original->cellSize * original->size);

	return retVal;
}

char* _cuDynamicArrayGetDirectPointerOnArray(CU_NOTNULL const dynamic_1D_array* da) {
	return da->array;
}

dynamic_2D_matrix* _cuDynamic2DMatrixNew(size_t cellSize, int rows, int columns) {
	dynamic_2D_matrix* retVal = malloc(sizeof(dynamic_2D_matrix));
	if (retVal == NULL) {
		ERROR_NULL("index out of bounds");
	}

	retVal->rows = rows;
	retVal->columns = columns;

	setupDynamicArray(&retVal->matrix, cellSize, rows * columns);
	return retVal;
}

void cuDynamic2DMatrixDestroy(const dynamic_2D_matrix* d2m, CU_NULLABLE const struct var_args* context) {
	free((void*)d2m->matrix.array);
	free((void*)d2m);
}

void _cuDynamic2DMatrixSetCellTo(CU_NOTNULL dynamic_2D_matrix* d2m, int row, int column, CU_NOTNULL const void* value) {
#ifdef DEBUG
	if (row < 0 || row >= d2m->rows) {
		ERROR_NULL("index out of bounds");
	}
	if (column < 0  || column >= d2m->columns) {
		ERROR_NULL("index out of bounds");
	}
#endif
	_cuDynamicArraySetCellTo(&d2m->matrix, row * d2m->columns + column, value);
}

void* _cuDynamic2DMatrixGetCellValue(const dynamic_2D_matrix* d2m, int row, int column) {
#ifdef DEBUG
	if (row < 0 || row >= d2m->rows) {
		ERROR_NULL("index out of bounds");
	}
	if (column < 0  || column >= d2m->columns) {
		ERROR_NULL("index out of bounds");
	}
#endif
	return _cuDynamicArrayGetCellValue(&d2m->matrix, row * d2m->columns + column);
}

void _cuDynamic2DMatrixClearTo(CU_NOTNULL dynamic_2D_matrix* d2m, const void* value) {
	_cuDynamicArrayClearTo(&d2m->matrix, value);
}

int cuDynamic2DMatrixGetRows(CU_NOTNULL const dynamic_2D_matrix* d2m) {
	return d2m->rows;
}

int cuDynamic2DMatrixGetColumns(const dynamic_2D_matrix* d2m) {
	return d2m->columns;
}

int cuDynamic2DMatrixGetSize(const dynamic_2D_matrix* d2m) {
	return cuDynamicArrayGetSize(&d2m->matrix);
}

dynamic_2D_matrix* cuDynamic2DMatrixClone(const dynamic_2D_matrix* original) {
	dynamic_2D_matrix* retVal = _cuDynamic2DMatrixNew(original->matrix.cellSize, original->rows, original->columns);
	memcpy(retVal->matrix.array, original->matrix.array, original->matrix.cellSize * original->matrix.size);

	return retVal;
}

char* _cuDynamic2DMatrixGetDirectPointerOnRow(const dynamic_2D_matrix* d2m, int row) {
	return POINTER_OF_CELL((&d2m->matrix), row*d2m->columns);
}

static dynamic_1D_array* setupDynamicArray(dynamic_1D_array* da, size_t cellSize, int cellsNumber) {
	da->cellSize = cellSize;
	da->size = cellsNumber;
	da->array = malloc(sizeof(char) * cellSize * cellsNumber);
	if (da->array == NULL) {
		ERROR_MALLOC();
	}

	return da;
}
