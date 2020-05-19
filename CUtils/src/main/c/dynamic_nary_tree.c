/**
 * @file
 *
 * @author koldar
 * @date May 7, 2018
 */

#include "dynamic_nary_tree.h"
#include "macros.h"
#include "errors.h"

struct dynamic_nary_tree_node {
	void* payload;
	int childNumber;
	struct dynamic_nary_tree_node* parent;
	struct dynamic_nary_tree_node* firstChild;
	struct dynamic_nary_tree_node* nextSibling;
};

struct dynamic_nary_tree {
	/**
	 * pointer to the root node
	 */
	struct dynamic_nary_tree_node* root;
	/**
	 * number of elements in the tree
	 */
	int size;
	/**
	 * function to operate with the tree node payloads
	 */
	const payload_functions* payloadFunctions;
};

static struct dynamic_nary_tree_node* newNaryTreeNode(const struct dynamic_nary_tree_node* parent, const void* payload);
static void cuDestroyNaryTreeNodeRecursive(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node, bool destroyPayload);

struct dynamic_nary_tree* cuNaryTreeNew(const void* rootPayload, const payload_functions* payloadFunctions) {
	struct dynamic_nary_tree* result = CU_MALLOC(struct dynamic_nary_tree);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	result->size = 1;
	result->root = newNaryTreeNode(NULL, rootPayload);
	result->payloadFunctions = payloadFunctions;

	return result;
}

struct dynamic_nary_tree_node* cuNaryTreeGetRoot(const struct dynamic_nary_tree* tree) {
	return tree->root;
}

struct dynamic_nary_tree_node* cuNaryTreeGetParentOf(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node) {
	return node->parent;
}

int cuNaryTreeGetChildrenNumber(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node) {
	return node->childNumber;
}

struct dynamic_nary_tree_node* cuNaryTreeGetNthNode(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node, int item) {
	struct dynamic_nary_tree_node* result = node->firstChild;

	while (item > 0) {
		result = result->nextSibling;
		item--;
	}

	return result;
}

struct dynamic_nary_tree_node* cuNaryTreeAddNodeOnHead(struct dynamic_nary_tree* tree, struct dynamic_nary_tree_node* node, const void* payload) {
	struct dynamic_nary_tree_node* oldHead = node->firstChild;
	struct dynamic_nary_tree_node* newHead = newNaryTreeNode(node->parent, payload);

	if (oldHead != NULL) {
		node->firstChild = newHead;
		newHead->nextSibling = oldHead;
	} else {
		node->firstChild = newHead;
	}

	node->childNumber += 1;
	return newHead;
}

struct dynamic_nary_tree_node* cuNaryTreeAddNodeOnTail(struct dynamic_nary_tree* tree, struct dynamic_nary_tree_node* node, const void* payload) {
	struct dynamic_nary_tree_node* oldTail = node->firstChild;
	struct dynamic_nary_tree_node* newTail = newNaryTreeNode(node->parent, payload);

	if (oldTail == NULL) {
		node->firstChild = newTail;
	} else {

		while (oldTail->nextSibling != NULL) {
			oldTail = oldTail->nextSibling;
		}
		oldTail->nextSibling = newTail;
	}

	return newTail;
}

struct dynamic_nary_tree_node* cuNaryTreeGetFirstChild(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node) {
	return node->firstChild;
}

struct dynamic_nary_tree_node* cuNaryTreeGetLastChild(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node) {
	struct dynamic_nary_tree_node* result = node->firstChild;

	//there a no children
	if (result == NULL) {
		return NULL;
	}

	while (result->nextSibling != NULL) {
		result = result->nextSibling;
	}

	return result;
}

dynamic_nary_tree_node_list* cuNaryTreeGetChildrenList(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node) {
	struct dynamic_nary_tree_node* tmp = node->firstChild;
	dynamic_nary_tree_node_list* result = cuListNew();

	//there a no children
	if (tmp == NULL) {
		return result;
	}

	while (tmp->nextSibling != NULL) {
		cuListAddTail(result, tmp);
		tmp = tmp->nextSibling;
	}

	return result;
}

bool cuNaryTreeIsNodeLeaf(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node) {
	return node->firstChild == NULL;
}

bool cuNaryTreeIsNodeRoot(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node) {
	return node->parent == NULL;
}

bool cuNaryTreeIsNodeIntraTree(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node) {
	return (node->parent != NULL) && (node->firstChild != NULL);
}

int cuNaryTreeGetSize(const struct dynamic_nary_tree* tree) {
	return tree->size;
}

bool cuNaryTreeHasOnlyRoot(const struct dynamic_nary_tree* tree) {
	return tree->size == 1;
}

void cuNaryTreeDestroy(const struct dynamic_nary_tree* tree, CU_NULLABLE const struct var_args* context) {
	cuDestroyNaryTreeNodeRecursive(tree, tree->root, false);
	free((void*)tree);
}

void cuDestroyNaryTreeWithElements(const struct dynamic_nary_tree* tree, CU_NULLABLE const struct var_args* context) {
	cuDestroyNaryTreeNodeRecursive(tree, tree->root, true);
	CU_FREE(tree);
}

static void cuDestroyNaryTreeNodeRecursive(const struct dynamic_nary_tree* tree, const struct dynamic_nary_tree_node* node, bool destroyPayload) {
	const struct dynamic_nary_tree_node* tmp;
	const struct dynamic_nary_tree_node* tmp2;

	if (destroyPayload) {
		tree->payloadFunctions->destroy(node->payload, NULL);
	}

	if (node->firstChild != NULL) {
		tmp = node->firstChild->nextSibling;
		cuDestroyNaryTreeNodeRecursive(tree, node->firstChild, destroyPayload);

		while (tmp != NULL) {
			tmp2 = tmp->nextSibling;
			cuDestroyNaryTreeNodeRecursive(tree, tmp, destroyPayload);
			tmp = tmp2;
		}

	}

	CU_FREE(node);
}

static struct dynamic_nary_tree_node* newNaryTreeNode(const struct dynamic_nary_tree_node* parent, const void* payload) {
	struct dynamic_nary_tree_node* result = CU_MALLOC(struct dynamic_nary_tree_node);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	CU_SAFE_ASSIGN(result->payload, payload);
	CU_SAFE_ASSIGN(result->parent, parent);
	result->childNumber = 0;
	result->firstChild = NULL;
	result->nextSibling = NULL;

	return result;
}

