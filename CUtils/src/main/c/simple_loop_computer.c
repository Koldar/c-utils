/*
 * simple_loop_computer.c
 *
 *  Created on: Jul 25, 2017
 *      Author: koldar
 */


#include "simple_loops_computer.h"
#include <stdbool.h>
#include "scc.h"
#include "heap.h"
#include "static_stack.h"
#include "dynamic_array.h"
#include "log.h"
#include "errors.h"

typedef HT node_list_ht;
typedef dynamic_1D_array bool_array;
typedef static_stack node_stack;

typedef struct johnson_support {
	/**
	 * array of booleans.
	 *
	 * A cell is true iff the node is inside the building loop.
	 * False otherwise
	 */
	bool_array* blocked;
	/**
	 * An hashtable representing B
	 *
	 * In B where each key is an id of a node in graph while
	 * the value is a list of nodes in graph.
	 */
	node_list_ht* B;
	/**
	 * A stack where to put the vertex on function circuit
	 */
	node_stack* stack;
	/**
	 * The function used to check if an edge can be traversed or not
	 */
	edge_traverser t;
	/**
	 * an hashtable where containing each node that we need to consider inside the SCC algorithm computation.
	 *
	 * If we need to consider a node, we need to have an entry in this field indexed by the **node id**.
	 */
	bool_ht* included;
} johnson_support;

static void unblock(bool_array* blocked, node_list_ht* b, NodeId u);
static bool circuit(const PredSuccGraph* graph, const scc_graph* sccGraph, const scc* sccOfRoot, node_stack* stack, bool_array* blocked, node_list_ht* b, NodeId rootId, NodeId vId, loop_list* retVal);
static void cricuit_finding_algorithm(johnson_support* support, const PredSuccGraph* graph, loop_list* retVal);

johnson_support* cuJohnsonSupportNew() {
	johnson_support* retVal = malloc(sizeof (johnson_support));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->B = cuHTNew();
	retVal->blocked = NULL;
	retVal->stack = NULL;
	retVal->t = NULL;
	retVal->included = cuHTNew();

	return retVal;
}

void cuJohnsonSupportDestroy(johnson_support* support, CU_NULLABLE const struct var_args* context) {
	cuDynamicArrayDestroy(support->blocked, context);
	cuHTDestroyWithElements(support->B, CU_AS_DESTRUCTOR(cuListDestroy));
	cuStaticStackDestroy(support->stack, context);
	cuHTDestroy(support->included, context);
	free(support);
}

loop_list* cuJohnsonSupportComputeSimpleLoops(johnson_support* support, const PredSuccGraph* g, edge_traverser t) {
	loop_list* retVal = cuListNew(cuSimpleLoopComputerPayloadFunctionsLoop());

	// ***************** SETUP SUPPORT ********************
	if (support->blocked != NULL && cuDynamicArrayGetSize(support->blocked) < cuPredSuccGraphGetVertexNumber(g)) {
		cuDynamicArrayDestroy(support->blocked, NULL); //TODO context null
	}
	support->blocked = cuDynamicArrayNew(bool, cuPredSuccGraphGetVertexNumber(g));

	if (support->stack != NULL && cuStaticStackGetCapacity(support->stack) < cuPredSuccGraphGetVertexNumber(g)) {
		cuStaticStackDestroy(support->stack, NULL); //TODO context null
	}
	support->stack = cuStaticStackNew(cuPredSuccGraphGetVertexNumber(g));
	support->t = t;

	cuHTClear(support->included);
	cuStaticStackClear(support->stack);
	cuDynamicArrayClearTo(support->blocked, bool, true);
	CU_ITERATE_OVER_HASHTABLE(support->B, nodeId, nl, node_list*) {
		cuListDestroy(nl, NULL); //TODO context null
	}
	cuHTClear(support->B);
//	TODO remove CU_ITERATE_OVER_HT(support->B, el) {
//		cuListDestroy((node_list*)el->data);
//		free(el);
//	}

	// ****************** EXECUTE ALGORITHM *******************

	cricuit_finding_algorithm(support, g, retVal);

	return retVal;
}

payload_functions cuSimpleLoopComputerPayloadFunctionsLoop() {
	payload_functions result;

	result.destroy = CU_AS_DESTRUCTOR(cuListDestroy);

	return result;
}

static void cricuit_finding_algorithm(johnson_support* support, const PredSuccGraph* graph, loop_list* retVal) {
	scc_graph* sccGraph = NULL;
	scc* sccAnalyzed;

	for (NodeId id=0; id<cuPredSuccGraphGetVertexNumber(graph); id++) {
		cuHTAddItem(support->included, id, CU_CAST_BOOL2PTR(true));
	}

	//loop over the nodes
	for (NodeId s=0; s<cuPredSuccGraphGetVertexNumber(graph); s++) {
		sccGraph = cuStronglyConnectedComponentsGraphNew(graph, support->t, false, support->included);
		//scc containing s
		sccAnalyzed = cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, s);
		if (sccAnalyzed == NULL) {
			break;
		}
		Node* root = cuStronglyConnectedComponentsGetNodeWithMinimumId(sccAnalyzed);

		CU_ITERATE_THROUGH_HEAP(cuStronglyConnectedComponentsGetNodesIn(sccAnalyzed), node, Node*) {
			cuDynamicArraySetCellTo(support->blocked, node->id, bool, false);
			NodeList* b_node = cuHTGetItem(support->B, node->id);
			if (b_node == NULL) {
				cuHTAddItem(support->B, node->id, cuListNew());
			} else {
				cuListClear(b_node);
			}
		}

		circuit(graph, sccGraph, sccAnalyzed, support->stack, support->blocked, support->B, root->id, root->id, retVal);

		//we now remove the node "s" from the graph, since we have computed all the loop involving it
		if (sccGraph != NULL) {
			cuStronglyConnectedComponentsGraphDestroy(sccGraph, NULL); //TODO context null
		}
		cuHTRemoveItem(support->included, s);
	}
}

static void unblock(bool_array* blocked, node_list_ht* b, NodeId u) {
	//put the node outside the building loop
	cuDynamicArraySetCellTo(blocked, u, bool, false);

	NodeList* bu = cuHTGetItem(b, u);
	CU_VARIABLE_ITERATE_OVER_LIST(bu, prevCell, cell, w, Node*) {
		cuListRemoveItemInListCell(bu, &prevCell, cell);
		if (cuDynamicArrayGetCellValue(blocked, w->id, bool)) {
			unblock(blocked, b, w->id);
		}
	}
}

static bool circuit(const PredSuccGraph* graph, const scc_graph* sccGraph, const scc* sccOfRoot, node_stack* stack, bool_array* blocked, node_list_ht* b, NodeId rootId, NodeId vId, loop_list* retVal) {
	bool f;
	Node* v;
	Node* w;

	f = false;
	v = cuPredSuccGraphGetNodeById(graph, vId);
	cuStaticStackPush(stack, v);
	cuDynamicArraySetCellTo(blocked, vId, bool, true);

	// ******************** L1 ***************************

	CU_ITERATE_OVER_HT_VALUES(v->successors, e, Edge*) {
		w = e->sink;

		if (!cuStronglyConnectedComponentsIsNodeInside(sccGraph, w->id, sccOfRoot->id)) {
			//the successor is in another scc. We ignore it since it can't lead to cycles
			continue;
		}

		if (w->id == rootId) {
			//output circuit composed of stack followed by s
			loop* l = cuListNew();
			CU_ITERATE_OVER_STATIC_STACK(stack, n, Node*) {
				cuListAddTail(l, n);
			}
			cuListAddTail(retVal, l);

			f = true;
		} else if (!cuDynamicArrayGetCellValue(blocked, w->id, bool)) {
			if (circuit(graph, sccGraph, sccOfRoot, stack, blocked, b, rootId, w->id, retVal)) {
				f = true;
			}
 		}
	}

	// ******************** L2 ************************

	if (f) {
		unblock(blocked, b, vId);
	} else {
		bool found;
		CU_ITERATE_OVER_HT_VALUES(v->successors, e, Edge*) {
			w = e->sink;
			NodeId wId = w->id;
			if (!cuStronglyConnectedComponentsIsNodeInside(sccGraph, w->id, sccOfRoot->id)) {
				//the successor is in another scc. We ignore it since it can't lead to cycles
				continue;
			}

			NodeList* bw = cuHTGetItem(b, wId);
			if (bw == NULL) {
				bw = cuListNew();
				cuHTAddItem(b, wId, bw);
			}

			//search in bw for v
			found = false;
			CU_ITERATE_OVER_LIST(bw, cell, n, Node*) {
				if (n->id == vId) {
					found = true;
					break;
				}
			}

			if (!found) {
				cuListAddTail(bw, v);
			}

		}
	}

	Node* expectedV = cuStaticStackPop(stack);
#ifdef DEBUG
	if (expectedV->id != v->id) {
		ERROR_IMPOSSIBLE_SCENARIO("popped from stack vertex %ld", expectedV->id);
	}
#endif

	return f;
}
