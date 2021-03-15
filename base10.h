#ifndef __BASE10__
#define __BASE10__
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//================Different Configuration Parameters============
#define UNIT_CAPACITY 1000000000
#define UNIT_LEN 9
#define KARATSUBA_SWITCH 20
//==============================================================

//================Data Structure================================
typedef struct BigInteger{
    bool is_positive;
    int* array;
    size_t array_size;
}BigInteger;

// use the same data representation as BigInteger, however, it always satisfies 1 <= |base| < 10
typedef BigInteger BigFloatBase;

typedef struct BigFloat{
    BigFloatBase *base;
    bool is_zero;
    long exp;
}BigFloat;
//==============================================================

//==========================Compute Function====================
BigInteger* karatsuba(BigInteger*, BigInteger*);
BigFloat *biginteger_div(BigInteger*, BigInteger*);
BigInteger* biginteger_mul(BigInteger*, BigInteger*);
BigInteger* biginteger_mul_scalar(BigInteger*, int);
BigInteger* biginteger_add(BigInteger*, BigInteger*);
//==============================================================

//Other Include Files
#include "base10_helper.h"
#endif
