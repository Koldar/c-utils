/*
 * redBlackTree.c
 *
 *  Created on: Jun 9, 2017
 *      Author: koldar
 */



#include "macros.h"
#include "redBlackTree.h"
#include "log.h"
#include "errors.h"
#include "var_args.h"

typedef enum {
	RED,
	BLACK
} rb_color;

typedef struct rb_node {
	struct rb_node* parent;
	struct rb_node* left;
	struct rb_node* right;
	rb_color color;

	void* payload;
} rb_node;

struct rb_tree {
	///the root of the the red black tree itself
	rb_node* root;
	///the number of elements inside the red black tree
	int size;
	/**
	 * Set of functions to easily manage the payload of the red black tree
	 */
	payload_functions functions;
};

/**
 * A sentinel used by the structure to point something not existing.
 *
 * Normally if a node has no left child, we set the left child pointer to NULL.
 * Here, to ease the algorithm implementation, we point to the ::_NIL structure.
 * Why? Because we can read the color of NIL: by default, NIL sentinel has BLACK color.
 *
 * We don't have to create exception to deal with the sentinel: it just works with the flow of the algorithm:
 * we can fetch the color of the sentinel just like with any other nodes; the same does not apply with the explicit NULL
 * value.
 */
static rb_node _NIL = {NULL, NULL, NULL, BLACK, NULL};
/**
 * pointer to ::NIL
 */
static rb_node* NIL = &_NIL;

static rb_node* newRedBlackNode(CU_NULLABLE const void* payload);
static void leftRotate(CU_NOTNULL rb_tree* tree, CU_NOTNULL rb_node* y);
static void rightRotate(CU_NOTNULL rb_tree* tree, CU_NOTNULL rb_node* y);
static void destroyRedBlackNode(CU_NOTNULL rb_node* n, bool withElements, CU_NULLABLE destructor d);
static void redBlackInsertFixUp(CU_NOTNULL rb_tree* tree, CU_NOTNULL rb_node* z);
static void redBlackTransplant(CU_NOTNULL rb_tree* tree, CU_NOTNULL rb_node* replaced, CU_NOTNULL rb_node* replacer);
static rb_node* getMinimumInRedBlackNode(CU_NOTNULL rb_node* n);
static void redBlackDeleteFixUp(CU_NOTNULL rb_tree* tree, CU_NOTNULL rb_node* x);
static bool _removeItemInRedBlackTree(CU_NOTNULL rb_tree* tree, CU_NULLABLE void* payload, bool withElement);
static rb_node* getMaximumInRedBlackNode(CU_NOTNULL rb_node* n);

rb_tree* cuRedBlackTreeNew(payload_functions functions) {
	rb_tree* retVal = CU_MALLOC(rb_tree);
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->functions = functions;

	retVal->root = NIL;
	retVal->size = 0;

	return retVal;
}

void cuRedBlackTreeDestroy(CU_NOTNULL const rb_tree* tree, CU_NULLABLE const struct var_args* context) {
	destroyRedBlackNode(tree->root, false, NULL);
	CU_FREE(tree);
}

void cuRedBlackTreeDestroyWithElements(CU_NOTNULL const rb_tree* tree, CU_NULLABLE const struct var_args* context) {
	destroyRedBlackNode(tree->root, true, tree->functions.destroy);
	CU_FREE(tree);
}

static void destroyRedBlackNode(CU_NOTNULL rb_node* n, bool withElements, CU_NULLABLE destructor d) {
	if (n == NIL) {
		return;
	}
	if (n->left != NIL) {
		destroyRedBlackNode(n->left, withElements, d);
	}
	if (n->right != NIL) {
		destroyRedBlackNode(n->right, withElements, d);
	}
	if (withElements) {
		d(n->payload, NULL); //TODO context null
	}
	CU_FREE(n);
}

bool cuRedBlackTreeAddItem(CU_NOTNULL rb_tree* tree, CU_NULLABLE void* payload) {
	rb_node* x = tree->root;
	rb_node* y = NIL;
	rb_node* z = newRedBlackNode(payload);
	while (x != NIL) {
		y = x;
		if (tree->functions.order(z->payload, y->payload) < 0) {
			x = x->left;
		} else {
			x = x->right;
		}
	}
	//ok, we have reached the bottom of the tree. We need to establish if we need to put z on the left or on the right
	z->parent = y;
	if (y == NIL) {
		//we might not have run the loop even once. This means the root was NULL all along.
		tree->root = z;
	} else if (tree->functions.order(z->payload, y->payload) < 0) {
		y->left = z;
	} else {
		y->right = z;
	}
	z->color = RED;
	redBlackInsertFixUp(tree, z);
	tree->size++;
	return true;
}

int cuRedBlackTreeGetSize(CU_NOTNULL const rb_tree* tree) {
	return tree->size;
}

bool cuRedBlackTreeIsEmpty(CU_NOTNULL const rb_tree* tree) {
	return tree->size == 0;
}

bool cuRedBlackTreeContainsItem(CU_NOTNULL const rb_tree* tree, CU_NULLABLE void* payload) {
	rb_node* x = tree->root;
	int order = 0;
	while (x != NIL) {
		order = tree->functions.order(payload, x->payload);
		if (order == 0) {
			return true;
		}
		if (order < 0) {
			x = x->left;
		} else {
			x = x->right;
		}
	}
	return false;
}

bool cuRedBlackTreeRemoveItem(CU_NOTNULL rb_tree* tree, CU_NULLABLE void* payload) {
	return _removeItemInRedBlackTree(tree, payload, false);
}

bool cuRedBlackTreeRemoveItemWithElement(CU_NOTNULL rb_tree* tree, CU_NULLABLE void* payload) {
	return _removeItemInRedBlackTree(tree, payload, true);
}

void* cuRedBlackTreeGetMinimum(CU_NOTNULL const rb_tree* tree) {
	if (tree->root == NIL) {
		return NULL;
	}
	return getMinimumInRedBlackNode(tree->root)->payload;
}

void* cuRedBlackTreeGetMaximum(CU_NOTNULL const rb_tree* tree) {
	if (tree->root == NIL) {
		return NULL;
	}
	return getMaximumInRedBlackNode(tree->root)->payload;
}

static bool _removeItemInRedBlackTree(CU_NOTNULL rb_tree* tree, CU_NULLABLE void* payload, bool withElement) {
	rb_node* z;
	register int order;

	//find z inside the red black tree
	z = tree->root;
	while (z != NIL) {
		order = tree->functions.order(payload, z->payload);
		if (order == 0) {
			break;
		} else if (order < 0) {
			z = z->left;
		} else {
			z = z->right;
		}
	}

	if (z == NIL) {
		//the payload has not been found within the red black tree
		return false;
	}

	rb_node* x;
	rb_node* y;
	rb_color yoriginal;
	y = z;
	yoriginal = y->color;

	if (z->left == NIL) {
		x = z->right;
		debug("z left: %p z right: %p", z->left, z->right);
		redBlackTransplant(tree, z, z->right);
	} else if (z->right == NIL) {
		x = z->left;
		redBlackTransplant(tree, z, z->left);
	} else {
		y = getMinimumInRedBlackNode(z->right);
		yoriginal = y->color;
		x = y->right;
		if (y->parent == z) {
			x->parent = y;
		} else {
			redBlackTransplant(tree, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}
		redBlackTransplant(tree, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}

	if (yoriginal == BLACK) {
		redBlackDeleteFixUp(tree, x);
	}

	//free z from memory
	if (withElement) {
		tree->functions.destroy(z->payload, NULL);
	}
	free(z);

	tree->size--;
	return true;
}

static void redBlackDeleteFixUp(CU_NOTNULL rb_tree* tree, CU_NOTNULL rb_node* x) {
	rb_node* w;

	while ((x != tree->root) && (x->color == BLACK)) {
		if (x == x->parent->left) {
			w = x->parent->right;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				leftRotate(tree, x->parent);
				w = x->parent->right;
			}
			if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->parent;
			} else {
				if (w->right->color == BLACK) {
					w->left->color = BLACK;
					w->color = RED;
					rightRotate(tree, w);
					w = x->parent->right;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				leftRotate(tree, x->parent);
				x = tree->root;
			}
		} else {
			w = x->parent->left;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				leftRotate(tree, x->parent);
				w = x->parent->left;
			}
			if (w->right->color == BLACK && w->left->color == BLACK) {
				w->color = RED;
				x = x->parent;
			} else {
				if (w->left->color == BLACK) {
					w->right->color = BLACK;
					w->color = RED;
					leftRotate(tree, w);
					w = x->parent->left;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				rightRotate(tree, x->parent);
				x = tree->root;
			}
		}
	}

	x->color = BLACK;
}

static rb_node* getMinimumInRedBlackNode(CU_NOTNULL rb_node* n) {
	if (n->left == NIL) {
		return n;
	}
	return getMinimumInRedBlackNode(n->left);
}

static rb_node* getMaximumInRedBlackNode(CU_NOTNULL rb_node* n) {
	if (n->right == NIL) {
		return n;
	}
	return getMaximumInRedBlackNode(n->right);
}

static void redBlackTransplant(CU_NOTNULL rb_tree* tree, CU_NOTNULL rb_node* replaced, CU_NOTNULL rb_node* replacer) {
	if (replaced->parent == NIL) {
		tree->root = replacer;
	} else if (replaced == replaced->parent->left) {
		replaced->parent->left = replacer;
	} else {
		replaced->parent->right = replacer;
	}

	debug("replacer is %p", replacer);
	replacer->parent = replaced->parent;
}

static void redBlackInsertFixUp(CU_NOTNULL rb_tree* tree, CU_NOTNULL rb_node* z) {
	rb_node* y;
	//z parent is NULL only if z is the root itself.
	while (z->parent->color == RED) {
		if (z->parent == z->parent->parent->left) {
			//the parent of z is the left child of its parent. Then y is the right child
			debug("fetching uncle of z: %p", z->parent->parent->right);
			y = z->parent->parent->right;
			if (y->color == RED) {
				z->parent->color= BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			} else {
				if (z == z->parent->right) {
					z = z->parent;
					leftRotate(tree, z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rightRotate(tree, z->parent->parent);
			}
		} else {
			//the parent of z is the right child of its parent. Then y is the left child
			y = z->parent->parent->left;
			if (y->color == RED) {
				z->parent->color= BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			} else {
				if (z == z->parent->left) {
					z = z->parent;
					rightRotate(tree, z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				leftRotate(tree, z->parent->parent);
			}
		}
	}
	tree->root->color = BLACK;
}

/**
 * Performs left rotation
 *
 * Given the following graph:
 * @dot
 * digraph {
 * 	X;Y;ALPHA;BETA;GAMMA;
 *
 * 	X -> ALPHA;
 * 	X -> Y;
 * 	Y -> BETA;
 * 	Y -> GAMMA;
 * }
 * @enddot
 *
 * Left roation changes the tree by altering into:
 *
 * @dot
 * digraph {
 * 	X;Y;ALPHA;BETA;GAMMA;
 *
 * 	X -> ALPHA;
 * 	Y -> X;
 * 	X -> BETA;
 * 	Y -> GAMMA;
 * }
 * @enddot
 *
 *
 * @param[inout] tree the tree to alter
 * @param[in] x the node you have just added inside the tree
 */
static void leftRotate(CU_NOTNULL rb_tree* tree, CU_NOTNULL rb_node* x) {
	rb_node* y;

	//find y
	y = x->right;
	//we move BETA. So we replaced X->Y with X->BETA
	x->right = y->left;
	if (y->left != NIL) {
		y->left->parent = x;
	}

	//we move Y. So we replace PARENT->X with PARENT->Y;
	y->parent = x->parent;
	//we need to change the parent branch pointer as well.
	if (x->parent == NIL) {
		//X was the root. So now Y is the new root
		tree->root = y;
	} else if (x == x->parent->left) {
		//If x was the left child of parent, now y is the left child of the parent
		x->parent->left = y;
	} else {
		//If x was the right child of parent, now y is the right child of the parent
		x->parent->right = y;
	}
	//we move Y. So we replace X->Y with Y->X
	y->left = x;
	x->parent = y;
}

/**
 * Performs right rotation
 *
 * Given the following graph:
 * @dot
 * digraph {
 * 	X;Y;ALPHA;BETA;GAMMA;
 *
 * 	X -> ALPHA;
 * 	Y -> X;
 * 	X -> BETA;
 * 	Y -> GAMMA;
 * }
 * @enddot
 *
 * Left roation changes the tree by altering into:
 *
 * @dot
 * digraph {
 * 	X;Y;ALPHA;BETA;GAMMA;
 *
 * 	X -> ALPHA;
 * 	X -> Y;
 * 	Y -> BETA;
 * 	Y -> GAMMA;
 * }
 * @enddot
 *
 * @param[inout] tree the tree to alter
 * @param[in] x the node you have just added inside the tree
 */
static void rightRotate(CU_NOTNULL rb_tree* tree, CU_NOTNULL rb_node* y) {
	rb_node* x;

	//find x
	x = y->left;
	//we move BETA. So we replaced Y->X with Y->BETA
	y->left = x->right;
	if (x->right != NIL) {
		x->right->parent = y;
	}

	//we move X. So we replace PARENT->Y with PARENT->X;
	x->parent = y->parent;
	//we need to change the parent branch pointer as well.
	if (y->parent == NIL) {
		//X was the root. So now Y is the new root
		tree->root = x;
	} else if (y == y->parent->left) {
		//If x was the left child of parent, now y is the left child of the parent
		y->parent->left = x;
	} else {
		//If x was the right child of parent, now y is the right child of the parent
		y->parent->right = x;
	}
	//we move Y. So we replace X->Y with Y->X
	x->right = y;
	y->parent = x;
}

static rb_node* newRedBlackNode(CU_NULLABLE const void* payload) {
	rb_node* retVal = malloc(sizeof(rb_node));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->left = NIL;
	retVal->right = NIL;
	retVal->payload = (void*) payload;
	retVal->color = BLACK;
	retVal->parent = NIL;

	return retVal;
}


