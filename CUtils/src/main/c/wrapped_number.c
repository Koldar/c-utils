#include "wrapped_number.h"

static inf_int PLUS_INFINITY = 1 << sizeof(inf_int) ;

bool cuINIsInfinite(inf_int n) {

}

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

