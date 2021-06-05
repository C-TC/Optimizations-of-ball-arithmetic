#include "big_integer_add.h"
#include <immintrin.h>
#include <x86intrin.h>

const int UINT_NUM_BYTES = (sizeof(unsigned long));
const int UINT_NUM_BITS = 32;
const unsigned long bit_mask = UINT_MAX;
// extern const int UINT_NUM_BYTES;
// extern const int UINT_NUM_BITS;
// extern const unsigned long bit_mask;

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

// typedef void(*comp_func)(const BigInteger, const BigInteger, BigInteger*);
// void add_function(comp_func f, char* name, int flop);

void big_integer_add_inplace_inline(const BigInteger left,
                                    const BigInteger right,
                                    BigInteger *pResult) {
  int size = MAX(left.data.size, right.data.size);
  if (pResult->data.capacity <= size + 1) {
    printf("resize pResult\n");
    unsigned long *bits = (unsigned long *)malloc((size + 1) * UINT_NUM_BYTES);
    free(pResult->data.bits);
    pResult->data.bits = bits;
    pResult->data.capacity = size + 1;
  }
  if (left.sign == 0) {
    pResult->sign = right.sign;
    pResult->data.size = right.data.size;
    memcpy(pResult->data.bits, right.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }
  if (right.sign == 0) {
    pResult->sign = left.sign;
    pResult->data.size = left.data.size;
    memcpy(pResult->data.bits, left.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  if (left.sign == right.sign) {
    pResult->sign = left.sign;
    unsigned long sum = 0;
    int i;
    for (i = 0; i < size; ++i) {
      // if (i < left.data.size) {
      //   sum += left.data.bits[i];
      // }
      // if (i < right.data.size) {
      //   sum += right.data.bits[i];
      // }
      // assume left and right have same size
      sum += left.data.bits[i] + right.data.bits[i];
      pResult->data.bits[i] = sum & bit_mask;
      sum >>= UINT_NUM_BITS;
    }
    if (sum > 0) {
      assert(sum <= bit_mask);
      pResult->data.bits[i] = sum;
      i++;
    }
    // printf("--------%lu------\n", pResult->data.bits[3]);
    pResult->data.size = i;
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  // int compRes = big_integer_compare_data(&left.data, &right.data);
  int compRes = -2;
  if (left.data.size < right.data.size) {
    compRes = 1;
  } else if (left.data.size > right.data.size) {
    compRes = -1;
  } else {
    int i;
    for (i = size - 1; i >= 0; --i) {
      if (left.data.bits[i] > right.data.bits[i]) {
        compRes = 1;
        break;
      }
      if (left.data.bits[i] < right.data.bits[i]) {
        compRes = -1;
        break;
      }
    }
    if (compRes == -2) {
      compRes = 0;
    }
  }

  // if (compRes == 0) {
  //   big_integer_set(0, pResult);
  // } else if (compRes > 0) {
  //   // left > right
  //   pResult->sign = left.sign;
  //   big_integer_subtract_data_inplace(left.data, right.data, &pResult->data);
  // } else {
  //   pResult->sign = right.sign;
  //   big_integer_subtract_data_inplace(right.data, left.data, &pResult->data);
  // }
  if (compRes == 0) {
    pResult->sign = 0;
    pResult->data.size = 0;
    memset(pResult->data.bits, 0, pResult->data.capacity * UINT_NUM_BYTES);
  } else if (compRes > 0) {
    pResult->sign = left.sign;
    unsigned long borrow = 0;
    int i;
    for (i = 0; i < size; ++i) {
      borrow = left.data.bits[i] - right.data.bits[i] - borrow;
      pResult->data.bits[i] = borrow & bit_mask;
      borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    assert(borrow == 0);
    memset(pResult->data.bits + size, 0, pResult->data.capacity - size);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
    // memset(pResult->data.bits + pResult->data.size, 0,
    //        pResult->data.capacity - pResult->data.size);
  } else {
    pResult->sign = right.sign;
    unsigned long borrow = 0;
    int i;
    for (i = 0; i < size; ++i) {
      borrow = right.data.bits[i] - left.data.bits[i] - borrow;
      pResult->data.bits[i] = borrow & bit_mask;
      borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    assert(borrow == 0);
    memset(pResult->data.bits + size, 0, pResult->data.capacity - size);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
    // memset(pResult->data.bits + pResult->data.size, 0,
    //        pResult->data.capacity - pResult->data.size);
  }
  return;
}

// use intrinsics
void big_integer_add_inplace_inline_intrinsics(const BigInteger left,
                                               const BigInteger right,
                                               BigInteger *pResult) {
  int size = MAX(left.data.size, right.data.size);
  if (pResult->data.capacity <= size + 1) {
    printf("resize pResult\n");
    unsigned long *bits = (unsigned long *)malloc((size + 1) * UINT_NUM_BYTES);
    free(pResult->data.bits);
    pResult->data.bits = bits;
    pResult->data.capacity = size + 1;
  }
  if (left.sign == 0) {
    pResult->sign = right.sign;
    pResult->data.size = right.data.size;
    memcpy(pResult->data.bits, right.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }
  if (right.sign == 0) {
    pResult->sign = left.sign;
    pResult->data.size = left.data.size;
    memcpy(pResult->data.bits, left.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  if (left.sign == right.sign) {
    pResult->sign = left.sign;
    // unsigned long sum = 0;
    int i;
    unsigned char carry = 0;
    unsigned int *uintpLeft = (unsigned int *)left.data.bits;
    unsigned int *uintpRight = (unsigned int *)right.data.bits;
    unsigned int *uintpResult = (unsigned int *)pResult->data.bits;

    for (i = 0; i < size; ++i) {
      // assume left and right have same size
      carry = _addcarryx_u32(carry, uintpLeft[2 * i], uintpRight[2 * i],
                             uintpResult + 2 * i);
      // sum += left.data.bits[i] + right.data.bits[i];
      // pResult->data.bits[i] = sum & bit_mask;
      // sum >>= UINT_NUM_BITS;
    }
    if (carry > 0) {
      pResult->data.bits[i] = carry;
      i++;
    }
    // printf("--------%lu------\n", pResult->data.bits[3]);
    pResult->data.size = i;
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  // int compRes = big_integer_compare_data(&left.data, &right.data);
  int compRes = -2;
  if (left.data.size < right.data.size) {
    compRes = 1;
  } else if (left.data.size > right.data.size) {
    compRes = -1;
  } else {
    int i;
    for (i = size - 1; i >= 0; --i) {
      if (left.data.bits[i] > right.data.bits[i]) {
        compRes = 1;
        break;
      }
      if (left.data.bits[i] < right.data.bits[i]) {
        compRes = -1;
        break;
      }
    }
    if (compRes == -2) {
      compRes = 0;
    }
  }

  // if (compRes == 0) {
  //   big_integer_set(0, pResult);
  // } else if (compRes > 0) {
  //   // left > right
  //   pResult->sign = left.sign;
  //   big_integer_subtract_data_inplace(left.data, right.data, &pResult->data);
  // } else {
  //   pResult->sign = right.sign;
  //   big_integer_subtract_data_inplace(right.data, left.data, &pResult->data);
  // }
  if (compRes == 0) {
    pResult->sign = 0;
    pResult->data.size = 0;
    memset(pResult->data.bits, 0, pResult->data.capacity * UINT_NUM_BYTES);
  } else if (compRes > 0) {
    pResult->sign = left.sign;
    // unsigned long borrow = 0;
    int i;
    unsigned int carry = 0;
    unsigned int *uintpLeft = (unsigned int *)left.data.bits;
    unsigned int *uintpRight = (unsigned int *)right.data.bits;
    unsigned int *uintpResult = (unsigned int *)pResult->data.bits;
    for (i = 0; i < size; ++i) {
      _addcarry_u32(0, uintpLeft[2 * i] + carry, -uintpRight[2 * i],
                    uintpResult + 2 * i);
      if (uintpLeft[2 * i] + carry >= uintpRight[2 * i]) {
        // printf(">= carry %u\n", carry);
        carry = 0;
        // printf(">= carry %u\n", carry);
      } else {
        // printf("< carry %u\n", carry);
        carry = -1;
        // printf("< carry %u\n", carry);
      }
      // borrow = left.data.bits[i] - right.data.bits[i] - borrow;
      // pResult->data.bits[i] = borrow & bit_mask;
      // borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    // if (carry != 0) {
    //  // printf("original: %u; carry: %u\n", uintpResult[2 * i], carry);
    //  uintpResult[2 * i - 2] -= carry;
    //}
    // assert(borrow == 0);
    assert(carry == 0);
    memset(pResult->data.bits + size, 0, pResult->data.capacity - size);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
  } else {
    pResult->sign = right.sign;
    unsigned int carry = 0;
    unsigned int *uintpLeft = (unsigned int *)left.data.bits;
    unsigned int *uintpRight = (unsigned int *)right.data.bits;
    unsigned int *uintpResult = (unsigned int *)pResult->data.bits;
    int i;
    for (i = 0; i < size; ++i) {
      _addcarry_u32(0, -uintpLeft[2 * i], uintpRight[2 * i] + carry,
                    uintpResult + 2 * i);
      if (uintpRight[2 * i] + carry >= uintpLeft[2 * i]) {
        carry = 0;
      } else {
        carry = -1;
      }
      // borrow = right.data.bits[i] - left.data.bits[i] - borrow;
      // pResult->data.bits[i] = borrow & bit_mask;
      // borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    if (carry != 0) {
      // printf("original: %u; carry: %u\n", uintpResult[2 * i], carry);
      uintpResult[2 * i - 2] -= carry;
    }
    // assert(borrow == 0);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    memset(pResult->data.bits + size, 0, pResult->data.capacity - size);
    assert(i != 0 || pResult->data.bits[0] > 0);
  }
  return;
}

void big_integer_add_inplace_inline_intrinsics_v2(const BigInteger left,
                                                  const BigInteger right,
                                                  BigInteger *pResult) {
  int size = MAX(left.data.size, right.data.size);
  if (pResult->data.capacity <= size + 1) {
    printf("resize pResult\n");
    unsigned long *bits = (unsigned long *)malloc((size + 1) * UINT_NUM_BYTES);
    free(pResult->data.bits);
    pResult->data.bits = bits;
    pResult->data.capacity = size + 1;
  }
  if (left.sign == 0) {
    pResult->sign = right.sign;
    pResult->data.size = right.data.size;
    memcpy(pResult->data.bits, right.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }
  if (right.sign == 0) {
    pResult->sign = left.sign;
    pResult->data.size = left.data.size;
    memcpy(pResult->data.bits, left.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  if (left.sign == right.sign) {
    pResult->sign = left.sign;
    // unsigned long sum = 0;
    int i;
    unsigned char carry = 0;
    unsigned int *uintpLeft = (unsigned int *)left.data.bits;
    unsigned int *uintpRight = (unsigned int *)right.data.bits;
    unsigned int *uintpResult = (unsigned int *)pResult->data.bits;

    for (i = 0; i < size * 2; i += 2) {
      // assume left and right have same size
      carry =
          _addcarryx_u32(carry, uintpLeft[i], uintpRight[i], uintpResult + i);
      // sum += left.data.bits[i] + right.data.bits[i];
      // pResult->data.bits[i] = sum & bit_mask;
      // sum >>= UINT_NUM_BITS;
    }
    if (carry > 0) {
      pResult->data.bits[i / 2] = carry;
      i += 2;
    }
    // printf("--------%lu------\n", pResult->data.bits[3]);
    pResult->data.size = i / 2;
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  // int compRes = big_integer_compare_data(&left.data, &right.data);
  int compRes = -2;
  if (left.data.size < right.data.size) {
    compRes = 1;
  } else if (left.data.size > right.data.size) {
    compRes = -1;
  } else {
    int i;
    for (i = size - 1; i >= 0; --i) {
      if (left.data.bits[i] > right.data.bits[i]) {
        compRes = 1;
        break;
      }
      if (left.data.bits[i] < right.data.bits[i]) {
        compRes = -1;
        break;
      }
    }
    if (compRes == -2) {
      compRes = 0;
    }
  }

  // if (compRes == 0) {
  //   big_integer_set(0, pResult);
  // } else if (compRes > 0) {
  //   // left > right
  //   pResult->sign = left.sign;
  //   big_integer_subtract_data_inplace(left.data, right.data, &pResult->data);
  // } else {
  //   pResult->sign = right.sign;
  //   big_integer_subtract_data_inplace(right.data, left.data, &pResult->data);
  // }
  if (compRes == 0) {
    pResult->sign = 0;
    pResult->data.size = 0;
    memset(pResult->data.bits, 0, pResult->data.capacity * UINT_NUM_BYTES);
  } else if (compRes > 0) {
    pResult->sign = left.sign;
    // unsigned long borrow = 0;
    int i;
    unsigned int carry = 0;
    unsigned int *uintpLeft = (unsigned int *)left.data.bits;
    unsigned int *uintpRight = (unsigned int *)right.data.bits;
    unsigned int *uintpResult = (unsigned int *)pResult->data.bits;
    for (i = 0; i < size; ++i) {
      _addcarry_u32(0, uintpLeft[2 * i] + carry, -uintpRight[2 * i],
                    uintpResult + 2 * i);
      if (uintpLeft[2 * i] + carry >= uintpRight[2 * i]) {
        // printf(">= carry %u\n", carry);
        carry = 0;
        // printf(">= carry %u\n", carry);
      } else {
        // printf("< carry %u\n", carry);
        carry = -1;
        // printf("< carry %u\n", carry);
      }
      // borrow = left.data.bits[i] - right.data.bits[i] - borrow;
      // pResult->data.bits[i] = borrow & bit_mask;
      // borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    // if (carry != 0) {
    //  // printf("original: %u; carry: %u\n", uintpResult[2 * i], carry);
    //  uintpResult[2 * i - 2] -= carry;
    //}
    // assert(borrow == 0);
    assert(carry == 0);
    memset(pResult->data.bits + size, 0, pResult->data.capacity - size);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
  } else {
    pResult->sign = right.sign;
    unsigned int carry = 0;
    unsigned int *uintpLeft = (unsigned int *)left.data.bits;
    unsigned int *uintpRight = (unsigned int *)right.data.bits;
    unsigned int *uintpResult = (unsigned int *)pResult->data.bits;
    int i;
    for (i = 0; i < size; ++i) {
      _addcarry_u32(0, -uintpLeft[2 * i], uintpRight[2 * i] + carry,
                    uintpResult + 2 * i);
      if (uintpRight[2 * i] + carry >= uintpLeft[2 * i]) {
        carry = 0;
      } else {
        carry = -1;
      }
      // borrow = right.data.bits[i] - left.data.bits[i] - borrow;
      // pResult->data.bits[i] = borrow & bit_mask;
      // borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    if (carry != 0) {
      // printf("original: %u; carry: %u\n", uintpResult[2 * i], carry);
      uintpResult[2 * i - 2] -= carry;
    }
    // assert(borrow == 0);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    memset(pResult->data.bits + size, 0, pResult->data.capacity - size);
    assert(i != 0 || pResult->data.bits[0] > 0);
  }
  return;
}

void big_integer_add_inplace_inline_intrinsics_v3(const BigInteger left,
                                                  const BigInteger right,
                                                  BigInteger *pResult) {
  int size = MAX(left.data.size, right.data.size);
  if (pResult->data.capacity <= size + 1) {
    printf("resize pResult\n");
    unsigned long *bits = (unsigned long *)malloc((size + 1) * UINT_NUM_BYTES);
    free(pResult->data.bits);
    pResult->data.bits = bits;
    pResult->data.capacity = size + 1;
  }
  if (left.sign == 0) {
    pResult->sign = right.sign;
    pResult->data.size = right.data.size;
    memcpy(pResult->data.bits, right.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }
  if (right.sign == 0) {
    pResult->sign = left.sign;
    pResult->data.size = left.data.size;
    memcpy(pResult->data.bits, left.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  if (left.sign == right.sign) {
    pResult->sign = left.sign;
    // unsigned long sum = 0;
    int i;
    unsigned char carry1 = 0;
    unsigned char carry2 = 0;
    int half = size / 2;
    unsigned int *uintpLeft1 = (unsigned int *)left.data.bits;
    unsigned int *uintpRight1 = (unsigned int *)right.data.bits;
    unsigned int *uintpResult1 = (unsigned int *)pResult->data.bits;
    unsigned int *uintpLeft2 = (unsigned int *)(left.data.bits + half);
    unsigned int *uintpRight2 = (unsigned int *)(right.data.bits + half);
    unsigned int *uintpResult2 = (unsigned int *)(pResult->data.bits + half);

    for (i = 0; i < half * 2; i += 2) {
      // assume left and right have same size
      carry1 = _addcarryx_u32(carry1, uintpLeft1[i], uintpRight1[i],
                              uintpResult1 + i);
      carry2 = _addcarryx_u32(carry2, uintpLeft2[i], uintpRight2[i],
                              uintpResult2 + i);
      // sum += left.data.bits[i] + right.data.bits[i];
      // pResult->data.bits[i] = sum & bit_mask;
      // sum >>= UINT_NUM_BITS;
    }
    if (size % 2 != 0) {
      carry2 = _addcarryx_u32(carry2, uintpLeft2[i], uintpRight2[i],
                              uintpResult2 + i);
      // i += 2;
    }
    if (carry2 > 0) {
      uintpResult2[size] = carry2;
      // pResult->data.bits[i / 2] = carry2;
      // i += 2;
      pResult->data.size = size + 1;
    } else {
      pResult->data.size = size;
    }
    while (i < size * 2 && carry1 > 0) {
      carry1 = _addcarryx_u32(carry1, uintpLeft1[i], uintpRight1[i],
                              uintpResult1 + i);
      i += 2;
    }
    if (carry1 > 0) {
      pResult->data.bits[i / 2] = carry2;
      i += 2;
    }
    // printf("--------%lu------\n", pResult->data.bits[3]);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  // int compRes = big_integer_compare_data(&left.data, &right.data);
  int compRes = -2;
  if (left.data.size < right.data.size) {
    compRes = 1;
  } else if (left.data.size > right.data.size) {
    compRes = -1;
  } else {
    int i;
    for (i = size - 1; i >= 0; --i) {
      if (left.data.bits[i] > right.data.bits[i]) {
        compRes = 1;
        break;
      }
      if (left.data.bits[i] < right.data.bits[i]) {
        compRes = -1;
        break;
      }
    }
    if (compRes == -2) {
      compRes = 0;
    }
  }

  // if (compRes == 0) {
  //   big_integer_set(0, pResult);
  // } else if (compRes > 0) {
  //   // left > right
  //   pResult->sign = left.sign;
  //   big_integer_subtract_data_inplace(left.data, right.data, &pResult->data);
  // } else {
  //   pResult->sign = right.sign;
  //   big_integer_subtract_data_inplace(right.data, left.data, &pResult->data);
  // }
  if (compRes == 0) {
    pResult->sign = 0;
    pResult->data.size = 0;
    memset(pResult->data.bits, 0, pResult->data.capacity * UINT_NUM_BYTES);
  } else if (compRes > 0) {
    pResult->sign = left.sign;
    // unsigned long borrow = 0;
    int i;
    unsigned int carry = 0;
    unsigned int *uintpLeft = (unsigned int *)left.data.bits;
    unsigned int *uintpRight = (unsigned int *)right.data.bits;
    unsigned int *uintpResult = (unsigned int *)pResult->data.bits;
    for (i = 0; i < size; ++i) {
      _addcarry_u32(0, uintpLeft[2 * i] + carry, -uintpRight[2 * i],
                    uintpResult + 2 * i);
      if (uintpLeft[2 * i] + carry >= uintpRight[2 * i]) {
        // printf(">= carry %u\n", carry);
        carry = 0;
        // printf(">= carry %u\n", carry);
      } else {
        // printf("< carry %u\n", carry);
        carry = -1;
        // printf("< carry %u\n", carry);
      }
      // borrow = left.data.bits[i] - right.data.bits[i] - borrow;
      // pResult->data.bits[i] = borrow & bit_mask;
      // borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    // if (carry != 0) {
    //  // printf("original: %u; carry: %u\n", uintpResult[2 * i], carry);
    //  uintpResult[2 * i - 2] -= carry;
    //}
    // assert(borrow == 0);
    assert(carry == 0);
    memset(pResult->data.bits + size, 0, pResult->data.capacity - size);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
  } else {
    pResult->sign = right.sign;
    unsigned int carry = 0;
    unsigned int *uintpLeft = (unsigned int *)left.data.bits;
    unsigned int *uintpRight = (unsigned int *)right.data.bits;
    unsigned int *uintpResult = (unsigned int *)pResult->data.bits;
    int i;
    for (i = 0; i < size; ++i) {
      _addcarry_u32(0, -uintpLeft[2 * i], uintpRight[2 * i] + carry,
                    uintpResult + 2 * i);
      if (uintpRight[2 * i] + carry >= uintpLeft[2 * i]) {
        carry = 0;
      } else {
        carry = -1;
      }
      // borrow = right.data.bits[i] - left.data.bits[i] - borrow;
      // pResult->data.bits[i] = borrow & bit_mask;
      // borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    if (carry != 0) {
      // printf("original: %u; carry: %u\n", uintpResult[2 * i], carry);
      uintpResult[2 * i - 2] -= carry;
    }
    // assert(borrow == 0);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    memset(pResult->data.bits + size, 0, pResult->data.capacity - size);
    assert(i != 0 || pResult->data.bits[0] > 0);
  }
  return;
}

void big_integer_add_inplace_inline_intrinsics_v4(const BigInteger left,
                                                  const BigInteger right,
                                                  BigInteger *pResult) {
  int size = MAX(left.data.size, right.data.size);
  if (pResult->data.capacity <= size + 1) {
    printf("resize pResult\n");
    unsigned long *bits = (unsigned long *)malloc((size + 1) * UINT_NUM_BYTES);
    free(pResult->data.bits);
    pResult->data.bits = bits;
    pResult->data.capacity = size + 1;
  }
  if (left.sign == 0) {
    pResult->sign = right.sign;
    pResult->data.size = right.data.size;
    memcpy(pResult->data.bits, right.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }
  if (right.sign == 0) {
    pResult->sign = left.sign;
    pResult->data.size = left.data.size;
    memcpy(pResult->data.bits, left.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  if (left.sign == right.sign) {
    pResult->sign = left.sign;
    // unsigned long sum = 0;
    int i;
    __m256i op1, op2, sum;
    for (i = 0; i < size - 3; i += 4) {
      op1 = _mm256_loadu_si256((__m256i *)(left.data.bits + i));
      op2 = _mm256_loadu_si256((__m256i *)(right.data.bits + i));
      sum = _mm256_add_epi64(op1, op2);
      _mm256_storeu_si256((__m256i *)(pResult->data.bits + i), sum);
    }
    for (; i < size; ++i) {
      pResult->data.bits[i] = left.data.bits[i] + right.data.bits[i];
    }

    // __m256i mask = _mm256_set1_epi64x(0x0000000000000001);
    // __m256i mask2 = _mm256_set1_epi64x(0x00000000ffffffff);
    // unsigned long *shiftedResult =
    //     (unsigned long *)(((unsigned int *)pResult->data.bits) + 1);
    // __m256i shifted_sum;
    // __m256i carry;
    // for (i = 0; i < size - 4; i += 4) {
    //   shifted_sum = _mm256_loadu_si256((__m256i *)(shiftedResult + i));
    //   carry = _mm256_and_si256(shifted_sum, mask);
    //   printf("%lu, %lu, %lu, %lu\n", carry[3], carry[2], carry[1], carry[0]);
    //   sum = _mm256_loadu_si256((__m256i *)pResult->data.bits + i + 1);
    //   printf("%lu, %lu, %lu, %lu\n", sum[0], sum[1], sum[2], sum[3]);
    //   sum = _mm256_add_epi64(sum, carry);
    //   sum = _mm256_and_si256(sum, mask2);
    //   printf("%lu, %lu, %lu, %lu\n", sum[0], sum[1], sum[2], sum[3]);
    //   carry = _mm256_andnot_si256(mask, shifted_sum);
    //   _mm256_storeu_si256((__m256i *)(shiftedResult + i), carry);
    //   _mm256_storeu_si256((__m256i *)(pResult->data.bits + i + 1), sum);
    // }
    // for (; i < size; ++i) {
    //   if ((shiftedResult[i] & 0x00000001) > 0) {
    //     shiftedResult[i] &= 0xffff0000;
    //     pResult->data.bits[i + 1] += 1;
    //   }
    // }

    // unsigned int *uintpResult = (unsigned int *)pResult;
    // unsigned char carry = 0;

    unsigned long carry = 0;
    unsigned long mask = 0x0000000100000000;
    unsigned long mask2 = 0x00000000ffffffff;
    for (i = 0; i < size; i++) {
      if (pResult->data.bits[i] & mask) {
        pResult->data.bits[i] += carry;
        pResult->data.bits[i] &= mask2;
        carry = 1;
      } else {
        pResult->data.bits[i] += carry;
        carry = 0;
      }
      // printf("%d\n", i);
      // if (uintpResult[i - 1] != 0 || carry != 0) {
      // if (uintpResult[i - 1] != 0) {
      // uintpResult[i - 1] = 0;
      // carry = _addcarryx_u32(carry, 1, uintpResult[i], &uintpResult[i]);
      // }
    }
    if (carry > 0) {
      pResult->data.bits[size] = 1;
      pResult->data.size = size + 1;
    } else {
      pResult->data.size = size;
    }
    return;
  }

  // int compRes = big_integer_compare_data(&left.data, &right.data);
  int compRes = -2;
  if (left.data.size < right.data.size) {
    compRes = 1;
  } else if (left.data.size > right.data.size) {
    compRes = -1;
  } else {
    int i;
    for (i = size - 1; i >= 0; --i) {
      if (left.data.bits[i] > right.data.bits[i]) {
        compRes = 1;
        break;
      }
      if (left.data.bits[i] < right.data.bits[i]) {
        compRes = -1;
        break;
      }
    }
    if (compRes == -2) {
      compRes = 0;
    }
  }

  // if (compRes == 0) {
  //   big_integer_set(0, pResult);
  // } else if (compRes > 0) {
  //   // left > right
  //   pResult->sign = left.sign;
  //   big_integer_subtract_data_inplace(left.data, right.data, &pResult->data);
  // } else {
  //   pResult->sign = right.sign;
  //   big_integer_subtract_data_inplace(right.data, left.data, &pResult->data);
  // }
  if (compRes == 0) {
    pResult->sign = 0;
    pResult->data.size = 0;
    memset(pResult->data.bits, 0, pResult->data.capacity * UINT_NUM_BYTES);
  } else if (compRes > 0) {
    pResult->sign = left.sign;
    // unsigned long borrow = 0;
    int i;
    unsigned int carry = 0;
    unsigned int *uintpLeft = (unsigned int *)left.data.bits;
    unsigned int *uintpRight = (unsigned int *)right.data.bits;
    unsigned int *uintpResult = (unsigned int *)pResult->data.bits;
    for (i = 0; i < size; ++i) {
      _addcarry_u32(0, uintpLeft[2 * i] + carry, -uintpRight[2 * i],
                    uintpResult + 2 * i);
      if (uintpLeft[2 * i] + carry >= uintpRight[2 * i]) {
        // printf(">= carry %u\n", carry);
        carry = 0;
        // printf(">= carry %u\n", carry);
      } else {
        // printf("< carry %u\n", carry);
        carry = -1;
        // printf("< carry %u\n", carry);
      }
      // borrow = left.data.bits[i] - right.data.bits[i] - borrow;
      // pResult->data.bits[i] = borrow & bit_mask;
      // borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    // if (carry != 0) {
    //  // printf("original: %u; carry: %u\n", uintpResult[2 * i], carry);
    //  uintpResult[2 * i - 2] -= carry;
    //}
    // assert(borrow == 0);
    assert(carry == 0);
    memset(pResult->data.bits + size, 0, pResult->data.capacity - size);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
  } else {
    pResult->sign = right.sign;
    unsigned int carry = 0;
    unsigned int *uintpLeft = (unsigned int *)left.data.bits;
    unsigned int *uintpRight = (unsigned int *)right.data.bits;
    unsigned int *uintpResult = (unsigned int *)pResult->data.bits;
    int i;
    for (i = 0; i < size; ++i) {
      _addcarry_u32(0, -uintpLeft[2 * i], uintpRight[2 * i] + carry,
                    uintpResult + 2 * i);
      if (uintpRight[2 * i] + carry >= uintpLeft[2 * i]) {
        carry = 0;
      } else {
        carry = -1;
      }
      // borrow = right.data.bits[i] - left.data.bits[i] - borrow;
      // pResult->data.bits[i] = borrow & bit_mask;
      // borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    if (carry != 0) {
      // printf("original: %u; carry: %u\n", uintpResult[2 * i], carry);
      uintpResult[2 * i - 2] -= carry;
    }
    // assert(borrow == 0);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    memset(pResult->data.bits + size, 0, pResult->data.capacity - size);
    assert(i != 0 || pResult->data.bits[0] > 0);
  }
  return;
}

// deprecated
/*
void big_integer_add_inplace_inline_unfold_1x(const BigInteger left,
                                              const BigInteger right,
                                              BigInteger *pResult) {
  int size = MAX(left.data.size, right.data.size);
  if (pResult->data.capacity <= size + 1) {
    printf("resize pResult\n");
    unsigned long *bits = (unsigned long *)malloc((size + 1) * UINT_NUM_BYTES);
    free(pResult->data.bits);
    pResult->data.bits = bits;
    pResult->data.capacity = size + 1;
  }
  if (left.sign == 0) {
    pResult->sign = right.sign;
    pResult->data.size = right.data.size;
    memcpy(pResult->data.bits, right.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }
  if (right.sign == 0) {
    pResult->sign = left.sign;
    pResult->data.size = left.data.size;
    memcpy(pResult->data.bits, left.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  if (left.sign == right.sign) {
    pResult->sign = left.sign;
    int i;
    for (i = 0; i < size - 3; i += 4) {
      // if (i < left.data.size) {
      //   sum += left.data.bits[i];
      // }
      // if (i < right.data.size) {
      //   sum += right.data.bits[i];
      // }
      // assume left and right have same size
      // sum0 += left.data.bits[i] + right.data.bits[i];
      // sum1 += left.data.bits[i+1] + right.data.bits[i+1];
      // sum2 += left.data.bits[i+2] + right.data.bits[i+2];
      // sum3 += left.data.bits[i+3] + right.data.bits[i+3];
      // pResult->data.bits[i] = sum0 & bit_mask;
      // pResult->data.bits[i+1] = sum1 & bit_mask;
      // pResult->data.bits[i+2] = sum2 & bit_mask;
      // pResult->data.bits[i+3] = sum3 & bit_mask;
      // sum0 >>= UINT_NUM_BITS;
      // sum1 >>= UINT_NUM_BITS;
      // sum2 >>= UINT_NUM_BITS;
      // sum3 >>= UINT_NUM_BITS;
      pResult->data.bits[i] = left.data.bits[i] + right.data.bits[i];
      pResult->data.bits[i + 1] =
          left.data.bits[i + 1] + right.data.bits[i + 1];
      pResult->data.bits[i + 2] =
          left.data.bits[i + 2] + right.data.bits[i + 2];
      pResult->data.bits[i + 3] =
          left.data.bits[i + 3] + right.data.bits[i + 3];
    }
    for (; i < size; ++i) {
      pResult->data.bits[i] = left.data.bits[i] + right.data.bits[i];
    }
    // if (sum > 0) {
    //   assert(sum <= bit_mask);
    //   pResult->data.bits[i] = sum;
    //   i++;
    // }
    // printf("--------%lu------\n", pResult->data.bits[3]);
    pResult->data.size = i;
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  // int compRes = big_integer_compare_data(&left.data, &right.data);
  int compRes = -2;
  if (left.data.size < right.data.size) {
    compRes = 1;
  } else if (left.data.size > right.data.size) {
    compRes = -1;
  } else {
    int i;
    for (i = size - 1; i >= 0; --i) {
      if (left.data.bits[i] > right.data.bits[i]) {
        compRes = 1;
        break;
      }
      if (left.data.bits[i] < right.data.bits[i]) {
        compRes = -1;
        break;
      }
    }
    if (compRes == -2) {
      compRes = 0;
    }
  }

  // if (compRes == 0) {
  //   big_integer_set(0, pResult);
  // } else if (compRes > 0) {
  //   // left > right
  //   pResult->sign = left.sign;
  //   big_integer_subtract_data_inplace(left.data, right.data, &pResult->data);
  // } else {
  //   pResult->sign = right.sign;
  //   big_integer_subtract_data_inplace(right.data, left.data, &pResult->data);
  // }
  if (compRes == 0) {
    pResult->sign = 0;
    pResult->data.size = 0;
    memset(pResult->data.bits, 0, pResult->data.capacity * UINT_NUM_BYTES);
  } else if (compRes > 0) {
    pResult->sign = left.sign;
    unsigned long borrow = 0;
    int i;
    for (i = 0; i < size; ++i) {
      borrow = left.data.bits[i] - right.data.bits[i] - borrow;
      pResult->data.bits[i] = borrow & bit_mask;
      borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    assert(borrow == 0);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
  } else {
    pResult->sign = right.sign;
    unsigned long borrow = 0;
    int i;
    for (i = 0; i < size; ++i) {
      borrow = right.data.bits[i] - left.data.bits[i] - borrow;
      pResult->data.bits[i] = borrow & bit_mask;
      borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    assert(borrow == 0);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
  }
  return;
}

// deprecated
void big_integer_add_inplace_inline_unfold_1x_intrinsics(const BigInteger left,
                                                         const BigInteger right,
                                                         BigInteger *pResult) {
  int size = MAX(left.data.size, right.data.size);
  if (pResult->data.capacity <= size + 1) {
    printf("resize pResult\n");
    unsigned long *bits = (unsigned long *)malloc((size + 1) * UINT_NUM_BYTES);
    free(pResult->data.bits);
    pResult->data.bits = bits;
    pResult->data.capacity = size + 1;
  }
  if (left.sign == 0) {
    pResult->sign = right.sign;
    pResult->data.size = right.data.size;
    memcpy(pResult->data.bits, right.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }
  if (right.sign == 0) {
    pResult->sign = left.sign;
    pResult->data.size = left.data.size;
    memcpy(pResult->data.bits, left.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  if (left.sign == right.sign) {
    pResult->sign = left.sign;
    int i;
    __m256i op1, op2, sum;
    for (i = 0; i < size - 3; i += 4) {
      // if (i < left.data.size) {
      //   sum += left.data.bits[i];
      // }
      // if (i < right.data.size) {
      //   sum += right.data.bits[i];
      // }
      // assume left and right have same size
      // sum0 += left.data.bits[i] + right.data.bits[i];
      // sum1 += left.data.bits[i+1] + right.data.bits[i+1];
      // sum2 += left.data.bits[i+2] + right.data.bits[i+2];
      // sum3 += left.data.bits[i+3] + right.data.bits[i+3];
      // pResult->data.bits[i] = sum0 & bit_mask;
      // pResult->data.bits[i+1] = sum1 & bit_mask;
      // pResult->data.bits[i+2] = sum2 & bit_mask;
      // pResult->data.bits[i+3] = sum3 & bit_mask;
      // sum0 >>= UINT_NUM_BITS;
      // sum1 >>= UINT_NUM_BITS;
      // sum2 >>= UINT_NUM_BITS;
      // sum3 >>= UINT_NUM_BITS;
      // pResult->data.bits[i] = left.data.bits[i] + right.data.bits[i];
      // pResult->data.bits[i + 1] =
      //     left.data.bits[i + 1] + right.data.bits[i + 1];
      // pResult->data.bits[i + 2] =
      //     left.data.bits[i + 2] + right.data.bits[i + 2];
      // pResult->data.bits[i + 3] =
      //     left.data.bits[i + 3] + right.data.bits[i + 3];
      op1 = _mm256_loadu_si256((__m256i *)(left.data.bits + i));
      op2 = _mm256_loadu_si256((__m256i *)(right.data.bits + i));
      sum = _mm256_add_epi64(op1, op2);
      _mm256_storeu_si256((__m256i *)(pResult->data.bits + i), sum);
    }
    for (; i < size; ++i) {
      pResult->data.bits[i] = left.data.bits[i] + right.data.bits[i];
    }
    // if (sum > 0) {
    //   assert(sum <= bit_mask);
    //   pResult->data.bits[i] = sum;
    //   i++;
    // }
    // printf("--------%lu------\n", pResult->data.bits[3]);
    pResult->data.size = i;
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  // int compRes = big_integer_compare_data(&left.data, &right.data);
  int compRes = -2;
  if (left.data.size < right.data.size) {
    compRes = 1;
  } else if (left.data.size > right.data.size) {
    compRes = -1;
  } else {
    int i;
    for (i = size - 1; i >= 0; --i) {
      if (left.data.bits[i] > right.data.bits[i]) {
        compRes = 1;
        break;
      }
      if (left.data.bits[i] < right.data.bits[i]) {
        compRes = -1;
        break;
      }
    }
    if (compRes == -2) {
      compRes = 0;
    }
  }

  // if (compRes == 0) {
  //   big_integer_set(0, pResult);
  // } else if (compRes > 0) {
  //   // left > right
  //   pResult->sign = left.sign;
  //   big_integer_subtract_data_inplace(left.data, right.data, &pResult->data);
  // } else {
  //   pResult->sign = right.sign;
  //   big_integer_subtract_data_inplace(right.data, left.data, &pResult->data);
  // }
  if (compRes == 0) {
    pResult->sign = 0;
    pResult->data.size = 0;
    memset(pResult->data.bits, 0, pResult->data.capacity * UINT_NUM_BYTES);
  } else if (compRes > 0) {
    pResult->sign = left.sign;
    unsigned long borrow = 0;
    int i;
    for (i = 0; i < size; ++i) {
      borrow = left.data.bits[i] - right.data.bits[i] - borrow;
      pResult->data.bits[i] = borrow & bit_mask;
      borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    assert(borrow == 0);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
  } else {
    pResult->sign = right.sign;
    unsigned long borrow = 0;
    int i;
    for (i = 0; i < size; ++i) {
      borrow = right.data.bits[i] - left.data.bits[i] - borrow;
      pResult->data.bits[i] = borrow & bit_mask;
      borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    assert(borrow == 0);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
  }
  return;
}

void big_integer_add_inplace_inline_unfold_2x_intrinsics(const BigInteger left,
                                                         const BigInteger right,
                                                         BigInteger *pResult) {
  int size = MAX(left.data.size, right.data.size);
  if (pResult->data.capacity <= size + 1) {
    printf("resize pResult\n");
    unsigned long *bits = (unsigned long *)malloc((size + 1) * UINT_NUM_BYTES);
    free(pResult->data.bits);
    pResult->data.bits = bits;
    pResult->data.capacity = size + 1;
  }
  if (left.sign == 0) {
    pResult->sign = right.sign;
    pResult->data.size = right.data.size;
    memcpy(pResult->data.bits, right.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }
  if (right.sign == 0) {
    pResult->sign = left.sign;
    pResult->data.size = left.data.size;
    memcpy(pResult->data.bits, left.data.bits,
           pResult->data.size * UINT_NUM_BYTES);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  if (left.sign == right.sign) {
    pResult->sign = left.sign;
    int i;
    __m256i op11, op12, sum1;
    __m256i op21, op22, sum2;
    for (i = 0; i < size - 7; i += 8) {
      // if (i < left.data.size) {
      //   sum += left.data.bits[i];
      // }
      // if (i < right.data.size) {
      //   sum += right.data.bits[i];
      // }
      // assume left and right have same size
      // sum0 += left.data.bits[i] + right.data.bits[i];
      // sum1 += left.data.bits[i+1] + right.data.bits[i+1];
      // sum2 += left.data.bits[i+2] + right.data.bits[i+2];
      // sum3 += left.data.bits[i+3] + right.data.bits[i+3];
      // pResult->data.bits[i] = sum0 & bit_mask;
      // pResult->data.bits[i+1] = sum1 & bit_mask;
      // pResult->data.bits[i+2] = sum2 & bit_mask;
      // pResult->data.bits[i+3] = sum3 & bit_mask;
      // sum0 >>= UINT_NUM_BITS;
      // sum1 >>= UINT_NUM_BITS;
      // sum2 >>= UINT_NUM_BITS;
      // sum3 >>= UINT_NUM_BITS;
      // pResult->data.bits[i] = left.data.bits[i] + right.data.bits[i];
      // pResult->data.bits[i + 1] =
      //     left.data.bits[i + 1] + right.data.bits[i + 1];
      // pResult->data.bits[i + 2] =
      //     left.data.bits[i + 2] + right.data.bits[i + 2];
      // pResult->data.bits[i + 3] =
      //     left.data.bits[i + 3] + right.data.bits[i + 3];
      op11 = _mm256_loadu_si256((__m256i *)(left.data.bits + i));
      op12 = _mm256_loadu_si256((__m256i *)(right.data.bits + i));
      sum1 = _mm256_add_epi64(op11, op12);
      op21 = _mm256_loadu_si256((__m256i *)(left.data.bits + i + 4));
      op22 = _mm256_loadu_si256((__m256i *)(right.data.bits + i + 4));
      sum2 = _mm256_add_epi64(op21, op22);
      _mm256_storeu_si256((__m256i *)(pResult->data.bits + i), sum1);
      _mm256_storeu_si256((__m256i *)(pResult->data.bits + i + 4), sum2);
    }
    for (; i < size; ++i) {
      pResult->data.bits[i] = left.data.bits[i] + right.data.bits[i];
    }
    // if (sum > 0) {
    //   assert(sum <= bit_mask);
    //   pResult->data.bits[i] = sum;
    //   i++;
    // }
    // printf("--------%lu------\n", pResult->data.bits[3]);
    pResult->data.size = i;
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
    return;
  }

  // int compRes = big_integer_compare_data(&left.data, &right.data);
  int compRes = -2;
  if (left.data.size < right.data.size) {
    compRes = 1;
  } else if (left.data.size > right.data.size) {
    compRes = -1;
  } else {
    int i;
    for (i = size - 1; i >= 0; --i) {
      if (left.data.bits[i] > right.data.bits[i]) {
        compRes = 1;
        break;
      }
      if (left.data.bits[i] < right.data.bits[i]) {
        compRes = -1;
        break;
      }
    }
    if (compRes == -2) {
      compRes = 0;
    }
  }

  // if (compRes == 0) {
  //   big_integer_set(0, pResult);
  // } else if (compRes > 0) {
  //   // left > right
  //   pResult->sign = left.sign;
  //   big_integer_subtract_data_inplace(left.data, right.data, &pResult->data);
  // } else {
  //   pResult->sign = right.sign;
  //   big_integer_subtract_data_inplace(right.data, left.data, &pResult->data);
  // }
  if (compRes == 0) {
    pResult->sign = 0;
    pResult->data.size = 0;
    memset(pResult->data.bits, 0, pResult->data.capacity * UINT_NUM_BYTES);
  } else if (compRes > 0) {
    pResult->sign = left.sign;
    unsigned long borrow = 0;
    int i;
    for (i = 0; i < size; ++i) {
      borrow = left.data.bits[i] - right.data.bits[i] - borrow;
      pResult->data.bits[i] = borrow & bit_mask;
      borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    assert(borrow == 0);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
  } else {
    pResult->sign = right.sign;
    unsigned long borrow = 0;
    int i;
    for (i = 0; i < size; ++i) {
      borrow = right.data.bits[i] - left.data.bits[i] - borrow;
      pResult->data.bits[i] = borrow & bit_mask;
      borrow = (borrow >> UINT_NUM_BITS) & 1;
    }
    assert(borrow == 0);
    for (; i >= 0; --i) {
      if (pResult->data.bits[i] > 0) {
        pResult->data.size = i + 1;
        break;
      }
    }
    assert(i != 0 || pResult->data.bits[0] > 0);
    memset(pResult->data.bits + pResult->data.size, 0,
           pResult->data.capacity - pResult->data.size);
  }
  return;
}
*/
