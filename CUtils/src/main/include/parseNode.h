/**
 * @file
 *
 * Parse node used in the command line parser.
 *
 * The AST is seomthing as follows:
 *
 * @code
 * -a -bde -c 1 --verbose --ranker=4
 * @endcode
 *
 * @dot
 * digraph {
 * 	rankdir="TB";
 * 	CLI [label="PNT_CLI"];
 * 	STATEMENTS [label="PNT_STATEMENTS"];
 * 	CLI -> STATEMENTS
 * 	subgraph {
 * 		rankdir="same";
 * 		SHORT_FLAG [label="PNT_SHORT_OPTION"];
 * 		SHORT_FLAGS_A [label="PNT_SHORT_OPTION"];
 * 		SHORT_FLAGS_B [label="PNT_SHORT_OPTION"];
 * 		SHORT_FLAGS_C [label="PNT_SHORT_OPTION"];
 * 		SHORT_VALUE [label="PNT_SHORT_OPTION"];
 * 		LONG_FLAG [label="PNT_LONG_OPTION"];
 * 		LONG_VALUE [label="PNT_LONG_OPTION"];
 * 	}
 * 	subgraph{
 * 		rankdir="same"
 * 		SHORT_FLAG_1 [label="PNT_VALUE\na"];
 * 		SHORT_FLAGS_A_1 [label="PNT_VALUE\\nb"];
 * 		SHORT_FLAGS_B_1 [label="PNT_VALUE\\nd"];
 * 		SHORT_FLAGS_C_1 [label="PNT_VALUE\\ne"];
 * 		SHORT_VALUE_1 [label="PNT_VALUE\\nc"];
 * 		SHORT_VALUE_2 [label="PNT_VALUE\\n1"];
 * 		LONG_FLAG_1 [label="PNT_VALUE\\verbose"];
 * 		LONG_VALUE_1 [label="PNT_VALUE\\nranker"];
 * 		LONG_VALUE_2 [label="PNT_VALUE\\n4"];
 * 	}
 *
 * 	STATEMENTS -> SHORT_FLAG;
 * 	STATEMENTS -> SHORT_FLAGS_A;
 * 	STATEMENTS -> SHORT_FLAGS_B;
 * 	STATEMENTS -> SHORT_FLAGS_C;
 * 	STATEMENTS -> SHORT_VALUE;
 * 	STATEMENTS -> LONG_FLAG;
 * 	STATEMENTS -> LONG_VALUE;
 *
 * 	SHORT_FLAG -> SHORT_FLAG_1
 * 	SHORT_FLAGS_A -> SHORT_FLAGS_A_1;
 * 	SHORT_FLAGS_B -> SHORT_FLAGS_B_1;
 * 	SHORT_FLAGS_C -> SHORT_FLAGS_C_1;
 *
 * 	SHORT_VALUE -> SHORT_VALUE_1;
 * 	SHORT_VALUE -> SHORT_VALUE_2;
 *
 * 	LONG_FLAG -> LONG_FLAG_1;
 *
 * 	LONG_VALUE -> LONG_VALUE_1;
 * 	LONG_VALUE -> LONG_VALUE_2;
 *
 * }
 * @enddot
 */

#ifndef PARSENODE_H__
#define PARSENODE_H__

#include "lext.h"
#include "var_args.h"

enum value_node_type {
	VNT_DECIMAL_VALUE,
	VNT_INTEGER_VALUE,
	VNT_BOOLEAN_VALUE,
	VNT_STRING_VALUE,
	VNT_CHARACTER_VALUE,
};

enum parse_node_type {
	PNT_VALUE,
	PNT_LONG_OPTION,
	PNT_SHORT_OPTION,
	PNT_STATEMENTS,
	PNT_CLI
};

struct value_node {
	enum value_node_type valueType;
	union lext value;
};

struct parse_node {
	enum parse_node_type type;
	struct value_node valueNode;

	struct parse_node* parent;
	struct parse_node* firstChild;
	struct parse_node* nextSibling;
};

/**
 * Initialize a new parse node with a given type and a list of children
 *
 * @param[in] type the type the newly generate parse node needs to have
 * @param[in] parent the parent of this new ::parse_node
 * @param[in] va a list of variadic all containing ::parse_node. Each of them is considered as a child of the newly generated node
 * @return the newly generated parse node
 */
struct parse_node* newNonTerminalParseNode(enum parse_node_type type, const struct parse_node* parent, const struct var_args* va);

/**
 * Initialize a new terminal value node
 *
 * @param[in] type the type of the value within \c value
 * @param[in] parent the parent of this parse node
 * @param[in] value the value within this particular value node
 * @return the value node just built
 */
struct parse_node* newValueParseNode(enum value_node_type type, const struct parse_node* parent, union lext value);

/**
 * Initialize a parse node such that:
 *
 * @li first argument of \c va is a parse_node which is a value node containing a string. Such string represents all the short options (like "cxz");
 * @li the second argument of \c va is whatever value node
 *
 * @note
 * The function will generate a list of PNT_SHORT_OPTION parse_nodes, one sibling to the other. It will return only the **first** parse_node though.
 *
 * Each PNT_SHORT_OPTION parse_node will contain one vlaue parse node (of type "character") and the same value node from the second argument (it is copied by value though).
 *
 * So from:
 * @dot
 * digraph {
 *		OPTION_NODE [label="czf"];
 *		VALUE_NODE [label="4"];
 * }
 * @enddot
 *
 * it generates:
 * @dot
 * digraph {
 * 	O1 [label="PNT_SHORT_OPTION"];
 * 	O1_1 [label="PNT_VALUE\\nc"];
 * 	O1_2 [label="PNT_VALUE\\n4"];
 * 	O2_1 [label="PNT_VALUE\\nz"];
 * 	O2_2 [label="PNT_VALUE\\n4"];
 * 	O3_1 [label="PNT_VALUE\\ncf"];
 * 	O3_2 [label="PNT_VALUE\\n4"];
 *
 * 	O1 -> O1_1;
 * 	O1 -> O1_2;
 *
 * 	O1 -> O2_1;
 * 	O1 -> O2_2;
 *
 * 	O1 -> O3_1;
 * 	O1 -> O3_2;
 * }
 * @enddot
 *
 * @param[in] parent the parent of this new ::parse_node
 * @param[in] va the arguments to consider
 * @return a sequence of parse node, each of them of type PNT_SHORT_OPTION. We reutnr only the first thouhg.
 * The other are accessible through ::parse_node::nextSibling
 *
 */
struct parse_node* newShortOptionMultiValueParseNode(const struct parse_node* parent, const struct var_args* va);

//TODO doc
void destroyParseNode(const struct parse_node* n, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_destroyParseNode_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

#endif
