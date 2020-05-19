/*
 * \file redBlackTree.h
 *
 * The module allows you to use red black trees
 *
 * For further information about the red black trees see the following references:
 *
 * \li Cormen, introduction to algorithms, 3rd edition, Chapter 13;
 *
 * <h3>Why use them?</h3>
 *
 * Red black trees are an enhanced vesion of BST: they allow to maintain a balanced tree even with lots of insertion and removal:
 * this lead to a cost of insertion/search/removal of just \f$ O(n log_2 n) \f$ averagely. Viceversa, BSTs suffer from potential tree
 * unbalacing: when a BST has lots of insertion and removal, it may become a list, thereby creating a cost of insertion/lookup/deletion of \f$ O(n)\f$.
 *
 * **So use red black trees when your tree has lots of insertion/deletion**
 *
 * <h3>Implementation details</h3>
 * The only thing to remember is that while Cormen says that there exist a NIL sentinel that is black, here we implicitly say that the NULL value
 * is by default black.
 *
 *  Created on: Jun 9, 2017
 *      Author: koldar
 */

#ifndef REDBLACKTREE_H_
#define REDBLACKTREE_H_

#include <stdbool.h>
#include "payload_functions.h"
#include "typedefs.h"
#include "macros.h"


typedef struct rb_tree rb_tree;

/**
 * Creates a new red black tree in the memory
 *
 * @param[in] functions set of functions to easily manage the payload
 * @return the instance of the red black tree
 */
rb_tree* cuRedBlackTreeNew(payload_functions functions);

/**
 * Destroy the tree
 *
 * \note
 * All the items in the tree **won't be released** from the memory at all!
 * If you want to release them as well, use ::cuRedBlackTreeDestroyWithElements
 *
 * @param[in] tree the tree to free
 */
void cuRedBlackTreeDestroy(CU_NOTNULL const rb_tree* tree, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuRedBlackTreeDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * like ::cuRedBlackTreeDestroy except that all items in the tree will be release from the memory as well
 *
 * @param[in] tree the tree to free
 */
void cuRedBlackTreeDestroyWithElements(CU_NOTNULL const rb_tree* tree, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuRedBlackTreeDestroyWithElements_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Adds an item inside the tree
 *
 * @param[inout] tree the tree to alter
 * @param[in] payload the item to add in the tree
 */
bool cuRedBlackTreeAddItem(CU_NOTNULL rb_tree* tree, CU_NULLABLE void* payload);

/**
 * @param[in] tree the tree to analyze
 * @return the number of items inside the tree
 */
int cuRedBlackTreeGetSize(CU_NOTNULL const rb_tree* tree);

/**
 * @param[in] tree the tree to analyze
 * @return
 *  \li true if the tree has no elements inside it;
 *  \li false otherwise
 */
bool cuRedBlackTreeIsEmpty(CU_NOTNULL const rb_tree* tree);

/**
 * @param[in] tree the tree to analyze
 * @param[in] payload the item to look for
 * @return
 * 	\li true if \c tree has the item \c payload;
 * 	\li false otherwise
 */
bool cuRedBlackTreeContainsItem(CU_NOTNULL const rb_tree* tree, CU_NULLABLE void* payload);

/**
 * @param[in] tree the tree to analyze
 * @return
 * 	\li the minimum item inside the \c tree;
 * 	\li NULL if the \c tree is empty
 */
void* cuRedBlackTreeGetMinimum(CU_NOTNULL const rb_tree* tree);

/**
 * @param[in] tree the tree to analyze
 * @return
 * 	\li the maximum item inside the \c tree;
 * 	\li NULL if the \c tree is empty
 */
void* cuRedBlackTreeGetMaximum(CU_NOTNULL const rb_tree* tree);

/**
 * Removes an element from the \c tree
 *
 * @param[in] tree the tree to alter
 * @param[in] payload the item to look for
 * @return
 * 	\li true if the element has been found and has been removed in the tree;
 * 	\li false if we couldn't find the \c paylaod in the tree
 */
bool cuRedBlackTreeRemoveItem(CU_NOTNULL rb_tree* tree, CU_NULLABLE void* payload);

/**
 * like ::cuRedBlackTreeRemoveItemWithElement but we release the \c payload from the memory as well
 *
 * @param[in] tree the tree to alter
 * @param[in] payload the item to look for
 * @return
 * 	\li true if the element has been found and has been removed in the tree;
 * 	\li false if we couldn't find the \c paylaod in the tree
 */
bool cuRedBlackTreeRemoveItemWithElement(CU_NOTNULL rb_tree* tree, CU_NULLABLE void* payload);


#endif /* REDBLACKTREE_H_ */
