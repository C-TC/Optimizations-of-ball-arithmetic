#ifndef BIG_FLOAT_H
#define BIG_FLOAT_H
#include "big_integer.h"

typedef struct BigFloat
{
    BigInteger mantissa;
    long long power;
} BigFloat;

#endif /* BIG_FLOAT_H */