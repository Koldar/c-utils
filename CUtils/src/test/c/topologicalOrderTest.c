/*
 * topologicalOrderTest.c
 *
 *  Created on: Jun 1, 2017
 *      Author: koldar
 */


#include "CuTest.h"
#include "topologicalOrder.h"
#include <assert.h>
#include "log.h"

void testCormen01(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 0, 1, NULL);
	cuPredSuccGraphAddEdge(g, 1, 2, NULL);
	cuPredSuccGraphAddEdge(g, 2, 3, NULL);
	cuPredSuccGraphAddEdge(g, 3, 4, NULL);

	NodeList* output = cuListNew();
	cuStaticTopologicalOrderDoWith(TO_CORMEN, g, output);

	int i = 0;
	CU_ITERATE_OVER_LIST(output, cell, n, Node*) {
		info("node in position %d is %ld", i, n->id);
		switch (i) {
		case 0: assert(n-> id == 0); break;
		case 1: assert(n-> id == 1); break;
		case 2: assert(n-> id == 2); break;
		case 3: assert(n-> id == 3); break;
		case 4: assert(n-> id == 4); break;
		}
		i += 1;
	}

	cuListDestroy(output, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

void testCormen02(CuTest* tc) {
	PredSuccGraph* g = cuPredSuccGraphNew();

	cuPredSuccGraphAddNodeInGraphById(g, 0, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 1, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 2, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 3, NULL);
	cuPredSuccGraphAddNodeInGraphById(g, 4, NULL);

	cuPredSuccGraphAddEdge(g, 3, 2, NULL);
	cuPredSuccGraphAddEdge(g, 2, 4, NULL);
	cuPredSuccGraphAddEdge(g, 2, 0, NULL);
	cuPredSuccGraphAddEdge(g, 3, 1, NULL);

	cuPredSuccGraphPrintGraph(g, __func__);

	NodeList* output = cuListNew();
	cuStaticTopologicalOrderDoWith(TO_CORMEN, g, output);

	int i=0;
	CU_ITERATE_OVER_LIST(output, cell, node, Node*) {
		info("%d-th element of topological sort is %ld", i, node->id);
		i++;
	}
	assert(
			(
					(((Node*)cuListGetNthItem(output, 0, Node*))->id == 3) &&
					(((Node*)cuListGetNthItem(output, 1, Node*))->id == 2) &&
					(((Node*)cuListGetNthItem(output, 2, Node*))->id == 1) &&
					(((Node*)cuListGetNthItem(output, 3, Node*))->id == 4) &&
					(((Node*)cuListGetNthItem(output, 4, Node*))->id == 0)
			) || (
					(((Node*)cuListGetNthItem(output, 0, Node*))->id == 3) &&
					(((Node*)cuListGetNthItem(output, 1, Node*))->id == 2) &&
					(((Node*)cuListGetNthItem(output, 2, Node*))->id == 1) &&
					(((Node*)cuListGetNthItem(output, 3, Node*))->id == 0) &&
					(((Node*)cuListGetNthItem(output, 4, Node*))->id == 4)
			) || (
					(((Node*)cuListGetNthItem(output, 0, Node*))->id == 3) &&
					(((Node*)cuListGetNthItem(output, 1, Node*))->id == 1) &&
					(((Node*)cuListGetNthItem(output, 2, Node*))->id == 2) &&
					(((Node*)cuListGetNthItem(output, 3, Node*))->id == 4) &&
					(((Node*)cuListGetNthItem(output, 4, Node*))->id == 0)
			) || (
					(((Node*)cuListGetNthItem(output, 0, Node*))->id == 3) &&
					(((Node*)cuListGetNthItem(output, 1, Node*))->id == 1) &&
					(((Node*)cuListGetNthItem(output, 2, Node*))->id == 2) &&
					(((Node*)cuListGetNthItem(output, 3, Node*))->id == 0) &&
					(((Node*)cuListGetNthItem(output, 4, Node*))->id == 4)
			) || (
					(((Node*)cuListGetNthItem(output, 0, Node*))->id == 3) &&
					(((Node*)cuListGetNthItem(output, 1, Node*))->id == 2) &&
					(((Node*)cuListGetNthItem(output, 2, Node*))->id == 4) &&
					(((Node*)cuListGetNthItem(output, 3, Node*))->id == 1) &&
					(((Node*)cuListGetNthItem(output, 4, Node*))->id == 0)
			) || (
					(((Node*)cuListGetNthItem(output, 0, Node*))->id == 3) &&
					(((Node*)cuListGetNthItem(output, 1, Node*))->id == 2) &&
					(((Node*)cuListGetNthItem(output, 2, Node*))->id == 4) &&
					(((Node*)cuListGetNthItem(output, 3, Node*))->id == 0) &&
					(((Node*)cuListGetNthItem(output, 4, Node*))->id == 1)
			)
	);

	cuListDestroy(output, NULL);
	cuPredSuccGraphDestroyWithElements(g, NULL);
}

CuSuite* CuTopologicalOrderSuite() {
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, testCormen01);
	SUITE_ADD_TEST(suite, testCormen02);

	return suite;
}

