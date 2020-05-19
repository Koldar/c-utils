/*
 * PredSuccGraph.c
 *
 *  Created on: Dec 22, 2016
 *      Author: koldar
 */


#include "predsuccgraph.h"

#include "node.h"
#include "log.h"
#include "file_utils.h"
#include "defaultFunctions.h"
#include "hash_set.h"
#include "dynamic_array.h"
#include "errors.h"
#include "string_utils.h"

static void computeDotFile(const PredSuccGraph* graph, const char* fileName, NodeId highlightedNodeid);
static bool _getFirstNodeWhichIsNotDescendantOf(CU_NOTNULL const PredSuccGraph* g, CU_NOTNULL const Node* current, CU_NOTNULL pint_hash_set* possibleDescendantIds, CU_NOTNULL pint_hash_set* visited, bool (*traverser)(CU_NOTNULL const Edge* edge));

CU_DEFINE_DEFAULT_VALUES(cuPredSuccGraphNew,
		false,
		cuPayloadFunctionsDefault(),
		cuPayloadFunctionsDefault()
);


PredSuccGraph* cuPredSuccGraphNew(bool enablePredecessors, payload_functions vertexPayload, payload_functions edgePayload) {
	PredSuccGraph* retVal = (PredSuccGraph*) malloc(sizeof(PredSuccGraph));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}
	retVal->size = 0;
	retVal->nodes = cuHTNew();
	retVal->enablePredecessors = enablePredecessors;

	retVal->nodeFunctions = vertexPayload;
	retVal->edgeFunctions = edgePayload;
	//TODo remove
	//	retVal->nodePayloadCloner = CU_AS_CLONER(cuDefaultFunctionsClonerObject);
	//	retVal->nodePayloadComparator = CU_AS_COMPARER(cuDefaultFunctionsComparatorObject);
	//	retVal->nodePayloadDestructor = CU_AS_DESTRUCTOR(cuDefaultFunctionsDestructorObject);
	//	retVal->nodePayloadBufferStringer = CU_AS_BUFFERSTRINGER(cuDefaultFunctionsBufferStringerObject);
	//	retVal->nodePayloadColorer = CU_AS_COLORER(cuDefaultFunctionsColorerObject);
	//	retVal->nodePayloadSerializer = CU_AS_SERIALIZER(cuDefaultFunctionsSerializerObject);
	//	retVal->nodePayloadDeserializer = CU_AS_DESERIALIZER(cuDefaultFunctionsDeserializerObject);

	//	retVal->edgePayloadCloner = CU_AS_CLONER(cuDefaultFunctionsClonerObject);
	//	retVal->edgePayloadComparator = CU_AS_COMPARER(cuDefaultFunctionsComparatorObject);
	//	retVal->edgePayloadDestructor = CU_AS_DESTRUCTOR(cuDefaultFunctionsDestructorObject);
	//	retVal->edgePayloadBufferStringer = CU_AS_BUFFERSTRINGER(cuDefaultFunctionsBufferStringerObject);
	//	retVal->edgePayloadColorer = CU_AS_COLORER(cuDefaultFunctionsColorerObject);
	//	retVal->edgePayloadSerializer = CU_AS_SERIALIZER(cuDefaultFunctionsSerializerObject);
	//	retVal->edgePayloadDeserializer = CU_AS_DESERIALIZER(cuDefaultFunctionsDeserializerObject);
	return retVal;
}

static const PredSuccGraph* contextDestroyNodesGraph = NULL;
static void lambdaDestroyNodes(const Node* n, CU_NULLABLE const struct var_args* context) {
	destroyNodeWithPayload(n, contextDestroyNodesGraph->nodeFunctions.destroy, contextDestroyNodesGraph->edgeFunctions.destroy);
}
#define CU_FUNCTION_POINTER_destructor_void_lambdaDestroyNodes_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

void cuPredSuccGraphDestroyWithElements(const PredSuccGraph* g, CU_NULLABLE const struct var_args* context) {
	contextDestroyNodesGraph = g;
	cuHTDestroyWithElements(g->nodes, CU_AS_DESTRUCTOR(lambdaDestroyNodes));
	free((void*)g);
}

PredSuccGraph* cuPredSuccGraphClone(CU_NOTNULL const PredSuccGraph* graph) {
	PredSuccGraph* cloned = cuPredSuccGraphNew();
	Node* source = NULL;
	Node* sink = NULL;

	cloned->nodeFunctions = graph->nodeFunctions;
	cloned->edgeFunctions = graph->edgeFunctions;

	debug("cloning nodes... size=%d", cloned->size);
	for (NodeId nodeId=0; nodeId<graph->size; nodeId++) {
		source = cuPredSuccGraphGetNodeById(graph, nodeId);
		if (source == NULL) {
			ERROR_OBJECT_NOT_FOUND("node", "%ld", nodeId);
		}
		_cuPredSuccGraphAddVertexInstanceInGraph(cloned, newPredSuccNode(nodeId, graph->nodeFunctions.clone(source->payload), graph->enablePredecessors));
	}
	debug("done");

	EdgeList* edgeList = cuListNew();

	//ok, now we clone the edges as well
	for (NodeId nodeId=0; nodeId<cloned->size; nodeId++) {
		source = cuPredSuccGraphGetNodeById(graph, nodeId);
		if (source == NULL) {
			ERROR_OBJECT_NOT_FOUND("node", "%ld", nodeId);
		}

		CU_ITERATE_OVER_HT_VALUES(source->successors, edge, Edge*) {
			debug("adding edge %d->%d in the clone", source->id, edge->sink->id);
			cuListAddHead(
					edgeList,
					newEdge(
							cuPredSuccGraphGetNodeById(cloned, source->id),
							cuPredSuccGraphGetNodeById(cloned, edge->sink->id),
							cloned->edgeFunctions.clone(edge->payload)
					)
			);
		}

		CU_ITERATE_OVER_LIST(edgeList, cell, value, Edge*) {
			_cuPredSuccGraphAddEdgeDirectly(cloned, value);
		}
		cuListClear(edgeList);
	}
	cuListDestroy(edgeList, NULL); //TODO context null
	return cloned;

}

void _cuPredSuccGraphAddVertexInstanceInGraph(CU_NOTNULL PredSuccGraph* g, CU_NOTNULL const Node* n) {
	cuHTAddItem(g->nodes, n->id, n);
	g->size++;
}

CU_NOTNULL Node* cuPredSuccGraphAddNodeInGraphById(CU_NOTNULL PredSuccGraph* g, NodeId id, CU_NULLABLE const void* payload) {
	Node* n = newPredSuccNode(id, payload, g->enablePredecessors);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n);
	return n;
}

Node* cuPredSuccGraphGetNodeById(const PredSuccGraph* g, const NodeId id) {
	debug("node id is %ld", id);
	return cuHTGetItem(g->nodes, id);
}

int cuPredSuccGraphGetNodeOutDegree(const PredSuccGraph* g, NodeId id) {
	Node* n = cuPredSuccGraphGetNodeById(g, id);
	if (n == NULL) {
		ERROR_OBJECT_NOT_FOUND("node", "%ld", id);
	}
	return getNumberOfEdgesInNode(n);
}

static bool _isNodeReachableFromNode(const PredSuccGraph* g, const Node* source, const Node* sink, bool (*traverser)(const Edge* edge), hash_set* nodesReached) {

	debug("checking successors of node %d", source->id);
	CU_ITERATE_OVER_HT_VALUES(source->successors, edge, Edge*) {
		debug("pondering edge %d->%d", edge->source->id, edge->sink->id);
		if (cuHashSetContainsItem(nodesReached, (void*)edge->sink)) {
			debug("successors %d has already been reached", edge->sink->id);
			continue;
		}
		if (!traverser(edge)) {
			debug("can't traverse edge %d->%d", edge->source->id, edge->sink->id);
			continue;
		}
		if (edge->sink == sink) {
			return true;
		}
		cuHashSetAddItem(nodesReached, (void*)edge->sink);
		if (_isNodeReachableFromNode(g, edge->sink, sink, traverser, nodesReached)) {
			return true;
		}
	}

	cuHashSetAddItem(nodesReached, (void*)source);
	return false;
}

bool cuPredSuccGraphIsVertexReachableFromVertex(const PredSuccGraph* g, NodeId sourceId, NodeId sinkId, bool (*traverser)(const Edge* edge)) {
	bool retVal;
	Node* source = cuPredSuccGraphGetNodeById(g, sourceId);
	if (source == NULL) {
		ERROR_OBJECT_NOT_FOUND("source", "%ld", sourceId);
	}
	Node* sink = cuPredSuccGraphGetNodeById(g, sinkId);
	if (sink == NULL) {
		ERROR_OBJECT_NOT_FOUND("sink", "%ld", sinkId);
	}
	hash_set* nodesReached = cuHashSetNew(cuPayloadFunctionsIntValue());

	retVal = _isNodeReachableFromNode(g, source, sink, traverser, nodesReached);

	cuHashSetDestroyWithElements(nodesReached, NULL); //TODO context null

	return retVal;
}

enum getFirstNodeWhichIsNotDescendantOf_result {
	POSSIBLE_DESCENDANT_EMPTY,
	TREE_EXPLORED
};

bool cuPredSuccGraphGetFirstVertexWhichIsNotDescendantOf(CU_NOTNULL const PredSuccGraph* g, NodeId id, const CU_NOTNULL pint_hash_set* possibleDescendantIds, bool (*traverser)(CU_NOTNULL const Edge* edge), CU_NOTNULL NodeId* output) {

	Node* source = cuPredSuccGraphGetNodeById(g, id);
	bool result;

	if (cuHashSetContainsItem(possibleDescendantIds, CU_CAST_UL2PTR(id))) {
		*output = id;
		return true;
	}

	//TODO context null
	CU_WITH(pbool_hash_set* visited = cuHashSetNew(cuPayloadFunctionsBooleanValue()))(cuHashSetDestroyWithElements(visited, NULL))
			CU_WITH(pint_hash_set* clone = cuHashSetCloneByReference(possibleDescendantIds))(cuHashSetDestroyWithElements(clone, NULL)) {

		bool isPossibleDescendantIdsEmpty = _getFirstNodeWhichIsNotDescendantOf(g, source, clone, visited, traverser);
		if (!isPossibleDescendantIdsEmpty) {
			//if false it means there is left some node in possibleDescendantIds
			CU_REQUIRE(!cuHashSetIsEmpty(possibleDescendantIds));

			*output = (NodeId) cuHashSetGetAnItem(clone);
			result = true;
		} else {
			result = false;
		}

	}

	return result;
}


/**
 * DFS on a node which checks if there is a descendant of the node which is not inside @c possibleDescendantIds
 *
 * @param[in] g the graph to operate on
 * @param[in] current the node we're operating the dfs on
 * @param[inout] possibleDescendantIds a list of possible descendants
 * @param[inout] visited NodeId within this set has been already been visisted by the DFS. avoid infinite loops
 * @param[in] traverser a function determining if you should actually traverse an edge or you should ignore it
 * @return
 *  @li true if @c possibleDescendantIds is empty: this means we do not have any more possible descendants to test, ergo there cannot be a node in @c possibleDescendantIds which is not a descendant
 *  @li false otherwise;
 */
static bool _getFirstNodeWhichIsNotDescendantOf(CU_NOTNULL const PredSuccGraph* g, CU_NOTNULL const Node* current, CU_NOTNULL pint_hash_set* possibleDescendantIds, CU_NOTNULL pint_hash_set* visited, bool (*traverser)(CU_NOTNULL const Edge* edge)) {
	if (cuHashSetContainsItem(possibleDescendantIds, CU_CAST_UL2PTR(current->id))) {
		cuHashSetRemoveItem(possibleDescendantIds, CU_CAST_UL2PTR(current->id));
	}

	if (cuHashSetIsEmpty(possibleDescendantIds)) {
		return true;
	}

	//FIXME note in DAGs this set si absolutely useless since there cannot be any cycles. maybe we should create a different function?
	cuHashSetAddItem(visited, CU_CAST_UL2PTR(current->id));
	CU_ITERATE_OVER_PREDSUCC_SUCCESSORS(g, current, edge, edgeData, void*) {
		if (cuHashSetContainsItem(visited, CU_CAST_UL2PTR(edge->sink->id))) {
			//we have already visited the node
			continue;
		}
		if (!traverser(edge)) {
			//we have to ignore the edge
			continue;
		}

		bool isPossibleDescendantIdsEmpty = _getFirstNodeWhichIsNotDescendantOf(g, edge->sink, possibleDescendantIds, visited, traverser);
		if (isPossibleDescendantIdsEmpty) {
			return true;
		}

	}

	return false;
}

CU_NULLABLE Node* cuPredSuccGraphGetFirstPredecessorOfVertexInstance(CU_NOTNULL const PredSuccGraph* g, CU_NOTNULL const Node* n) {
	CU_REQUIRE_TRUE(g->enablePredecessors);

	return cuHTGetFirstItem(n->predecessors);
}

CU_NULLABLE Node* cuPredSuccGraphGetFirstPredecessorOfVertex(CU_NOTNULL const PredSuccGraph* g, NodeId id) {
	CU_REQUIRE_TRUE(g->enablePredecessors);

	Node* n = cuPredSuccGraphGetNodeById(g, id);
	if (n == NULL) {
		ERROR_OBJECT_NOT_FOUND("node", "%ld", id);
	}

	return cuPredSuccGraphGetFirstPredecessorOfVertexInstance(g, n);
}

bool cuPredSuccGraphHasVertexNoSuccessors(CU_NOTNULL const PredSuccGraph* g, NodeId id) {
	Node* n = cuPredSuccGraphGetNodeById(g, id);
	if (n == NULL) {
		ERROR_OBJECT_NOT_FOUND("node", "%ld", id);
	}

	return cuHTIsEmpty(n->successors);
}

bool cuPredSuccGraphHasVertexInstanceNoSuccessors(CU_NOTNULL const PredSuccGraph* g, Node* n) {
	return cuHTIsEmpty(n->successors);
}

bool cuPredSuccGraphHasVertexNoPredecessors(CU_NOTNULL const PredSuccGraph* g, NodeId id) {
	Node* n = cuPredSuccGraphGetNodeById(g, id);
	if (n == NULL) {
		ERROR_OBJECT_NOT_FOUND("node", "%ld", id);
	}

	return cuHTIsEmpty(n->predecessors);
}

int cuPredSuccGraphGetPredecessorNumberOfVertex(CU_NOTNULL const PredSuccGraph* g, NodeId id) {
	Node* n = cuPredSuccGraphGetNodeById(g, id);
	if (n == NULL) {
		ERROR_OBJECT_NOT_FOUND("node", "%ld", id);
	}

	return cuHTGetSize(n->predecessors);
}

Edge* _cuPredSuccGraphAddEdgeDirectly(PredSuccGraph* g, const Edge* edge) {
	Node* source = cuPredSuccGraphGetNodeById(g, edge->source->id);
	if (source == NULL) {
		ERROR_OBJECT_NOT_FOUND("source of edge", "%ld", edge->source->id);
	}
	addEdgeDirectlyInNode(source, edge);
	return (Edge*) edge;
}

CU_NOTNULL Edge* _cuPredSuccGraphAddEdge(CU_NOTNULL Node* restrict source, CU_NOTNULL Node* restrict sink, CU_NULLABLE const void* edgePayload) {
	return addEdgeInNode(source, sink, edgePayload);
}

CU_NOTNULL Edge* cuPredSuccGraphAddEdge(CU_NOTNULL PredSuccGraph* graph, NodeId sourceId, NodeId sinkId, CU_NULLABLE const void* edgePayload) {
	Node* source = cuPredSuccGraphGetNodeById(graph, sourceId);
	if (source == NULL) {
		ERROR_OBJECT_NOT_FOUND("source", "%ld", sourceId);
	}
	Node* sink = cuPredSuccGraphGetNodeById(graph, sinkId);
	if (sink == NULL) {
		ERROR_OBJECT_NOT_FOUND("sink", "%ld", sinkId);
	}
	return _cuPredSuccGraphAddEdge(source, sink, edgePayload);
}

void _cuPredSuccGraphRemoveEdge(Node* restrict source, Node* restrict sink) {
	removeEdgeInNode(source, sink);
}

void cuPredSuccGraphRemoveEdge(PredSuccGraph* graph, const NodeId sourceId, const NodeId sinkId, const bool removeFlipped) {
	Node* source = cuPredSuccGraphGetNodeById(graph, sourceId);
	Node* sink = cuPredSuccGraphGetNodeById(graph, sinkId);
	if (source == NULL) {
		ERROR_OBJECT_NOT_FOUND("source", "%ld", sourceId);
	}
	if (sink == NULL) {
		ERROR_OBJECT_NOT_FOUND("sink", "%ld", sinkId);
	}
	_cuPredSuccGraphRemoveEdge(source, sink);

	if (removeFlipped) {
		_cuPredSuccGraphRemoveEdge(sink, source);
	}
}

Edge* _cuPredSuccGraphGetEdge(const Node* restrict source, const Node* restrict sink) {
	return findEdgeInNode(source, sink);
}

Edge* cuPredSuccGraphGetEdgeInGraph(const PredSuccGraph* graph, const NodeId sourceId, const NodeId sinkId) {
	Node* source = NULL;
	Node* sink = NULL;
	source = cuPredSuccGraphGetNodeById(graph, sourceId);
	sink = cuPredSuccGraphGetNodeById(graph, sinkId);
	if (source == NULL || sink == NULL) {
		return NULL;
	}
	return findEdgeInNode(source, sink);
}

void* _cuPredSuccGraphGetEdgePayloadInGraph(CU_NOTNULL const PredSuccGraph* graph, NodeId sourceId, NodeId sinkId) {
	Node* source = NULL;
	Node* sink = NULL;
	source = cuPredSuccGraphGetNodeById(graph, sourceId);
	sink = cuPredSuccGraphGetNodeById(graph, sinkId);
	if (source == NULL || sink == NULL) {
		ERROR_OBJECT_NOT_FOUND("source or sink", "%ld", sourceId);
	}
	if (!cuHTContainsItem(source->successors, sourceId)) {
		ERROR_OBJECT_NOT_FOUND("edge between source and sink", "%ld", sourceId);
	}
	return findEdgeInNode(source, sink);

}

bool cuPredSuccGraphContainsEdgeInGraph(const PredSuccGraph* graph, NodeId sourceId, NodeId sinkId) {
	return cuPredSuccGraphGetEdgeInGraph(graph, sourceId, sinkId) != NULL;
}

//TODO the name should be a property of the graph
void cuPredSuccGraphPrintGraph(PredSuccGraph* graph, const char* fileName) {
	_cuPredSuccGraphPrintGraphWithOptions(graph, DC_CIRCO, fileName, NULL, -1);
}

void _cuPredSuccGraphPrintGraphWithAdditionalName(PredSuccGraph* graph, const char* fileName, const char* additionalName) {
	_cuPredSuccGraphPrintGraphWithOptions(graph, DC_CIRCO, fileName, additionalName, -1);
}

void _cuPredSuccGraphPrintGraphWithOptions(const PredSuccGraph* graph, dot_commands cmd, const char* fileName, const char* additionalName, NodeId highlightedNode) {
	//FIXME do not crash if circo is not available on the system
	char nameBuffer[BUFFER_SIZE];
	info("additional name is %s", additionalName);
	sprintf(nameBuffer, "%s_%s", fileName, (additionalName == NULL) ? "" : additionalName);

	computeDotFile(graph, nameBuffer, highlightedNode);

	char cmdName[BUFFER_SIZE];
	switch (cmd) {
	case DC_DOT: { sprintf(cmdName, "dot"); break; }
	case DC_CIRCO: { sprintf(cmdName, "circo"); break; }
	case DC_NEATO: { sprintf(cmdName, "neato"); break; }
	case DC_SFDP: { sprintf(cmdName, "sfdp"); break; }
	default: {
		ERROR_UNHANDLED_CASE("dot command", cmd);
	}
	}

	char buffer[BUFFER_SIZE];
	cuSnprintf(buffer, BUFFER_SIZE, "%s -Tsvg %s.dot -o %s.svg", cmdName, nameBuffer, nameBuffer);
	int dotExitCode = system(buffer);
	//removes the dotfile, if exists
	cuSnprintf(buffer, BUFFER_SIZE, "%s.dot", nameBuffer);
	//try to delete dot files. If removes fails we ignore the return value
	remove(buffer);
}

static void computeDotFile(const PredSuccGraph* graph, const char* fileName, NodeId highlightedNodeid) {
	char fileNameWithExtension[BUFFER_SIZE];
	cuSnprintf(fileNameWithExtension, BUFFER_SIZE, "%s.dot", fileName);

	FILE* fout = fopen(fileNameWithExtension, "w");
	Node* tmp;
	int tab = 0;
	EdgeList* successors;
	char str[BUFFER_SIZE];

	if (fout == NULL) {
		ERROR_FILE(fileName);
	}

	cuFileUtilsTabbedWrite(fout, tab, "digraph {");
	tab += 1;
	for (NodeId i=0; i<cuPredSuccGraphGetVertexNumber(graph); i++) {
		Node* n = cuPredSuccGraphGetNodeById(graph, i);
		if (n == NULL) {
			ERROR_OBJECT_NOT_FOUND("node", "%ld", i);
		}
		str[0] = '\0';
		char payloadBuffer[BUFFER_SIZE];
		graph->nodeFunctions.bufferString(n->payload, payloadBuffer);
		if (strlen(payloadBuffer) > 0) {
			strcat(str, "\\n");
			strcat(str, payloadBuffer);
		}
		if (i != highlightedNodeid) {
			cuFileUtilsTabbedWrite(fout, tab, "N%04d [label=\"%d%s\"];\n", i, i, str);
		} else {
			cuFileUtilsTabbedWrite(fout, tab, "N%04d [label=\"%d%s\" style=\"filled\" fillcolor=\"blue\"];\n", i, i, str);
		}
	}

	for (int i=0; i<cuPredSuccGraphGetVertexNumber(graph); i++) {
		tmp = cuPredSuccGraphGetNodeById(graph, i);


		CU_ITERATE_OVER_HT_VALUES(tmp->successors, edge, Edge*) {
			char edgePayloadBuffer[BUFFER_SIZE];
			graph->edgeFunctions.bufferString(edge->payload, edgePayloadBuffer);
			cuFileUtilsTabbedWrite(fout, tab, "N%04d -> N%04d [label=\"%s\" color=\"#%06d\"];\n",
					tmp->id,
					edge->sink->id,
					edgePayloadBuffer,
					0
			);
		}


		//		TODO REMOVE successors = tmp->successors;
		//		while (successors != NULL) {
		//			cuFileUtilsTabbedWrite(fout, tab, "N%04d -> N%04d [label=\"%s\" color=\"%s\"];\n",
		//					tmp->id,
		//					((Edge*)successors->pointer)->sink->id,
		//					getStringFromEdgeLabel(((Edge*)successors->pointer)->label),
		//					isEdgeFlawed(((Edge*)successors->pointer)) ? "red" : "black"
		//			);
		//			successors = successors->next;
		//		}
		cuFileUtilsTabbedWrite(fout, tab, "\n");
	}
	cuFileUtilsTabbedWrite(fout, tab, "}");

	fflush(fout);
	fclose(fout);
}

int cuPredSuccGraphGetVertexNumber(const PredSuccGraph* graph) {
	return graph->size;
}

EdgeList* cuPredSuccGraphGetEdgeList(const PredSuccGraph* graph) {
	EdgeList* retVal = (EdgeList*) cuListNew();
	Node* source = NULL;
	EdgeList* sourceSuccessors = NULL;
	Edge* fe;

	for (int sourceId=0; sourceId<graph->size;sourceId++ ) {
		source = cuPredSuccGraphGetNodeById(graph, sourceId);
		if (source == NULL) {
			ERROR_NOT_SUCH_OBJECT("source", sourceId);
		}

		CU_ITERATE_OVER_HT_VALUES(source->successors, edge, Edge*) {
			cuListAddHead(retVal, edge);
		}

		//		todo sourceSuccessors = getEdgeListOfNode(source);
		//		while (sourceSuccessors != NULL) {
		//			retVal = addHead(retVal, (Edge*)sourceSuccessors->pointer);
		//			sourceSuccessors = sourceSuccessors->next;
		//		}
	}

	return retVal;
}

CU_NOTNULL EdgeList* cuPredSuccGraphGetVertexOutEdges(CU_NOTNULL const PredSuccGraph* graph, NodeId sourceId) {
	EdgeList* result = cuListNew(graph->edgeFunctions);
	Node* source = cuPredSuccGraphGetNodeById(graph, sourceId);
	if (source == NULL) {
		ERROR_OBJECT_NOT_FOUND("node with id", "%ld", sourceId);
	}

	CU_ITERATE_OVER_HT_VALUES(source->successors, edge, Edge*) {
		cuListAddTail(result, edge);
	}

	return result;
}

int cuPredSuccGraphGetEdgesNumber(const PredSuccGraph* graph) {
	int retVal = 0;
	Node* source = NULL;
	EdgeList* sourceSuccessors = NULL;
	Edge* fe;

	for (int sourceId=0; sourceId<graph->size;sourceId++ ) {
		source = cuPredSuccGraphGetNodeById(graph, sourceId);
		if (source == NULL) {
			ERROR_NOT_SUCH_OBJECT("source", sourceId);
		}

		retVal += cuHTGetSize(source->successors);

		//		todo sourceSuccessors = getEdgeListOfNode(source);
		//		while (sourceSuccessors != NULL) {
		//			retVal++;
		//			sourceSuccessors = sourceSuccessors->next;
		//		}
	}

	return retVal;
}

#define CU_FUNCTION_POINTER_finder_boolean_lambdaFindEdgeByOther_voidConstPtr_var_argsConstPtr CU_FINDER_ID
static bool lambdaFindEdgeByOther(Edge* e, const var_args* va) {
	return getEdgeCmpWithPayload(e, cuVarArgsGetItem(va, 0, Edge*), cuVarArgsGetItem(va, 1, comparator));
}

bool cuPredSuccGraphCompare(const PredSuccGraph* restrict g1, const PredSuccGraph* restrict g2) {
	bool retVal = false;

	info("g1 %d g2 %d", cuPredSuccGraphGetVertexNumber(g1), cuPredSuccGraphGetVertexNumber(g2));
	if (cuPredSuccGraphGetVertexNumber(g1) != cuPredSuccGraphGetVertexNumber(g2)) {
		return false;
	}

	EdgeHT* edgeHT1 = NULL;
	EdgeHT* edgeHT2 = NULL;
	Edge* e1 = NULL;
	Edge* e2 = NULL;
	Node* n1 = NULL;
	Node* n2 = NULL;

	for (NodeId id=0; id < cuPredSuccGraphGetVertexNumber(g1); id++) {
		n1 = cuPredSuccGraphGetNodeById(g1, id);
		n2 = cuPredSuccGraphGetNodeById(g2, id);

		edgeHT1 = getEdgeHTOfNode(n1);
		edgeHT2 = getEdgeHTOfNode(n2);

		if (cuHTGetSize(edgeHT1) != cuHTGetSize(edgeHT2)) {
			return false;
		}
		info("g1 comaprator is %p while g2 comaprator is %p", g1->edgeFunctions.compare, g2->edgeFunctions.compare);
		if (g1->edgeFunctions.compare != g2->edgeFunctions.compare) {
			ERROR_EQUALITY_FAILED("graph edge payload comparator", "g1", "graph edge payload comparator", "g2");
		}
		CU_ITERATE_OVER_HT_VALUES(edgeHT1, edge, Edge*) {
			cuInitVarArgsOnStack(va, edge, g1->edgeFunctions.compare);
			if (!cuHTContainsItems(edgeHT2, CU_AS_FINDER(lambdaFindEdgeByOther), va)) {
				info("edge %d -> %d is not present inside hashtable 2", edge->source->id, edge->sink->id);
				return false;
			}
		}


		//		while (edgeHT1 != NULL) {
		//			e1 = (Edge*)edgeHT1->pointer;
		//			contextFindEdgeBySink = e1->sink;
		//			e2 = find(edgeHT2, lambdaFindEdgeBySink);
		//			if (e2 == NULL) {
		//				return false;
		//			}
		//			if (e1->label != e2->label) {
		//				return false;
		//			}
		//			edgeHT1 = edgeHT1->next;
		//		}
	}

	return true;
}

//static object_serializer lambdaStoreNodeInFileContext_nodeSerializer = NULL;
//static object_serializer lambdaStoreNodeInFileContext_edgeSerializer = NULL;
//static int lambdaStoreNodeInFile(FILE* f, void* n) {
//	storeNodeInFileWithoutEdges(f, n, lambdaStoreNodeInFileContext_nodeSerializer);
//	//storeNodeInFile(f, n, lambdaStoreNodeInFileContext_nodeSerializer, lambdaStoreNodeInFileContext_edgeSerializer);
//	return 0;
//}
//
//static object_deserializer lambdaLoadNodeFromFileContext_nodeDeserializer = NULL;
//static object_deserializer lambdaLoadNodeFromFileContext_edgeDeserializer = NULL;
//static PredSuccGraph* lambdaLoadNodeFromFileContext_graph = NULL;
//static int lambdaLoadNodeFromFile(FILE* f, void** n) {
//	*n = loadNodeFromFile(f, lambdaLoadNodeFromFileContext_graph, lambdaLoadNodeFromFileContext_nodeDeserializer, lambdaLoadNodeFromFileContext_edgeDeserializer);
//	return 0;
//}

static object_serializer lambdaStoreEdgeInFileContext_edgeSerializer = NULL;
static int lambdaStoreEdgeInFile(FILE* f, const Edge* e) {
	storeEdgeInFile(f, e, lambdaStoreEdgeInFileContext_edgeSerializer);
	return 0;
}


static object_deserializer lambdaLoadEdgeFromFileContext_edgeDeserializer = NULL;
static PredSuccGraph* lambdaLoadEdgeFromFileContext_graph = NULL;
static int lambdaLoadEdgeFromFile(FILE* f, Edge** e) {
	*e = loadEdgeFromFile(f, lambdaLoadEdgeFromFileContext_graph, lambdaLoadEdgeFromFileContext_edgeDeserializer);
	return 0;
}

void cuPredSuccGraphSerialize(FILE* f, const PredSuccGraph* g) {
	//predecesors active
	fwrite(&g->enablePredecessors, sizeof(g->enablePredecessors), 1, f);
	//size
	fwrite(&g->size, sizeof(g->size), 1, f);
	//we store the elements
	CU_ITERATE_OVER_HT_VALUES(g->nodes, n, Node*) {
		storeNodeInFileWithoutEdges(f, n, g->nodeFunctions.serialize);
	}
	//now we store the edges
	int count;
	CU_ITERATE_OVER_HT_VALUES(g->nodes, n2, Node*) {
		fwrite(&n2->id, sizeof(n2->id), 1, f);
		lambdaStoreEdgeInFileContext_edgeSerializer = g->edgeFunctions.serialize;
		cuHTStoreInFile(f, n2->successors, CU_AS_SERIALIZER(lambdaStoreEdgeInFile));
	}
}

PredSuccGraph* cuPredSuccGraphDeserialize(FILE* f, payload_functions vertexPayload, payload_functions edgePayload) {
	Node* n;
	int size;

	PredSuccGraph* retVal = cuPredSuccGraphNew(false, vertexPayload, edgePayload);
	//predecesors active
	size_t bytesRead = fread(&retVal->enablePredecessors, sizeof(retVal->enablePredecessors), 1, f);
	//size
	bytesRead += fread(&size, sizeof(retVal->size), 1, f);
	//elements
	for (int i=0; i<size; i++) {
		n = loadNodeFromFileWithoutEdges(f, retVal->enablePredecessors, retVal->nodeFunctions.deserialize);
		if (n == NULL) {
			ERROR_NOT_SUCH_OBJECT("node", i);
		}
		cuHTDestroy(n->successors, NULL); //TODO context null
		_cuPredSuccGraphAddVertexInstanceInGraph(retVal, n);
	}
	//now we load the edge
	NodeId id;
	CU_ITERATE_OVER_HT_VALUES(retVal->nodes, n2, Node*) {
		//the id of the node to read
		int idRead = fread(&id, sizeof(id), 1, f);
		lambdaLoadEdgeFromFileContext_edgeDeserializer = retVal->edgeFunctions.deserialize;
		lambdaLoadEdgeFromFileContext_graph = retVal;
		payload_functions functions = cuPayloadFunctionsError();
		functions.deserialize = CU_AS_DESERIALIZER(lambdaLoadEdgeFromFile);
		n2->successors = cuHTLoadFromFile(f, functions);
		if (cuPredSuccGraphHasPredecessorsActive(retVal)) {
			CU_ITERATE_OVER_HT_VALUES(n2->successors, edge, Edge*) {
				cuHTAddItem(edge->sink->predecessors, n2->id, edge);
			}
		}
	}
	return retVal;
}

bool cuAlwaysTraverse(CU_NOTNULL const Edge* e) {
	return true;
}

bool cuPredSuccGraphHasPredecessorsActive(CU_NOTNULL const PredSuccGraph* g) {
	return g->enablePredecessors;
}
