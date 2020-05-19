/*
 * graphTest.c
 *
 *  Created on: May 18, 2017
 *      Author: koldar
 */

#define _GNU_SOURCE

#include <stdio.h>
#include "CuTest.h"
#include "edge.h"
#include "node.h"
#include "log.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "predsuccgraph.h"
#include "defaultFunctions.h"
#include "file_utils.h"

int LEQ =	0x00000110;
int EQ =	0x00000010;
int NOTEQ = 0x00000101;
int LT =	0x00000100;

static bool lambdaFindEdgeByBounds(const CU_PDOC(Edge*) _e, const var_args* va) {
	Edge* e = (Edge*) _e;
	return	(e->source->id == cuVarArgsGetItem(va, 0, Edge*)->source->id) &&
			(e->sink->id == cuVarArgsGetItem(va, 0, Edge*)->sink->id);
}

static bool lambdaFindEdgeByNodes(const CU_PDOC(Edge*) _e, const var_args* va) {
	Edge* e = (Edge*) _e;
	return (e->source->id == cuVarArgsGetItem(va, 0, Node*)->id) && (e->sink->id == cuVarArgsGetItem(va, 1, Node*)->id);
}

static bool lambdaFindEdgeBySinkAndLabel(const CU_PDOC(Edge*) _e, const var_args* va) {
	Edge* e = (Edge*) _e;
	debug("payloads: e %d context %d", *(int*)e->payload, *(int*)cuVarArgsGetItem(va, 0, Edge*)->payload);
	if (*getEdgePayloadAs(e, int*) != *getEdgePayloadAs(cuVarArgsGetItem(va, 0, Edge*), int*)) {
		return false;
	}
	if (e->sink->id != cuVarArgsGetItem(va, 0, Edge*)->sink->id) {
		return false;
	}
	return true;
}

static bool compare2Integer(int* a, int* b) {
	return *a == *b;
}

#define CU_FUNCTION_POINTER_destructor_void_destroyInteger_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID
static void destroyInteger(int* n, CU_NULLABLE const struct var_args* context) {
	if (n != NULL) {
		free(n);
	}
}

static int* cloneInteger(const int* n) {
	if (n != NULL) {
		int* retVal = malloc(sizeof(int));
		*retVal = *n;
		return retVal;
	}
	return NULL;
}
#define CU_FUNCTION_POINTER_cloner_voidPtr_cloneInteger_voidConstPtr CU_CLONER_ID

static const char* stringifyInteger(int* n) {
	if (n == NULL) {
		return NULL;
	}
	char* retVal;
	asprintf(&retVal, "%d", *n);
	return retVal;
}

void TestDrawGraph(CuTest* tc) {

	PredSuccGraph* g = cuPredSuccGraphNew();
	Node* n0 = newNode(0);
	Node* n1 = newNode(1);
	Node* n2 = newNode(2);
	Node* n3 = newNode(3);
	Node* n4 = newNode(4);
	Node* n5 = newNode(5);
	Node* n6 = newNode(6);
	Node* n7 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(g, n0);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n1);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n2);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n3);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n4);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n5);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n6);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n7);

	int* p;
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n1, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n2, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n3, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n5, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n5, n0, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n5, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n5, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n6, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n7, n6, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n6, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n2, n6, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n7, p);

	cuPredSuccGraphPrintGraph(g, __func__);

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void testgetEdgeInGraphById01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	Node* n0 = newNode(0);
	Node* n1 = newNode(1);
	Node* n2 = newNode(2);
	Node* n3 = newNode(3);
	Node* n4 = newNode(4);
	Node* n5 = newNode(5);
	Node* n6 = newNode(6);
	Node* n7 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(g, n0);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n1);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n2);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n3);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n4);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n5);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n6);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n7);

	int* p;
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n1, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n2, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n3, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n5, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n5, n0, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n5, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n5, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n6, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n7, n6, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n6, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n6, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n7, p);

	cuPredSuccGraphPrintGraph(g, __func__);

	assert(cuPredSuccGraphGetEdgeInGraph(g, n0->id, n1->id) != NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, n2->id, n3->id) != NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, n6->id, n7->id) != NULL);
	//non existent edges
	assert(cuPredSuccGraphGetEdgeInGraph(g, n6->id, n0->id) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, n0->id, n3->id) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, n3->id, n0->id) == NULL);

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

///compute the consistency of a graph with all non-flawed edges
void testcheckPredSuccGraphDestruction(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	Node* n0 = newNode(0);
	Node* n1 = newNode(1);
	Node* n2 = newNode(2);
	Node* n3 = newNode(3);
	Node* n4 = newNode(4);
	Node* n5 = newNode(5);
	Node* n6 = newNode(6);
	Node* n7 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(g, n0);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n1);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n2);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n3);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n4);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n5);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n6);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n7);

	int* p;
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n1, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n2, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n3, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n5, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n5, n0, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n3, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n5, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n5, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n6, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n7, n6, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n6, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n2, n6, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n7, p);

	cuPredSuccGraphPrintGraph(g, __func__);

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

///the 2 graphs have different sizes
void testpredSuccGraphCmp01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	Node* n0 = newNode(0);
	Node* n1 = newNode(1);
	Node* n2 = newNode(2);
	Node* n3 = newNode(3);
	Node* n4 = newNode(4);
	Node* n5 = newNode(5);
	Node* n6 = newNode(6);
	Node* n7 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(g, n0);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n1);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n2);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n3);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n4);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n5);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n6);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n7);


	g->edgeFunctions.compare = CU_AS_COMPARER(compare2Integer);

	int* p;
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n1, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n2, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n3, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n5, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n5, n0, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n3, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n5, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n5, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n6, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n7, n6, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n6, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n2, n6, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n7, p);

	PredSuccGraph* g2 = cuPredSuccGraphNew();
	Node* n02 = newNode(0);
	Node* n12 = newNode(1);
	Node* n22 = newNode(2);
	Node* n32 = newNode(3);
	Node* n42 = newNode(4);
	Node* n52 = newNode(5);
	Node* n62 = newNode(6);

	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n02);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n12);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n22);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n32);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n42);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n52);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n62);

	g2->edgeFunctions.compare = CU_AS_COMPARER(compare2Integer);
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n12, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n12, n22, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n22, n32, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n32, n42, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n42, n52, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n52, n02, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n32, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n12, n42, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n22, n52, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n12, n52, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n62, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n22, n62, p);

	assert(cuPredSuccGraphCompare(g, g2) == false);

	cuPredSuccGraphDestroyWithElements(g, NULL);
	cuPredSuccGraphDestroyWithElements(g2, NULL);
}

///the 2 graphs are the same, but the second one does not have an edge
void testpredSuccGraphCmp02(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	Node* n0 = newNode(0);
	Node* n1 = newNode(1);
	Node* n2 = newNode(2);
	Node* n3 = newNode(3);
	Node* n4 = newNode(4);
	Node* n5 = newNode(5);
	Node* n6 = newNode(6);
	Node* n7 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(g, n0);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n1);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n2);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n3);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n4);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n5);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n6);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n7);

	g->edgeFunctions.compare = CU_AS_COMPARER(compare2Integer);

	int* p;
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n1, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n2, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n3, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n5, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n5, n0, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n3, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n5, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n5, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n6, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n7, n6, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n6, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n2, n6, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n7, p);

	PredSuccGraph* g2 = cuPredSuccGraphNew();
	Node* n02 = newNode(0);
	Node* n12 = newNode(1);
	Node* n22 = newNode(2);
	Node* n32 = newNode(3);
	Node* n42 = newNode(4);
	Node* n52 = newNode(5);
	Node* n62 = newNode(6);
	Node* n72 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n02);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n12);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n22);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n32);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n42);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n52);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n62);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n72);

	g2->edgeFunctions.compare = CU_AS_COMPARER(compare2Integer);
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n12, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n12, n22, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n22, n32, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n32, n42, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n42, n52, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n52, n02, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n32, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n12, n42, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n22, n52, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n12, n52, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n62, n72, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n72, n62, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n62, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n22, n62, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n32, n72, p);

	assert(cuPredSuccGraphCompare(g, g2) == false);

	cuPredSuccGraphDestroyWithElements(g, NULL);
	cuPredSuccGraphDestroyWithElements(g2, NULL);

}

///the 2 graphs are the same but a label of an edge in the second graph is different
void testpredSuccGraphCmp03(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	Node* n0 = newNode(0);
	Node* n1 = newNode(1);
	Node* n2 = newNode(2);
	Node* n3 = newNode(3);
	Node* n4 = newNode(4);
	Node* n5 = newNode(5);
	Node* n6 = newNode(6);
	Node* n7 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(g, n0);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n1);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n2);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n3);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n4);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n5);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n6);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n7);

	g->edgeFunctions.compare = CU_AS_COMPARER(compare2Integer);

	int* p;
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n1, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n2, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n3, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n5, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n5, n0, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n3, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n5, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n5, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n6, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n7, n6, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n6, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n2, n6, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n7, p);

	PredSuccGraph* g2 = cuPredSuccGraphNew();
	Node* n02 = newNode(0);
	Node* n12 = newNode(1);
	Node* n22 = newNode(2);
	Node* n32 = newNode(3);
	Node* n42 = newNode(4);
	Node* n52 = newNode(5);
	Node* n62 = newNode(6);
	Node* n72 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n02);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n12);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n22);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n32);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n42);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n52);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n62);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n72);

	g2->edgeFunctions.compare = CU_AS_COMPARER(compare2Integer);
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n12, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n12, n22, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n22, n32, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n32, n42, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n42, n52, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n52, n02, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n32, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n12, n42, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n22, n52, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n12, n52, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n62, n72, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n72, n62, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n62, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n22, n62, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n32, n72, p);
	p = malloc(sizeof(int)); *p=LT; _cuPredSuccGraphAddEdge(n42, n72, p);

	assert(cuPredSuccGraphCompare(g, g2) == false);

	cuPredSuccGraphDestroyWithElements(g, NULL);
	cuPredSuccGraphDestroyWithElements(g2, NULL);
}

///the 2 graphs are the same
void testpredSuccGraphCmp04(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();
	Node* n0 = newNode(0);
	Node* n1 = newNode(1);
	Node* n2 = newNode(2);
	Node* n3 = newNode(3);
	Node* n4 = newNode(4);
	Node* n5 = newNode(5);
	Node* n6 = newNode(6);
	Node* n7 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(g, n0);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n1);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n2);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n3);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n4);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n5);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n6);
	_cuPredSuccGraphAddVertexInstanceInGraph(g, n7);

	g->edgeFunctions.compare = CU_AS_COMPARER(compare2Integer);

	int* p;
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n1, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n2, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n3, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n5, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n5, n0, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n3, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n5, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n1, n5, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n6, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n7, n6, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n6, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n2, n6, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n7, p);

	PredSuccGraph* g2 = cuPredSuccGraphNew();
	Node* n02 = newNode(0);
	Node* n12 = newNode(1);
	Node* n22 = newNode(2);
	Node* n32 = newNode(3);
	Node* n42 = newNode(4);
	Node* n52 = newNode(5);
	Node* n62 = newNode(6);
	Node* n72 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n02);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n12);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n22);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n32);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n42);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n52);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n62);
	_cuPredSuccGraphAddVertexInstanceInGraph(g2, n72);

	g2->edgeFunctions.compare = CU_AS_COMPARER(compare2Integer);
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n12, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n12, n22, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n22, n32, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n32, n42, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n42, n52, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n52, n02, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n32, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n12, n42, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n22, n52, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n12, n52, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n62, n72, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n72, n62, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n02, n62, p);
	p = malloc(sizeof(int)); *p=EQ; _cuPredSuccGraphAddEdge(n22, n62, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n32, n72, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n42, n72, p);

	assert(cuPredSuccGraphCompare(g, g2) == true);

	cuPredSuccGraphDestroyWithElements(g, NULL);
	cuPredSuccGraphDestroyWithElements(g2, NULL);
}


static bool compare2Integers(int* a, int* b) {
	return *a == *b;
}

void testgetEdgeListOfPredSuccGraph01(CuTest* tc) {
	PredSuccGraph* graph = cuPredSuccGraphNew();
	Node* n0 = newNode(0);
	Node* n1 = newNode(1);
	Node* n2 = newNode(2);
	Node* n3 = newNode(3);
	Node* n4 = newNode(4);
	Node* n5 = newNode(5);
	Node* n6 = newNode(6);
	Node* n7 = newNode(7);

	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n0);
	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n1);
	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n2);
	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n3);
	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n4);
	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n5);
	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n6);
	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n7);

	int* p;
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n1, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n2, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n3, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n4, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n5, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n5, n0, p);
	p = malloc(sizeof(int)); *p=LT; _cuPredSuccGraphAddEdge(n0, n3, p);
	p = malloc(sizeof(int)); *p=NOTEQ; _cuPredSuccGraphAddEdge(n1, n4, p);
	p = malloc(sizeof(int)); *p=LT; _cuPredSuccGraphAddEdge(n2, n5, p);
	p = malloc(sizeof(int)); *p=NOTEQ; _cuPredSuccGraphAddEdge(n1, n5, p);

	//second SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n6, n7, p);
	p = malloc(sizeof(int)); *p=LT; _cuPredSuccGraphAddEdge(n7, n6, p);

	//edge between SCC
	p = malloc(sizeof(int)); *p=LT; _cuPredSuccGraphAddEdge(n0, n6, p);
	p = malloc(sizeof(int)); *p=NOTEQ; _cuPredSuccGraphAddEdge(n2, n6, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n3, n7, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n4, n7, p);


	EdgeList* list = cuPredSuccGraphGetEdgeList(graph);
	assert(cuListGetSize(list) == 16);

	{cuInitVarArgsOnStack(va, _cuPredSuccGraphGetEdge(n0, n1));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n1, n2));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n2, n3));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n3, n4));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n4, n5));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n5, n0));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n0, n3));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n1, n4));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n2, n5));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n1, n5));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}

	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n6, n7));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n7, n6));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}

	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n0, n6));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n2, n6));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n3, n7));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}
	{cuInitVarArgsOnStack(va,  _cuPredSuccGraphGetEdge(n4, n7));	assert( cuListfindItem(list, lambdaFindEdgeByBounds, va) != NULL);}

	//NULL edges
	{cuInitVarArgsOnStack(va, n0, n4); assert( cuListfindItem(list, lambdaFindEdgeByNodes, va) == NULL);}
	{cuInitVarArgsOnStack(va, n0, n5); assert( cuListfindItem(list, lambdaFindEdgeByNodes, va) == NULL);}
	{cuInitVarArgsOnStack(va, n0, n7); assert( cuListfindItem(list, lambdaFindEdgeByNodes, va) == NULL);}
	{cuInitVarArgsOnStack(va, n1, n3); assert( cuListfindItem(list, lambdaFindEdgeByNodes, va) == NULL);}

	cuListDestroy(list, NULL);
	cuPredSuccGraphDestroyWithElements(graph, NULL);
}

void testclonePredSuccGraph01(CuTest* tc) {
	PredSuccGraph* graph = cuPredSuccGraphNew();
	graph->edgeFunctions.destroy = CU_AS_DESTRUCTOR(destroyInteger);
	graph->edgeFunctions.clone = CU_AS_CLONER(cloneInteger);
	graph->edgeFunctions.bufferString = CU_AS_BUFFERSTRINGER(cuDefaultFunctionsBufferStringerIntPtr);

	Node* n0 = newNode(0);
	Node* n1 = newNode(1);
	Node* n2 = newNode(2);
	Node* n3 = newNode(3);

	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n0);
	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n1);
	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n2);
	_cuPredSuccGraphAddVertexInstanceInGraph(graph, n3);


	int* p;
	//first SCC
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n0, n1, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n1, n2, p);
	p = malloc(sizeof(int)); *p=LEQ; _cuPredSuccGraphAddEdge(n2, n3, p);
	p = malloc(sizeof(int)); *p=LT; _cuPredSuccGraphAddEdge(n0, n3, p);
	p = malloc(sizeof(int)); *p=LT; _cuPredSuccGraphAddEdge(n3, n0, p);

	debug("cloning...");
	PredSuccGraph* clone = cuPredSuccGraphClone(graph);
	debug("done cloning");

	//pointer are differents
	assert(clone != graph);
	assert(cuPredSuccGraphGetNodeById(graph, 0) != cuPredSuccGraphGetNodeById(clone, 0));
	assert(cuPredSuccGraphGetNodeById(graph, 1) != cuPredSuccGraphGetNodeById(clone, 1));
	assert(cuPredSuccGraphGetNodeById(graph, 2) != cuPredSuccGraphGetNodeById(clone, 2));
	assert(cuPredSuccGraphGetNodeById(graph, 3) != cuPredSuccGraphGetNodeById(clone, 3));

	assert(cuPredSuccGraphGetVertexNumber(graph) == cuPredSuccGraphGetVertexNumber(clone));

	for (NodeId id=0; id<graph->size; id++) {
		Node* tmp = cuPredSuccGraphGetNodeById(graph, id);
		Node* tmp2 = cuPredSuccGraphGetNodeById(clone, id);

		EdgeList* list1 = getEdgeListOfNode(tmp);
		EdgeList* list2 = getEdgeListOfNode(tmp2);

		assert(cuListGetSize(list1) == cuListGetSize(list2));
		assert(tmp != tmp2);
		assert(tmp->id == tmp2->id);
		debug("getForwardListSize(list1) = %d", cuListGetSize(list1));

		assert(list1 != list2);

		CU_ITERATE_OVER_LIST(list1, cell, e, Edge*) {
			//in list pointers are different but the sink ids are the same
			cuInitVarArgsOnStack(va, e);
			Edge* found = cuListfindItem(list2, lambdaFindEdgeBySinkAndLabel, va);
			assert(found != NULL);
		}

		cuListDestroy(list1, NULL);
		cuListDestroy(list2, NULL);
	}

	cuPredSuccGraphDestroyWithElements(graph, NULL);
	cuPredSuccGraphDestroyWithElements(clone, NULL);
	info("all destoryed!");
}

void testSerialization01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew(false, cuPayloadFunctionsIntValue(), cuPayloadFunctionsIntValue());

	for (int i=0; i<5; i++) {
		cuPredSuccGraphAddNodeInGraphById(g, i, CU_CAST_INT2PTR(i));
	}

	cuPredSuccGraphAddEdge(g, 0, 4, CU_CAST_INT2PTR(7));
	cuPredSuccGraphAddEdge(g, 0, 3, CU_CAST_INT2PTR(8));
	cuPredSuccGraphAddEdge(g, 0, 2, CU_CAST_INT2PTR(9));
	cuPredSuccGraphAddEdge(g, 3, 0, CU_CAST_INT2PTR(10));
	cuPredSuccGraphAddEdge(g, 3, 1, CU_CAST_INT2PTR(11));
	cuPredSuccGraphAddEdge(g, 4, 3, CU_CAST_INT2PTR(12));

	FILE* f = fopen(__func__, "wb");
	cuPredSuccGraphSerialize(f, g);
	fclose(f);
	cuPredSuccGraphDestroyWithElements(g, NULL);

	f = fopen(__func__, "rb");
	g = cuPredSuccGraphDeserialize(f, cuPayloadFunctionsIntValue(), cuPayloadFunctionsIntValue());
	fclose(f);
	//TODO remove
//	g->edgeFunctions.destroy = cuDefaultFunctionDestructorNOP;
//	g->nodeFunctions.destroy = cuDefaultFunctionDestructorNOP;

	assert(cuPredSuccGraphGetVertexNumber(g) == 5);

	assert(cuPredSuccGraphGetEdgeInGraph(g, 0, 1) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 0, 2) != NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 0, 3) != NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 0, 4) != NULL);

	assert(cuPredSuccGraphGetEdgeInGraph(g, 1, 0) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 1, 2) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 1, 3) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 1, 4) == NULL);

	assert(cuPredSuccGraphGetEdgeInGraph(g, 2, 0) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 2, 1) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 2, 3) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 2, 4) == NULL);

	assert(cuPredSuccGraphGetEdgeInGraph(g, 3, 0) != NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 3, 1) != NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 3, 2) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 3, 4) == NULL);

	assert(cuPredSuccGraphGetEdgeInGraph(g, 4, 0) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 4, 1) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 4, 2) == NULL);
	assert(cuPredSuccGraphGetEdgeInGraph(g, 4, 3) != NULL);

	assert(getEdgePayloadAs(cuPredSuccGraphGetEdgeInGraph(g, 0, 2), intptr_t) == 9);
	assert(getEdgePayloadAs(cuPredSuccGraphGetEdgeInGraph(g, 0, 3), intptr_t) == 8);
	assert(getEdgePayloadAs(cuPredSuccGraphGetEdgeInGraph(g, 0, 4), intptr_t) == 7);

	assert(getEdgePayloadAs(cuPredSuccGraphGetEdgeInGraph(g, 3, 0), intptr_t) == 10);
	assert(getEdgePayloadAs(cuPredSuccGraphGetEdgeInGraph(g, 3, 1), intptr_t) == 11);

	assert(getEdgePayloadAs(cuPredSuccGraphGetEdgeInGraph(g, 4, 3), intptr_t) == 12);

	assert(getNodePayloadAs(cuPredSuccGraphGetNodeById(g, 0), intptr_t) == 0);
	assert(getNodePayloadAs(cuPredSuccGraphGetNodeById(g, 1), intptr_t) == 1);
	assert(getNodePayloadAs(cuPredSuccGraphGetNodeById(g, 2), intptr_t) == 2);
	assert(getNodePayloadAs(cuPredSuccGraphGetNodeById(g, 3), intptr_t) == 3);
	assert(getNodePayloadAs(cuPredSuccGraphGetNodeById(g, 4), intptr_t) == 4);

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

static bool canTraverseNode(const Edge* e) {
	return getEdgePayloadAs(e, bool) == true;
}

void test_isNodeReachableFromNode_01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	g->edgeFunctions.destroy = cuDefaultFunctionDestructorNOP;

	for (int i=0 ;i<10; i++) {
		cuPredSuccGraphAddNodeInGraphById(g, i, NULL);
	}

	cuPredSuccGraphAddEdge(g, 0, 1, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 1, 2, CU_CAST_BOOL2PTR(true)); //0->1->2 deadend

	cuPredSuccGraphAddEdge(g, 0, 3, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 3, 4, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 4, 5, CU_CAST_BOOL2PTR(true));//0->3->4->5 deadend

	cuPredSuccGraphAddEdge(g, 3, 6, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 6, 7, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 7, 5, CU_CAST_BOOL2PTR(true)); //0->3->6->7->5 already visited
	cuPredSuccGraphAddEdge(g, 7, 3, CU_CAST_BOOL2PTR(true)); //0->3->6->7->3 cycle

	cuPredSuccGraphAddEdge(g, 3, 8, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 3, 9, CU_CAST_BOOL2PTR(false)); //0->3->8->9 edge not travrsable

	cuPredSuccGraphAddEdge(g, 0, 9, CU_CAST_BOOL2PTR(true)); //correct path

	cuPredSuccGraphPrintGraph(g, __func__);
	assert(cuPredSuccGraphIsVertexReachableFromVertex(g, 0, 9, canTraverseNode));

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void test_isNodeReachableFromNode_02(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	g->edgeFunctions.destroy = cuDefaultFunctionDestructorNOP;

	for (int i=0 ;i<10; i++) {
		cuPredSuccGraphAddNodeInGraphById(g, i, NULL);
	}

	cuPredSuccGraphAddEdge(g, 0, 1, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 1, 2, CU_CAST_BOOL2PTR(true)); //0->1->2 deadend

	cuPredSuccGraphAddEdge(g, 0, 3, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 3, 4, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 4, 5, CU_CAST_BOOL2PTR(true));//0->3->4->5 deadend

	cuPredSuccGraphAddEdge(g, 3, 6, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 6, 7, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 7, 5, CU_CAST_BOOL2PTR(true)); //0->3->6->7->5 already visited
	cuPredSuccGraphAddEdge(g, 7, 3, CU_CAST_BOOL2PTR(true)); //0->3->6->7->3 cycle

	cuPredSuccGraphAddEdge(g, 3, 8, CU_CAST_BOOL2PTR(true));
	cuPredSuccGraphAddEdge(g, 3, 9, CU_CAST_BOOL2PTR(false)); //0->3->8->9 edge not travrsable

	cuPredSuccGraphAddEdge(g, 0, 9, CU_CAST_BOOL2PTR(false)); //correct path

	assert(!cuPredSuccGraphIsVertexReachableFromVertex(g, 0, 9, canTraverseNode));

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void testIterateThroughNodes_01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	g->edgeFunctions.destroy = cuDefaultFunctionDestructorNOP;
	g->nodeFunctions.destroy = cuDefaultFunctionDestructorNOP;

	for (int i=0 ;i<5; i++) {
		cuPredSuccGraphAddNodeInGraphById(g, i, CU_CAST_INT2PTR(i*i));
	}

	int sum = 0;

	CU_ITERATE_OVER_PREDSUCC_NODES(g, node, id, data, intptr_t) {
		//the paylod is the power of 2 of the id
		info("node: %p, id: %ld, data: %d", node, id, data);
		sum += data;
	}
	assert(sum == (0+1+4+9+16));

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

///test empty graph
void testIterateThroughNodes_02(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	g->edgeFunctions.destroy = cuDefaultFunctionDestructorNOP;
	g->nodeFunctions.destroy = cuDefaultFunctionDestructorNOP;

	cuPredSuccGraphAddNodeInGraphById(g, 0, CU_CAST_INT2PTR(5));

	int sum = 0;
	CU_ITERATE_OVER_PREDSUCC_NODES(g, node, id, data, intptr_t) {
		//the paylod is the power of 2 of the id
		sum += data;
	}
	assert(sum == (5));

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

///test graph with a single node
void testIterateThroughNodes_03(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	g->edgeFunctions.destroy = cuDefaultFunctionDestructorNOP;
	g->nodeFunctions.destroy = cuDefaultFunctionDestructorNOP;

	int sum = 0;
	CU_ITERATE_OVER_PREDSUCC_NODES(g, node, id, data, intptr_t) {
		//the paylod is the power of 2 of the id
		sum += data;
	}
	assert(sum == (0));

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void testIterateThroughEdges_01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	g->edgeFunctions.destroy = cuDefaultFunctionDestructorNOP;
	g->nodeFunctions.destroy = cuDefaultFunctionDestructorNOP;

	for (int i=0 ;i<5; i++) {
		cuPredSuccGraphAddNodeInGraphById(g, i, CU_CAST_INT2PTR(i*i));
	}

	cuPredSuccGraphAddEdge(g, 0, 1, CU_CAST_INT2PTR(5));
	cuPredSuccGraphAddEdge(g, 2, 1, CU_CAST_INT2PTR(3));
	cuPredSuccGraphAddEdge(g, 0, 2, CU_CAST_INT2PTR(2));
	cuPredSuccGraphAddEdge(g, 3, 4, CU_CAST_INT2PTR(1));
	cuPredSuccGraphAddEdge(g, 4, 3, CU_CAST_INT2PTR(0));
	cuPredSuccGraphAddEdge(g, 4, 0, CU_CAST_INT2PTR(-1));

	int sum = 0;

	CU_ITERATE_OVER_PREDSUCC_EDGES(g,edge, source, sink, label, intptr_t) {
		//the paylod is the power of 2 of the id
		//info("edge: %p, id: %ld, data: %d", edge, source->id, label);
		sum += label;
	}
	assert(sum == (5+3+2+1+0+(-1)));

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void testIterateThroughEdges_02(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	g->edgeFunctions.destroy = cuDefaultFunctionDestructorNOP;
	g->nodeFunctions.destroy = cuDefaultFunctionDestructorNOP;

	for (int i=0 ;i<5; i++) {
		cuPredSuccGraphAddNodeInGraphById(g, i, CU_CAST_INT2PTR(i*i));
	}

	int sum = 0;

	CU_ITERATE_OVER_PREDSUCC_EDGES(g,edge, source, sink, label, intptr_t) {
		//the paylod is the power of 2 of the id
		//info("edge: %p, id: %ld, data: %d", edge, label);
		sum += label;
	}
	assert(sum == (0));

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void testIterateThroughEdges_03(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	g->edgeFunctions.destroy = cuDefaultFunctionDestructorNOP;
	g->nodeFunctions.destroy = cuDefaultFunctionDestructorNOP;

	for (int i=0 ;i<5; i++) {
		cuPredSuccGraphAddNodeInGraphById(g, i, CU_CAST_INT2PTR(i*i));
	}
	cuPredSuccGraphAddEdge(g, 3, 4, CU_CAST_INT2PTR(1));

	int sum = 0;

	CU_ITERATE_OVER_PREDSUCC_EDGES(g, edge, source, sink, label, intptr_t) {
		//the paylod is the power of 2 of the id
		//info("edge: %p, id: %ld, data: %d", edge, source->id, label);
		sum += label;
	}
	assert(sum == (1));

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

static bool alwaysTraverse(CU_NOTNULL const Edge* e) {
	return true;
}

/**
 * test with source not in possibleDescendant. Only one node is not a descendant
 */
void test_getFirstNodeWhichIsNotDescendantOf_01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 1, 3, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 4, 3, NULL);

	pint_hash_set* possibleDescendantIds = cuHashSetNew(cuPayloadFunctionsIntValue());
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(4));


	NodeId output;
	bool result = cuPredSuccGraphGetFirstVertexWhichIsNotDescendantOf(g, 1, possibleDescendantIds, alwaysTraverse, &output);

	assert(result);
	assert(output == 4);

	cuHashSetDestroyWithElements(possibleDescendantIds, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

/**
 * test with source not in possibleDescendant. No descendant
 */
void test_getFirstNodeWhichIsNotDescendantOf_02(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 1, 3, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 4, 3, NULL);

	pint_hash_set* possibleDescendantIds = cuHashSetNew(cuPayloadFunctionsIntValue());
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(3));


	NodeId output;
	bool result = cuPredSuccGraphGetFirstVertexWhichIsNotDescendantOf(g, 1, possibleDescendantIds, alwaysTraverse, &output);

	assert(!result);

	cuHashSetDestroyWithElements(possibleDescendantIds, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

/**
 * test with source not in possibleDescendant. Several item not descendant
 */
void test_getFirstNodeWhichIsNotDescendantOf_03(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 1, 3, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 4, 3, NULL);

	pint_hash_set* possibleDescendantIds = cuHashSetNew(cuPayloadFunctionsIntValue());
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(0));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(4));


	NodeId output;
	bool result = cuPredSuccGraphGetFirstVertexWhichIsNotDescendantOf(g, 1, possibleDescendantIds, alwaysTraverse, &output);

	assert(result);
	assert(output == 4 || output == 0);

	cuHashSetDestroyWithElements(possibleDescendantIds, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

/**
 * test with source in possibleDescendant. Only one node is not a descendant
 */
void test_getFirstNodeWhichIsNotDescendantOf_04(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 1, 3, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 4, 3, NULL);

	pint_hash_set* possibleDescendantIds = cuHashSetNew(cuPayloadFunctionsIntValue());
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(4));


	NodeId output;
	bool result = cuPredSuccGraphGetFirstVertexWhichIsNotDescendantOf(g, 1, possibleDescendantIds, alwaysTraverse, &output);

	assert(result);
	assert(output == 4);

	cuHashSetDestroyWithElements(possibleDescendantIds, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

/**
 * test with source in possibleDescendant. No descendant
 */
void test_getFirstNodeWhichIsNotDescendantOf_05(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 1, 3, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 4, 3, NULL);

	pint_hash_set* possibleDescendantIds = cuHashSetNew(cuPayloadFunctionsIntValue());
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(3));


	NodeId output;
	bool result = cuPredSuccGraphGetFirstVertexWhichIsNotDescendantOf(g, 1, possibleDescendantIds, alwaysTraverse, &output);

	assert(!result);

	cuHashSetDestroyWithElements(possibleDescendantIds, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

/**
 * test with source in possibleDescendant. Several item not descendant
 */
void test_getFirstNodeWhichIsNotDescendantOf_06(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 1, 3, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 4, 3, NULL);

	pint_hash_set* possibleDescendantIds = cuHashSetNew(cuPayloadFunctionsIntValue());
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(0));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(2));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(3));
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(4));


	NodeId output;
	bool result = cuPredSuccGraphGetFirstVertexWhichIsNotDescendantOf(g, 1, possibleDescendantIds, alwaysTraverse, &output);

	assert(result);
	assert(output == 4 || output == 0);

	cuHashSetDestroyWithElements(possibleDescendantIds, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

/**
 * test with source in possibleDescendant. Set only one item
 */
void test_getFirstNodeWhichIsNotDescendantOf_07(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 1, 3, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 4, 3, NULL);

	pint_hash_set* possibleDescendantIds = cuHashSetNew(cuPayloadFunctionsIntValue());
	cuHashSetAddItem(possibleDescendantIds, CU_CAST_INT2PTR(0));


	NodeId output;
	bool result = cuPredSuccGraphGetFirstVertexWhichIsNotDescendantOf(g, 0, possibleDescendantIds, alwaysTraverse, &output);

	assert(result);
	assert(output == 0);

	cuHashSetDestroyWithElements(possibleDescendantIds, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void test_hasVertexNoSuccessors_01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 1, 3, NULL);

	assert(!cuPredSuccGraphHasVertexNoSuccessors(g, 0));
	assert(!cuPredSuccGraphHasVertexNoSuccessors(g, 1));
	assert(cuPredSuccGraphHasVertexNoSuccessors(g, 2));

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void test_addEdgeInGraphById_pred_01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew(true);

	Node* n0 = cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	Node* n1 = cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	Node* n2 = cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	Node* n3 = cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	Node* n4 = cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 1, 3, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);

	assert(cuPredSuccGraphHasPredecessorsActive(g));
	int sum = 0;

	//no predecessors
	sum = 0;
	CU_ITERATE_OVER_PREDDSUCC_PREDECESSORS(g, n0, e, sourcePayload, void*) {
		sum += (int)e->source->id;
	}
	assert(sum == 0);

	//one predecessors
	sum = 0;
	CU_ITERATE_OVER_PREDDSUCC_PREDECESSORS(g, n2, e, sourcePayload, void*) {
		sum += (int)e->source->id;
	}
	assert(sum == 1);

	//several predecessors
	sum = 0;
	CU_ITERATE_OVER_PREDDSUCC_PREDECESSORS(g, n3, e, sourcePayload, void*) {
		sum += (int)e->source->id;
	}
	assert(sum == 3);

	cuPredSuccGraphDestroyWithElements(g, NULL);
}

/**
 * save and  load graph which has predecessors enabled
 */
void test_saveAndLoadPredSuccGraph_pred_01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew(true, cuPayloadFunctionsIntValue(), cuPayloadFunctionsIntValue());

	Node* n0 = cuPredSuccGraphAddNodeInGraphById(g, 0, CU_CAST_INT2PTR(10));
	Node* n1 = cuPredSuccGraphAddNodeInGraphById(g, 1, CU_CAST_INT2PTR(11));
	Node* n2 = cuPredSuccGraphAddNodeInGraphById(g, 2, CU_CAST_INT2PTR(12));
	Node* n3 = cuPredSuccGraphAddNodeInGraphById(g, 3, CU_CAST_INT2PTR(13));
	Node* n4 = cuPredSuccGraphAddNodeInGraphById(g, 4, CU_CAST_INT2PTR(14));

	cuPredSuccGraphAddEdge(g, 0, 1, CU_CAST_INT2PTR(1));
	cuPredSuccGraphAddEdge(g, 1, 2, CU_CAST_INT2PTR(1));
	cuPredSuccGraphAddEdge(g, 1, 3, CU_CAST_INT2PTR(1));
	cuPredSuccGraphAddEdge(g, 2, 3, CU_CAST_INT2PTR(1));

	FILE* f = fopen(__func__, "wb");
	cuPredSuccGraphSerialize(f, g);
	fclose(f);

	cuPredSuccGraphDestroyWithElements(g, NULL);

	f = fopen(__func__, "rb");
	g = cuPredSuccGraphDeserialize(f, cuPayloadFunctionsIntValue(), cuPayloadFunctionsIntValue());

	assert(cuPredSuccGraphHasPredecessorsActive(g));
	assert(cuPredSuccGraphGetVertexNumber(g) == 5);
	int sum = 0;

	sum = 0;
	n0 = cuPredSuccGraphGetNodeById(g, 0);
	n1 = cuPredSuccGraphGetNodeById(g, 1);
	n2 = cuPredSuccGraphGetNodeById(g, 2);
	n3 = cuPredSuccGraphGetNodeById(g, 3);
	n4 = cuPredSuccGraphGetNodeById(g, 4);
	CU_ITERATE_OVER_PREDDSUCC_PREDECESSORS(g, n0, e, sourceData, int_ptr) {
		sum += 1;
	}
	assert(sum == 0);

	sum = 0;
	CU_ITERATE_OVER_PREDSUCC_SUCCESSORS(g, n0, e, sinkData, int_ptr) {
		sum += 1;
	}
	assert(sum == 1);

	sum = 0;
	CU_ITERATE_OVER_PREDDSUCC_PREDECESSORS(g, n1, e, sourceData, int_ptr) {
		sum += 1;
	}
	assert(sum == 1);

	sum = 0;
	CU_ITERATE_OVER_PREDSUCC_SUCCESSORS(g, n1, e, sinkData, int_ptr) {
		sum += 1;
	}
	assert(sum == 2);

	sum = 0;
	CU_ITERATE_OVER_PREDDSUCC_PREDECESSORS(g, n2, e, sourceData, int_ptr) {
		sum += 1;
	}
	assert(sum == 1);

	sum = 0;
	CU_ITERATE_OVER_PREDSUCC_SUCCESSORS(g, n2, e, sinkData, int_ptr) {
		sum += 1;
	}
	assert(sum == 1);

	sum = 0;
	CU_ITERATE_OVER_PREDDSUCC_PREDECESSORS(g, n3, e, sourceData, int_ptr) {
		sum += 1;
	}
	assert(sum == 2);

	sum = 0;
	CU_ITERATE_OVER_PREDSUCC_SUCCESSORS(g, n3, e, sinkData, int_ptr) {
		sum += 1;
	}
	assert(sum == 0);

	cuFileUtilsDeleteFile(__func__);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

CuSuite* CuGraphSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestDrawGraph);
	SUITE_ADD_TEST(suite, testclonePredSuccGraph01);
	SUITE_ADD_TEST(suite, testgetEdgeListOfPredSuccGraph01);
	SUITE_ADD_TEST(suite, testpredSuccGraphCmp04);
	SUITE_ADD_TEST(suite, testpredSuccGraphCmp03);
	SUITE_ADD_TEST(suite, testpredSuccGraphCmp02);
	SUITE_ADD_TEST(suite, testpredSuccGraphCmp01);
	SUITE_ADD_TEST(suite, testcheckPredSuccGraphDestruction);
	SUITE_ADD_TEST(suite, testgetEdgeInGraphById01);

	SUITE_ADD_TEST(suite, testSerialization01);

	SUITE_ADD_TEST(suite, test_isNodeReachableFromNode_01);
	SUITE_ADD_TEST(suite, test_isNodeReachableFromNode_02);

	SUITE_ADD_TEST(suite, testIterateThroughNodes_01);
	SUITE_ADD_TEST(suite, testIterateThroughNodes_02);
	SUITE_ADD_TEST(suite, testIterateThroughNodes_03);

	SUITE_ADD_TEST(suite, testIterateThroughEdges_01);
	SUITE_ADD_TEST(suite, testIterateThroughEdges_02);
	SUITE_ADD_TEST(suite, testIterateThroughEdges_03);

	SUITE_ADD_TEST(suite, test_getFirstNodeWhichIsNotDescendantOf_01);
	SUITE_ADD_TEST(suite, test_getFirstNodeWhichIsNotDescendantOf_02);
	SUITE_ADD_TEST(suite, test_getFirstNodeWhichIsNotDescendantOf_03);
	SUITE_ADD_TEST(suite, test_getFirstNodeWhichIsNotDescendantOf_04);
	SUITE_ADD_TEST(suite, test_getFirstNodeWhichIsNotDescendantOf_05);
	SUITE_ADD_TEST(suite, test_getFirstNodeWhichIsNotDescendantOf_06);
	SUITE_ADD_TEST(suite, test_getFirstNodeWhichIsNotDescendantOf_07);

	SUITE_ADD_TEST(suite, test_hasVertexNoSuccessors_01);

	SUITE_ADD_TEST(suite, test_addEdgeInGraphById_pred_01);
	SUITE_ADD_TEST(suite, test_saveAndLoadPredSuccGraph_pred_01);


	return suite;
}

