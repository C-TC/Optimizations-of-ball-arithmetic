#ifndef __BASE10_HELPER__
#define __BASE10_HELPER__
#include "base10.h"
//================Helper Macro==================================
#define SAFE_FREE(P) do {if (P) free(P);} while(0)
//================Helper Function Configuration=================
//how many digits after the decimal point
#define DIV_PRE 4
//==============================================================

inline bool biginteger_is_one(BigInteger* p_biginteger){
    return p_biginteger->array_size == 1 && p_biginteger->array[0] == 1 && p_biginteger->is_positive;
}

inline bool biginteger_is_negative_one(BigInteger* p_biginteger){
    return p_biginteger->array_size == 1 && p_biginteger->array[0] == 1 && !p_biginteger->is_positive;
}

inline bool biginteger_is_zero(BigInteger* p_biginteger){
    return p_biginteger->array_size == 1 && p_biginteger->array[0] == 0;
}

//=================Helper Function Integer=====================================
int biginteger_delete(BigInteger*);
int biginteger_print(BigInteger*);
BigInteger* biginteger_zero();
int biginteger_split(BigInteger*, int, BigInteger* , BigInteger*);
BigInteger* biginteger_from_number(int);
BigInteger* biginteger_add_leading_zero(BigInteger*, int);
BigInteger* biginteger_copy(BigInteger*);
BigInteger* biginteger_left_shift(BigInteger*, int);
BigInteger* biginteger_right_shift(BigInteger*, int);
int biginteger_abs_comp(BigInteger*, BigInteger*);
BigInteger* biginteger_from_string(char*, size_t);
int biginteger_print_debug(BigInteger*);
//============================================================================

//=================Helper Function Float======================================
void bigfloat_print(BigFloat*);
void bigfloat_delete(BigFloat*);
//=============================================================================
#endif
