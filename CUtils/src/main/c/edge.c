/*
 * Edge.c
 *
 *  Created on: Dec 22, 2016
 *      Author: koldar
 */

# define _GNU_SOURCE
#include <stdio.h>
#include "node.h"
#include "log.h"
#include "utility.h"
#include <stdlib.h>
#include "edge.h"
#include "macros.h"
#include "defaultFunctions.h"
#include "errors.h"

Edge* newEdge(const struct Node* restrict source, const struct Node* restrict sink, const void* payload) {
	Edge* retVal = (Edge*) malloc(sizeof(Edge));
	if (retVal == NULL) {
		ERROR_MALLOC();
	}

	retVal->source = (struct Node*) source;
	retVal->sink = (struct Node*) sink;
	retVal->payload = (void*) payload;

	return retVal;
}

Edge* cloneEdge(const Edge* e) {
	return newEdge(e->source, e->sink, e->payload);
}

Edge* cloneEdgeWithPayload(const Edge* e, cloner payloadCloner) {
	return newEdge(e->source, e->sink, payloadCloner(e->payload));
}

void destroyEdge(const Edge* _e, CU_NULLABLE const struct var_args* context) {
	free((void*)_e);
}

void destroyEdgeWithPayload(const Edge* e, destructor payloadDestructor) {
	if (e->payload != NULL) {
		payloadDestructor(e->payload, NULL); //TODO context NULL
	}
	destroyEdge(e, NULL); //TODO context NULL
}

//TODO remove it
int getHashOfEdge(const Edge* e) {
	//to avoid the warning. see https://stackoverflow.com/a/26805655/1887602
	return (intptr_t)e;
}

bool getEdgeCmp(const Edge* e1, const Edge* e2) {
	if ((e1 == NULL) ^ (e2 == NULL)) {
		return false;
	}
	return (e1->source->id == e2->source->id)&&(e1->sink->id == e2->sink->id);
}

bool getEdgeCmpWithPayload(const Edge* e1, const Edge* e2, comparator payloadComparator) {
	if (!getEdgeCmp(e1, e2)) {
		return false;
	}
	if (e1->payload == NULL ^ e2->payload == NULL) {
		return true;
	}
	return payloadComparator(e1->payload, e2->payload);
}

int getBufferStringOfEdge(const Edge* e, char* buffer) {
	int i = 0;

	i += sprintf(&buffer[i], "%ld->%ld", e->source->id, e->sink->id);

	return i;
}

void storeEdgeInFile(FILE* f, const Edge* e, object_serializer payload) {
	fwrite(&e->source->id, sizeof(NodeId), 1, f);
	fwrite(&e->sink->id, sizeof(NodeId), 1, f);
	payload(f, e->payload);
}

Edge* loadEdgeFromFile(FILE* f, struct PredSuccGraph* g, object_deserializer edgeDeserializer) {
	NodeId id = 0;
	Node* source;
	Node* sink;
	//we need to clear it because it may contain an integer and sizeof(int) < sizeof(void*), hence some bits may
	//remain unintialized, which can lead to invalid read.
	void* payload = NULL;

	Node* cuPredSuccGraphGetNodeById(struct PredSuccGraph* g, NodeId id);

	//to make a warning disappear
	int sourceRead = fread(&id, sizeof(id), 1, f);
	source = cuPredSuccGraphGetNodeById(g, id);
	//to make a warning disappear
	int sinkRead = fread(&id, sizeof(id), 1, f);
	sink = cuPredSuccGraphGetNodeById(g, id);
	edgeDeserializer(f, &payload);
	return newEdge(source, sink, payload);
}

payload_functions cuEdgeGetPayoadFunctions() {
	payload_functions result;

	result.destroy = CU_AS_DESTRUCTOR(destroyEdge);

	return result;
}
