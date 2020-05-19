#include "parseNode.h"
#include "lext.h"
#include "macros.h"
#include "errors.h"

static void destroyOnStackLext(struct value_node vn);
static void _destroyParseNode(const struct parse_node* n);
static struct parse_node* cloneValueParseNode(const struct parse_node* n, const struct parse_node* parent);

struct parse_node* newNonTerminalParseNode(enum parse_node_type type, const struct parse_node* parent, const struct var_args* va) {
	struct parse_node* retVal = malloc(sizeof(struct parse_node));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}
	retVal->type = type;
	retVal->valueNode.valueType = VNT_DECIMAL_VALUE;
	retVal->valueNode.value = getEmptyLexT();
	retVal->firstChild = NULL;
	retVal->nextSibling = NULL;
	CU_SAFE_ASSIGN(retVal->parent, parent);

	struct parse_node* next;
	CU_ITERATE_ON_VARIADIC(va, i, v) {
		if (retVal->firstChild == NULL) {
			retVal->firstChild = cuVarArgsGetItem(va, i, struct parse_node*);
			next = retVal->firstChild;
		} else {
			next->nextSibling = cuVarArgsGetItem(va, i, struct parse_node*);
			next = next->nextSibling;
		}
	}

	return retVal;
}

struct parse_node* newShortOptionMultiValueParseNode(const struct parse_node* parent,const struct var_args* va) {
	struct parse_node* retVal = CU_MALLOC(struct parse_node);
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	struct parse_node* shortOptions = cuVarArgsGetItem(va, 0, struct parse_node*);
	struct parse_node* valueParseNode = NULL;
	if (cuVarArgsGetSize(va) == 2) {
		//there is a value as well
		valueParseNode = cuVarArgsGetItem(va, 1, struct parse_node*);
	}


	struct parse_node* result = NULL;
	struct parse_node* tail = NULL;
	for (int i=0; shortOptions->valueNode.value.sval[i] != '\0'; i++) {
		cuInitVarArgsOnStack(va);
		if (result == NULL) {
			result = newNonTerminalParseNode(PNT_SHORT_OPTION, parent, va);
			tail = result;
		} else {
			tail->nextSibling = newNonTerminalParseNode(PNT_SHORT_OPTION, parent, va);
			tail = tail->nextSibling;
		}
		//tail represents the parse node we've jsut created as well

		//we set the first child of the short option
		union lext tmp;
		tmp.cval = shortOptions->valueNode.value.sval[i];
		tail->firstChild = newValueParseNode(VNT_CHARACTER_VALUE, tail, tmp);
		//if the second vararg is present, we clone the value parse node
		if (valueParseNode != NULL) {
			tail->firstChild->nextSibling = cloneValueParseNode(valueParseNode, tail);
		}
	}

	return retVal;

}

struct parse_node* newValueParseNode(enum value_node_type type, const struct parse_node* parent, union lext value) {
	struct parse_node* result = CU_MALLOC(struct parse_node);
	if (result == NULL) {
		ERROR_MALLOC();
	}

	result->type = PNT_VALUE;
	CU_SAFE_ASSIGN(result->parent, parent);
	result->firstChild = NULL;
	result->nextSibling = NULL;
	result->valueNode.valueType = type;
	result->valueNode.value = value;

	return result;
}

/**
 * Destroy whatever has been allocated in a lext
 *
 * @note
 * This function operates ::value_node which has not been created with \c malloc.
 * Its content, on the other hand, **might** be on the heap.
 *
 * @param[in] vn the value node to remove
 */
static void destroyOnStackLext(struct value_node vn) {
	if (vn.valueType == VNT_STRING_VALUE) {
		free(vn.value.sval);
	}
}

static void _destroyParseNode(const struct parse_node* n) {
	switch (n->type) {
	case PNT_VALUE: {
		destroyOnStackLext(n->valueNode);
		CU_FREE(n);
		break;
	}
	case PNT_LONG_OPTION:
	case PNT_SHORT_OPTION:
	case PNT_CLI:
	case PNT_STATEMENTS: {
		struct parse_node* tmp = n->firstChild;
		while (tmp != NULL) {
			_destroyParseNode(tmp);
			tmp = tmp->nextSibling;
		}
		CU_FREE(n);
		break;
	}
	default: {
		ERROR_INVALID_SWITCH_CASE("parse node type", "%d", n->type);
	}
	}
}

void destroyParseNode(const struct parse_node* n, CU_NULLABLE const struct var_args* context) {
	_destroyParseNode(n);
}

/**
 * Clone the value parse node
 *
 * @note
 * This funciton works only with \c n with type ::PNT_VALUE
 *
 * @param[in] n the node we're cloning from
 * @param[in] parent the new parent of the parse node cloned
 */
static struct parse_node* cloneValueParseNode(const struct parse_node* n, const struct parse_node* parent) {
	if (n->type != PNT_VALUE) {
		ERROR_ON_APPLICATION("clone", "%s", __func__, "parse node", "%p", n);
	}

	struct parse_node* retVal = CU_MALLOC(struct parse_node);
	if (retVal == NULL) {
		ERROR_MALLOC();
	}
	retVal->type = n->type;
	retVal->valueNode = n->valueNode;
	retVal->firstChild = NULL;
	retVal->nextSibling = NULL;
	CU_SAFE_ASSIGN(retVal->parent, parent);

	return retVal;
}
