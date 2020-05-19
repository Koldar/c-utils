/*
 * @file
 *
 * @brief represent a number which cannot go out of bounds
 *
 * @date May 8, 2019
 * @author koldar
 */

#ifndef WRAPPED_NUMBER_H_
#define WRAPPED_NUMBER_H_

#include <limits.h>
#include <stdbool.h>
#include "macros.h"

typedef int32_t w_int;


typedef int32_t inf_int;
typedef uint32_t inf_uint;

bool cuINIsInfinite(inf_int n);
bool cuINIsPlusInfinite(inf_int n);
bool cuINIsMinusInfinite(inf_int n);

inf_int cuINSum(inf_int a, inf_int b);
inf_int cuINSubstract(inf_int a, inf_int b);
inf_int cuInTimes(inf_int a, inf_int b);
inf_int cuInDiv(inf_int a, inf_int b);

bool cuUINIsInfinite(inf_int n);
bool cuUINIsPlusInfinite(inf_int n);
bool cuUINIsMinusInfinite(inf_int n);

inf_int cuUINSum(inf_uint a, inf_uint b);
inf_int cuUINSubstract(inf_uint a, inf_uint b);
inf_int cuUInTimes(inf_uint a, inf_uint b);
inf_int cuUInDiv(inf_uint a, inf_uint b);


#endif /* WRAPPED_NUMBER_H_ */
