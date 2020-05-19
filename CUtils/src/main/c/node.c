/*
 * Node.c
 *
 *  Created on: Dec 22, 2016
 *      Author: koldar
 */

#include "node.h"
#include "log.h"
#include "edge.h"
#include "defaultFunctions.h"
#include "errors.h"

static NodeId autoId = 0;


Node* newNode(const NodeId id) {
	return newPredSuccNode(id, NULL, false);
}

CU_NOTNULL Node* newPredSuccNode(NodeId id, CU_NULLABLE const void* payload, bool predecessorsEnable) {
	Node* result = (Node*) malloc(sizeof(Node));
	if (result == NULL) {
		ERROR_MALLOC();
	}

	result->id = id;
	result->payload = (void*) payload;
	result->successors = cuHTNew();
	result->predecessors = predecessorsEnable ? cuHTNew() : NULL;

	return result;
}

Node* newNodeAuto() {
	return newNode(autoId++);
}

//TODO use var args context to remove this lambda
static destructor contextDestroyEdgePaylaodDestructor = NULL;
static void lambdaDestroyEdge(const Edge* _e, CU_NULLABLE const struct var_args* context) {
	destroyEdgeWithPayload(_e, contextDestroyEdgePaylaodDestructor);
}
#define CU_FUNCTION_POINTER_destructor_void_lambdaDestroyEdge_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

void destroyNodeWithPayload(const Node* node, destructor nodePayloadDestructor, destructor edgePayloadDestructor) {
	debug("destroying node %ld", node->id);
	contextDestroyEdgePaylaodDestructor = edgePayloadDestructor;
	if (nodeHasPredecessorsActive(node)) {
		cuHTDestroy(node->predecessors, NULL); //TODO context null
	}
	cuHTDestroyWithElements(node->successors, CU_AS_DESTRUCTOR(lambdaDestroyEdge));
	if (node->payload != NULL) {
		nodePayloadDestructor(node->payload, NULL); //TODO context null
	}
	free((void*)node);
}

EdgeList* getEdgeListOfNode(const Node* source) {
	EdgeList* retVal = cuListNew();
	CU_ITERATE_OVER_HT_VALUES(source->successors, edge, Edge*) {
		debug("adding edge %d->%d to list", edge->source->id, edge->sink->id);
		cuListAddHead(retVal, edge);
	}
	return retVal;
}

CU_NOTNULL Edge* addEdgeInNode(CU_NOTNULL Node* restrict source, const CU_NOTNULL Node* restrict sink, const CU_NULLABLE void* edgePayload) {
	Edge* edge = newEdge(source, sink, edgePayload);

	debug("find previous edge to remove: %d -> %d %p", source->id, sink->id, edgePayload);
	addEdgeDirectlyInNode(source, edge);
	return edge;
}

CU_NOTNULL Edge* addEdgeDirectlyInNode(CU_NOTNULL Node* source, const CU_NOTNULL Edge* e) {
	cuHTRemoveItemWithElement(source->successors, e->sink->id, CU_AS_DESTRUCTOR(destroyEdge));
	cuHTAddItem(source->successors, e->sink->id, (const void*)e);
	if (nodeHasPredecessorsActive(e->sink)) {
		cuHTAddItem(e->sink->predecessors, source->id, e);
	}
	return (Edge*) e;
}

//TODO rename in something that tells that the edges are out going
void removeEdgeInNode(Node* restrict source, Node* restrict sink) {
	cuHTRemoveItemWithElement(source->successors, sink->id, CU_AS_DESTRUCTOR(destroyEdge));
	if (nodeHasPredecessorsActive(sink)) {
		cuHTRemoveItemWithElement(sink->predecessors, source->id, CU_AS_DESTRUCTOR(destroyEdge));
	}
}

//TODO rename in something that tells that the edges are out going
Edge* findEdgeInNode(const Node* restrict source, const Node* restrict sink) {
	return cuHTGetItem(source->successors, sink->id);
}

//TODO rename in something that tells that the edges are out going
EdgeHT* getEdgeHTOfNode(const Node* source) {
	return source->successors;
}

//TODO rename in something that tells that the edges are out going
int getNumberOfEdgesInNode(const Node* source) {
	return cuHTGetSize(source->successors);
}

int cuNodeGetPredecessorNumberOfVertex(CU_NOTNULL const Node* n) {
	return cuHTGetSize(n->predecessors);
}

CU_NOTNULL Node* cuNodeGetPredecessor(CU_NOTNULL const Node* n) {
	CU_REQUIRE(cuHTGetSize(n->predecessors) == 1);
	return cuHTGetFirstItem(n->predecessors);
}

int getHashOfNode(const Node* n) {
	return CU_CAST_PTR2INT(n);
}



Node* cloneNode(const Node* n) {
	return newPredSuccNode(n->id, n->payload, nodeHasPredecessorsActive(n));
}

Node* cloneNodeWithPayload(const Node* n, cloner payloadCloner) {
	return newPredSuccNode(n->id, payloadCloner(n->payload), nodeHasPredecessorsActive(n));
}

bool compareNodes(const Node* a, const Node* b, comparator payloadComparator) {
	if (a->id != a->id) {
		return false;
	}
	return payloadComparator(a->payload, b->payload);
}

//static object_serializer lambdaStoreEdgeInFileContext = NULL;
//static int lambdaStoreEdgeInFile(FILE* f, void* e) {
//	storeEdgeInFile(f, e, lambdaStoreEdgeInFileContext);
//	return 0;
//}
//
//static struct PredSuccGraph* lamdaLoadNodeFromFileContext_graph = NULL;
//static object_deserializer lambdaLoadNodeFromFileContext_nodeDeserializer = NULL;
//static object_deserializer lambdaLoadNodeFromFileContext_edgeDeserializer = NULL;
//static int lambdaLoadNodeFromFile(FILE* f, void** p) {
//	Edge* other = loadEdgeFromFile(f, lamdaLoadNodeFromFileContext_graph, lambdaLoadNodeFromFileContext_nodeDeserializer, lambdaLoadNodeFromFileContext_edgeDeserializer);
//	Edge* inGraph = findEdgeInNode(other->source->id, other->sink->id);
//	//we create a new edge only if it is not inside the graph yet.
//	if (inGraph != NULL) {
//		destroyEdge(other);
//		*p = inGraph;
//	} else {
//		*p = other;
//	}
//	return 0;
//}
//
//void storeNodeInFile(FILE* f, const Node* n, object_serializer nodeSerializer, object_serializer edgeSerializer) {
//	char buffer[BUFFER_SIZE];
//
//	//id
//	fwrite(&n->id, sizeof(n->id), 1, f);
//	//payload
//	nodeSerializer(f, n->payload);
//	//successors
//	lambdaStoreEdgeInFileContext = edgeSerializer;
//	cuHTStoreInFile(f, n->successors, lambdaStoreEdgeInFile);
//}

void storeNodeInFileWithoutEdges(FILE* f, const Node* n, object_serializer nodeSerializer) {
	char buffer[BUFFER_SIZE];

	//id
	fwrite(&n->id, sizeof(n->id), 1, f);
	//payload
	nodeSerializer(f, n->payload);
}

Node* loadNodeFromFileWithoutEdges(FILE* f, bool enablePredecessors, object_deserializer nodeDeserializer) {
	Node* n;
	NodeId id = 0;
	//we need to clear it because it may contain an integer and sizeof(int) < sizeof(void*), hence some bits may
	//remain unintialized, which can lead to invalid read.
	void* payload = NULL;
	//id
	int idRead = fread(&id, sizeof(id), 1, f);
	//payload
	nodeDeserializer(f, &payload);
	n = newPredSuccNode(id, payload, enablePredecessors);
	return n;
}

bool nodeHasPredecessorsActive(CU_NOTNULL const Node* n) {
	return n->predecessors != NULL;
}
