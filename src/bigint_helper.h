#ifndef __BIGINT_HELPER__
#define __BIGINT_HELPER__
#include "bigint.h"
//================Helper Macro==================================
#define SAFE_FREE(P) do {if (P) free(P);} while(0)
//==============================================================

bool biginteger_is_one(BigInteger*);

bool biginteger_is_negative_one(BigInteger*);

bool biginteger_is_zero(BigInteger*);

//=================Helper Function Integer=====================================
int biginteger_delete(BigInteger*);
int biginteger_print(BigInteger*);
BigInteger* biginteger_zero();
int biginteger_split(BigInteger*, int, BigInteger* , BigInteger*);
BigInteger* biginteger_from_number(int);
BigInteger* biginteger_add_leading_zero(BigInteger*, int);
BigInteger* biginteger_copy(BigInteger*, BigInteger*);
BigInteger* biginteger_left_shift(BigInteger*, int);
BigInteger* biginteger_right_shift(BigInteger*, int);
int biginteger_abs_comp(BigInteger*, BigInteger*);
BigInteger* biginteger_from_string(char*, size_t);
void biginteger_set_inline(BigInteger*, int);
int biginteger_print_debug(BigInteger*);

int biginteger_num_valid_digits(BigInteger*);// 0 has 0 valid bits
void biginteger_set_exact_size(BigInteger*);
BigInteger* biginteger_left_shift_digits(BigInteger*,int);//1 digit is 1 zero, different from biginteger_add_leading_zero
//============================================================================

#endif
