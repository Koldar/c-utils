/**
 * @file
 *
 * An array whose dimension can be set runtime instead of compile time
 *
 * We exchange some time to malloc but improve substantially flexibility.
 * "Dynamic" stands for such enchanced flexibility.
 *
 * The module also provides a similar feature with a dynamic matrix.
 *
 * For example the main usage can be something as follows:
 *
 * @code
 * //create a dynamic array with 10 elements. note that "c" is a runtime value
 * int c = 10;
 * dynamic_1D_array* da = cuDynamicArrayNew(int, c);
 * //set a cell of the array (much like da[0] = 5;)
 * cuDynamicArraySetCellTo(da, 0, int, 5);
 * //get the value of a cell (much like da[3];)
 * cuDynamicArrayGetCellValue(da, 3, int)
 * //get the number of elements in the array
 * cuDynamicArrayGetSize(da);
 * @endcode
 *
 * @date Jul 12, 2017
 * @author koldar
 */

#ifndef DYNAMIC_ARRAY_H_
#define DYNAMIC_ARRAY_H_

#include <stdlib.h>
#include "macros.h"
#include "var_args.h"

typedef struct dynamic_1D_array dynamic_1D_array;
typedef struct dynamic_2D_matrix dynamic_2D_matrix;

/**
 * an array of int
 */
typedef dynamic_1D_array int_dynamic_array;
/**
 * an array of unsigned long
 */
typedef dynamic_1D_array ulong_dynamic_array;
/**
 * an array of booleans
 */
typedef dynamic_1D_array bool_dynamic_array;

/**
 * initialize a new dynamic array
 *
 * @param[in] cellSize the number of bytes each cell occupies
 * @param[in] cellsNumber the number of cells of the arrays
 * @return the handle of the dynamic array
 */
dynamic_1D_array* _cuDynamicArrayNew(size_t cellSize, int cellsNumber);

/**
 * destroy a dynamic array
 *
 * @param[in] da the dynamic array to dealloc from tyhe memory
 */
void cuDynamicArrayDestroy(CU_NOTNULL const dynamic_1D_array* da, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuDynamicArrayDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * set a cell in the array
 *
 * @private
 *
 * @param[inout] da the dynamic array to set
 * @param[in] cellNumber the index fo the cell involved
 * @param[in] value the new value to assign the cell
 */
void _cuDynamicArraySetCellTo(CU_NOTNULL dynamic_1D_array* da, int cellNumber, CU_NOTNULL const void* value);

/**
 * get a cell in the array
 *
 * @private
 *
 * @param[in] da the dynamic array involved
 * @param[in] cellNumber the index fo the cell to retrieve
 * @return the value in the cell with index @c cellNumber
 */
void* _cuDynamicArrayGetCellValue(CU_NOTNULL const dynamic_1D_array* da, int cellNumber);

/**
 * Clear all the cells
 *
 * @private
 *
 * @note
 * this operation takes \f$ O(n) \f$
 *
 * @param[inout] da the dynamic array to clear
 * @param[in] value the value to assign to each cell of the array
 */
void _cuDynamicArrayClearTo(CU_NOTNULL dynamic_1D_array* da, CU_NOTNULL const void* value);

/**
 * the number of cells in the array
 *
 * @param[in] da the dynamic array involved
 * @return the number of cell this array has
 */
int cuDynamicArrayGetSize(CU_NOTNULL const dynamic_1D_array* da);

/**
 * clone a dynamic_1D_array
 *
 *
 * @param[in] original the array to copy
 * @return a clone of the dynamic array
 */
dynamic_1D_array* cuDynamicArrayClone(CU_NOTNULL const dynamic_1D_array* original);

/**
 * Get the pointer of the element starting the array
 *
 * \note
 * Be careful: array index mismatching **will** alter other elements of the array!
 *
 * @param[in] da the dynamic array to operate on
 * @return the pointer where the first element of \c da is
 * 			You can treat such pointer as an array
 */
char* _cuDynamicArrayGetDirectPointerOnArray(const dynamic_1D_array* da);

/**
 * Initialize an array long \c cellsNumber elements, everyone of type \c type
 *
 * @param[in] type the name of the type that will be stored within the array. For example "int" or "list_cell"
 * @param[in] cellsNumber the number of elements inside this array
 * @return an instance of ::dynamic_1D_array
 */
#define cuDynamicArrayNew(type, cellsNumber) _cuDynamicArrayNew(sizeof(type), cellsNumber)
/**
 * Set the value of a cell inside a ::dynamic_1D_array
 *
 * @param[inout] da the array to alter
 * @param[in] cellNumber the index of the cell to edit
 * @param[in] type the type of the value inside \c value
 * @param[in] value the value to add inside the array. Values passed by **value**;
 */
#define cuDynamicArraySetCellTo(da, cellNumber, type, value) {\
	type UV(tmp) = (value); \
	_cuDynamicArraySetCellTo(da, cellNumber, (void*)(&UV(tmp))); \
}

/**
 * Like ::cuDynamicArraySetCellTo but requires you to put a **lvalue** in variable
 *
 * \pre
 * 	\li \c variable is a **l-value**;
 *
 * @param[inout] da the array to alter
 * @param[in] cellNumber the index of the cell to edit
 * @param[in] type the type of the value inside \c value
 * @param[in] variable an **lvalue** containing the value to put inside the array
 */
#define cuDynamicArraySetCellToVariableValue(da, cellNumber, type, variable) _cuDynamicArraySetCellTo(da, cellNumber, &variable)

/**
 * Retrive a value within the dynamic array
 *
 * @param[in] da the dynamic array where we want to fetch the data from
 * @param[in] cellNumber the index of the cell to retrieve;
 * @param[in] type the type of the value to retrieve. For example "int" or "list_cell";
 */
#define cuDynamicArrayGetCellValue(da, cellNumber, type) (*((type*)_cuDynamicArrayGetCellValue(da, cellNumber)))

/**
 * clear the dynamic array
 *
 * @param[inout] da the dynamic array involved
 * @param[in] type the type any cell of the dynamic_1D_array has
 * @param[in] value the value every dell in the array will have
 */
#define cuDynamicArrayClearTo(da, type, value) { \
		type UV(tmp) = (value); \
		_cuDynamicArrayClearTo(da, (void*)(&UV(tmp))); \
	}

/**
 * retrieve the pointer of the underlying array
 *
 * @param[in] da the dynamic_1D_array involved
 * @param[in] type the type each cell of the dynamic_1D_array has
 * @return a pointer to the first cell of the underlying array of this structure
 */
#define cuDynamicArrayGetDirectPointerOnArray(da, type) ((type*)_cuDynamicArrayGetDirectPointerOnArray(da))

/**
 * Init a new dynamic matrix
 *
 * @param[in] cellSize the size eadch cells occupies in the memory (in byte);
 * @param[in] rows the number fo rows this matrix has
 * @param[in] columns the number of columns this matrix has
 * @return the pointer to the matrix
 */
dynamic_2D_matrix* _cuDynamic2DMatrixNew(size_t cellSize, int rows, int columns);

/**
 * Destroy from memory the matrix itself
 *
 * @param[in] d2m the dynamic matrix to deallocate
 */
void cuDynamic2DMatrixDestroy(CU_NOTNULL const dynamic_2D_matrix* d2m, CU_NULLABLE const struct var_args* context);

/**
 * Set the cell value inside a particular cell
 *
 * @param[inout] d2m the matrix to alter
 * @param[in] row the row in the interval \f$ [0; rows) \f$ of the cell
 * @param[in] column the column in the interval \f$ [0; columns) \f$ of the cell
 * @param[in] value a pointer to the value we need to set within the cell
 */
void _cuDynamic2DMatrixSetCellTo(CU_NOTNULL dynamic_2D_matrix* d2m, int row, int column, CU_NOTNULL const void* value);

/**
 * Get the value of a cell in the dynamic 2d matrix
 *
 * @param[in] d2m the dynamic matrix to handle
 * @param[in] row the row in the interval \f$ [0; rows) \f$ of the cell
 * @param[in] column the column in the interval \f$ [0; columns) \f$ of the cell
 * @return a pointer to the cell involved
 */
void* _cuDynamic2DMatrixGetCellValue(CU_NOTNULL const dynamic_2D_matrix* d2m, int row, int column);

/**
 * Set every cell in the matrix to a particular one
 *
 * @note
 * this operation can be CPU intensive
 *
 * @param[inout] d2m the matrix to alter
 * @param[in] value a pointer to the value each cell will have
 */
void _cuDynamic2DMatrixClearTo(CU_NOTNULL dynamic_2D_matrix* d2m, CU_NOTNULL const void* value);

/**
 * the number of rows the matrix has
 *
 * @param[in] d2m the matrix to handle
 * @return the rows of the matrix
 */
int cuDynamic2DMatrixGetRows(CU_NOTNULL const dynamic_2D_matrix* d2m);

/**
 * the number of columns the matrix has
 *
 * @param[in] d2m the matrix to handle
 * @return the columns of the matrix
 */
int cuDynamic2DMatrixGetColumns(CU_NOTNULL const dynamic_2D_matrix* d2m);

/**
 * The number of cell the matrix has
 *
 * @param[in] d2m the matrix involved
 * @return the number of cells of the matrix
 */
int cuDynamic2DMatrixGetSize(CU_NOTNULL const dynamic_2D_matrix* d2m);

/**
 * Clone the given matrix
 *
 * @param[in] original the matrix to clone
 * @return the cloned matrix
 */
dynamic_2D_matrix* cuDynamic2DMatrixClone(const dynamic_2D_matrix* original);

/**
 * Get the pointer of the element starting the given row in the matrix
 *
 * \note
 * Be careful: array index mismatching **will** alter other elements of the matrix!
 *
 * @param[in] d2m the matrix to operate on
 * @param[in] row the row we're looking for
 * @return the pointer where the first element of \c row is.
 * 			You can treat such pointer as an array to look inside the given row of the matrix
 */
char* _cuDynamic2DMatrixGetDirectPointerOnRow(const dynamic_2D_matrix* d2m, int row);

/**
 * Init a new dynamic matrix
 *
 * @param[in] type the size eadch cells occupies in the memory (in byte);
 * @param[in] rows the number fo rows this matrix has
 * @param[in] columns the number of columns this matrix has
 * @return the pointer to the matrix
 */
#define cuDynamic2DMatrixNew(type, rows, columns) _cuDynamic2DMatrixNew(sizeof(type), rows, columns)

/**
 * Set the cell value inside a particular cell
 *
 * @param[inout] da the matrix to alter
 * @param[in] row the row in the interval \f$ [0; rows) \f$ of the cell
 * @param[in] column the column in the interval \f$ [0; columns) \f$ of the cell
 * @param[in] type the type of each cell **and** of @c value
 * @param[in] value a pointer to the value we need to set within the cell
 */
#define cuDynamic2DMatrixSetCellTo(da, row, column, type, value) { \
	type UV(tmp) = (value); \
	_cuDynamic2DMatrixSetCellTo(da, row, column, &UV(tmp)); \
}

/**
 * Get the value of a cell in the dynamic 2d matrix
 *
 * @param[in] da the dynamic matrix to handle
 * @param[in] row the row in the interval \f$ [0; rows) \f$ of the cell
 * @param[in] column the column in the interval \f$ [0; columns) \f$ of the cell
 * @param[in] type the type each cell in the matrix has
 * @return a pointer to the cell involved
 */
#define cuDynamic2DMatrixGetCellValue(da, row, column, type) (*((type*)_cuDynamic2DMatrixGetCellValue(da, row, column)))

/**
 * Set every cell in the matrix to a particular one
 *
 * @note
 * this operation can be CPU intensive
 *
 * @param[inout] da the matrix to alter
 * @param[in] type the type each cell in the matrix has
 * @param[in] value a pointer to the value each cell will have
 */
#define cuDynamic2DMatrixClearTo(da, type, value) { \
	type UV(tmp) = (value); \
	_cuDynamic2DMatrixClearTo(da, &UV(tmp)); \
}

/**
 * Get the pointer of the element starting the given row in the matrix
 *
 * \note
 * Be careful: array index mismatching **will** alter other elements of the matrix!
 *
 * @param[in] da the matrix to operate on
 * @param[in] row the row we're looking for
 * @param[in] type the type each cell of the matrix has
 * @return the pointer where the first element of \c row is.
 * 			You can treat such pointer as an array to look inside the given row of the matrix
 */
#define cuDynamic2DMatrixGetDirectPointerOnRow(da, row, type) ((type*)_cuDynamic2DMatrixGetDirectPointerOnRow(da, row))

/**
 * Convenience macro to automatically iterate through a dynamic array
 *
 * An example of usage might be;
 * @code
 * 	CU_ITERATE_THROUGH_DYNAMIC_ARRAY(arrayOfDoubles, i, v, double) {
 * 		printf("at index %d there is the double %2.3f\n", i, v);
 * 	}
 * @endcode
 */
#define CU_ITERATE_THROUGH_DYNAMIC_ARRAY(_da, index, var, type) \
	dynamic_1D_array* UV(da) = (_da); \
	for ( \
			int index=0, UV(keepGoing)=1 \
			; \
			index<cuDynamicArrayGetSize(UV(da)) \
			; \
			index++, UV(keepGoing)=1 \
	) \
		for (type var=cuDynamicArrayGetCellValue(UV(da), index, type); UV(keepGoing) == 1; UV(keepGoing)=0)


#endif /* DYNAMIC_ARRAY_H_ */
