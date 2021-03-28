#ifndef BIG_FLOAT_H
#define BIG_FLOAT_H
#include "big_integer.h"

typedef struct BigFloat
{
    BigInteger mantissa;
    long long power;
} BigFloat;

BigFloat big_float_multiply(BigFloat, BigFloat);
BigFloat big_float_div(BigFloat, BigFloat);

#endif /* BIG_FLOAT_H */