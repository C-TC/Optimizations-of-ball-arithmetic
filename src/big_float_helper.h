#ifndef BIG_FLOAT_HELPER
#define BIG_FLOAT_HELPER
#include "big_float.h"
#include "big_integer.h"
BigFloat big_float_create(BigInteger, long long);
void big_float_destroy(BigFloat *);
BigFloat big_float_deep_copy(const BigFloat);
#endif
