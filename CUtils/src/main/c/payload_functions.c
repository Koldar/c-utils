/*
 * payload_functions.c
 *
 *  Created on: Oct 11, 2017
 *      Author: koldar
 */


#include "payload_functions.h"
#include "defaultFunctions.h"
#include "errors.h"

payload_functions cuPayloadFunctionsDefault() {
	payload_functions result;
	result.destroy = CU_AS_DESTRUCTOR(cuDefaultFunctionsDestructorObject);
	result.clone = CU_AS_CLONER(cuDefaultFunctionsClonerObject);
	result.bufferString = CU_AS_BUFFERSTRINGER(cuDefaultFunctionsBufferStringerObject);
	result.order = CU_AS_ORDERER(cuDefaultFunctionsOrdererObject);
	result.compare = CU_AS_COMPARER(cuDefaultFunctionsComparatorObject);
	result.serialize = CU_AS_SERIALIZER(cuDefaultFunctionsSerializerObject);
	result.deserialize = CU_AS_DESERIALIZER(cuDefaultFunctionsDeserializerObject);
	return result;
}

payload_functions cuPayloadFunctionsError() {
	payload_functions result;

	result.destroy = CU_AS_DESTRUCTOR(cuDefaultFunctionsDestructorNullObject);
	result.clone = CU_AS_CLONER(cuDefaultFunctionsClonerNullObject);
	result.bufferString = CU_AS_BUFFERSTRINGER(cuDefaultFunctionsBufferStringerNullObject);
	result.order = CU_AS_ORDERER(cuDefaultFunctionsOrdererNullObject);
	result.compare = CU_AS_COMPARER(cuDefaultFunctionsComparatorNullObject);
	result.serialize = CU_AS_SERIALIZER(cuDefaultFunctionsSerializerNullObject);
	result.deserialize = CU_AS_DESERIALIZER(cuDefaultFunctionsDeserializerNullObject);

	return result;
}

payload_functions cuPayloadFunctionsIntValue() {
	payload_functions result;
	result.destroy = CU_AS_DESTRUCTOR(cuDefaultFunctionDestructorNOP);
	result.clone = CU_AS_CLONER(cuDefaultFunctionsClonerObject);
	result.bufferString = CU_AS_BUFFERSTRINGER(cuDefaultFunctionsBufferStringerIntValue);
	result.order = CU_AS_ORDERER(cuDefaultFunctionsOrdererIntValue);
	result.compare = CU_AS_COMPARER(cuDefaultFunctionsComparatorIntValue);
	result.serialize = CU_AS_SERIALIZER(cuDefaultFunctionsSerializerIntValue);
	result.deserialize = CU_AS_DESERIALIZER(cuDefaultFunctionsDeserializerIntValue);
	return result;
}

payload_functions cuPayloadFunctionsIntPtr() {
	payload_functions result;

	result.destroy = CU_AS_DESTRUCTOR(cuDefaultFunctionsDestructorObject);
	result.clone = CU_AS_CLONER(cuDefaultFunctionClonerIntPtr);
	result.bufferString = CU_AS_BUFFERSTRINGER(cuDefaultFunctionsBufferStringerIntPtr);
	result.order = CU_AS_ORDERER(cuDefaultFunctionsOrdererIntPtr);
	result.compare = CU_AS_COMPARER(cuDefaultFunctionsComparatorIntPtr);
	result.serialize = CU_AS_SERIALIZER(cuDefaultFunctionsSerializerIntPtr);
	result.deserialize = CU_AS_DESERIALIZER(cuDefaultFunctionsDeserializerIntPtr);

	return result;

}

payload_functions cuPayloadFunctionsString() {
	payload_functions result;

	result.destroy = CU_AS_DESTRUCTOR(cuDefaultFunctionDestructorNOP);
	result.clone = CU_AS_CLONER(cuDefaultFunctionsClonerObject);
	result.bufferString = CU_AS_BUFFERSTRINGER(cuDefaultFunctionsBufferStringerString);
	result.order = CU_AS_ORDERER(cuDefaultFunctionsOrdererIntValue);
	result.compare = CU_AS_COMPARER(cuDefaultFunctionsComparatorIntValue);
	result.serialize = CU_AS_SERIALIZER(cuDefaultFunctionsSerializerIntValue);
	result.deserialize = CU_AS_DESERIALIZER(cuDefaultFunctionsDeserializerIntValue);

	return result;
}

payload_functions cuPayloadFunctionsBooleanValue() {
	payload_functions result;

	result.destroy = CU_AS_DESTRUCTOR(cuDefaultFunctionDestructorNOP);
	result.clone = CU_AS_CLONER(cuDefaultFunctionClonerIntPtr);
	result.bufferString = CU_AS_BUFFERSTRINGER(cuDefaultFunctionsBufferStringerBoolValue);
	result.order = CU_AS_ORDERER(cuDefaultFunctionsOrdererIntPtr);
	result.compare = CU_AS_COMPARER(cuDefaultFunctionsComparatorIntPtr);
	result.serialize = CU_AS_SERIALIZER(cuDefaultFunctionsSerializerIntPtr);
	result.deserialize = CU_AS_DESERIALIZER(cuDefaultFunctionsDeserializerIntPtr);

	return result;
}
