/*
 * heap.c
 *
 *  Created on: Jun 2, 2017
 *      Author: koldar
 */

#include "heap.h"
#include "macros.h"
#include <stdlib.h>
#include <math.h>
#include "log.h"
#include "defaultFunctions.h"
#include "errors.h"

/**
 * Represents the id to return if the given cell doesn't have the requested cell
 * (ie root can't have parent node or leaf node can't have right child).
 */
#define H_NOID -1

struct heap {
	/**
	 * The maximum number of values that can be added in the heap
	 */
	int maxSize;
	/**
	 * Represents the index fo the first leaf node in the heap.
	 *
	 * leaves are all at the end of the heap. So knowing the id of the first leaf
	 * means you can detect whether a node is a leaf or not very quickly by simply comparing
	 * its id with this variable
	 */
	int firstLeafNodeIndex;
	/**
	 * The number of elements inside this data structure
	 */
	int size;
	/**
	 * A set of functions which are used to generally handle the payload
	 */
	payload_functions payloadFunctions;
	/**
	 * Represents the index of a cell in ::heap::elements that should be filled with a new value next
	 */
	int nextCellAvailable;
	/**
	 * An array of elements
	 *
	 * The array is ::heap::maxSize long
	 */
	void** elements;
};

static bool isIndexRoot(const heap* h, int index);
static bool isLeafNode(const heap* h, int index);
static int getParentCell(const heap* h, int index);
static int getNewNextCellAvailable(const heap* h);
static int getLeftChild(const heap* h, int index);
static int getRightChild(const heap* h, int index);
static void percolateUp(heap* h, int index);
static void percolateDown(heap* h, int parentIndex);
static bool containsItemInHeapRecursive(CU_NOTNULL const heap* h, int currentId, const void* lookedFor);

heap* cuHeapNew(int maxSize, payload_functions functions) {
	heap* retVal = malloc(sizeof(heap));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->maxSize = maxSize;
	retVal->firstLeafNodeIndex = pow(2, floor(log2(retVal->maxSize)));
	retVal->size = 0;
	retVal->payloadFunctions = functions;
	retVal->nextCellAvailable = 1;
	//for implementation convenience, this array content start from 1, not from 0.
	//this because if the root has index 1, every left child has id equal to 2*parentNode while
	//every right node has id equal to 2*parentNode + 1
	retVal->elements = malloc(sizeof(void*) * (maxSize + 1));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	return retVal;
}

void cuHeapDestroy(CU_NOTNULL const heap* h, CU_NULLABLE const struct var_args* context) {
	CU_FREE(h->elements);
	CU_FREE(h);
}

void cuHeapDestroyWithElements(CU_NOTNULL const heap* h, CU_NULLABLE const struct var_args* context) {
	for (int i=1; i<=h->size; i++) {
		h->payloadFunctions.destroy(h->elements[i], context);
	}
	cuHeapDestroy(h, context);
}

void cuHeapClear(CU_NOTNULL heap* h) {
	h->size = 0;
	h->nextCellAvailable = 1;
}

bool cuHeapInsertItem(CU_NOTNULL heap* h, CU_NULLABLE const void* item) {
	if (cuHeapIsFull(h)) {
		critical("Cannot add item %p inside heap %p because the heap is full", item, h);
		return false;
	}
	//TODO adds a parameter which determine if we need to check the insertion of the heap
	if (cuHeapContainsItem(h, item)) {
		return false;
	}
	CU_SAFE_ASSIGN(h->elements[h->nextCellAvailable], item);
	h->nextCellAvailable = getNewNextCellAvailable(h);
	h->size++;
	percolateUp(h, h->nextCellAvailable - 1);
	return true;
}

bool cuHeapIsEmpty(CU_NOTNULL const heap* h) {
	return h->size == 0;
}

bool cuHeapIsFull(CU_NOTNULL const heap* h) {
	return h->size == h->maxSize;
}

void* cuHeapPeekMinItem(CU_NOTNULL const heap* h) {
	return cuHeapIsEmpty(h) ? NULL : h->elements[1];
}

void* cuHeapRemoveMinItem(CU_NOTNULL heap* h) {
	if (cuHeapIsEmpty(h)) {
		return NULL;
	}

	void* retVal = h->elements[1];
	SWAP(h->elements[1], h->elements[h->nextCellAvailable - 1], void*);
	h->nextCellAvailable--;
	h->size--;
	//percolate down from the root
	percolateDown(h, 1);
	return retVal;
}

int cuHeapGetSize(CU_NOTNULL const heap* h) {
	return h->size;
}

int cuHeapGetMaxSize(CU_NOTNULL const heap* h) {
	return h->maxSize;
}

bool cuHeapContainsItem(CU_NOTNULL const heap* h, const CU_NULLABLE void* item) {
	return containsItemInHeapRecursive(h, 1, item);
}

void* _cuHeapGetNthItem(CU_NOTNULL const heap* h, int i) {
	return h->elements[i];
}

int cuHeapGetBufferString(CU_NOTNULL const heap* h, CU_NOTNULL char* buffer) {
	int i = 0;
	i += sprintf(&buffer[i], "{ ");
	CU_ITERATE_THROUGH_HEAP(h, payload, void*) {
		i += h->payloadFunctions.bufferString(payload, &buffer[i]);
		i += sprintf(&buffer[i], " ");
	}
	i += sprintf(&buffer[i], "}");
	return i;
}

heap* cuHeapClone(CU_NOTNULL const heap* h) {
	heap* retVal = cuHeapNew(h->maxSize, h->payloadFunctions);

	CU_ITERATE_THROUGH_HEAP(h, payload, void*) {
		cuHeapInsertItem(retVal, h->payloadFunctions.clone(payload));
	}

	return retVal;
}

void cuHeapMoveItems(CU_NOTNULL heap* restrict dst,CU_NOTNULL heap* restrict src) {
	while (!cuHeapIsEmpty(src)) {
		cuHeapInsertItem(dst, cuHeapRemoveMinItem(src));
	}
}

static bool containsItemInHeapRecursive(CU_NOTNULL const heap* h, int currentId, const CU_NOTNULL void* lookedFor) {
	//we have arrived to a cell in the heap that is not initialized yet. We have reached the end fo the heap for
	//this path
	if (currentId == H_NOID || currentId >= h->nextCellAvailable) {
		return false;
	}

	finest("currentId set to %d. NextCellAvailable is %d", currentId, h->nextCellAvailable);
	//FIXME here there should be a comparator to check if the value are the same, not an orderer. If you want, you should put an flag in the constructor telling if you want to use a comprator at all
	int i = h->payloadFunctions.order(lookedFor, h->elements[currentId]);
	if (i == 0) {
		//we found our values
		return true;
	}

	if (i < 0) {
		//the element we're look for is smaller than the current node, then we won't bother searching in this subtree, since all the nodes
		//there are bigger than the current node nethertheless.
		return false;
	}

	if (i > 0) {
		//ok, maybe in the subtree there is something
		if (containsItemInHeapRecursive(h, getLeftChild(h, currentId), lookedFor)) {
			return true;
		}
		if (containsItemInHeapRecursive(h, getRightChild(h, currentId), lookedFor)) {
			return true;
		}

		return false;
	}

	ERROR_IMPOSSIBLE_SCENARIO("this shouldn't happen");
	return false;
}

/**
 * @param[in] h the heap analyzed
 * @param[in] index the index to handle
 * @return
 * 	\li true if \c index represents the id of the root;
 * 	\li false otherwise
 */
static bool isIndexRoot(const heap* h, int index) {
	return index == 1;
}

/**
 * @param[in] h the heap analyzed
 * @param[in] index the index to handle
 * @return
 * 	\li true if \c index lead to a leaf node;
 * 	\li false otherwise;
 */
static bool isLeafNode(const heap* h, int index) {
	return (index < h->maxSize) && (index >= h->firstLeafNodeIndex);
}

/**
 * Fetch the id of the parent node of a given node
 *
 * @param[in] h the heap analyzed
 * @param[in] index the position of the cell considered
 * @return
 * 	\li the id of the parent node of the heap;
 * 	\li ::H_NOID if such node doesn't exist
 */
static int getParentCell(const heap* h, int index) {
	return isIndexRoot(h, index) ? H_NOID : index/2;
}

/**
 * Fetch the id of the left child node of a given node
 *
 * @param[in] h the heap analyzed
 * @param[in] index the position of the cell considered
 * @return
 * 	\li the id of the left child node;
 * 	\li ::H_NOID if such node doesn't exist
 */
static int getLeftChild(const heap* h, int index) {
	if (isLeafNode(h, index)) {
		return H_NOID;
	}
	if (h->size < 2*index) {
		return H_NOID;
	}
	return 2*index;
}

/**
 * Fetch the id of the right child node of a given node
 *
 * @param[in] h the heap analyzed
 * @param[in] index the position of the cell considered
 * @return
 * 	\li the id of the right child node;
 * 	\li ::H_NOID if such node doesn't exist
 */
static int getRightChild(const heap* h, int index) {
	if (isLeafNode(h, index)) {
		return H_NOID;
	}
	if (h->size < (2*index + 1)) {
		return H_NOID;
	}
	return (2*index + 1);
}

/**
 * Perform heap percolate operation
 *
 * \note
 * After the operation, the heap is correctly repaired
 *
 * @param[in] h the heap to repair
 * @param[in] index the index when starting to percolate up
 */
static void percolateUp(heap* h, int index) {
	register int parentId = getParentCell(h, index);
	if (parentId == H_NOID) {
		return;
	}
	if (h->payloadFunctions.order(h->elements[index], h->elements[parentId]) < 0 ) {
		//the child is less than the parent. We need to swap
		SWAP(h->elements[index], h->elements[parentId], void*);
	}
	percolateUp(h, parentId);
}

static void percolateDown(heap* h, int parentIndex) {
	if (isLeafNode(h, parentIndex)) {
		return;
	}

	register int lc = getLeftChild(h, parentIndex);
	register int rc = getRightChild(h, parentIndex);
	register int small = 0;

	if (rc == H_NOID) {
		if (lc == H_NOID) {
			//both left and right children don't exist. We do nothing
			return;
		}
		//right child doesn't exist, but left does. We swap parent with the left child
		small = lc;
		finer("parent %d(%p) lc[small] = %d(%p) size = %d", parentIndex, h->elements[parentIndex], lc, h->elements[lc], h->size);
	} else {
		//both children exist. We need to determin which one we need to swap with.
		info("parent %d(%p) lc = %d(%p) rc = %d(%p) size = %d", parentIndex, h->elements[parentIndex], lc, h->elements[lc], rc, h->elements[rc], h->size);
		if (h->payloadFunctions.order(h->elements[lc], h->elements[rc]) < 0) {
			//left is less than right
			small = lc;
		} else {
			//right is less than left
			small = rc;
		}
	}
	if (h->payloadFunctions.order(h->elements[parentIndex], h->elements[small]) < 0) {
		return;
	}
	SWAP(h->elements[small], h->elements[parentIndex], void*);
	percolateDown(h, small);
}

static int getNewNextCellAvailable(const heap* h) {
	return h->nextCellAvailable + 1;
}


