/*
 * simple_loops_computer.h
 *
 *  Created on: Jul 25, 2017
 *      Author: koldar
 */

#ifndef SIMPLE_LOOPS_COMPUTER_H_
#define SIMPLE_LOOPS_COMPUTER_H_

#include "predsuccgraph.h"
#include "scc.h"
#include "var_args.h"

/**
 * a list of nodes ::Node
 */
typedef list node_list;
/**
 * a loop is a node list
 */
typedef node_list loop;
/**
 * a loop list is a list containing node_lists as elements
 */
typedef list loop_list;

typedef struct johnson_support johnson_support;

johnson_support* cuJohnsonSupportNew();

void cuJohnsonSupportDestroy(johnson_support* support, CU_NULLABLE const struct var_args* context);
#define CU_FUNCTION_POINTER_destructor_void_cuJohnsonSupportDestroy_voidConstPtr_var_argsConstPtr CU_DESTRUCTOR_ID

loop_list* cuJohnsonSupportComputeSimpleLoops(johnson_support* support, const PredSuccGraph* g, edge_traverser t);

/**
 * Compute a set fo functions to easily manage the paylaod of a container when ::loop are the payload
 *
 * @return the specified payload_functions
 */
payload_functions cuSimpleLoopComputerPayloadFunctionsLoop();

#endif /* SIMPLE_LOOPS_COMPUTER_H_ */
