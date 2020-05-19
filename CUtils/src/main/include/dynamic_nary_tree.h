/**
 * @file
 *
 * Represents a tree where every node may have multiple, albeit different number of nodes.
 * Dynamic stands for nodes tree which do not have a maximum number of children
 *
 * This tree always has a single node called "root"
 *
 * @author koldar
 * @date May 7, 2018
 */

#ifndef DYNAMIC_NARY_TREE_H_
#define DYNAMIC_NARY_TREE_H_

#include <stdbool.h>
#include "list.h"
#include "payload_functions.h"

/**
 * A struct representing the whole tree
 */
struct dynamic_nary_tree;
/**
 * A struct representing a single node within the tree
 */
struct dynamic_nary_tree_node;
/**
 * A list where each item is of type ::dynamic_nary_tree_node
 */
typedef list dynamic_nary_tree_node_list;

/**
 * Creates a new tree
 *
 * @param[in] rootPayload the paylaod associated to the root of the tree
 * @param[in] payloadFunctions a set of funcitons used to gneerally handle the payload of the structure
 * @return the instance of ::dynamic_nary_tree
 */
struct dynamic_nary_tree* cuNaryTreeNew(const void* rootPayload, const payload_functions* payloadFunctions);

/**
 * get the root of this tree
 *
 * @param[in] tree the tree we're handling
 * @return the node representing the root of the tree
 */
struct dynamic_nary_tree_node* cuNaryTreeGetRoot(const struct dynamic_nary_tree* tree);

/**
 * get the parent of a given node in the tree
 *
 * @note
 * NULL if \c node is the root itself
 *
 * @param[in] tree the tree we're handling
 * @param[in] node to consider
 * @return
 * 	@li the parent of \c node;
 * 	@li NULL if \c node is the root;
 */
struct dynamic_nary_tree_node* cuNaryTreeGetParentOf(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node);

/**
 * the number fo children a node has
 *
 * @param[in] tree the tree to consider
 * @param[in] node the node to consider
 * @return the number of children \c node has
 */
int cuNaryTreeGetChildrenNumber(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node);

/**
 * the i-th children of a given node
 *
 * @note
 * UB if you give an invalid \c item
 *
 * @param[in] tree the tree to consider
 * @param[in] node the node to consider
 * @param[in] item the index
 * @return the i-th child of \c node
 */
struct dynamic_nary_tree_node* cuNaryTreeGetNthNode(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node, int item);

/**
 * add on the **head** of the list of children the new child of a node
 *
 * @param[inout] tree the tree to update
 * @param[inout] node the parent node which will have a new child
 * @param[in] payload the payload of the new child
 * @return the newly create child
 */
struct dynamic_nary_tree_node* cuNaryTreeAddNodeOnHead(struct dynamic_nary_tree* tree, struct dynamic_nary_tree_node* node, const void* payload);

/**
 * add on the **tail** of the list of children the new child of a node
 *
 * @param[inout] tree the tree to update
 * @param[inout] node the parent node which will have a new child
 * @param[in] payload the payload of the new child
 * @return the newly create child
 */
struct dynamic_nary_tree_node* cuNaryTreeAddNodeOnTail(struct dynamic_nary_tree* tree, struct dynamic_nary_tree_node* node, const void* payload);

/**
 * get the first child of a given node
 *
 * @param[in] tree the tree to consider
 * @param[in] node the node to consider
 * @return the first child of \c node
 */
struct dynamic_nary_tree_node* cuNaryTreeGetFirstChild(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node);

/**
 * get the last child of a given node
 *
 * @note
 * you will need to manually free the memory from the list generated by this method
 *
 * @param[in] tree the tree to consider
 * @param[in] node the node to consider
 * @return the last child of \c node
 */
struct dynamic_nary_tree_node* cuNaryTreeGetLastChild(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node);

/**
 * get the list of children of a given node
 *
 * @param[in] tree the tree we're handling right now
 * @param[in] node the node to consider
 * @return the (possibley empty) list of nodes which are the children of \c node
 */
dynamic_nary_tree_node_list* cuNaryTreeGetChildrenList(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node);

/**
 * check if a node is a leaf one
 *
 * @param[in] tree the tree we're handling right now
 * @param[in] node the node to consider
 * @return
 * 	@li true if \c node has no children;
 * 	@li false otherwise
 */
bool cuNaryTreeIsNodeLeaf(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node);

/**
 * check if a node is a root one
 *
 * @param[in] tree the tree we're handling right now
 * @param[in] node the node to consider
 * @return
 * 	@li true if \c node has no parent;
 * 	@li false otherwise
 */
bool cuNaryTreeIsNodeRoot(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node);

/**
 * check if a node is an intra tree one
 *
 * @param[in] tree the tree we're handling right now
 * @param[in] node the node to consider
 * @return
 * 	@li true if \c node has both children and parent;
 * 	@li false otherwise
 */
bool cuNaryTreeIsNodeIntraTree(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node);

/**
 * the number of nodes in the tree (root included)
 *
 * @param[in] tree the tree we're handling right now
 * @return the number of nodes in \c tree (root included)
 */
int cuNaryTreeGetSize(const struct dynamic_nary_tree* tree);

/**
 * check if the tree has other nodes aside root
 *
 * @param[in] tree the tree we're handling right now
 * @return
 * 	@li true if \c tree has one only node which is the root
 * 	@li false otheriwse
 */
bool cuNaryTreeHasOnlyRoot(const struct dynamic_nary_tree* tree);

/**
 * Destroy the tree
 *
 * @note
 * the payloads won't be freed from the memory though
 *
 * @param[in] tree the tree we're handling right now
 * @see destroyNaryTreeWithElements
 */
void cuNaryTreeDestroy(const struct dynamic_nary_tree* tree, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuDestroyNaryTree_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

/**
 * Destroy the tree
 *
 * @note
 * the payloads will be freed from the memory as well
 *
 * @param[in] tree the tree we're handling right now
 * @see destroyNaryTree
 */
void cuDestroyNaryTreeWithElements(const struct dynamic_nary_tree* tree, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuDestroyNaryTreeWithElements_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID


#endif /* DYNAMIC_NARY_TREE_H_ */
