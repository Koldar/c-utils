/*
 * scc.c
 *
 *  Created on: May 25, 2017
 *      Author: koldar
 */

#include "scc.h"
#include "macros.h"
#include <math.h>
#include "static_stack.h"
#include "hashtable.h"
#include "redBlackTree.h"
#include "defaultFunctions.h"
#include "heap.h"
#include "log.h"
#include "errors.h"

typedef list ht_edge_list;

typedef HT SCCHT;

struct scc_graph {
	PredSuccGraph* sccs;

	edge_traverser traverser;
	/**
	 * If true, then the scc will store information about edges between SCCs as well.
	 *
	 * Setting the field to false will slightly increase performance, but it will remove information of inter scc connection
	 */
	bool trackInterSCCEdges;
	/**
	 * map whose keys are the NodeId of the original graph while values stores the SCC pointer of the SCC containing that node
	 *
	 * A key is the id of the original graph;
	 * the associated value is a pointer to the SCC containing such node;
	 *
	 * This field is useful when you want to get the scc containing a node quickly
	 */
	SCCHT* node2scc;
};

typedef struct scc_data {
	/**
	 * Represents the nodes of the original graph inside the scc.
	 *
	 * Inside this tree there are stored the **references** of the original nodes
	 */
	node_heap* graphNodes;
} scc_data;

static void destroySCCData(const scc_data* sccData, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_destroySCCData_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID
static int getMinimum(int a, int b);
static void performTarjan(scc_graph* sccGraph, const PredSuccGraph* graph, const bool_ht* included);
static void performTarjanDFS(scc_graph* sccGraph, const PredSuccGraph* graph, const bool_ht* included, Node* n, NodeId* nextSccNodeId, int* nextIndex, static_stack* nodeStack, int_ht* lowlink, int_ht* index, bool_ht* onStack, static_stack* interSCCEdgeStack, Node** sccCreated, bool* shouldStop);
static int orderer_onId(const Node* n1, const Node* n2);

scc_graph* cuStronglyConnectedComponentsGraphNew(const PredSuccGraph* graph, edge_traverser traverser, bool trackInterSCCEdges, const bool_ht* included) {
	scc_graph* retVal = malloc(sizeof(scc_graph));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->sccs = cuPredSuccGraphNew(false);
	retVal->traverser = traverser;
	retVal->trackInterSCCEdges = trackInterSCCEdges;
	retVal->node2scc = cuHTNew();

	retVal->sccs->nodeFunctions.destroy = CU_AS_DESTRUCTOR(destroySCCData);
	retVal->sccs->edgeFunctions.destroy = CU_AS_DESTRUCTOR(cuListDestroy);

	performTarjan(retVal, graph, included);
	return retVal;
}

void cuStronglyConnectedComponentsGraphDestroy(const scc_graph* _sccGraph, CU_NULLABLE const struct var_args* context) {
	scc_graph* sccGraph = (scc_graph*) _sccGraph;
	cuPredSuccGraphDestroyWithElements(sccGraph->sccs, context);
	cuHTDestroy(sccGraph->node2scc, context);
	free(sccGraph);
}

et_outcome edge_traverser_alwaysAccept(Edge* e) {
	return ET_TOANALYZE;
}

PredSuccGraph* cuStronglyConnectedComponentsGraphAsPredSuccGraph(const scc_graph* sccGraph) {
	return sccGraph->sccs;
}

int cuStronglyConnectedComponentsGraphGetNumberOfNodes(const Node* scc) {
	scc_data* d = getNodePayloadAs(scc, scc_data*);
	if (d == NULL) {
		return 0;
	}
	return cuHeapGetSize(d->graphNodes);
}

bool cuStronglyConnectedComponentsIsNodeInsideByInstance(const scc_graph* sccGraph, Node* n, scc* scc) {
	return cuStronglyConnectedComponentsIsNodeInside(sccGraph, n->id, scc->id);
}

bool cuStronglyConnectedComponentsIsNodeInside(const scc_graph* sccGraph, NodeId nid, NodeId sccId) {
	Node* scc = cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, nid);
	return scc != NULL ? scc->id == sccId : false;
}

scc* cuStronglyConnectedComponentsGetComponentOfNodeByInstance(const scc_graph* sccGraph, const Node* n) {
	return cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, n->id);
}

scc* cuStronglyConnectedComponentsGetComponentOfNode(const scc_graph* sccGraph, NodeId id) {
	return cuHTGetItem(sccGraph->node2scc, id);
}

scc* cuStronglyConnectedComponentsGetComponentOfEdge(const scc_graph* sccGraph, NodeId sourceId, NodeId sinkId) {
	Node* sccSource = cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, sourceId);
	Node* sccSink = cuStronglyConnectedComponentsGetComponentOfNode(sccGraph, sinkId);
	return sccSource == sccSink ? sccSource : NULL;
}

scc* cuStronglyConnectedComponentsGetComponentOfEdgeByInstance(const scc_graph* sccGraph, Edge* e) {
	return cuStronglyConnectedComponentsGetComponentOfEdge(sccGraph, e->source->id, e->sink->id);
}

Node* cuStronglyConnectedComponentsGetNodeWithMinimumId(const scc* s) {
	return cuHeapPeekMinItem(getNodePayloadAs(s, scc_data*)->graphNodes);
}

node_heap* cuStronglyConnectedComponentsGetNodesIn(const scc* s) {
	return getNodePayloadAs(s, scc_data*)->graphNodes;
}

static int orderer_onId(const Node* n1, const Node* n2) {
	return n1->id - n2->id;
}

static payload_functions getSCCPayloadFunction() {
	payload_functions result;

	result.destroy = CU_AS_DESTRUCTOR(cuDefaultFunctionDestructorNOP);
	result.order = CU_AS_ORDERER(orderer_onId);

	return result;
}

static scc_data* newSCCData() {
	scc_data* retVal = malloc(sizeof(scc_data));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->graphNodes = cuHeapNew(100, getSCCPayloadFunction());

	return retVal;
}

static void destroySCCData(const scc_data* sccData, CU_NULLABLE const struct var_args* context) {
	cuHeapDestroyWithElements(sccData->graphNodes, context);
	free((void*)sccData);
}

static void performTarjan(scc_graph* sccGraph, const PredSuccGraph* graph, const bool_ht* included) {
	if (cuPredSuccGraphGetVertexNumber(graph) >= CUTILS_ARRAY_SIZE) {
		ERROR_ON_CONSTRUCTION("size of graph too large", "%d", cuPredSuccGraphGetVertexNumber(graph));
	}

	int_ht* lowlink = cuHTNew();
	int_ht* index = cuHTNew();
	bool_ht* onStack = cuHTNew();
	static_stack* nodeStack = cuStaticStackNew(CUTILS_ARRAY_SIZE);
	static_stack* interSCCEdgeStack = NULL;
	NodeId nextSccNodeId = 0;
	//we start by 1: in this way NULL cannot be confused with actual legit payload
	int nextIndex = 1;
	Node* sccCreated;
	bool shouldStop;

	if (sccGraph->trackInterSCCEdges) {
		interSCCEdgeStack = cuStaticStackNew(CUTILS_ARRAY_SIZE);
	}

	shouldStop = false;
	bool isIncluded;
	for (NodeId id=0; id<cuPredSuccGraphGetVertexNumber(graph); id++) {
		if (included != NULL) {
			isIncluded = cuHTGetItem(included, id);
			if (!isIncluded) {
				continue;
			}
		}

		info("is node %d visited? %p", id, cuHTGetItem(index, id));
		if (cuHTGetItem(index, id) != NULL) {
			debug("node %d has been already visited!", id);
			//the nodes has been already visited. Do nothing
			continue;
		}

		//start a new DFS from this node
		Node* n = cuPredSuccGraphGetNodeById(graph, id);
		if (n == NULL) {
			ERROR_OBJECT_NOT_FOUND("node", "%ld", id);
		}
		info("starting dfs from node %d", n->id);
		performTarjanDFS(sccGraph, graph, included, n, &nextSccNodeId, &nextIndex, nodeStack, lowlink, index, onStack, interSCCEdgeStack, &sccCreated, &shouldStop);
		if (shouldStop) {
			//tarjan said that we have to immediately stop the algorithm
			break;
		}
	}

	cuHTDestroy(lowlink, NULL); //todo context null
	cuHTDestroy(index, NULL); //TODo context null
	cuHTDestroy(onStack, NULL); //TODO context null
	cuStaticStackDestroy(nodeStack, NULL); //TODo context null
	if (interSCCEdgeStack != NULL) {
		cuStaticStackDestroy(interSCCEdgeStack, NULL); //TODO context null
	}

}

/**
 * perform a DFS specific for tarjan
 *
 * After this operation we have create the scc that contains the node \c n and all the included nodes inside such scc.
 *
 * @param[inout] sccGraph the sccgraph to build
 * @param[in] graph the original graph \c sccGraph will be based upon
 * @param[in] included an hashtable containing as keys the id of the nodes in \c graph. If the keys are presents, the nodes are considered within tarjan. Ignored otherwise
 * @param[in] n the node to analyze
 * @param[inout] nextSccNodeId the place where you can find the id of the scc that might be built
 * @param[inout] nextId the place where you can find the id of the "index" the node \c n will have
 * @param[inout] nodeStack a stack containing all the nodes inside the DFS
 * @param[inout]
 * @param[inout] edgeStack a stack where we put all the edges that will be put inside the edge between 2 sccs
 * @param [out] sccCreated output variable. Can be:
 * 	\li the reference to the newly created SCC;
 * 	\li NULL if no SCC was actually created: this happens when the call has been invoked from another ::performTarjanDFS call
 * @param[out] shouldStop if after the function is true, it means that the ::edge_traverser said we should stop computing Tarjan.
 *
 */
static void performTarjanDFS(scc_graph* sccGraph, const PredSuccGraph* graph, const bool_ht* included, Node* n, NodeId* nextSccNodeId, int* nextIndex, static_stack* nodeStack, int_ht* lowlink, int_ht* index, bool_ht* onStack, static_stack* interSCCEdgeStack, Node** sccCreated, bool* shouldStop) {

	cuHTAddItem(index, n->id, CU_CAST_INT2PTR(*nextIndex));
	cuHTAddItem(lowlink, n->id, CU_CAST_INT2PTR(*nextIndex));
	fine("added %d of index[%p] and lowlink[%p] tables...", n->id, cuHTGetItem(index, n->id), cuHTGetItem(lowlink, n->id));
	*nextIndex += 1;
	cuStaticStackPush(nodeStack, n);
	cuHTAddItem(onStack, n->id, CU_CAST_BOOL2PTR(true));

	fine("analyzing node %d", n->id);
	CU_ITERATE_OVER_HT_VALUES(n->successors, e, Edge*) {
		if (included != NULL) {
			if (!cuHTGetItem(included, e->sink->id)) {
				continue;
			}
		}

		et_outcome et = sccGraph->traverser(e);
		switch (et) {
		case ET_TOANALYZE: {
			break;
		}
		case ET_TOIGNORE: {
			//the traverser determine that the edge cannot be traversed at all
			debug("we need to ignore the edge %d->%d", e->source->id, e->sink->id);
			continue;
		}
		case ET_STOP: {
			debug("we need to stop Tarjan algorithm");
			*shouldStop = true;
			return;
		}
		default: {
			ERROR_UNHANDLED_CASE("et_outcome", et);
		}
		}
		finer("analyzing edge %d->%d", e->source->id, e->sink->id);

		if (cuHTGetItem(index, e->sink->id) == NULL) {
			//we still need to visit the successor e->sink
			debug("we need to analyze node %d", e->sink->id);
			performTarjanDFS(sccGraph, graph, included, e->sink, nextSccNodeId, nextIndex, nodeStack, lowlink, index, onStack, interSCCEdgeStack, sccCreated, shouldStop);
			if (*shouldStop) {
				//we need to immediately stop the tarjan algorithm
				debug("we need to stop tarjan!");
				*sccCreated = NULL;
				return;
			}
			//after this operation, the scc from the root e->sink has been created.
			//furthermore lowlink and index has been forged.
			int lowLinkNId = CU_CAST_PTR2INT(cuHTGetItem(lowlink, n->id));
			int lowLinkSinkId = CU_CAST_PTR2INT(cuHTGetItem(lowlink, e->sink->id));
			cuHTUpdateItem(lowlink, n->id, CU_CAST_INT2PTR(getMinimum(lowLinkNId, lowLinkSinkId)));
			//an scc may have been created during the last performTarjanDFS
			if ((sccGraph->trackInterSCCEdges) && (*sccCreated != NULL)) {
				//a SCC has been created. We push the edge we have just followed in order to add it later when we have
				//created this scc
				debug("adding %d->%d to interSCC edge stack after DFS", e->source->id, e->sink->id);
				cuStaticStackPush(interSCCEdgeStack, e);
			}
		} else if (cuHTContainsItem(onStack, e->sink->id)) {
			debug("updating lowlink of %d to min(%d, %d)", n->id, cuHTGetItem(lowlink, n->id), cuHTGetItem(index, e->sink->id));
			//we have already visited the node e->sink->id and it is in the current DFS. This means it is contained in the same SCC as ours
			cuHTUpdateItem(lowlink, n->id, CU_CAST_INT2PTR(getMinimum(CU_CAST_PTR2INT(cuHTGetItem(lowlink, n->id)), CU_CAST_PTR2INT(cuHTGetItem(index, e->sink->id)))));
		} else {
			debug("node %d and node %d are from 2 different scc", n->id, e->sink->id);
			//the node e->sink belongs to a different SCC. We need to update the edge between the 2 nodes
			if (sccGraph->trackInterSCCEdges) {
				debug("adding %d->%d to interSCC edge stack while DFS", e->source->id, e->sink->id);
				cuStaticStackPush(interSCCEdgeStack, e);
			}
		}

	}
	//we have analyzed every edge coming out from the node n

	if (cuHTGetItem(index, n->id) != cuHTGetItem(lowlink, n->id)) {
		*sccCreated = NULL;
		return;
	}

	//we can create a new scc
	finer("new SCC found from root %d", n->id);
	scc_data* sccData = newSCCData();
	if (sccData == NULL) {
		ERROR_MALLOC();
	}
	debug("the new scc will have the id %d", *nextSccNodeId);
	Node* newScc = newPredSuccNode(*nextSccNodeId, sccData, false);
	if (n == NULL) {
		ERROR_MALLOC();
	}
	*nextSccNodeId += 1;
	Node* m = NULL;
	do {
		m = cuStaticStackPop(nodeStack);
		cuHTRemoveItem(onStack, m->id);

		//add m to the building SCC
		debug("adding %d to scc with index %d [size=%d]", m->id, newScc->id, cuHeapGetSize(sccData->graphNodes));
		cuHeapInsertItem(sccData->graphNodes, m);
		cuHTAddItem(sccGraph->node2scc, m->id, newScc);
	} while (n != m);

	_cuPredSuccGraphAddVertexInstanceInGraph(sccGraph->sccs, newScc);

	if (sccGraph->trackInterSCCEdges) {
		debug("popping edges from edge stack");
		//we need to update the edges between this newly created SCC and the ones we have encoutered during the DFS
		do {
			if (cuStaticStackIsEmpty(interSCCEdgeStack)) {
				break;
			}
			Edge* e = cuStaticStackPeek(interSCCEdgeStack);

			//ok, we need to add the new edge in the list of edges between 2 sccs
			Node* sccSource = cuStronglyConnectedComponentsGetComponentOfNodeByInstance(sccGraph, e->source);
			Node* sccSink = cuStronglyConnectedComponentsGetComponentOfNodeByInstance(sccGraph, e->sink);

			if (sccSource != newScc) {
				break;
			}
			cuStaticStackPop(interSCCEdgeStack);

#ifdef DEBUG
			if (sccSource->id == sccSink->id) {
				critical("during inter SCC edge %d->%d we discovered that such edge is not among scc at all! (scc %d)", e->source->id, e->sink->id, sccSource->id);
				ERROR_IMPOSSIBLE_SCENARIO("check if scc are the same %ld", sccSource->id);
			}
#endif

			debug("adding edges %d->%d to scc big edge", e->source->id, e->sink->id);
			Edge* sccEdge = _cuPredSuccGraphGetEdge(sccSource, sccSink);
			EdgeList* originalEdgeList = NULL;
			if (sccEdge == NULL) {
				//create the new edge between scc
				originalEdgeList = cuListNew();
				sccEdge = _cuPredSuccGraphAddEdge(sccSource, sccSink, originalEdgeList);
				debug("added edge %d->%d with list %p list is %p", sccSource->id, sccSink->id, _cuPredSuccGraphGetEdge(sccSource, sccSink)->payload, originalEdgeList);
			} else {
				originalEdgeList = getEdgePayloadAs(sccEdge, EdgeList*);
				debug("updating list %p", originalEdgeList);
			}
			//insert the edge inside the list
			cuListAddTail(originalEdgeList, e);
			debug("size of list is %d", cuListGetSize(originalEdgeList));
		} while (true);
	}

	*sccCreated = newScc;
}

static int getMinimum(int a, int b) {
	return a < b ? a : b;
}

