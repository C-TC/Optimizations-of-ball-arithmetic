#include "big_integer.h"
#include "limits.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void big_integer_add_inplace_inline(const BigInteger left,
                                    const BigInteger right,
                                    BigInteger *pResult);

void big_integer_add_inplace_inline_intrinsics(const BigInteger left,
                                               const BigInteger right,
                                               BigInteger *pResult);

void big_integer_add_inplace_inline_intrinsics_v2(const BigInteger left,
                                                  const BigInteger right,
                                                  BigInteger *pResult);

void big_integer_add_inplace_inline_intrinsics_v3(const BigInteger left,
                                                  const BigInteger right,
                                                  BigInteger *pResult);

void big_integer_add_inplace_inline_intrinsics_v4(const BigInteger left,
                                                  const BigInteger right,
                                                  BigInteger *pResult);

/*
void big_integer_add_inplace_inline_unfold_1x(const BigInteger left,
                                              const BigInteger right,
                                              BigInteger *pResult);

void big_integer_add_inplace_inline_unfold_1x_intrinsics(const BigInteger left,
                                                         const BigInteger right,
                                                         BigInteger *pResult);

void big_integer_add_inplace_inline_unfold_2x_intrinsics(const BigInteger left,
                                                         const BigInteger right,
                                                         BigInteger *pResult);
*/