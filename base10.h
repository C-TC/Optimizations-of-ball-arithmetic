#ifndef __BBASE10__
#define __BBASE10__
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
//================Different Configuration Parameters============
#define UNIT_CAPACITY 1000000000
#define UNIT_LEN 9
#define KARATSUBA_SWITCH 20
//how many digits after the decimal point
#define DIV_PREC 4
//==============================================================

//================Data Structure================================
typedef struct BigInteger{
    bool is_positive;
    int* array;
    size_t array_size;
}BigInteger;
//==============================================================

//==========================Compute Function====================
BigInteger* karatsuba(BigInteger*, BigInteger*);
int biginteger_div(BigInteger*, BigInteger*, BigInteger*, BigInteger*);
BigInteger* biginteger_mul(BigInteger*, BigInteger*);
BigInteger* biginteger_mul_scalar(BigInteger*, int);
BigInteger* biginteger_add(BigInteger*, BigInteger*, BigInteger*);
//==============================================================

//Other Include Files
#include "base10_helper.h"
#endif
