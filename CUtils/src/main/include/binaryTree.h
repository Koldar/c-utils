/**
 * @file
 *
 * Implements a binary tree. Useful for set implementations
 *
 * Binary Search Trees (BST) allows you to search, insert and delete in worst case O(n). However,
 * averagely, they allow you to search, insert and delete in \f$ O( n log_2 n ) \f$ time.
 *
 * Some references:
 *
 * \li <a href="http://web.cs.wpi.edu/~cs2102/common/notes-d13/bsts-and-avls.html">link1</a>;
 * \li <a href="http://www.algolist.net/Data_structures/Binary_search_tree/Removal">link2</a>;
 *
 * @date Jun 6, 2017
 * @author koldar
 */

#ifndef BINARYTREE_H_
#define BINARYTREE_H_

#include <stdlib.h>
#include <stdbool.h>
#include "typedefs.h"
#include "stringBuilder.h"
#include "dynamic_array.h"
#include "var_args.h"
#include "payload_functions.h"

/**
 * Represents the entry point for the whole binary tree
 */
typedef struct binary_tree binary_tree;

/**
 * Represents a node inside the binary tree
 */
typedef struct binary_node binary_node;

/**
 * Initialize the inary tree
 *
 * @param[in] payloadFunctions the functions used to better manage the paylod within the binary tree
 * @return a new instance of the binary tree
 */
binary_tree* cuBinaryTreeNew(payload_functions payloadFunctions);

#define CU_FUNCTION_POINTER_destructor_void_cuBinaryTreeDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID
/**
 * Destroy the tree without removing from memory the elements references by the tree
 *
 * @param[in] binaryTree the tree to dispose of
 */
void cuBinaryTreeDestroy(CU_NOTNULL const binary_tree* binaryTree, CU_NULLABLE const struct var_args* context);

/**
 * Destroy the tree **with** removing from memory the elements references by the tree as well
 *
 * @param[in] binaryTree the tree to dispose of
 */
void cuBinaryTreeDestroyWithElements(CU_NOTNULL const binary_tree* binaryTree, CU_NULLABLE const struct var_args* context);

/**
 * Adds an item inside the given binary tree
 *
 * This operation is \f$ O( n log_2 n) \f$ long
 *
 * @param[in] binarytree the tree involved
 * @param[in] payload the data you want to add inside the tree
 * @return
 * 	\li true if the data has been added inside the tree;
 * 	\li false otherwise;
 */
bool cuBinaryTreeAddItem(CU_NOTNULL binary_tree* binarytree, CU_NULLABLE const void* payload);

/**
 * Check the membership of a data inside the binary tree
 *
 * this operation takes up to \f$ O(n log_2 n) \f$
 *
 * @param[in] binaryTree the tree involved
 * @param[in] payload the data you're looking for
 * @return
 * 	\li true if the payload is inside the tree;
 * 	\li false otherwise;
 */
bool cuBinaryTreeContainsItem(CU_NOTNULL const binary_tree* binaryTree, CU_NULLABLE const void* payload);

/**
 * Find an element satisfying the given criterion in the BST
 *
 * @param[in] binaryTree the tree to analyze
 * @param[in] f the criterion to use
 * @param[in] va the context to use
 * @return
 * 	\li a non NULL value representing the first value found inside the tree satisfying the criterion
 * 	\li NULL if not elements within the tree satisfied the criterion;
 */
CU_NULLABLE void* cuBinaryTreeFindItem(CU_NOTNULL const binary_tree* binaryTree, finder f, const var_args* va);

/**
 * @param[in] binaryTree the tree involved
 * @return the number of elements inside this binary tree
 */
int cuBinaryTreeGetSize(CU_NOTNULL const binary_tree* binaryTree);

/**
 * Removes an item from the binary search tree
 *
 * @param[in] binaryTree the tree involved
 * @param[in] payload the payload we want to look for
 * @return
 * 	\li true if the elements was found and removed;
 * 	\li false otherwise;
 */
bool cuBinaryTreeRemoveItem(CU_NOTNULL binary_tree* binaryTree, CU_NULLABLE void* payload);

/**
 * clear every elements inside the binary search tree (without releasing the memory)
 *
 * @param[in] binarytree the tree to clear
 */
void cuBinaryTreeClear(CU_NOTNULL binary_tree* binarytree);

/**
 * get the root of the binary tree
 *
 * @private
 *
 * @param[in] binaryTree the tree to analyze
 * @return the root of the binary tree (if any)
 */
binary_node* _cuBinaryTreeGetRoot(CU_NOTNULL const binary_tree* binaryTree);

/**
 * get the left branch of a node
 *
 * @private
 *
 * @param[in] binaryNode the node to consider
 * @see _cuBinaryNodeGetRight
 * @return
 * 	@li the left child of the node;
 * 	@li null if it doesn't exist
 */
CU_NULLABLE binary_node* _cuBinaryNodeGetLeft(CU_NOTNULL const binary_node* binaryNode);

/**
 * get the right branch of a node
 *
 * @private
 *
 * @param[in] binaryNode the node to consider
 * @return
 *  @li the right child of the node;
 *  @li null if it doesn't exist
 */
CU_NULLABLE binary_node* _cuBinaryNodeGetRight(CU_NOTNULL const binary_node* binaryNode);

/**
 * get the paylod associated to a node
 *
 * @private
 *
 * @param[in] binaryNode the node to consider
 * @return the paylod associated
 */
CU_NULLABLE void* _cuBinaryNodeGetPayload(CU_NOTNULL const binary_node* binaryNode);

#define ITERATE_THROUGH_BST(_bst, _payload, type) \
	binary_tree* UV(bst) = (_bst); \
	binary_node* UV(tmp) = _cuBinaryTreeGetRoot(UV(bst)); \
	binary_node* UV(tmp2) = NULL; \
	bool UV(outerLoop) = cuBinaryTreeGetSize(UV(bst)) != 0; \
	int UV(visitedIndex) = 0; \
	dynamic_1D_array* UV(visited) = NULL; \
	if (UV(outerLoop)) \
		for ( \
			type _payload \
			; \
			UV(outerLoop) ? (UV(visited) = cuDynamicArrayNew(binary_node*, cuBinaryTreeGetSize(UV(bst))), true) : (cuDynamicArrayDestroy(UV(visited), NULL), false) \
			; \
			UV(outerLoop) = false \
		) \
			for ( \
					cuDynamicArraySetCellToVariableValue(UV(visited), UV(visitedIndex)++, binary_node*, UV(tmp)) \
					; \
					(_payload =  UV(tmp) != NULL ? ((type)_cuBinaryNodeGetPayload(UV(tmp))) : _payload, UV(tmp) != NULL) \
					; \
					UV(visitedIndex)--, \
					UV(tmp2) = _cuBinaryNodeGetLeft(UV(tmp)), \
					cuDynamicArraySetCellToVariableValue(UV(visited), UV(visitedIndex), binary_node*, UV(tmp2)), \
					UV(visitedIndex) += cuDynamicArrayGetCellValue(UV(visited), UV(visitedIndex), binary_node*) != NULL ? 1 : 0, \
					UV(tmp2) = _cuBinaryNodeGetRight(UV(tmp)), \
					cuDynamicArraySetCellToVariableValue(UV(visited), UV(visitedIndex), binary_node*, UV(tmp2)), \
					UV(visitedIndex) += cuDynamicArrayGetCellValue(UV(visited), UV(visitedIndex), binary_node*) != NULL ? 1 : 0, \
					UV(tmp) = UV(visitedIndex) > 0 ? cuDynamicArrayGetCellValue(UV(visited), UV(visitedIndex) - 1, binary_node*) : NULL \
				) \

#endif /* BINARYTREE_H_ */
