/*
** big_integer.c
** Code largely based on this_ repo:
*<https://github.com/andreazevedo/biginteger>
** Modifications: support for arbitrary large numbers, bigint multiplication
**/

#include "big_integer.h"
#include <assert.h>
#include <immintrin.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

const int UINT_NUM_BYTES = (sizeof(unsigned long));
const int UINT_NUM_BITS = 32;
// const unsigned long bit_mask = (1lu << UINT_NUM_BITS) - 1;
// const unsigned long bit_mask = UINT_MAX;
const unsigned long bit_mask = 4294967295ul;

/* PRIVATE FUNCTIONS DECLARATIONS */

/* ---------- Data Creation & Memory Management ---------- */
/* create a big_integer_data with size, capacity 0 & bits NULL */
BigIntegerData big_integer_empty_data();
/* create a BigIntegerData with size `size`, capacity 2*`size`, & bits copied */
BigIntegerData big_integer_create_data(const unsigned int bits[],
                                       const int size);
/* set a big_integer with with size `size`, capacity 2*`size`, & bits copied */
void big_integer_set_data(const unsigned int bits[], const int size,
                          BigIntegerData *pBigIntData);
/* create a BigInteger with sign `sign` & data `data` */
BigInteger big_integer_create_internal(const int sign,
                                       const BigIntegerData data);
/* free the allocated bits array and set size/capacity to 0 */
void big_integer_destroy_data(BigIntegerData *pBigIntData);
/* allocate a new bits array with `new_capacity` & copy the old data into it */
void big_integer_resize_data(BigIntegerData *pBigIntData,
                             const int new_capacity);
void big_integer_deepcopy_inplace(const BigInteger other, BigInteger *this_);
BigIntegerData big_integer_deepcopy_data(const BigIntegerData other);
void big_integer_deepcopy_data_inplace(const BigIntegerData other,
                                       BigIntegerData *this_);
/* set the size according to the largest index i s.t. bits[i] is not zero */
void big_integer_normalize(BigIntegerData *pBigIntData);
void big_integer_normalize_from(BigIntegerData *pBigIntData, const int from);
/* set the memory from bits[size] to bits[capacity-1] to  0 */
void big_integer_clear_trash_data(BigIntegerData *pBigIntData);

/* ---------- Comparision ---------- */
/* compare a unsigned bigint with an uint */
int big_integer_compare_data_uint(const BigIntegerData *pBigIntData,
                                  unsigned int value);

/* ----------Arithmetic ---------- */
/* add/subtract two unsigned bigints */
BigIntegerData big_integer_add_data(const BigIntegerData left,
                                    const BigIntegerData right);
void big_integer_add_data_inplace(const BigIntegerData left,
                                  const BigIntegerData right,
                                  BigIntegerData *pResult);
void big_integer_add_inplace_fixed_precision_data(BigIntegerData *,
                                                  const BigIntegerData,
                                                  const int, int *);
/* subtraction assumes that left > right always holds */
BigIntegerData big_integer_subtract_data(const BigIntegerData left,
                                         const BigIntegerData right);
void big_integer_subtract_inplace_fixed_precision_data(
    const BigIntegerData left, const BigIntegerData right,
    BigIntegerData *result, const int precision, int *carried);
void big_integer_subtract_data_inplace(const BigIntegerData left,
                                       const BigIntegerData right,
                                       BigIntegerData *pResult);
/* add/subtract a bigint with a uint value */
void big_integer_increment_data(BigIntegerData *pBigIntData,
                                const unsigned int value);
void big_integer_decrement_data(BigIntegerData *pBigIntData,
                                const unsigned int value);

/* multiply an unsigned bigint with a uint inplace. left can be the same as
 * result. not validated yet */
void big_integer_multiply_data_with_uint(const BigIntegerData left,
                                         unsigned int right,
                                         BigIntegerData *pResult);
/* multiply two unsigned bigints */
BigIntegerData big_integer_multiply_data(const BigIntegerData left,
                                         const BigIntegerData right);
void big_integer_multiply_data_inplace(const BigIntegerData left,
                                       const BigIntegerData right,
                                       BigIntegerData *pResult);
/* left = left * right */
void big_integer_multiply_data_two_operands_opt(BigIntegerData *pLeft,
                                                const BigIntegerData right);
/* multiply two unsigned bigints using karatsuba algorithm */
BigIntegerData big_integer_multiply_data_karatsuba(const BigIntegerData left,
                                                   const BigIntegerData right);
void big_integer_multiply_data_inplace_karatsuba(const BigIntegerData left,
                                                 const BigIntegerData right,
                                                 BigIntegerData *pResult);
void big_integer_split_data(const BigIntegerData source, const int mid,
                            BigIntegerData *pHigh, BigIntegerData *pLow);
/* shift an unsigned bigint to the left by d*UINT_NUM_BITS */
void big_integer_left_shift_data(BigIntegerData *pBigIntData, int d);

/* PRIVATE FUNCTIONS IMPLEMENTATION */
BigIntegerData big_integer_empty_data() {
  BigIntegerData bigIntData;
  bigIntData.size = 0;
  bigIntData.capacity = 0;
  bigIntData.bits = NULL;
  big_integer_clear_trash_data(&bigIntData);
  return bigIntData;
}

BigIntegerData big_integer_create_data(const unsigned int bits[],
                                       const int size) {
  assert(size > 0);

  BigIntegerData bigIntData;
  // allocate memory
  bigIntData.size = size;
  // by default we allocate memory twice as needed
  // TODO: this_ is a designed choice to be discussed
  bigIntData.capacity = 2 * size;
  bigIntData.bits =
      (unsigned long *)malloc(bigIntData.capacity * UINT_NUM_BYTES);
  if (bits && size > 0)
    memcpy(bigIntData.bits, bits, UINT_NUM_BYTES * size);

  big_integer_clear_trash_data(&bigIntData);

  return bigIntData;
}

void big_integer_set_data(const unsigned int bits[], const int size,
                          BigIntegerData *pBigIntData) {
  assert(size > 0);

  if (pBigIntData->capacity <= size) {
    pBigIntData->size = 0;
    big_integer_resize_data(pBigIntData, size * 2);
  }
  // allocate memory
  pBigIntData->size = size;
  // by default we allocate memory twice as needed
  // TODO: this_ is a designed choice to be discussed
  if (bits && size > 0)
    memcpy(pBigIntData->bits, bits, size * UINT_NUM_BYTES);

  big_integer_clear_trash_data(pBigIntData);

  return;
}

void big_integer_print_data(const BigIntegerData bigIntData, const char *msg) {
  printf("%s\n", msg);
  printf("%4d%4d\t", bigIntData.capacity, bigIntData.size);
  int i;
  for (i = 0; i < bigIntData.size; ++i) {
    printf("%14lu\t", bigIntData.bits[i]);
  }
  printf("\n");
}

BigInteger big_integer_create_internal(const int sign,
                                       const BigIntegerData data) {
  BigInteger bigInt;
  bigInt.sign = sign;
  bigInt.data = data;

  return bigInt;
}

// TODO: maybe we can just leave sign/size/capacity untouched?
// TODO: should we design an INVALID_SIGN symbol?
void big_integer_destroy(BigInteger *pBigInt) {
  pBigInt->sign = 0;
  pBigInt->data.capacity = 0;
  pBigInt->data.size = 0;
  free(pBigInt->data.bits);
}

// TODO: maybe we can just leave sign/size/capacity untouched?
void big_integer_destroy_data(BigIntegerData *pBigIntData) {
  pBigIntData->capacity = 0;
  pBigIntData->size = 0;
  free(pBigIntData->bits);
}

void big_integer_resize_data(BigIntegerData *pBigIntData,
                             const int new_capacity) {
  pBigIntData->capacity = new_capacity;
  unsigned long *bits =
      (unsigned long *)malloc(pBigIntData->capacity * UINT_NUM_BYTES);
  memcpy(bits, pBigIntData->bits, pBigIntData->size * UINT_NUM_BYTES);
  free(pBigIntData->bits);
  pBigIntData->bits = bits;
  big_integer_clear_trash_data(pBigIntData);
  return;
}

BigInteger big_integer_deepcopy(const BigInteger other) {
  BigInteger this_;
  this_.sign = other.sign;
  this_.data.capacity = other.data.capacity;
  this_.data.size = other.data.size;
  this_.data.bits =
      (unsigned long *)malloc(this_.data.capacity * UINT_NUM_BYTES);
  memcpy(this_.data.bits, other.data.bits,
         this_.data.capacity * UINT_NUM_BYTES);
  return this_;
}

void big_integer_deepcopy_inplace(const BigInteger other, BigInteger *this_) {
  this_->sign = other.sign;
  if (this_->data.capacity < other.data.size) {
    assert(other.data.capacity > other.data.size);
    big_integer_resize_data(&this_->data, other.data.capacity);
  }
  this_->data.size = other.data.size;
  memcpy(this_->data.bits, other.data.bits, this_->data.size * UINT_NUM_BYTES);
  big_integer_clear_trash_data(&this_->data);
  return;
}

BigIntegerData big_integer_deepcopy_data(const BigIntegerData other) {
  BigIntegerData this_;
  this_.capacity = other.capacity;
  this_.size = other.size;
  this_.bits = (unsigned long *)malloc(this_.capacity * UINT_NUM_BYTES);
  memcpy(this_.bits, other.bits, this_.capacity * UINT_NUM_BYTES);
  return this_;
}

void big_integer_deepcopy_data_inplace(const BigIntegerData other,
                                       BigIntegerData *this_) {
  if (this_->capacity < other.size) {
    assert(other.capacity > other.size);
    big_integer_resize_data(this_, other.capacity);
  }
  this_->size = other.size;
  memcpy(this_->bits, other.bits, this_->size * UINT_NUM_BYTES);
  big_integer_clear_trash_data(this_);
  return;
}

void big_integer_normalize(BigIntegerData *pBigIntData) {
  big_integer_normalize_from(pBigIntData, pBigIntData->capacity - 1);
}

void big_integer_normalize_from(BigIntegerData *pBigIntData, const int from) {
  int i;
  for (i = from; i >= 0; --i) {
    if (pBigIntData->bits[i] != 0) {
      pBigIntData->size = i + 1;
      break;
    }
  }
  if (i == -1) {
    pBigIntData->size = 1;
  }
}

void big_integer_clear_trash_data(BigIntegerData *pBigIntData) {
  if (pBigIntData == NULL)
    return;

  int i;
  for (i = pBigIntData->size; i < pBigIntData->capacity; ++i)
    pBigIntData->bits[i] = 0;

  // TODO: is memset more efficient?
  // memset(&pBigIntData->bits[pBigIntData->size], 0,
  // (pBigIntData->capacity-pBigIntData->size)*UINT_NUM_BYTES);
}

// void big_integer_report_overflow()
// {
//    fprintf(stderr, "BigInteger reported overflow!\n");
// }

int big_integer_compare_data(const BigIntegerData *pLeft,
                             const BigIntegerData *pRight) {
  /* if the sizes are different */
  if (pLeft->size > pRight->size)
    return 1;
  if (pLeft->size < pRight->size)
    return -1;

  int size = pLeft->size;
  int i;
  for (i = (size - 1); i >= 0; --i) {
    if (pLeft->bits[i] > pRight->bits[i])
      return 1;
    if (pLeft->bits[i] < pRight->bits[i])
      return -1;
  }

  return 0;
}

int big_integer_compare_data_uint(const BigIntegerData *pBigIntData,
                                  unsigned int value) {
  if (pBigIntData->size == 0)
    return -1;
  if (pBigIntData->size > 1)
    return 1;

  if (pBigIntData->bits[0] > value)
    return 1;
  else if (pBigIntData->bits[0] < value)
    return -1;

  return 0;
}

BigIntegerData big_integer_add_data(const BigIntegerData left,
                                    const BigIntegerData right) {
  BigIntegerData result = big_integer_empty_data();

  int size = MAX(left.size, right.size);
  int capacity = MAX(left.capacity, right.capacity);

  result.bits = (unsigned long *)malloc(capacity * UINT_NUM_BYTES);
  result.capacity = capacity;

  unsigned long sum = 0;
  int i;
  for (i = 0; i < size; ++i) {
    sum += left.bits[i] + right.bits[i];
    result.bits[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum > 0) {
    result.bits[i] = sum & bit_mask;
    i++;
  }

  result.size = i;
  big_integer_clear_trash_data(&result);

  assert(result.size <= result.capacity);
  // if (result.size == result.capacity) {
  //   big_integer_resize_data(&result, result.capacity * 2);
  // }

  return result;
}

// left or right can be the same data as *pResult
void big_integer_add_data_inplace(const BigIntegerData left,
                                  const BigIntegerData right,
                                  BigIntegerData *pResult) {
  assert(&left != pResult);
  assert(&right != pResult);
  // assume pResult's bits is allocated and the capacity is properly set
  // the contents in bits & size won't matter as they will be written again.
  // big_integer_print_data(left, "left: ");
  // big_integer_print_data(right, "right: ");

  int size = MAX(left.size, right.size);
  int capacity = MAX(left.capacity, right.capacity);

  if (pResult->capacity < size + 1) {
    // TODO: this_ is a design choice: make the result as large as its operands.
    // make sure overflow won't happen
    // if (capacity < size) {
    //     printf("%u %u\n", left.bits[0], right.bits[0]);
    //     printf("%d %d; %d %d\n", left.size, left.capacity, right.size,
    //     right.capacity);
    // }
    assert(capacity >= size);
    big_integer_resize_data(pResult, capacity);
  }

  // big_integer_print_data(left, "left: ");
  // big_integer_print_data(right, "right: ");

  unsigned long sum = 0;
  int i;
  for (i = 0; i < size; ++i) {
    if (i < left.size) {
      sum += left.bits[i];
    }
    if (i < right.size) {
      sum += right.bits[i];
    }
    // printf("left: %u, right: %u, sum: %llu\n", left.bits[i], right.bits[i],
    // sum); sum += (unsigned long long)left.bits[i] + right.bits[i];
    pResult->bits[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum > 0) {
    pResult->bits[i] = sum & bit_mask;
    i++;
  }

  pResult->size = i;
  big_integer_clear_trash_data(pResult);

  assert(pResult->size <= pResult->capacity);
  if (pResult->size == pResult->capacity) {
    // hope this_ won't happen much in a addition
    // big_integer_resize_data(pResult, pResult->capacity * 2);
  }
  // big_integer_print_data(*pResult, "*pResult in add: ");

  return;
}

/* left > right always */
BigIntegerData big_integer_subtract_data(const BigIntegerData left,
                                         const BigIntegerData right) {
  BigIntegerData result = big_integer_empty_data();

  int size = MAX(left.size, right.size);
  int capacity = MAX(left.capacity, right.capacity);

  result.bits = (unsigned long *)calloc(capacity, UINT_NUM_BYTES);
  result.capacity = capacity;

  unsigned long borrow = 0;
  int i;
  for (i = 0; i < size; ++i) {
    /* what happens here is that, if left is less than right, borrow will become
       "negative" (not really because it is unsigned), and the bit pattern for
       that is the 1's complement (complementing it to get to 0), which is
       exactly the remainder of this_ term in the subtraction. */
    borrow = left.bits[i] - right.bits[i] - borrow;

    result.bits[i] = borrow & bit_mask;

    /* here we just want the first 1 after removing the lower order term */
    borrow = (borrow >> UINT_NUM_BITS) & 1;
  }

  big_integer_normalize_from(&result, i);

  return result;
}

void big_integer_subtract_data_inplace(const BigIntegerData left,
                                       const BigIntegerData right,
                                       BigIntegerData *pResult) {
  assert(&left != pResult);
  assert(&right != pResult);
  int size = MAX(left.size, right.size);
  int capacity = MAX(left.capacity, right.capacity);

  if (pResult->capacity < size + 1) {
    // TODO: this_ is a design choice: make the result as large as its operands.
    // make sure overflow won't happen
    assert(capacity > size);
    big_integer_resize_data(pResult, capacity);
  }

  unsigned long borrow = 0;
  int i;
  for (i = 0; i < size; ++i) {
    /* what happens here is that, if left is less than right, borrow will become
       "negative" (not really because it is unsigned), and the bit pattern for
       that is the 1's complement (complementing it to get to 0), which is
       exactly the remainder of this_ term in the subtraction. */
    borrow = left.bits[i] - right.bits[i] - borrow;

    pResult->bits[i] = borrow & bit_mask;

    /* here we just want the first 1 after removing the lower order term */
    borrow = (borrow >> UINT_NUM_BITS) & 1;
  }

  // assume bits[size] to bits[capacity-1] is clean (set to 0)
  memset(pResult->bits + size, 0,
         (pResult->capacity - pResult->size) * UINT_NUM_BYTES);
  big_integer_normalize_from(pResult, i);

  return;
}

void big_integer_increment_data(BigIntegerData *pBigIntData,
                                const unsigned int value) {
  unsigned long carry = value;
  int i = 0;
  while (carry > 0) {
    carry += pBigIntData->bits[i];
    pBigIntData->bits[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    ++i;
  }

  if (i > pBigIntData->size)
    pBigIntData->size = i;

  assert(pBigIntData->size <= pBigIntData->capacity);
  if (pBigIntData->size == pBigIntData->capacity) {
    // big_integer_resize_data(pBigIntData, pBigIntData->capacity * 2);
  }
}

/* pBigIntData > value */
void big_integer_decrement_data(BigIntegerData *pBigIntData,
                                const unsigned int value) {
  unsigned long borrow = value;
  int i = 0;
  while (borrow > 0) {
    borrow = pBigIntData->bits[i] - borrow;
    pBigIntData->bits[i] = borrow & bit_mask;
    borrow = (borrow >> UINT_NUM_BITS) & 1;
    ++i;
  }

  big_integer_normalize_from(pBigIntData, i);
  assert(pBigIntData->size <= pBigIntData->capacity);
  if (pBigIntData->size == pBigIntData->capacity) {
    // big_integer_resize_data(pBigIntData, pBigIntData->capacity * 2);
  }
}

void big_integer_multiply_data_with_uint(const BigIntegerData left,
                                         unsigned int right,
                                         BigIntegerData *pResult) {
  // use ulong to store the value to avoid implicit conversion
  unsigned long ulright = right, product = 0;
  int i;

  // make sure pResult have enough space to store the result
  if (pResult->capacity < left.size + 1) {
    // avoid copy
    pResult->size = 0;
    big_integer_resize_data(pResult, left.size * 2);
  }
  for (i = 0; i < left.size; ++i) {
    product += ulright * left.bits[i];
    // TODO: not sure if this_ is correct and the most efficient way to extract
    // the lower 32 bits pResult->bits[i] = product & 0x00000000ffffffff;
    pResult->bits[i] = product & bit_mask;
    // get upper 32 bits
    product >>= UINT_NUM_BITS;
  }

  // i is just left.size
  if (product > 0) {
    pResult->bits[i] = product & bit_mask;
    pResult->size = i + 1;
  } else {
    pResult->size = i;
  }

  // TODO: skip this_ as we won't visit that area?
  big_integer_clear_trash_data(pResult);

  return;
}

BigIntegerData big_integer_multiply_data(const BigIntegerData left,
                                         const BigIntegerData right) {
  BigIntegerData left_copy = big_integer_deepcopy_data(left);
  int capacity = MAX(left.capacity, right.capacity);
  BigIntegerData result, tmpResult, mulResult;
  // calloc is fater than malloc+memset
  // good for initialization
  result.bits = (unsigned long *)calloc(capacity * 2, UINT_NUM_BYTES);
  result.capacity = capacity * 2;
  result.size = 0;

  tmpResult.bits = (unsigned long *)calloc(capacity * 2, UINT_NUM_BYTES);
  tmpResult.capacity = capacity * 2;
  tmpResult.size = 1;

  mulResult.bits = (unsigned long *)calloc(capacity * 2, UINT_NUM_BYTES);
  mulResult.capacity = capacity * 2;
  mulResult.size = 0;

  int i;
  for (i = 0; i < right.size; i++) {
    big_integer_multiply_data_with_uint(left_copy, right.bits[i], &mulResult);
    // for (j = 1; j <= right.bits[i]; j++) {
    //   // this_ is memory leaking!
    //   // result=big_integer_add_data(result,left);
    //   big_integer_add_data_inplace(result, left_copy, &result);
    // }
    big_integer_add_data_inplace(mulResult, tmpResult, &result);
    big_integer_deepcopy_data_inplace(result, &tmpResult);
    big_integer_left_shift_data(&left_copy, 1);
  }
  big_integer_destroy_data(&left_copy);
  big_integer_destroy_data(&tmpResult);
  big_integer_destroy_data(&mulResult);
  return result;
}

BigIntegerData big_integer_multiply_data_opt(const BigIntegerData left,
                                             const BigIntegerData right) {
  int capacity = MAX(left.capacity, right.capacity);
  BigIntegerData result;
  // calloc is fater than malloc+memset
  // good for initialization
  result.bits = (unsigned long *)calloc(capacity * 2, UINT_NUM_BYTES);
  result.capacity = capacity * 2;
  result.size = 0;

  unsigned long carry = 0;
  for (int i = 0; i < right.size; ++i) {
    // big_integer_multiply_data_with_uint(left_copy, right.bits[i],
    // &mulResult);
    carry = 0;
    for (int j = 0; j < left.size; ++j) {
      carry += left.bits[j] * right.bits[i] + result.bits[i + j];
      result.bits[i + j] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    if (carry > 0) {
      result.bits[left.size + i] = carry;
    }
    // big_integer_add_data_inplace(mulResult, tmpResult, &result);
    // big_integer_deepcopy_data_inplace(result, &tmpResult);
    // big_integer_left_shift_data(&left_copy, 1);
  }
  if (carry > 0) {
    result.size = left.size + right.size;
  } else {
    result.size = left.size + right.size - 1;
  }
  return result;
}

void big_integer_multiply_data_two_operands_opt(BigIntegerData *pLeft,
                                                const BigIntegerData right) {
  BigIntegerData left_copy = big_integer_deepcopy_data(*pLeft);
  // TODO: merge this_ resize and set to zero together
  if (pLeft->capacity <= pLeft->size + right.size) {
    big_integer_resize_data(pLeft, pLeft->capacity * 2);
  }
  // set *pLeft to zero
  for (int i = 0; i < pLeft->size; ++i) {
    pLeft->bits[i] = 0;
  }
  unsigned long carry = 0;
  for (int i = 0; i < right.size; ++i) {
    // big_integer_multiply_data_with_uint(left_copy, right.bits[i],
    // &mulResult);
    carry = 0;
    for (int j = 0; j < left_copy.size; ++j) {
      carry += left_copy.bits[j] * right.bits[i] + pLeft->bits[i + j];
      pLeft->bits[i + j] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    if (carry > 0) {
      pLeft->bits[left_copy.size + i] = carry;
    }
    // big_integer_add_data_inplace(mulResult, tmpResult, &result);
    // big_integer_deepcopy_data_inplace(result, &tmpResult);
    // big_integer_left_shift_data(&left_copy, 1);
  }
  if (carry > 0) {
    pLeft->size = left_copy.size + right.size;
  } else {
    // printf("%d %d %d\n", pLeft->size, right.size, result.size);
    pLeft->size = left_copy.size + right.size - 1;
    // printf("%d %d %d\n", pLeft->size, right.size, result.size);
  }
  big_integer_destroy_data(&left_copy);
  // printf("size: %d %d %d\n", pLeft->size, right.size, result.size);
  // big_integer_print_data(*pLeft, "pLeft: ");
  // big_integer_print_data(result, "result: ");
  // big_integer_destroy_data(pLeft);
  // printf("%p\n", pLeft);
  // printf("%p\n", ppLeft);
  // printf("%p\n", *ppLeft);
  // *ppLeft = &result;
  // printf("ppLeft size: %d\n", (*ppLeft)->size);
  // printf("%p\n", *ppLeft);
  // printf("%p\n", &result);
  // big_integer_print_data(**ppLeft, "ppLeft: ");
  return;
}

void big_integer_multiply_data_inplace(const BigIntegerData left,
                                       const BigIntegerData right,
                                       BigIntegerData *pResult) {
  BigIntegerData left_copy = big_integer_deepcopy_data(left);
  int capacity = MAX(left.capacity, right.capacity);
  int size = MAX(left.size, right.size);
  pResult->size = 0;
  // TODO: this_ is a design choice
  if (pResult->capacity < 2 * size) {
    assert(capacity > size);
    big_integer_resize_data(pResult, 2 * capacity);
  }
  big_integer_clear_trash_data(pResult);

  BigIntegerData tmpResult, mulResult;
  // tmpResult.bits = (unsigned int *)calloc(capacity * 2, UINT_NUM_BYTES);
  tmpResult.bits = (unsigned long *)calloc(capacity, UINT_NUM_BYTES);
  tmpResult.capacity = left.size + 1;
  tmpResult.size = 1;
  // mulResult.bits = (unsigned int *)calloc(capacity * 2, UINT_NUM_BYTES);
  mulResult.bits = (unsigned long *)calloc(capacity, UINT_NUM_BYTES);
  mulResult.capacity = left.size + 1;
  mulResult.size = 0;

  // big_integer_print_data(*pResult, "*pResult: ");
  int i;
  for (i = 0; i < right.size; i++) {
    big_integer_multiply_data_with_uint(left_copy, right.bits[i], &mulResult);
    // big_integer_print_data(tmpResult, "tmpResult: ");
    // for (j = 1; j <= right.bits[i]; j++) {
    //   // this_ is memory leaking!
    //   // result=big_integer_add_data(result,left);
    //   big_integer_add_data_inplace(*pResult, left_copy, pResult);
    // }
    // bug: left/right shouldn't be the same as result!
    big_integer_add_data_inplace(mulResult, tmpResult, pResult);
    big_integer_deepcopy_data_inplace(*pResult, &tmpResult);
    // big_integer_print_data(*pResult, "*pResult: ");
    big_integer_left_shift_data(&left_copy, 1);
  }
  big_integer_destroy_data(&left_copy);
  big_integer_destroy_data(&tmpResult);
  big_integer_destroy_data(&mulResult);
  return;
}

void big_integer_left_shift_data(BigIntegerData *pBigIntData, int d) {
  assert(d > 0);
  int i;
  if (pBigIntData->size == 1 && pBigIntData->bits[0] == 0)
    return;
  // TODO: understand why valgrind prompts bug here
  // bug: we need to make sure capacity > size always holds
  // printf("left_copy size: %d, capacity: %d\n", pBigIntData->size,
  // pBigIntData->capacity);
  if (pBigIntData->size + d >= pBigIntData->capacity)
    big_integer_resize_data(pBigIntData, 2 * (pBigIntData->size + d));
  for (i = pBigIntData->size; i >= 0; i--) {
    pBigIntData->bits[i + d] = pBigIntData->bits[i];
  }
  for (i = 0; i < d; i++) {
    pBigIntData->bits[i] = 0;
  }
  // TODO: use memset here?
  // memset(&pBigIntData->bits[0], 0, d*UINT_NUM_BYTES);
  pBigIntData->size += d;
  big_integer_clear_trash_data(pBigIntData);
}

#define KARATSUBA_SWITCH 2
BigIntegerData big_integer_multiply_data_karatsuba(const BigIntegerData left,
                                                   const BigIntegerData right) {
  // printf("left.size: %d, right.size: %d\n", left.size, right.size);
  if (left.size < KARATSUBA_SWITCH || right.size < KARATSUBA_SWITCH) {
    return big_integer_multiply_data(left, right);
  }
  // printf("hello\n");
  int size = MAX(left.size, right.size);
  int mid = size >> 1;
  BigIntegerData leftHigh, leftLow, rightHigh, rightLow;
  // big_integer_print_data(left, "left: ");
  big_integer_split_data(left, mid, &leftHigh, &leftLow);
  // big_integer_print_data(right, "right: ");
  big_integer_split_data(right, mid, &rightHigh, &rightLow);

  // a = leftHigh * rightHigh
  // b = leftLow * rightLow
  // c = leftHigh + leftLow
  // d = rightHigh + rightLow
  // e = c * d - a - d
  BigIntegerData a, b, c, d, e;
  a = big_integer_multiply_data_karatsuba(leftHigh, rightHigh);
  b = big_integer_multiply_data_karatsuba(leftLow, rightLow);
  c = big_integer_add_data(leftHigh, leftLow);
  d = big_integer_add_data(rightHigh, rightLow);
  e = big_integer_multiply_data_karatsuba(c, d);
  big_integer_subtract_data_inplace(e, a, &e);
  big_integer_subtract_data_inplace(e, b, &e);

  // result = a * 2**(mid*2) + e * 2**mid + b
  big_integer_left_shift_data(&a, mid * 2);
  big_integer_left_shift_data(&e, mid);
  big_integer_add_data_inplace(a, e, &a);
  big_integer_add_data_inplace(a, b, &a);

  // free all the intermediate results
  big_integer_destroy_data(&leftHigh);
  big_integer_destroy_data(&leftLow);
  big_integer_destroy_data(&rightHigh);
  big_integer_destroy_data(&rightLow);
  big_integer_destroy_data(&b);
  big_integer_destroy_data(&c);
  big_integer_destroy_data(&d);
  big_integer_destroy_data(&e);

  return a;
}

void big_integer_multiply_data_inplace_karatsuba(const BigIntegerData left,
                                                 const BigIntegerData right,
                                                 BigIntegerData *pResult);

void big_integer_split_data(const BigIntegerData source, const int mid,
                            BigIntegerData *pHigh, BigIntegerData *pLow) {
  // assume pHigh and pLow are unintialized BigIntgerData
  assert(mid > 0);
  if (source.size > mid) {
    // only allocate when source is larger than UINT_MAX**mid
    pHigh->size = source.size - mid;
    pHigh->capacity = source.size - mid + 1;
    // We don't allocate more than needed
    pHigh->bits = (unsigned long *)malloc(pHigh->capacity * UINT_NUM_BYTES);
    memcpy(pHigh->bits, source.bits + mid, pHigh->size * UINT_NUM_BYTES);
    big_integer_clear_trash_data(pHigh);
    // printf("mid: %d, bits: %u, bits+mid: %u\n", mid, *(source.bits),
    // *(source.bits+mid)); big_integer_print_data(*pHigh, "pHigh: ");
  } else {
    // set pHigh to zero
    // TODO: escape this_ next time?
    pHigh->size = 1;
    pHigh->capacity = 2;
    pHigh->bits = (unsigned long *)calloc(2, UINT_NUM_BYTES);
  }
  pLow->size = mid;
  pLow->capacity = mid + 1;
  pLow->bits = (unsigned long *)malloc(pLow->capacity * UINT_NUM_BYTES);
  memcpy(pLow->bits, source.bits, mid * UINT_NUM_BYTES);
  big_integer_clear_trash_data(pLow);
  // big_integer_print_data(*pLow, "pLow: ");

  return;
}

/* PUBLIC FUNCTIONS IMPLEMENTATION */
BigInteger big_integer_create(long long value) {
  BigInteger bigInt;
  bigInt.data.bits = (unsigned long *)malloc(32);
  bigInt.data.capacity = 4;

  if (value == 0) {
    bigInt.sign = 0;
    bigInt.data.bits[0] = 0;
    bigInt.data.size = 1;
  } else {
    unsigned long long uValue;
    if (value < 0) {
      bigInt.sign = -1;
      uValue = (unsigned long long)-value;
    } else {
      bigInt.sign = 1;
      uValue = (unsigned long long)value;
    }

    bigInt.data.size = 0;
    while (uValue > 0) {
      bigInt.data.bits[bigInt.data.size++] = uValue & bit_mask;
      uValue >>= UINT_NUM_BITS;
    }
  }

  big_integer_clear_trash_data(&bigInt.data);

  return bigInt;
}

void big_integer_set(long long value, BigInteger *pBigInt) {
  // TODO: by default 128 bits?
  if (pBigInt->data.capacity < 4) {
    big_integer_resize_data(&pBigInt->data, 4);
  }

  if (value == 0) {
    pBigInt->sign = 0;
    pBigInt->data.bits[0] = 0;
    pBigInt->data.size = 1;
  } else {
    unsigned long long uValue;
    if (value < 0) {
      pBigInt->sign = -1;
      uValue = (unsigned long)-value;
    } else {
      pBigInt->sign = 1;
      uValue = (unsigned long)value;
    }

    pBigInt->data.size = 0;
    while (uValue > 0) {
      pBigInt->data.bits[pBigInt->data.size++] = uValue & bit_mask;
      uValue >>= UINT_NUM_BITS;
    }
  }

  big_integer_clear_trash_data(&pBigInt->data);

  return;
}

BigInteger big_integer_create_from_file(FILE **ppFile) {
  BigInteger bigInt;
  int sign, size;
  int ret;
  int i;
  ret = fscanf(*ppFile, "%d%d", &sign, &size);
  if (ret != 2) {
    printf("Error during reading a bigint's sign & size!");
    exit(EXIT_FAILURE);
  }
  assert(size > 0);
  bigInt.sign = sign;
  bigInt.data.size = size;
  bigInt.data.capacity = 2 * size;
  bigInt.data.bits =
      (unsigned long *)calloc(bigInt.data.capacity, UINT_NUM_BYTES);
  for (i = 0; i < size; ++i) {
    ret = fscanf(*ppFile, "%lu", &bigInt.data.bits[i]);
    assert(ret == 1);
  }
  return bigInt;
}

void big_integer_output_to_file(const BigInteger bigInt, FILE **ppFile) {
  // TODO: remove this_ check
  if (bigInt.sign == 0) {
    assert(bigInt.data.size == 1);
    assert(bigInt.data.bits[0] == 0);
  }

  fprintf(*ppFile, "%d\t%d\t", bigInt.sign, bigInt.data.size);
  int i;
  for (i = 0; i < bigInt.data.size; ++i) {
    fprintf(*ppFile, "%lu\t\t\t", bigInt.data.bits[i]);
  }
  fprintf(*ppFile, "\n");
  return;
}

void big_integer_print(const BigInteger bigInt, const char *msg) {
  // TODO: remove this_ check
  if (bigInt.sign == 0) {
    assert(bigInt.data.size == 1);
    assert(bigInt.data.bits[0] == 0);
  }

  printf("%s: sign %d; size %d; capacity %d;\n", msg, bigInt.sign,
         bigInt.data.size, bigInt.data.capacity);
  // printf(
  //     "The sign of the big int is %d\nThe size(length) of the big int is
  //     %d\n", bigInt.sign, bigInt.data.size);
  // printf("The data is: \n");
  int i;
  for (i = 0; i < bigInt.data.size; ++i) {
    printf("%10lu ", bigInt.data.bits[i]);
  }
  printf("\n");
}

int big_integer_to_int(const BigInteger bigInt) {
  if (bigInt.sign == 0)
    return 0;

  /* overflow check */
  if (bigInt.data.size > 1 ||
      (bigInt.sign == 1 && bigInt.data.bits[0] > INT_MAX) ||
      (bigInt.sign == -1 && -(bigInt.data.bits[0]) < INT_MIN)) {
    printf("sign: %d; size: %d, bits[0]: %lu\n", bigInt.sign, bigInt.data.size,
           bigInt.data.bits[0]);
    printf(
        "cannot convert this_ bigint to an int: not in the range of an int\n");
    exit(EXIT_FAILURE);
  }

  if (bigInt.sign == -1)
    return -(int)bigInt.data.bits[0];

  return (int)bigInt.data.bits[0];
}

long long big_integer_to_long_long(const BigInteger bigInt) {
  if (bigInt.sign == 0)
    return 0;

  int maxLength = sizeof(long long) / UINT_NUM_BYTES;

  if (bigInt.data.size > maxLength) {
    printf("cannot convert this_ bigint to a long int: not in the range of a "
           "long int");
    exit(EXIT_FAILURE);
  }

  unsigned long result = 0;
  int i = 0;
  for (i = 0; i < bigInt.data.size; ++i) {
    result |= (bigInt.data.bits[i]) << (UINT_NUM_BITS * i);
  }

  if (bigInt.sign == -1)
    return -(long long)result;

  return result;
}

int big_integer_compare(const BigInteger left, const BigInteger right) {
  /* if one is positive and the other negative */
  if (left.sign > right.sign)
    return 1;
  if (left.sign < right.sign)
    return -1;

  /* if they have the same sign */
  int sign = left.sign;
  return sign * big_integer_compare_data(&left.data, &right.data);
}

BigInteger big_integer_add(const BigInteger left, const BigInteger right) {
  if (left.sign == 0)
    return big_integer_deepcopy(right);
  if (right.sign == 0)
    return big_integer_deepcopy(left);

  if (left.sign == right.sign)
    return big_integer_create_internal(
        left.sign, big_integer_add_data(left.data, right.data));

  int compRes = big_integer_compare_data(&left.data, &right.data);

  if (compRes == 0)
    return big_integer_create(0);
  else if (compRes > 0) // left > right
    return big_integer_create_internal(
        left.sign, big_integer_subtract_data(left.data, right.data));
  else
    return big_integer_create_internal(
        right.sign, big_integer_subtract_data(right.data, left.data));
}

void big_integer_add_inplace(const BigInteger left, const BigInteger right,
                             BigInteger *pResult) {
  if (left.sign == 0) {
    big_integer_deepcopy_inplace(right, pResult);
    return;
  }
  if (right.sign == 0) {
    big_integer_deepcopy_inplace(left, pResult);
    return;
  }

  if (left.sign == right.sign) {
    pResult->sign = left.sign;
    big_integer_add_data_inplace(left.data, right.data, &pResult->data);
    return;
  }

  int compRes = big_integer_compare_data(&left.data, &right.data);

  if (compRes == 0) {
    big_integer_set(0, pResult);
  } else if (compRes > 0) {
    // left > right
    pResult->sign = left.sign;
    big_integer_subtract_data_inplace(left.data, right.data, &pResult->data);
  } else {
    pResult->sign = right.sign;
    big_integer_subtract_data_inplace(right.data, left.data, &pResult->data);
  }
  return;
}

BigInteger big_integer_subtract(const BigInteger left, const BigInteger right) {
  if (left.sign == 0)
    return big_integer_create_internal(-right.sign,
                                       big_integer_deepcopy_data(right.data));
  if (right.sign == 0)
    return big_integer_deepcopy(left);

  if (left.sign != right.sign)
    return big_integer_create_internal(
        left.sign, big_integer_add_data(left.data, right.data));

  int compRes = big_integer_compare_data(&left.data, &right.data);

  if (compRes == 0)
    return big_integer_create(0);
  else if (compRes > 0) // left > right
    return big_integer_create_internal(
        left.sign, big_integer_subtract_data(left.data, right.data));
  else
    return big_integer_create_internal(
        -right.sign, big_integer_subtract_data(right.data, left.data));
}

void big_integer_subtract_inplace(const BigInteger left, const BigInteger right,
                                  BigInteger *pResult) {
  if (left.sign == 0) {
    pResult->sign = -right.sign;
    big_integer_deepcopy_data_inplace(right.data, &pResult->data);
    return;
  }
  if (right.sign == 0) {
    big_integer_deepcopy_inplace(left, pResult);
    return;
  }

  if (left.sign != right.sign) {
    pResult->sign = left.sign;
    big_integer_add_data_inplace(left.data, right.data, &pResult->data);
    return;
  }

  int compRes = big_integer_compare_data(&left.data, &right.data);

  if (compRes == 0) {
    big_integer_set(0, pResult);
  } else if (compRes > 0) {
    // left > right
    pResult->sign = left.sign;
    big_integer_subtract_data_inplace(left.data, right.data, &pResult->data);
  } else {
    pResult->sign = -right.sign;
    big_integer_subtract_data_inplace(right.data, left.data, &pResult->data);
  }
  return;
}

void big_integer_increment(BigInteger *bigInt, const unsigned int value) {
  if (bigInt->sign >= 0) {
    // bigInt >= 0
    if (bigInt->sign == 0 && value > 0)
      bigInt->sign = 1;
    big_integer_increment_data(&bigInt->data, value);
  } else {
    // bigInt < 0
    int compRes = big_integer_compare_data_uint(&bigInt->data, value);

    if (compRes == 0) {
      // |bigInt| == |value|
      bigInt->sign = 0;
      bigInt->data.size = 1;
      bigInt->data.bits[0] = 0;
      big_integer_clear_trash_data(&bigInt->data);
    } else if (compRes > 0) {
      // |bigInt| > |value|
      big_integer_decrement_data(&bigInt->data, value);
    } else {
      // |bigInt| < |value|
#ifdef DEBUG
      /* |bigInt| < |value| implies that bigInt has length 1
         because value, if expressed as a BigInteger, would have length 1. */
      assert(bigInt->data.size == 1);
#endif
      bigInt->sign = 1;
      bigInt->data.bits[0] = value - bigInt->data.bits[0];
    }
  }
}

void big_integer_decrement(BigInteger *bigInt, const unsigned int value) {
  if (bigInt->sign <= 0) {
    // bigInt <= 0
    if (bigInt->sign == 0 && value > 0)
      bigInt->sign = -1;
    big_integer_increment_data(&bigInt->data, value);
  } else {
    // bigInt > 0
    int compRes = big_integer_compare_data_uint(&bigInt->data, value);

    if (compRes == 0) {
      // |bigInt| == |value|
      bigInt->sign = 0;
      bigInt->data.size = 1;
      bigInt->data.bits[0] = 0;
      big_integer_clear_trash_data(&bigInt->data);
    } else if (compRes > 0) {
      // |bigInt| > |value|
      big_integer_decrement_data(&bigInt->data, value);
    } else {
      // |bigInt| < |value|
#ifdef DEBUG
      /* |bigInt| < |value| implies that bigInt has length 1 or less
         because value, if expressed as a BigInteger, would have length 1. */
      assert(bigInt->data.size == 1);
#endif
      bigInt->sign = -1;
      bigInt->data.bits[0] = value - bigInt->data.bits[0];
    }
  }
}

void big_integer_div_by_power_of_two_inplace_fixed_precision(
    BigInteger *bi, int nbits, const int precision) {
  assert(nbits < UINT_NUM_BITS);
  int offset = 0;
  unsigned long threshold = 1lu << nbits;
  unsigned long bitmask = threshold - 1;
  unsigned long buff_l = 0, buff_r = 0;
  if (bi->data.bits[bi->data.size - 1] < threshold)
    offset++;
  for (int idx = bi->data.size - 1;
       idx >= bi->data.size - precision - offset && idx >= 0; idx--) {
    buff_r = bi->data.bits[idx] & bitmask;
    bi->data.bits[idx] >>= nbits;
    bi->data.bits[idx] += buff_l << (UINT_NUM_BITS - nbits);
    buff_l = buff_r;
  }
  bi->data.size -= offset;
  if (bi->data.size == 0) {
    bi->sign = 0;
  }
}

BigInteger big_integer_multiply(const BigInteger left, const BigInteger right) {
  if (left.sign == 0 || right.sign == 0)
    return big_integer_create(0);
  return big_integer_create_internal(
      left.sign * right.sign, big_integer_multiply_data(left.data, right.data));
}

BigInteger big_integer_multiply_opt(const BigInteger left,
                                    const BigInteger right) {
  if (left.sign == 0 || right.sign == 0)
    return big_integer_create(0);
  return big_integer_create_internal(
      left.sign * right.sign,
      big_integer_multiply_data_opt(left.data, right.data));
}

void big_integer_multiply_inplace(const BigInteger left, const BigInteger right,
                                  BigInteger *pResult) {
  if (left.sign == 0 || right.sign == 0)
    big_integer_set(0, pResult);
  else {
    pResult->sign = left.sign * right.sign;
    big_integer_multiply_data_inplace(left.data, right.data, &pResult->data);
  }
}

void big_integer_multiply_two_operands_opt(BigInteger *pLeft,
                                           const BigInteger right) {
  if (pLeft->sign == 0 || right.sign == 0)
    big_integer_set(0, pLeft);
  else {
    pLeft->sign *= right.sign;
    // int allocated_size = pLeft->data.size + right.data.size + 1;
    // BigIntegerData result;
    // // BigIntegerData *pLeftData = &(pLeft->data);
    // result.bits = (unsigned int *)calloc(allocated_size, UINT_NUM_BYTES);
    // result.capacity = allocated_size;
    big_integer_multiply_data_two_operands_opt(&(pLeft->data), right.data);
    // pLeft->data = *pLeftData;
    // big_integer_destroy_data(&(pLeft->data));
    // pLeft->data.size = result.size;
    // pLeft->data.capacity = result.capacity;
    // free(pLeft->data.bits);
    // pLeft->data.bits = result.bits;
  }
}

BigInteger big_integer_multiply_karatsuba(const BigInteger left,
                                          const BigInteger right) {
  if (left.sign == 0 || right.sign == 0)
    return big_integer_create(0);
  return big_integer_create_internal(
      left.sign * right.sign,
      big_integer_multiply_data_karatsuba(left.data, right.data));
}

void big_integer_add_inplace_fixed_precision(BigInteger *left,
                                             const BigInteger right,
                                             const int precision,
                                             int *carried) {
  if (left->sign == 0) {
    // left is 0, copy right to left
    memmove(left->data.bits, right.data.bits + right.data.size - precision,
            precision * UINT_NUM_BYTES);
    left->data.size = precision;
    left->sign = right.sign;
    return;
  }
  if (right.sign == 0) {
    // right is 0
    memmove(left->data.bits, left->data.bits + left->data.size - precision,
            precision * UINT_NUM_BYTES);
    left->data.size = precision;
    return;
  }

  if (left->sign == right.sign) {
    big_integer_add_inplace_fixed_precision_data(&left->data, right.data,
                                                 precision, carried);
    return;
  }

  int compRes = big_integer_compare_data(&left->data, &right.data);

  if (compRes == 0) {
    // answer is 0
    left->sign = 0;
    left->data.size = precision;
    memset(left->data.bits, 0, precision * UINT_NUM_BYTES);
    return;
  }

  if (compRes > 0) {
    // left > right
    big_integer_subtract_inplace_fixed_precision_data(
        left->data, right.data, &left->data, precision, carried);
  } else {
    // right > left
    BigIntegerData result;
    result.bits = (unsigned long *)malloc(precision * UINT_NUM_BYTES);
    result.capacity = precision;
    result.size = precision;
    big_integer_subtract_inplace_fixed_precision_data(
        right.data, left->data, &result, precision, carried);
    left->sign = right.sign;
    free(left->data.bits); // free left's original memory space
    left->data = result;   // assign the new one
  }
}

/**
 * TODO: Optimization
 * 1. scalar replacement for left->size
 * 2. avoid explicit type casting?
 */
void big_integer_subtract_inplace_fixed_precision_data(
    const BigIntegerData left, const BigIntegerData right,
    BigIntegerData *result, const int precision, int *carried) {
  unsigned long borrow = 0;
  int offset = left.size - precision;
  for (int i = offset; i < left.size && i < right.size; i++) {
    /* what happens here is that, if left is less than right, borrow will become
       "negative" (not really because it is unsigned), and the bit pattern for
       that is the 1's complement (complementing it to get to 0), which is
       exactly the remainder of this_ term in the subtraction. */
    borrow = left.bits[i] - right.bits[i] - borrow;
    result->bits[i] = borrow & bit_mask;
    /* here we just want the first 1 after removing the lower order term */
    borrow = (borrow >> UINT_NUM_BITS) & 1;
  }
  // get number of zeros ahead, useful in bigfloat add
  *carried = 0;
  for (int i = left.size - 1; i >= offset; i--) {
    if (result->bits[i] == 0) {
      (*carried)--;
      offset--;
    } else {
      break;
    }
  }
  memmove(result->bits, result->bits + offset, precision * UINT_NUM_BYTES);
  result->size = precision;
}

/**
 * TODO: Optimization
 * 1. scalar replacement for left->size
 * 2. avoid explicit type casting?
 */
void big_integer_add_inplace_fixed_precision_data(BigIntegerData *left,
                                                  const BigIntegerData right,
                                                  const int precision,
                                                  int *carried) {
  unsigned long sum = 0;
  int offset = left->size - precision;
  for (int i = offset; i < left->size && i < right.size; i++) {
    sum += left->bits[i] + right.bits[i];
    left->bits[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }
  int data_size = precision;
  *carried = 0;
  if (sum != 0) {
    // carry-over
    offset++;
    data_size--;
    *carried = 1;
  }
  memmove(left->bits, left->bits + offset, data_size * UINT_NUM_BYTES);
  if (sum != 0) {
    // carry-over
    left->bits[left->size - 1] = sum & bit_mask;
  }
  left->size = precision;
  return;
}

/**
 * TODO: Optimization
 * 1. scalar replacement for left->data.size
 * 2. scalar replacement for right.data.size
 * 3. strength redection for idx
 * 4. avoid explicit type casting?
 */
void big_integer_multiply_inplace_fixed_precision(BigInteger *left,
                                                  const BigInteger right,
                                                  const int precision,
                                                  int *powerdiff) {
  if (left->sign == 0 || right.sign == 0) {
    // answer is 0
    left->sign = 0;
    left->data.size = precision;
    memset(left->data.bits, 0, precision * 8);
  }
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left->data.size - precision;
  int offset_right = right.data.size - precision;
  for (int j = offset_right; j < right.data.size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left->data.size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  *powerdiff = 0;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
      (*powerdiff)--;
    }
  }
  memmove(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

BigInteger big_integer_multiply_fixed_precision(BigInteger left,
                                                BigInteger right,
                                                const int precision,
                                                int *powerdiff) {
  if (left.sign == 0 || right.sign == 0) {
    // answer is 0
    BigInteger ans;
    ans.sign = 0;
    ans.data.size = precision;
    ans.data.capacity = precision;
    ans.data.bits = (unsigned long *)calloc(precision, sizeof(unsigned long));
    return ans;
  }
  BigInteger ans;
  ans.data.bits = (unsigned long *)malloc(precision * sizeof(unsigned long));
  unsigned long *tmp =
      (unsigned long *)calloc(2 * precision, sizeof(unsigned long));
  int offset_left = left.data.size - precision;
  int offset_right = right.data.size - precision;
  for (int j = offset_right; j < right.data.size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left.data.size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left.data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }
  ans.sign = left.sign * right.sign;
  ans.data.size = precision;
  ans.data.capacity = precision;
  *powerdiff = 0;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      (*powerdiff)--;
    }
  }
  memmove(ans.data.bits, tmp + precision + *powerdiff, precision * 8);
  free(tmp); // release temporal variable
  return ans;
}

void big_integer_multiply_toplace_fixed_precision(BigInteger left,
                                                  BigInteger right,
                                                  BigInteger *res,
                                                  const int precision,
                                                  int *powerdiff) {
  if (left.sign == 0 || right.sign == 0) {
    // answer is 0
    res->sign = 0;
    res->data.size = precision;
    res->data.capacity = precision;
  }
  unsigned long *tmp =
      (unsigned long *)calloc(2 * precision, sizeof(unsigned long));
  int offset_left = left.data.size - precision;
  int offset_right = right.data.size - precision;
  for (int j = offset_right; j < right.data.size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left.data.size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left.data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }
  res->sign = left.sign * right.sign;
  res->data.size = precision;
  *powerdiff = 0;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      (*powerdiff)--;
    }
  }
  memmove(res->data.bits, tmp + precision + *powerdiff, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_unflod(BigInteger *left,
                                                         const BigInteger right,
                                                         const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left->data.size - precision;
  int offset_right = right.data.size - precision;
  int j = offset_right;
  for (; j < right_size - 3; j += 4) {
    unsigned long carry1 = 0;
    unsigned long carry2 = 0;
    unsigned long carry3 = 0;
    unsigned long carry4 = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry1 += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      carry2 += left->data.bits[i] * right.data.bits[j + 1] + tmp[idx + 1];
      carry3 += left->data.bits[i] * right.data.bits[j + 2] + tmp[idx + 2];
      carry4 += left->data.bits[i] * right.data.bits[j + 3] + tmp[idx + 3];
      tmp[idx] = carry1 & bit_mask;
      tmp[idx + 1] = carry2 & bit_mask;
      tmp[idx + 2] = carry3 & bit_mask;
      tmp[idx + 3] = carry4 & bit_mask;
      carry1 >>= UINT_NUM_BITS;
      carry2 >>= UINT_NUM_BITS;
      carry3 >>= UINT_NUM_BITS;
      carry4 >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;

    unsigned long carry = 0;
    carry += tmp[idx] + carry1;
    tmp[idx] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 1] + carry2;
    tmp[idx + 1] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 2] + carry3;
    tmp[idx + 2] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 3] + carry4;
    tmp[idx + 3] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_verter_1x_unfold(
    BigInteger *left, const BigInteger right, const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left_size - precision;
  int offset_right = right_size - precision;
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);
  int j = offset_right;
  for (; j < right_size - 3; j += 4) {
    __m256i carry1 = _mm256_set1_epi64x(0);
    __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits + j));
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      __m256i lo = _mm256_set1_epi64x(left->data.bits[i]);

      __m256i mul_res1 = _mm256_mul_epu32(lo, ro1);

      carry1 = _mm256_add_epi64(carry1, mul_res1);
      carry1 = _mm256_add_epi64(
          carry1, _mm256_loadu_si256((const __m256i *)(tmp + idx)));

      _mm256_storeu_si256((__m256i *)(tmp + idx),
                          _mm256_and_si256(carry1, bit_maskv));
      carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
    }

    int idx = j - offset_right + precision;
    unsigned long carry = 0;
    unsigned long carry_array[] = {0, 0, 0, 0};
    __m256i *carry_array_addr = (__m256i *)carry_array;

    _mm256_storeu_si256(carry_array_addr, carry1);
    carry += tmp[idx] + carry_array[0];
    tmp[idx] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 1] + carry_array[1];
    tmp[idx + 1] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 2] + carry_array[2];
    tmp[idx + 2] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 3] + carry_array[3];
    tmp[idx + 3] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_verter_2x_unfold(
    BigInteger *left, const BigInteger right, const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left_size - precision;
  int offset_right = right_size - precision;
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);
  int j = offset_right;
  for (; j < right_size - 7; j += 8) {
    __m256i carry1 = _mm256_set1_epi64x(0);
    __m256i carry2 = _mm256_set1_epi64x(0);
    __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits + j));
    __m256i ro2 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 4));
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      __m256i lo = _mm256_set1_epi64x(left->data.bits[i]);

      __m256i mul_res1 = _mm256_mul_epu32(lo, ro1);
      __m256i mul_res2 = _mm256_mul_epu32(lo, ro2);

      carry1 = _mm256_add_epi64(carry1, mul_res1);
      carry1 = _mm256_add_epi64(
          carry1, _mm256_loadu_si256((const __m256i *)(tmp + idx)));
      carry2 = _mm256_add_epi64(carry2, mul_res2);
      carry2 = _mm256_add_epi64(
          carry2, _mm256_loadu_si256((const __m256i *)(tmp + idx + 4)));

      _mm256_storeu_si256((__m256i *)(tmp + idx),
                          _mm256_and_si256(carry1, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 4),
                          _mm256_and_si256(carry2, bit_maskv));
      carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
      carry2 = _mm256_srli_epi64(carry2, UINT_NUM_BITS);
    }

    int idx = j - offset_right + precision;
    unsigned long carry = 0;
    unsigned long carry_array[] = {0, 0, 0, 0};
    __m256i *carry_array_addr = (__m256i *)carry_array;

    _mm256_storeu_si256(carry_array_addr, carry1);
    carry += tmp[idx] + carry_array[0];
    tmp[idx] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 1] + carry_array[1];
    tmp[idx + 1] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 2] + carry_array[2];
    tmp[idx + 2] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 3] + carry_array[3];
    tmp[idx + 3] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry2);
    carry += tmp[idx + 4] + carry_array[0];
    tmp[idx + 4] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 5] + carry_array[1];
    tmp[idx + 5] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 6] + carry_array[2];
    tmp[idx + 6] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 7] + carry_array[3];
    tmp[idx + 7] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_verter_4x_unfold(
    BigInteger *left, const BigInteger right, const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left_size - precision;
  int offset_right = right_size - precision;
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);
  int j = offset_right;
  for (; j < right_size - 15; j += 16) {
    __m256i carry1 = _mm256_set1_epi64x(0);
    __m256i carry2 = _mm256_set1_epi64x(0);
    __m256i carry3 = _mm256_set1_epi64x(0);
    __m256i carry4 = _mm256_set1_epi64x(0);
    __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits + j));
    __m256i ro2 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 4));
    __m256i ro3 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 8));
    __m256i ro4 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 12));
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      __m256i lo = _mm256_set1_epi64x(left->data.bits[i]);

      __m256i mul_res1 = _mm256_mul_epu32(lo, ro1);
      __m256i mul_res2 = _mm256_mul_epu32(lo, ro2);
      __m256i mul_res3 = _mm256_mul_epu32(lo, ro3);
      __m256i mul_res4 = _mm256_mul_epu32(lo, ro4);

      carry1 = _mm256_add_epi64(carry1, mul_res1);
      carry1 = _mm256_add_epi64(
          carry1, _mm256_loadu_si256((const __m256i *)(tmp + idx)));
      carry2 = _mm256_add_epi64(carry2, mul_res2);
      carry2 = _mm256_add_epi64(
          carry2, _mm256_loadu_si256((const __m256i *)(tmp + idx + 4)));
      carry3 = _mm256_add_epi64(carry3, mul_res3);
      carry3 = _mm256_add_epi64(
          carry3, _mm256_loadu_si256((const __m256i *)(tmp + idx + 8)));
      carry4 = _mm256_add_epi64(carry4, mul_res4);
      carry4 = _mm256_add_epi64(
          carry4, _mm256_loadu_si256((const __m256i *)(tmp + idx + 12)));

      _mm256_storeu_si256((__m256i *)(tmp + idx),
                          _mm256_and_si256(carry1, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 4),
                          _mm256_and_si256(carry2, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 8),
                          _mm256_and_si256(carry3, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 12),
                          _mm256_and_si256(carry4, bit_maskv));
      carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
      carry2 = _mm256_srli_epi64(carry2, UINT_NUM_BITS);
      carry3 = _mm256_srli_epi64(carry3, UINT_NUM_BITS);
      carry4 = _mm256_srli_epi64(carry4, UINT_NUM_BITS);
    }

    int idx = j - offset_right + precision;
    unsigned long carry = 0;
    unsigned long carry_array[] = {0, 0, 0, 0};
    __m256i *carry_array_addr = (__m256i *)carry_array;

    _mm256_storeu_si256(carry_array_addr, carry1);
    carry += tmp[idx] + carry_array[0];
    tmp[idx] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 1] + carry_array[1];
    tmp[idx + 1] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 2] + carry_array[2];
    tmp[idx + 2] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 3] + carry_array[3];
    tmp[idx + 3] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry2);
    carry += tmp[idx + 4] + carry_array[0];
    tmp[idx + 4] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 5] + carry_array[1];
    tmp[idx + 5] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 6] + carry_array[2];
    tmp[idx + 6] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 7] + carry_array[3];
    tmp[idx + 7] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry3);
    carry += tmp[idx + 8] + carry_array[0];
    tmp[idx + 8] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 9] + carry_array[1];
    tmp[idx + 9] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 10] + carry_array[2];
    tmp[idx + 10] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 11] + carry_array[3];
    tmp[idx + 11] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry4);
    carry += tmp[idx + 12] + carry_array[0];
    tmp[idx + 12] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 13] + carry_array[1];
    tmp[idx + 13] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 14] + carry_array[2];
    tmp[idx + 14] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 15] + carry_array[3];
    tmp[idx + 15] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_verter_8x_unfold(
    BigInteger *left, const BigInteger right, const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left_size - precision;
  int offset_right = right_size - precision;
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);
  int j = offset_right;
  for (; j < right_size - 31; j += 32) {
    __m256i carry1 = _mm256_set1_epi64x(0);
    __m256i carry2 = _mm256_set1_epi64x(0);
    __m256i carry3 = _mm256_set1_epi64x(0);
    __m256i carry4 = _mm256_set1_epi64x(0);
    __m256i carry5 = _mm256_set1_epi64x(0);
    __m256i carry6 = _mm256_set1_epi64x(0);
    __m256i carry7 = _mm256_set1_epi64x(0);
    __m256i carry8 = _mm256_set1_epi64x(0);
    __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits + j));
    __m256i ro2 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 4));
    __m256i ro3 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 8));
    __m256i ro4 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 12));
    __m256i ro5 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 16));
    __m256i ro6 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 20));
    __m256i ro7 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 24));
    __m256i ro8 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 28));
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      __m256i lo = _mm256_set1_epi64x(left->data.bits[i]);

      __m256i mul_res1 = _mm256_mul_epu32(lo, ro1);
      __m256i mul_res2 = _mm256_mul_epu32(lo, ro2);
      __m256i mul_res3 = _mm256_mul_epu32(lo, ro3);
      __m256i mul_res4 = _mm256_mul_epu32(lo, ro4);
      __m256i mul_res5 = _mm256_mul_epu32(lo, ro5);
      __m256i mul_res6 = _mm256_mul_epu32(lo, ro6);
      __m256i mul_res7 = _mm256_mul_epu32(lo, ro7);
      __m256i mul_res8 = _mm256_mul_epu32(lo, ro8);

      carry1 = _mm256_add_epi64(carry1, mul_res1);
      carry1 = _mm256_add_epi64(
          carry1, _mm256_loadu_si256((const __m256i *)(tmp + idx)));
      carry2 = _mm256_add_epi64(carry2, mul_res2);
      carry2 = _mm256_add_epi64(
          carry2, _mm256_loadu_si256((const __m256i *)(tmp + idx + 4)));
      carry3 = _mm256_add_epi64(carry3, mul_res3);
      carry3 = _mm256_add_epi64(
          carry3, _mm256_loadu_si256((const __m256i *)(tmp + idx + 8)));
      carry4 = _mm256_add_epi64(carry4, mul_res4);
      carry4 = _mm256_add_epi64(
          carry4, _mm256_loadu_si256((const __m256i *)(tmp + idx + 12)));
      carry5 = _mm256_add_epi64(carry5, mul_res5);
      carry5 = _mm256_add_epi64(
          carry5, _mm256_loadu_si256((const __m256i *)(tmp + idx + 16)));
      carry6 = _mm256_add_epi64(carry6, mul_res6);
      carry6 = _mm256_add_epi64(
          carry6, _mm256_loadu_si256((const __m256i *)(tmp + idx + 20)));
      carry7 = _mm256_add_epi64(carry7, mul_res7);
      carry7 = _mm256_add_epi64(
          carry7, _mm256_loadu_si256((const __m256i *)(tmp + idx + 24)));
      carry8 = _mm256_add_epi64(carry8, mul_res8);
      carry8 = _mm256_add_epi64(
          carry8, _mm256_loadu_si256((const __m256i *)(tmp + idx + 28)));

      _mm256_storeu_si256((__m256i *)(tmp + idx),
                          _mm256_and_si256(carry1, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 4),
                          _mm256_and_si256(carry2, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 8),
                          _mm256_and_si256(carry3, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 12),
                          _mm256_and_si256(carry4, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 16),
                          _mm256_and_si256(carry5, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 20),
                          _mm256_and_si256(carry6, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 24),
                          _mm256_and_si256(carry7, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 28),
                          _mm256_and_si256(carry8, bit_maskv));
      carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
      carry2 = _mm256_srli_epi64(carry2, UINT_NUM_BITS);
      carry3 = _mm256_srli_epi64(carry3, UINT_NUM_BITS);
      carry4 = _mm256_srli_epi64(carry4, UINT_NUM_BITS);
      carry5 = _mm256_srli_epi64(carry5, UINT_NUM_BITS);
      carry6 = _mm256_srli_epi64(carry6, UINT_NUM_BITS);
      carry7 = _mm256_srli_epi64(carry7, UINT_NUM_BITS);
      carry8 = _mm256_srli_epi64(carry8, UINT_NUM_BITS);
    }

    int idx = j - offset_right + precision;
    unsigned long carry = 0;
    unsigned long carry_array[] = {0, 0, 0, 0};
    __m256i *carry_array_addr = (__m256i *)carry_array;

    _mm256_storeu_si256(carry_array_addr, carry1);
    carry += tmp[idx] + carry_array[0];
    tmp[idx] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 1] + carry_array[1];
    tmp[idx + 1] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 2] + carry_array[2];
    tmp[idx + 2] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 3] + carry_array[3];
    tmp[idx + 3] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry2);
    carry += tmp[idx + 4] + carry_array[0];
    tmp[idx + 4] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 5] + carry_array[1];
    tmp[idx + 5] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 6] + carry_array[2];
    tmp[idx + 6] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 7] + carry_array[3];
    tmp[idx + 7] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry3);
    carry += tmp[idx + 8] + carry_array[0];
    tmp[idx + 8] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 9] + carry_array[1];
    tmp[idx + 9] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 10] + carry_array[2];
    tmp[idx + 10] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 11] + carry_array[3];
    tmp[idx + 11] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry4);
    carry += tmp[idx + 12] + carry_array[0];
    tmp[idx + 12] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 13] + carry_array[1];
    tmp[idx + 13] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 14] + carry_array[2];
    tmp[idx + 14] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 15] + carry_array[3];
    tmp[idx + 15] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry5);
    carry += tmp[idx + 16] + carry_array[0];
    tmp[idx + 16] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 17] + carry_array[1];
    tmp[idx + 17] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 18] + carry_array[2];
    tmp[idx + 18] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 19] + carry_array[3];
    tmp[idx + 19] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry6);
    carry += tmp[idx + 20] + carry_array[0];
    tmp[idx + 20] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 21] + carry_array[1];
    tmp[idx + 21] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 22] + carry_array[2];
    tmp[idx + 22] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 23] + carry_array[3];
    tmp[idx + 23] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry7);
    carry += tmp[idx + 24] + carry_array[0];
    tmp[idx + 24] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 25] + carry_array[1];
    tmp[idx + 25] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 26] + carry_array[2];
    tmp[idx + 26] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 27] + carry_array[3];
    tmp[idx + 27] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry8);
    carry += tmp[idx + 28] + carry_array[0];
    tmp[idx + 28] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 29] + carry_array[1];
    tmp[idx + 29] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 30] + carry_array[2];
    tmp[idx + 30] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 31] + carry_array[3];
    tmp[idx + 31] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_verter_12x_unfold(
    BigInteger *left, const BigInteger right, const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left_size - precision;
  int offset_right = right_size - precision;
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);
  int j = offset_right;
  for (; j < right_size - 47; j += 48) {
    __m256i carry1 = _mm256_set1_epi64x(0);
    __m256i carry2 = _mm256_set1_epi64x(0);
    __m256i carry3 = _mm256_set1_epi64x(0);
    __m256i carry4 = _mm256_set1_epi64x(0);
    __m256i carry5 = _mm256_set1_epi64x(0);
    __m256i carry6 = _mm256_set1_epi64x(0);
    __m256i carry7 = _mm256_set1_epi64x(0);
    __m256i carry8 = _mm256_set1_epi64x(0);
    __m256i carry9 = _mm256_set1_epi64x(0);
    __m256i carry10 = _mm256_set1_epi64x(0);
    __m256i carry11 = _mm256_set1_epi64x(0);
    __m256i carry12 = _mm256_set1_epi64x(0);
    __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits + j));
    __m256i ro2 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 4));
    __m256i ro3 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 8));
    __m256i ro4 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 12));
    __m256i ro5 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 16));
    __m256i ro6 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 20));
    __m256i ro7 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 24));
    __m256i ro8 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 28));
    __m256i ro9 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 32));
    __m256i ro10 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 36));
    __m256i ro11 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 40));
    __m256i ro12 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 44));
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      __m256i lo = _mm256_set1_epi64x(left->data.bits[i]);

      __m256i mul_res1 = _mm256_mul_epu32(lo, ro1);
      __m256i mul_res2 = _mm256_mul_epu32(lo, ro2);
      __m256i mul_res3 = _mm256_mul_epu32(lo, ro3);
      __m256i mul_res4 = _mm256_mul_epu32(lo, ro4);
      __m256i mul_res5 = _mm256_mul_epu32(lo, ro5);
      __m256i mul_res6 = _mm256_mul_epu32(lo, ro6);
      __m256i mul_res7 = _mm256_mul_epu32(lo, ro7);
      __m256i mul_res8 = _mm256_mul_epu32(lo, ro8);
      __m256i mul_res9 = _mm256_mul_epu32(lo, ro9);
      __m256i mul_res10 = _mm256_mul_epu32(lo, ro10);
      __m256i mul_res11 = _mm256_mul_epu32(lo, ro11);
      __m256i mul_res12 = _mm256_mul_epu32(lo, ro12);

      carry1 = _mm256_add_epi64(carry1, mul_res1);
      carry1 = _mm256_add_epi64(
          carry1, _mm256_loadu_si256((const __m256i *)(tmp + idx)));
      carry2 = _mm256_add_epi64(carry2, mul_res2);
      carry2 = _mm256_add_epi64(
          carry2, _mm256_loadu_si256((const __m256i *)(tmp + idx + 4)));
      carry3 = _mm256_add_epi64(carry3, mul_res3);
      carry3 = _mm256_add_epi64(
          carry3, _mm256_loadu_si256((const __m256i *)(tmp + idx + 8)));
      carry4 = _mm256_add_epi64(carry4, mul_res4);
      carry4 = _mm256_add_epi64(
          carry4, _mm256_loadu_si256((const __m256i *)(tmp + idx + 12)));
      carry5 = _mm256_add_epi64(carry5, mul_res5);
      carry5 = _mm256_add_epi64(
          carry5, _mm256_loadu_si256((const __m256i *)(tmp + idx + 16)));
      carry6 = _mm256_add_epi64(carry6, mul_res6);
      carry6 = _mm256_add_epi64(
          carry6, _mm256_loadu_si256((const __m256i *)(tmp + idx + 20)));
      carry7 = _mm256_add_epi64(carry7, mul_res7);
      carry7 = _mm256_add_epi64(
          carry7, _mm256_loadu_si256((const __m256i *)(tmp + idx + 24)));
      carry8 = _mm256_add_epi64(carry8, mul_res8);
      carry8 = _mm256_add_epi64(
          carry8, _mm256_loadu_si256((const __m256i *)(tmp + idx + 28)));
      carry9 = _mm256_add_epi64(carry9, mul_res9);
      carry9 = _mm256_add_epi64(
          carry9, _mm256_loadu_si256((const __m256i *)(tmp + idx + 32)));
      carry10 = _mm256_add_epi64(carry10, mul_res10);
      carry10 = _mm256_add_epi64(
          carry10, _mm256_loadu_si256((const __m256i *)(tmp + idx + 36)));
      carry11 = _mm256_add_epi64(carry11, mul_res11);
      carry11 = _mm256_add_epi64(
          carry11, _mm256_loadu_si256((const __m256i *)(tmp + idx + 40)));
      carry12 = _mm256_add_epi64(carry12, mul_res12);
      carry12 = _mm256_add_epi64(
          carry12, _mm256_loadu_si256((const __m256i *)(tmp + idx + 44)));

      _mm256_storeu_si256((__m256i *)(tmp + idx),
                          _mm256_and_si256(carry1, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 4),
                          _mm256_and_si256(carry2, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 8),
                          _mm256_and_si256(carry3, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 12),
                          _mm256_and_si256(carry4, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 16),
                          _mm256_and_si256(carry5, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 20),
                          _mm256_and_si256(carry6, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 24),
                          _mm256_and_si256(carry7, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 28),
                          _mm256_and_si256(carry8, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 32),
                          _mm256_and_si256(carry9, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 36),
                          _mm256_and_si256(carry10, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 40),
                          _mm256_and_si256(carry11, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 44),
                          _mm256_and_si256(carry12, bit_maskv));
      carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
      carry2 = _mm256_srli_epi64(carry2, UINT_NUM_BITS);
      carry3 = _mm256_srli_epi64(carry3, UINT_NUM_BITS);
      carry4 = _mm256_srli_epi64(carry4, UINT_NUM_BITS);
      carry5 = _mm256_srli_epi64(carry5, UINT_NUM_BITS);
      carry6 = _mm256_srli_epi64(carry6, UINT_NUM_BITS);
      carry7 = _mm256_srli_epi64(carry7, UINT_NUM_BITS);
      carry8 = _mm256_srli_epi64(carry8, UINT_NUM_BITS);
      carry9 = _mm256_srli_epi64(carry9, UINT_NUM_BITS);
      carry10 = _mm256_srli_epi64(carry10, UINT_NUM_BITS);
      carry11 = _mm256_srli_epi64(carry11, UINT_NUM_BITS);
      carry12 = _mm256_srli_epi64(carry12, UINT_NUM_BITS);
    }

    int idx = j - offset_right + precision;
    unsigned long carry = 0;
    unsigned long carry_array[] = {0, 0, 0, 0};
    __m256i *carry_array_addr = (__m256i *)carry_array;

    _mm256_storeu_si256(carry_array_addr, carry1);
    carry += tmp[idx] + carry_array[0];
    tmp[idx] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 1] + carry_array[1];
    tmp[idx + 1] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 2] + carry_array[2];
    tmp[idx + 2] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 3] + carry_array[3];
    tmp[idx + 3] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry2);
    carry += tmp[idx + 4] + carry_array[0];
    tmp[idx + 4] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 5] + carry_array[1];
    tmp[idx + 5] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 6] + carry_array[2];
    tmp[idx + 6] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 7] + carry_array[3];
    tmp[idx + 7] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry3);
    carry += tmp[idx + 8] + carry_array[0];
    tmp[idx + 8] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 9] + carry_array[1];
    tmp[idx + 9] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 10] + carry_array[2];
    tmp[idx + 10] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 11] + carry_array[3];
    tmp[idx + 11] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry4);
    carry += tmp[idx + 12] + carry_array[0];
    tmp[idx + 12] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 13] + carry_array[1];
    tmp[idx + 13] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 14] + carry_array[2];
    tmp[idx + 14] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 15] + carry_array[3];
    tmp[idx + 15] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry5);
    carry += tmp[idx + 16] + carry_array[0];
    tmp[idx + 16] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 17] + carry_array[1];
    tmp[idx + 17] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 18] + carry_array[2];
    tmp[idx + 18] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 19] + carry_array[3];
    tmp[idx + 19] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry6);
    carry += tmp[idx + 20] + carry_array[0];
    tmp[idx + 20] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 21] + carry_array[1];
    tmp[idx + 21] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 22] + carry_array[2];
    tmp[idx + 22] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 23] + carry_array[3];
    tmp[idx + 23] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry7);
    carry += tmp[idx + 24] + carry_array[0];
    tmp[idx + 24] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 25] + carry_array[1];
    tmp[idx + 25] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 26] + carry_array[2];
    tmp[idx + 26] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 27] + carry_array[3];
    tmp[idx + 27] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry8);
    carry += tmp[idx + 28] + carry_array[0];
    tmp[idx + 28] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 29] + carry_array[1];
    tmp[idx + 29] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 30] + carry_array[2];
    tmp[idx + 30] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 31] + carry_array[3];
    tmp[idx + 31] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry9);
    carry += tmp[idx + 32] + carry_array[0];
    tmp[idx + 32] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 33] + carry_array[1];
    tmp[idx + 33] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 34] + carry_array[2];
    tmp[idx + 34] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 35] + carry_array[3];
    tmp[idx + 35] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry10);
    carry += tmp[idx + 36] + carry_array[0];
    tmp[idx + 36] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 37] + carry_array[1];
    tmp[idx + 37] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 38] + carry_array[2];
    tmp[idx + 38] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 39] + carry_array[3];
    tmp[idx + 39] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry11);
    carry += tmp[idx + 40] + carry_array[0];
    tmp[idx + 40] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 41] + carry_array[1];
    tmp[idx + 41] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 42] + carry_array[2];
    tmp[idx + 42] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 43] + carry_array[3];
    tmp[idx + 43] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry12);
    carry += tmp[idx + 44] + carry_array[0];
    tmp[idx + 44] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 45] + carry_array[1];
    tmp[idx + 45] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 46] + carry_array[2];
    tmp[idx + 46] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 47] + carry_array[3];
    tmp[idx + 47] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_verter_16x_unfold(
    BigInteger *left, const BigInteger right, const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left_size - precision;
  int offset_right = right_size - precision;
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);
  int j = offset_right;
  for (; j < right_size - 63; j += 64) {
    __m256i carry1 = _mm256_set1_epi64x(0);
    __m256i carry2 = _mm256_set1_epi64x(0);
    __m256i carry3 = _mm256_set1_epi64x(0);
    __m256i carry4 = _mm256_set1_epi64x(0);
    __m256i carry5 = _mm256_set1_epi64x(0);
    __m256i carry6 = _mm256_set1_epi64x(0);
    __m256i carry7 = _mm256_set1_epi64x(0);
    __m256i carry8 = _mm256_set1_epi64x(0);
    __m256i carry9 = _mm256_set1_epi64x(0);
    __m256i carry10 = _mm256_set1_epi64x(0);
    __m256i carry11 = _mm256_set1_epi64x(0);
    __m256i carry12 = _mm256_set1_epi64x(0);
    __m256i carry13 = _mm256_set1_epi64x(0);
    __m256i carry14 = _mm256_set1_epi64x(0);
    __m256i carry15 = _mm256_set1_epi64x(0);
    __m256i carry16 = _mm256_set1_epi64x(0);
    __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits + j));
    __m256i ro2 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 4));
    __m256i ro3 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 8));
    __m256i ro4 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 12));
    __m256i ro5 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 16));
    __m256i ro6 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 20));
    __m256i ro7 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 24));
    __m256i ro8 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 28));
    __m256i ro9 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 32));
    __m256i ro10 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 36));
    __m256i ro11 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 40));
    __m256i ro12 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 44));
    __m256i ro13 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 48));
    __m256i ro14 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 52));
    __m256i ro15 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 56));
    __m256i ro16 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 60));
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      __m256i lo = _mm256_set1_epi64x(left->data.bits[i]);

      __m256i mul_res1 = _mm256_mul_epu32(lo, ro1);
      __m256i mul_res2 = _mm256_mul_epu32(lo, ro2);
      __m256i mul_res3 = _mm256_mul_epu32(lo, ro3);
      __m256i mul_res4 = _mm256_mul_epu32(lo, ro4);
      __m256i mul_res5 = _mm256_mul_epu32(lo, ro5);
      __m256i mul_res6 = _mm256_mul_epu32(lo, ro6);
      __m256i mul_res7 = _mm256_mul_epu32(lo, ro7);
      __m256i mul_res8 = _mm256_mul_epu32(lo, ro8);
      __m256i mul_res9 = _mm256_mul_epu32(lo, ro9);
      __m256i mul_res10 = _mm256_mul_epu32(lo, ro10);
      __m256i mul_res11 = _mm256_mul_epu32(lo, ro11);
      __m256i mul_res12 = _mm256_mul_epu32(lo, ro12);
      __m256i mul_res13 = _mm256_mul_epu32(lo, ro13);
      __m256i mul_res14 = _mm256_mul_epu32(lo, ro14);
      __m256i mul_res15 = _mm256_mul_epu32(lo, ro15);
      __m256i mul_res16 = _mm256_mul_epu32(lo, ro16);

      carry1 = _mm256_add_epi64(carry1, mul_res1);
      carry1 = _mm256_add_epi64(
          carry1, _mm256_loadu_si256((const __m256i *)(tmp + idx)));
      carry2 = _mm256_add_epi64(carry2, mul_res2);
      carry2 = _mm256_add_epi64(
          carry2, _mm256_loadu_si256((const __m256i *)(tmp + idx + 4)));
      carry3 = _mm256_add_epi64(carry3, mul_res3);
      carry3 = _mm256_add_epi64(
          carry3, _mm256_loadu_si256((const __m256i *)(tmp + idx + 8)));
      carry4 = _mm256_add_epi64(carry4, mul_res4);
      carry4 = _mm256_add_epi64(
          carry4, _mm256_loadu_si256((const __m256i *)(tmp + idx + 12)));
      carry5 = _mm256_add_epi64(carry5, mul_res5);
      carry5 = _mm256_add_epi64(
          carry5, _mm256_loadu_si256((const __m256i *)(tmp + idx + 16)));
      carry6 = _mm256_add_epi64(carry6, mul_res6);
      carry6 = _mm256_add_epi64(
          carry6, _mm256_loadu_si256((const __m256i *)(tmp + idx + 20)));
      carry7 = _mm256_add_epi64(carry7, mul_res7);
      carry7 = _mm256_add_epi64(
          carry7, _mm256_loadu_si256((const __m256i *)(tmp + idx + 24)));
      carry8 = _mm256_add_epi64(carry8, mul_res8);
      carry8 = _mm256_add_epi64(
          carry8, _mm256_loadu_si256((const __m256i *)(tmp + idx + 28)));
      carry9 = _mm256_add_epi64(carry9, mul_res9);
      carry9 = _mm256_add_epi64(
          carry9, _mm256_loadu_si256((const __m256i *)(tmp + idx + 32)));
      carry10 = _mm256_add_epi64(carry10, mul_res10);
      carry10 = _mm256_add_epi64(
          carry10, _mm256_loadu_si256((const __m256i *)(tmp + idx + 36)));
      carry11 = _mm256_add_epi64(carry11, mul_res11);
      carry11 = _mm256_add_epi64(
          carry11, _mm256_loadu_si256((const __m256i *)(tmp + idx + 40)));
      carry12 = _mm256_add_epi64(carry12, mul_res12);
      carry12 = _mm256_add_epi64(
          carry12, _mm256_loadu_si256((const __m256i *)(tmp + idx + 44)));
      carry13 = _mm256_add_epi64(carry13, mul_res13);
      carry13 = _mm256_add_epi64(
          carry13, _mm256_loadu_si256((const __m256i *)(tmp + idx + 48)));
      carry14 = _mm256_add_epi64(carry14, mul_res14);
      carry14 = _mm256_add_epi64(
          carry14, _mm256_loadu_si256((const __m256i *)(tmp + idx + 52)));
      carry15 = _mm256_add_epi64(carry15, mul_res15);
      carry15 = _mm256_add_epi64(
          carry15, _mm256_loadu_si256((const __m256i *)(tmp + idx + 56)));
      carry16 = _mm256_add_epi64(carry16, mul_res16);
      carry16 = _mm256_add_epi64(
          carry16, _mm256_loadu_si256((const __m256i *)(tmp + idx + 60)));

      _mm256_storeu_si256((__m256i *)(tmp + idx),
                          _mm256_and_si256(carry1, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 4),
                          _mm256_and_si256(carry2, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 8),
                          _mm256_and_si256(carry3, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 12),
                          _mm256_and_si256(carry4, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 16),
                          _mm256_and_si256(carry5, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 20),
                          _mm256_and_si256(carry6, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 24),
                          _mm256_and_si256(carry7, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 28),
                          _mm256_and_si256(carry8, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 32),
                          _mm256_and_si256(carry9, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 36),
                          _mm256_and_si256(carry10, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 40),
                          _mm256_and_si256(carry11, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 44),
                          _mm256_and_si256(carry12, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 48),
                          _mm256_and_si256(carry13, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 52),
                          _mm256_and_si256(carry14, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 56),
                          _mm256_and_si256(carry15, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 60),
                          _mm256_and_si256(carry16, bit_maskv));
      carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
      carry2 = _mm256_srli_epi64(carry2, UINT_NUM_BITS);
      carry3 = _mm256_srli_epi64(carry3, UINT_NUM_BITS);
      carry4 = _mm256_srli_epi64(carry4, UINT_NUM_BITS);
      carry5 = _mm256_srli_epi64(carry5, UINT_NUM_BITS);
      carry6 = _mm256_srli_epi64(carry6, UINT_NUM_BITS);
      carry7 = _mm256_srli_epi64(carry7, UINT_NUM_BITS);
      carry8 = _mm256_srli_epi64(carry8, UINT_NUM_BITS);
      carry9 = _mm256_srli_epi64(carry9, UINT_NUM_BITS);
      carry10 = _mm256_srli_epi64(carry10, UINT_NUM_BITS);
      carry11 = _mm256_srli_epi64(carry11, UINT_NUM_BITS);
      carry12 = _mm256_srli_epi64(carry12, UINT_NUM_BITS);
      carry13 = _mm256_srli_epi64(carry13, UINT_NUM_BITS);
      carry14 = _mm256_srli_epi64(carry14, UINT_NUM_BITS);
      carry15 = _mm256_srli_epi64(carry15, UINT_NUM_BITS);
      carry16 = _mm256_srli_epi64(carry16, UINT_NUM_BITS);
    }

    int idx = j - offset_right + precision;
    unsigned long carry = 0;
    unsigned long carry_array[] = {0, 0, 0, 0};
    __m256i *carry_array_addr = (__m256i *)carry_array;

    _mm256_storeu_si256(carry_array_addr, carry1);
    carry += tmp[idx] + carry_array[0];
    tmp[idx] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 1] + carry_array[1];
    tmp[idx + 1] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 2] + carry_array[2];
    tmp[idx + 2] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 3] + carry_array[3];
    tmp[idx + 3] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry2);
    carry += tmp[idx + 4] + carry_array[0];
    tmp[idx + 4] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 5] + carry_array[1];
    tmp[idx + 5] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 6] + carry_array[2];
    tmp[idx + 6] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 7] + carry_array[3];
    tmp[idx + 7] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry3);
    carry += tmp[idx + 8] + carry_array[0];
    tmp[idx + 8] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 9] + carry_array[1];
    tmp[idx + 9] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 10] + carry_array[2];
    tmp[idx + 10] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 11] + carry_array[3];
    tmp[idx + 11] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry4);
    carry += tmp[idx + 12] + carry_array[0];
    tmp[idx + 12] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 13] + carry_array[1];
    tmp[idx + 13] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 14] + carry_array[2];
    tmp[idx + 14] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 15] + carry_array[3];
    tmp[idx + 15] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry5);
    carry += tmp[idx + 16] + carry_array[0];
    tmp[idx + 16] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 17] + carry_array[1];
    tmp[idx + 17] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 18] + carry_array[2];
    tmp[idx + 18] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 19] + carry_array[3];
    tmp[idx + 19] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry6);
    carry += tmp[idx + 20] + carry_array[0];
    tmp[idx + 20] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 21] + carry_array[1];
    tmp[idx + 21] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 22] + carry_array[2];
    tmp[idx + 22] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 23] + carry_array[3];
    tmp[idx + 23] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry7);
    carry += tmp[idx + 24] + carry_array[0];
    tmp[idx + 24] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 25] + carry_array[1];
    tmp[idx + 25] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 26] + carry_array[2];
    tmp[idx + 26] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 27] + carry_array[3];
    tmp[idx + 27] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry8);
    carry += tmp[idx + 28] + carry_array[0];
    tmp[idx + 28] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 29] + carry_array[1];
    tmp[idx + 29] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 30] + carry_array[2];
    tmp[idx + 30] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 31] + carry_array[3];
    tmp[idx + 31] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry9);
    carry += tmp[idx + 32] + carry_array[0];
    tmp[idx + 32] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 33] + carry_array[1];
    tmp[idx + 33] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 34] + carry_array[2];
    tmp[idx + 34] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 35] + carry_array[3];
    tmp[idx + 35] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry10);
    carry += tmp[idx + 36] + carry_array[0];
    tmp[idx + 36] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 37] + carry_array[1];
    tmp[idx + 37] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 38] + carry_array[2];
    tmp[idx + 38] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 39] + carry_array[3];
    tmp[idx + 39] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry11);
    carry += tmp[idx + 40] + carry_array[0];
    tmp[idx + 40] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 41] + carry_array[1];
    tmp[idx + 41] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 42] + carry_array[2];
    tmp[idx + 42] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 43] + carry_array[3];
    tmp[idx + 43] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry12);
    carry += tmp[idx + 44] + carry_array[0];
    tmp[idx + 44] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 45] + carry_array[1];
    tmp[idx + 45] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 46] + carry_array[2];
    tmp[idx + 46] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 47] + carry_array[3];
    tmp[idx + 47] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry13);
    carry += tmp[idx + 48] + carry_array[0];
    tmp[idx + 48] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 49] + carry_array[1];
    tmp[idx + 49] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 50] + carry_array[2];
    tmp[idx + 50] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 51] + carry_array[3];
    tmp[idx + 51] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry14);
    carry += tmp[idx + 52] + carry_array[0];
    tmp[idx + 52] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 53] + carry_array[1];
    tmp[idx + 53] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 54] + carry_array[2];
    tmp[idx + 54] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 55] + carry_array[3];
    tmp[idx + 55] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry15);
    carry += tmp[idx + 56] + carry_array[0];
    tmp[idx + 56] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 57] + carry_array[1];
    tmp[idx + 57] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 58] + carry_array[2];
    tmp[idx + 58] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 59] + carry_array[3];
    tmp[idx + 59] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;

    _mm256_storeu_si256(carry_array_addr, carry16);
    carry += tmp[idx + 60] + carry_array[0];
    tmp[idx + 60] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 61] + carry_array[1];
    tmp[idx + 61] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 62] + carry_array[2];
    tmp[idx + 62] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
    carry += tmp[idx + 63] + carry_array[3];
    tmp[idx + 63] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_quad_double(BigInteger *left,
                                              const BigInteger right) {
  unsigned long *tmp = (unsigned long *)calloc(16, 8);
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);

  __m256i carry1 = _mm256_set1_epi64x(0);
  __m256i carry2 = _mm256_set1_epi64x(0);
  __m256i carry3 = _mm256_set1_epi64x(0);
  __m256i carry4 = _mm256_set1_epi64x(0);
  __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits));
  __m256i ro2 = _mm256_loadu_si256((const __m256i *)(right.data.bits + 4));

  __m256i lo1, lo2, lo3, lo4, lo5, lo6, lo7, lo8;
  __m256i mul_res1, mul_res2;
  __m256i mul_res3, mul_res4;
  __m256i mul_res5, mul_res6;
  __m256i mul_res7, mul_res8;
  __m256i mul_res9, mul_res10;
  __m256i mul_res11, mul_res12;
  __m256i mul_res13, mul_res14;
  __m256i mul_res15, mul_res16;

  lo1 = _mm256_set1_epi64x(left->data.bits[0]);
  lo2 = _mm256_set1_epi64x(left->data.bits[1]);
  lo3 = _mm256_set1_epi64x(left->data.bits[2]);
  lo4 = _mm256_set1_epi64x(left->data.bits[3]);
  lo5 = _mm256_set1_epi64x(left->data.bits[4]);
  lo6 = _mm256_set1_epi64x(left->data.bits[5]);
  lo7 = _mm256_set1_epi64x(left->data.bits[6]);
  lo8 = _mm256_set1_epi64x(left->data.bits[7]);

  mul_res1 = _mm256_mul_epu32(lo1, ro1);
  mul_res2 = _mm256_mul_epu32(lo1, ro2);
  mul_res3 = _mm256_mul_epu32(lo2, ro1);
  mul_res4 = _mm256_mul_epu32(lo2, ro2);
  mul_res5 = _mm256_mul_epu32(lo3, ro1);
  mul_res6 = _mm256_mul_epu32(lo3, ro2);
  mul_res7 = _mm256_mul_epu32(lo4, ro1);
  mul_res8 = _mm256_mul_epu32(lo4, ro2);
  mul_res9 = _mm256_mul_epu32(lo5, ro1);
  mul_res10 = _mm256_mul_epu32(lo5, ro2);
  mul_res11 = _mm256_mul_epu32(lo6, ro1);
  mul_res12 = _mm256_mul_epu32(lo6, ro2);
  mul_res13 = _mm256_mul_epu32(lo7, ro1);
  mul_res14 = _mm256_mul_epu32(lo7, ro2);
  mul_res15 = _mm256_mul_epu32(lo8, ro1);
  mul_res16 = _mm256_mul_epu32(lo8, ro2);

  carry1 = _mm256_add_epi64(carry1, mul_res1);
  carry2 = _mm256_add_epi64(carry2, mul_res2);
  carry3 = _mm256_add_epi64(carry3, mul_res9);
  carry4 = _mm256_add_epi64(carry4, mul_res10);
  carry1 = _mm256_add_epi64(carry1, _mm256_loadu_si256((const __m256i *)(tmp)));
  carry2 =
      _mm256_add_epi64(carry2, _mm256_loadu_si256((const __m256i *)(tmp + 4)));
  carry3 = _mm256_add_epi64(carry3, _mm256_and_si256(carry2, bit_maskv));
  carry4 =
      _mm256_add_epi64(carry4, _mm256_loadu_si256((const __m256i *)(tmp + 8)));
  _mm256_storeu_si256((__m256i *)(tmp), _mm256_and_si256(carry1, bit_maskv));
  _mm256_storeu_si256((__m256i *)(tmp + 4),
                      _mm256_and_si256(carry3, bit_maskv));
  _mm256_storeu_si256((__m256i *)(tmp + 8),
                      _mm256_and_si256(carry4, bit_maskv));
  carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
  carry2 = _mm256_srli_epi64(carry2, UINT_NUM_BITS);
  carry3 = _mm256_srli_epi64(carry3, UINT_NUM_BITS);
  carry4 = _mm256_srli_epi64(carry4, UINT_NUM_BITS);

  carry1 = _mm256_add_epi64(carry1, mul_res3);
  carry2 = _mm256_add_epi64(carry2, mul_res4);
  carry3 = _mm256_add_epi64(carry3, mul_res11);
  carry4 = _mm256_add_epi64(carry4, mul_res12);
  carry1 =
      _mm256_add_epi64(carry1, _mm256_loadu_si256((const __m256i *)(tmp + 1)));
  carry2 =
      _mm256_add_epi64(carry2, _mm256_loadu_si256((const __m256i *)(tmp + 5)));
  carry3 = _mm256_add_epi64(carry3, _mm256_and_si256(carry2, bit_maskv));
  carry4 =
      _mm256_add_epi64(carry4, _mm256_loadu_si256((const __m256i *)(tmp + 9)));
  _mm256_storeu_si256((__m256i *)(tmp + 1),
                      _mm256_and_si256(carry1, bit_maskv));
  _mm256_storeu_si256((__m256i *)(tmp + 5),
                      _mm256_and_si256(carry3, bit_maskv));
  _mm256_storeu_si256((__m256i *)(tmp + 9),
                      _mm256_and_si256(carry4, bit_maskv));
  carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
  carry2 = _mm256_srli_epi64(carry2, UINT_NUM_BITS);
  carry3 = _mm256_srli_epi64(carry3, UINT_NUM_BITS);
  carry4 = _mm256_srli_epi64(carry4, UINT_NUM_BITS);

  carry1 = _mm256_add_epi64(carry1, mul_res5);
  carry2 = _mm256_add_epi64(carry2, mul_res6);
  carry3 = _mm256_add_epi64(carry3, mul_res13);
  carry4 = _mm256_add_epi64(carry4, mul_res14);
  carry1 =
      _mm256_add_epi64(carry1, _mm256_loadu_si256((const __m256i *)(tmp + 2)));
  carry2 =
      _mm256_add_epi64(carry2, _mm256_loadu_si256((const __m256i *)(tmp + 6)));
  carry3 = _mm256_add_epi64(carry3, _mm256_and_si256(carry2, bit_maskv));
  carry4 =
      _mm256_add_epi64(carry4, _mm256_loadu_si256((const __m256i *)(tmp + 10)));
  _mm256_storeu_si256((__m256i *)(tmp + 2),
                      _mm256_and_si256(carry1, bit_maskv));
  _mm256_storeu_si256((__m256i *)(tmp + 6),
                      _mm256_and_si256(carry3, bit_maskv));
  _mm256_storeu_si256((__m256i *)(tmp + 10),
                      _mm256_and_si256(carry4, bit_maskv));
  carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
  carry2 = _mm256_srli_epi64(carry2, UINT_NUM_BITS);
  carry3 = _mm256_srli_epi64(carry3, UINT_NUM_BITS);
  carry4 = _mm256_srli_epi64(carry4, UINT_NUM_BITS);

  carry1 = _mm256_add_epi64(carry1, mul_res7);
  carry2 = _mm256_add_epi64(carry2, mul_res8);
  carry3 = _mm256_add_epi64(carry3, mul_res15);
  carry4 = _mm256_add_epi64(carry4, mul_res16);
  carry1 =
      _mm256_add_epi64(carry1, _mm256_loadu_si256((const __m256i *)(tmp + 3)));
  carry2 =
      _mm256_add_epi64(carry2, _mm256_loadu_si256((const __m256i *)(tmp + 7)));
  carry3 = _mm256_add_epi64(carry3, _mm256_and_si256(carry2, bit_maskv));
  carry4 =
      _mm256_add_epi64(carry4, _mm256_loadu_si256((const __m256i *)(tmp + 11)));
  _mm256_storeu_si256((__m256i *)(tmp + 3),
                      _mm256_and_si256(carry1, bit_maskv));
  _mm256_storeu_si256((__m256i *)(tmp + 7),
                      _mm256_and_si256(carry3, bit_maskv));
  _mm256_storeu_si256((__m256i *)(tmp + 11),
                      _mm256_and_si256(carry4, bit_maskv));
  carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
  carry2 = _mm256_srli_epi64(carry2, UINT_NUM_BITS);
  carry3 = _mm256_srli_epi64(carry3, UINT_NUM_BITS);
  carry4 = _mm256_srli_epi64(carry4, UINT_NUM_BITS);

  unsigned long carry = 0;
  unsigned long carry_array1[] = {0, 0, 0, 0};
  unsigned long carry_array2[] = {0, 0, 0, 0};
  unsigned long carry_array3[] = {0, 0, 0, 0};
  __m256i *carry_array_addr1 = (__m256i *)carry_array1;
  __m256i *carry_array_addr2 = (__m256i *)carry_array2;
  __m256i *carry_array_addr3 = (__m256i *)carry_array3;
  carry2 = _mm256_add_epi64(carry2, carry3);
  _mm256_storeu_si256(carry_array_addr1, carry1);
  _mm256_storeu_si256(carry_array_addr2, carry2);
  _mm256_storeu_si256(carry_array_addr3, carry4);

  carry += tmp[4] + carry_array1[0];
  tmp[4] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;
  carry += tmp[5] + carry_array1[1];
  tmp[5] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;
  carry += tmp[6] + carry_array1[2];
  tmp[6] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;
  carry += tmp[7] + carry_array1[3];
  tmp[7] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;

  carry += tmp[8] + carry_array2[0];
  tmp[8] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;
  carry += tmp[9] + carry_array2[1];
  tmp[9] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;
  carry += tmp[10] + carry_array2[2];
  tmp[10] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;
  carry += tmp[11] + carry_array2[3];
  tmp[11] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;

  carry += tmp[12] + carry_array3[0];
  tmp[12] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;
  carry += tmp[13] + carry_array3[1];
  tmp[13] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;
  carry += tmp[14] + carry_array3[2];
  tmp[14] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;
  carry += tmp[15] + carry_array3[3];
  tmp[15] = carry & bit_mask;
  carry >>= UINT_NUM_BITS;

  left->sign *= right.sign;
  left->data.size = 8;
  int offset = 8;
  for (int i = 15; i >= 8; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, 64);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_1x(
    BigInteger *left, const BigInteger right, const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left_size - precision;
  int offset_right = right_size - precision;
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);
  int j = offset_right;
  for (; j < right_size - 63; j += 64) {
    __m256i carry1 = _mm256_set1_epi64x(0);
    __m256i carry3 = _mm256_set1_epi64x(0);
    __m256i carry5 = _mm256_set1_epi64x(0);
    __m256i carry7 = _mm256_set1_epi64x(0);
    __m256i carry9 = _mm256_set1_epi64x(0);
    __m256i carry11 = _mm256_set1_epi64x(0);
    __m256i carry13 = _mm256_set1_epi64x(0);
    __m256i carry15 = _mm256_set1_epi64x(0);
    __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits + j));
    __m256i ro2 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 4));
    __m256i ro3 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 8));
    __m256i ro4 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 12));
    __m256i ro5 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 16));
    __m256i ro6 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 20));
    __m256i ro7 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 24));
    __m256i ro8 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 28));
    __m256i ro9 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 32));
    __m256i ro10 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 36));
    __m256i ro11 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 40));
    __m256i ro12 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 44));
    __m256i ro13 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 48));
    __m256i ro14 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 52));
    __m256i ro15 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 56));
    __m256i ro16 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 60));
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      __m256i lo = _mm256_set1_epi64x(left->data.bits[i]);

      __m256i mul_res1 = _mm256_mul_epu32(lo, ro1);
      __m256i mul_res2 = _mm256_mul_epu32(lo, ro2);
      __m256i mul_res3 = _mm256_mul_epu32(lo, ro3);
      __m256i mul_res4 = _mm256_mul_epu32(lo, ro4);
      __m256i mul_res5 = _mm256_mul_epu32(lo, ro5);
      __m256i mul_res6 = _mm256_mul_epu32(lo, ro6);
      __m256i mul_res7 = _mm256_mul_epu32(lo, ro7);
      __m256i mul_res8 = _mm256_mul_epu32(lo, ro8);
      __m256i mul_res9 = _mm256_mul_epu32(lo, ro9);
      __m256i mul_res10 = _mm256_mul_epu32(lo, ro10);
      __m256i mul_res11 = _mm256_mul_epu32(lo, ro11);
      __m256i mul_res12 = _mm256_mul_epu32(lo, ro12);
      __m256i mul_res13 = _mm256_mul_epu32(lo, ro13);
      __m256i mul_res14 = _mm256_mul_epu32(lo, ro14);
      __m256i mul_res15 = _mm256_mul_epu32(lo, ro15);
      __m256i mul_res16 = _mm256_mul_epu32(lo, ro16);

      __m256i carry2 = mul_res2;
      __m256i carry4 = mul_res4;
      __m256i carry6 = mul_res6;
      __m256i carry8 = mul_res8;
      __m256i carry10 = mul_res10;
      __m256i carry12 = mul_res12;
      __m256i carry14 = mul_res14;
      __m256i carry16 = mul_res16;

      carry1 = _mm256_add_epi64(carry1, mul_res1);
      carry3 = _mm256_add_epi64(carry3, mul_res3);
      carry5 = _mm256_add_epi64(carry5, mul_res5);
      carry7 = _mm256_add_epi64(carry7, mul_res7);
      carry9 = _mm256_add_epi64(carry9, mul_res9);
      carry11 = _mm256_add_epi64(carry11, mul_res11);
      carry13 = _mm256_add_epi64(carry13, mul_res13);
      carry15 = _mm256_add_epi64(carry15, mul_res15);
      carry1 = _mm256_add_epi64(
          carry1, _mm256_loadu_si256((const __m256i *)(tmp + idx)));
      carry2 = _mm256_add_epi64(
          carry2, _mm256_loadu_si256((const __m256i *)(tmp + idx + 4)));
      carry3 = _mm256_add_epi64(
          carry3, _mm256_loadu_si256((const __m256i *)(tmp + idx + 8)));
      carry4 = _mm256_add_epi64(
          carry4, _mm256_loadu_si256((const __m256i *)(tmp + idx + 12)));
      carry5 = _mm256_add_epi64(
          carry5, _mm256_loadu_si256((const __m256i *)(tmp + idx + 16)));
      carry6 = _mm256_add_epi64(
          carry6, _mm256_loadu_si256((const __m256i *)(tmp + idx + 20)));
      carry7 = _mm256_add_epi64(
          carry7, _mm256_loadu_si256((const __m256i *)(tmp + idx + 24)));
      carry8 = _mm256_add_epi64(
          carry8, _mm256_loadu_si256((const __m256i *)(tmp + idx + 28)));
      carry9 = _mm256_add_epi64(
          carry9, _mm256_loadu_si256((const __m256i *)(tmp + idx + 32)));
      carry10 = _mm256_add_epi64(
          carry10, _mm256_loadu_si256((const __m256i *)(tmp + idx + 36)));
      carry11 = _mm256_add_epi64(
          carry11, _mm256_loadu_si256((const __m256i *)(tmp + idx + 40)));
      carry12 = _mm256_add_epi64(
          carry12, _mm256_loadu_si256((const __m256i *)(tmp + idx + 44)));
      carry13 = _mm256_add_epi64(
          carry13, _mm256_loadu_si256((const __m256i *)(tmp + idx + 48)));
      carry14 = _mm256_add_epi64(
          carry14, _mm256_loadu_si256((const __m256i *)(tmp + idx + 52)));
      carry15 = _mm256_add_epi64(
          carry15, _mm256_loadu_si256((const __m256i *)(tmp + idx + 56)));
      carry16 = _mm256_add_epi64(
          carry16, _mm256_loadu_si256((const __m256i *)(tmp + idx + 60)));

      _mm256_storeu_si256((__m256i *)(tmp + idx),
                          _mm256_and_si256(carry1, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 8),
                          _mm256_and_si256(carry3, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 16),
                          _mm256_and_si256(carry5, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 24),
                          _mm256_and_si256(carry7, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 32),
                          _mm256_and_si256(carry9, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 40),
                          _mm256_and_si256(carry11, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 48),
                          _mm256_and_si256(carry13, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 56),
                          _mm256_and_si256(carry15, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 4), carry2);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 12), carry4);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 20), carry6);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 28), carry8);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 36), carry10);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 44), carry12);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 52), carry14);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 60), carry16);
      carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
      carry3 = _mm256_srli_epi64(carry3, UINT_NUM_BITS);
      carry5 = _mm256_srli_epi64(carry5, UINT_NUM_BITS);
      carry7 = _mm256_srli_epi64(carry7, UINT_NUM_BITS);
      carry9 = _mm256_srli_epi64(carry9, UINT_NUM_BITS);
      carry11 = _mm256_srli_epi64(carry11, UINT_NUM_BITS);
      carry13 = _mm256_srli_epi64(carry13, UINT_NUM_BITS);
      carry15 = _mm256_srli_epi64(carry15, UINT_NUM_BITS);
    }

    int idx = j - offset_right + precision;
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx),
        _mm256_add_epi64(carry1,
                         _mm256_loadu_si256((const __m256i *)(tmp + idx))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 8),
        _mm256_add_epi64(carry3,
                         _mm256_loadu_si256((const __m256i *)(tmp + idx + 8))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 16),
        _mm256_add_epi64(
            carry5, _mm256_loadu_si256((const __m256i *)(tmp + idx + 16))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 24),
        _mm256_add_epi64(
            carry7, _mm256_loadu_si256((const __m256i *)(tmp + idx + 24))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 32),
        _mm256_add_epi64(
            carry9, _mm256_loadu_si256((const __m256i *)(tmp + idx + 32))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 40),
        _mm256_add_epi64(
            carry11, _mm256_loadu_si256((const __m256i *)(tmp + idx + 40))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 48),
        _mm256_add_epi64(
            carry13, _mm256_loadu_si256((const __m256i *)(tmp + idx + 48))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 56),
        _mm256_add_epi64(
            carry15, _mm256_loadu_si256((const __m256i *)(tmp + idx + 56))));
  }

  unsigned long carry = 0;
  for (int i = 0; i < 2 * precision; i++) {
    carry += tmp[i];
    tmp[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_2x(
    BigInteger *left, const BigInteger right, const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left_size - precision;
  int offset_right = right_size - precision;
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);
  int j = offset_right;
  for (; j < right_size - 63; j += 64) {
    __m256i carry1 = _mm256_set1_epi64x(0);
    __m256i carry5 = _mm256_set1_epi64x(0);
    __m256i carry9 = _mm256_set1_epi64x(0);
    __m256i carry13 = _mm256_set1_epi64x(0);
    __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits + j));
    __m256i ro2 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 4));
    __m256i ro3 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 8));
    __m256i ro4 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 12));
    __m256i ro5 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 16));
    __m256i ro6 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 20));
    __m256i ro7 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 24));
    __m256i ro8 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 28));
    __m256i ro9 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 32));
    __m256i ro10 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 36));
    __m256i ro11 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 40));
    __m256i ro12 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 44));
    __m256i ro13 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 48));
    __m256i ro14 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 52));
    __m256i ro15 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 56));
    __m256i ro16 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 60));
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      __m256i lo = _mm256_set1_epi64x(left->data.bits[i]);

      __m256i mul_res1 = _mm256_mul_epu32(lo, ro1);
      __m256i mul_res2 = _mm256_mul_epu32(lo, ro2);
      __m256i mul_res3 = _mm256_mul_epu32(lo, ro3);
      __m256i mul_res4 = _mm256_mul_epu32(lo, ro4);
      __m256i mul_res5 = _mm256_mul_epu32(lo, ro5);
      __m256i mul_res6 = _mm256_mul_epu32(lo, ro6);
      __m256i mul_res7 = _mm256_mul_epu32(lo, ro7);
      __m256i mul_res8 = _mm256_mul_epu32(lo, ro8);
      __m256i mul_res9 = _mm256_mul_epu32(lo, ro9);
      __m256i mul_res10 = _mm256_mul_epu32(lo, ro10);
      __m256i mul_res11 = _mm256_mul_epu32(lo, ro11);
      __m256i mul_res12 = _mm256_mul_epu32(lo, ro12);
      __m256i mul_res13 = _mm256_mul_epu32(lo, ro13);
      __m256i mul_res14 = _mm256_mul_epu32(lo, ro14);
      __m256i mul_res15 = _mm256_mul_epu32(lo, ro15);
      __m256i mul_res16 = _mm256_mul_epu32(lo, ro16);

      __m256i carry2 = mul_res2;
      __m256i carry3 = mul_res3;
      __m256i carry4 = mul_res4;
      __m256i carry6 = mul_res6;
      __m256i carry7 = mul_res7;
      __m256i carry8 = mul_res8;
      __m256i carry10 = mul_res10;
      __m256i carry11 = mul_res11;
      __m256i carry12 = mul_res12;
      __m256i carry14 = mul_res14;
      __m256i carry15 = mul_res15;
      __m256i carry16 = mul_res16;

      carry1 = _mm256_add_epi64(carry1, mul_res1);
      carry5 = _mm256_add_epi64(carry5, mul_res5);
      carry9 = _mm256_add_epi64(carry9, mul_res9);
      carry13 = _mm256_add_epi64(carry13, mul_res13);
      carry1 = _mm256_add_epi64(
          carry1, _mm256_loadu_si256((const __m256i *)(tmp + idx)));
      carry2 = _mm256_add_epi64(
          carry2, _mm256_loadu_si256((const __m256i *)(tmp + idx + 4)));
      carry3 = _mm256_add_epi64(
          carry3, _mm256_loadu_si256((const __m256i *)(tmp + idx + 8)));
      carry4 = _mm256_add_epi64(
          carry4, _mm256_loadu_si256((const __m256i *)(tmp + idx + 12)));
      carry5 = _mm256_add_epi64(
          carry5, _mm256_loadu_si256((const __m256i *)(tmp + idx + 16)));
      carry6 = _mm256_add_epi64(
          carry6, _mm256_loadu_si256((const __m256i *)(tmp + idx + 20)));
      carry7 = _mm256_add_epi64(
          carry7, _mm256_loadu_si256((const __m256i *)(tmp + idx + 24)));
      carry8 = _mm256_add_epi64(
          carry8, _mm256_loadu_si256((const __m256i *)(tmp + idx + 28)));
      carry9 = _mm256_add_epi64(
          carry9, _mm256_loadu_si256((const __m256i *)(tmp + idx + 32)));
      carry10 = _mm256_add_epi64(
          carry10, _mm256_loadu_si256((const __m256i *)(tmp + idx + 36)));
      carry11 = _mm256_add_epi64(
          carry11, _mm256_loadu_si256((const __m256i *)(tmp + idx + 40)));
      carry12 = _mm256_add_epi64(
          carry12, _mm256_loadu_si256((const __m256i *)(tmp + idx + 44)));
      carry13 = _mm256_add_epi64(
          carry13, _mm256_loadu_si256((const __m256i *)(tmp + idx + 48)));
      carry14 = _mm256_add_epi64(
          carry14, _mm256_loadu_si256((const __m256i *)(tmp + idx + 52)));
      carry15 = _mm256_add_epi64(
          carry15, _mm256_loadu_si256((const __m256i *)(tmp + idx + 56)));
      carry16 = _mm256_add_epi64(
          carry16, _mm256_loadu_si256((const __m256i *)(tmp + idx + 60)));

      _mm256_storeu_si256((__m256i *)(tmp + idx),
                          _mm256_and_si256(carry1, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 16),
                          _mm256_and_si256(carry5, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 32),
                          _mm256_and_si256(carry9, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 48),
                          _mm256_and_si256(carry13, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 4), carry2);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 8), carry3);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 12), carry4);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 20), carry6);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 24), carry7);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 28), carry8);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 36), carry10);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 40), carry11);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 44), carry12);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 52), carry14);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 56), carry15);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 60), carry16);
      carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
      carry5 = _mm256_srli_epi64(carry5, UINT_NUM_BITS);
      carry9 = _mm256_srli_epi64(carry9, UINT_NUM_BITS);
      carry13 = _mm256_srli_epi64(carry13, UINT_NUM_BITS);
    }

    int idx = j - offset_right + precision;
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx),
        _mm256_add_epi64(carry1,
                         _mm256_loadu_si256((const __m256i *)(tmp + idx))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 16),
        _mm256_add_epi64(
            carry5, _mm256_loadu_si256((const __m256i *)(tmp + idx + 16))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 32),
        _mm256_add_epi64(
            carry9, _mm256_loadu_si256((const __m256i *)(tmp + idx + 32))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 48),
        _mm256_add_epi64(
            carry13, _mm256_loadu_si256((const __m256i *)(tmp + idx + 48))));
  }

  unsigned long carry = 0;
  for (int i = 0; i < 2 * precision; i++) {
    carry += tmp[i];
    tmp[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_4x(
    BigInteger *left, const BigInteger right, const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left_size - precision;
  int offset_right = right_size - precision;
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);
  int j = offset_right;
  for (; j < right_size - 63; j += 64) {
    __m256i carry1 = _mm256_set1_epi64x(0);
    __m256i carry9 = _mm256_set1_epi64x(0);
    __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits + j));
    __m256i ro2 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 4));
    __m256i ro3 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 8));
    __m256i ro4 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 12));
    __m256i ro5 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 16));
    __m256i ro6 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 20));
    __m256i ro7 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 24));
    __m256i ro8 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 28));
    __m256i ro9 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 32));
    __m256i ro10 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 36));
    __m256i ro11 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 40));
    __m256i ro12 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 44));
    __m256i ro13 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 48));
    __m256i ro14 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 52));
    __m256i ro15 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 56));
    __m256i ro16 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 60));
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      __m256i lo = _mm256_set1_epi64x(left->data.bits[i]);

      __m256i mul_res1 = _mm256_mul_epu32(lo, ro1);
      __m256i mul_res2 = _mm256_mul_epu32(lo, ro2);
      __m256i mul_res3 = _mm256_mul_epu32(lo, ro3);
      __m256i mul_res4 = _mm256_mul_epu32(lo, ro4);
      __m256i mul_res5 = _mm256_mul_epu32(lo, ro5);
      __m256i mul_res6 = _mm256_mul_epu32(lo, ro6);
      __m256i mul_res7 = _mm256_mul_epu32(lo, ro7);
      __m256i mul_res8 = _mm256_mul_epu32(lo, ro8);
      __m256i mul_res9 = _mm256_mul_epu32(lo, ro9);
      __m256i mul_res10 = _mm256_mul_epu32(lo, ro10);
      __m256i mul_res11 = _mm256_mul_epu32(lo, ro11);
      __m256i mul_res12 = _mm256_mul_epu32(lo, ro12);
      __m256i mul_res13 = _mm256_mul_epu32(lo, ro13);
      __m256i mul_res14 = _mm256_mul_epu32(lo, ro14);
      __m256i mul_res15 = _mm256_mul_epu32(lo, ro15);
      __m256i mul_res16 = _mm256_mul_epu32(lo, ro16);

      __m256i carry2 = mul_res2;
      __m256i carry3 = mul_res3;
      __m256i carry4 = mul_res4;
      __m256i carry5 = mul_res5;
      __m256i carry6 = mul_res6;
      __m256i carry7 = mul_res7;
      __m256i carry8 = mul_res8;
      __m256i carry10 = mul_res10;
      __m256i carry11 = mul_res11;
      __m256i carry12 = mul_res12;
      __m256i carry13 = mul_res13;
      __m256i carry14 = mul_res14;
      __m256i carry15 = mul_res15;
      __m256i carry16 = mul_res16;

      carry1 = _mm256_add_epi64(carry1, mul_res1);
      carry9 = _mm256_add_epi64(carry9, mul_res9);
      carry1 = _mm256_add_epi64(
          carry1, _mm256_loadu_si256((const __m256i *)(tmp + idx)));
      carry2 = _mm256_add_epi64(
          carry2, _mm256_loadu_si256((const __m256i *)(tmp + idx + 4)));
      carry3 = _mm256_add_epi64(
          carry3, _mm256_loadu_si256((const __m256i *)(tmp + idx + 8)));
      carry4 = _mm256_add_epi64(
          carry4, _mm256_loadu_si256((const __m256i *)(tmp + idx + 12)));
      carry5 = _mm256_add_epi64(
          carry5, _mm256_loadu_si256((const __m256i *)(tmp + idx + 16)));
      carry6 = _mm256_add_epi64(
          carry6, _mm256_loadu_si256((const __m256i *)(tmp + idx + 20)));
      carry7 = _mm256_add_epi64(
          carry7, _mm256_loadu_si256((const __m256i *)(tmp + idx + 24)));
      carry8 = _mm256_add_epi64(
          carry8, _mm256_loadu_si256((const __m256i *)(tmp + idx + 28)));
      carry9 = _mm256_add_epi64(
          carry9, _mm256_loadu_si256((const __m256i *)(tmp + idx + 32)));
      carry10 = _mm256_add_epi64(
          carry10, _mm256_loadu_si256((const __m256i *)(tmp + idx + 36)));
      carry11 = _mm256_add_epi64(
          carry11, _mm256_loadu_si256((const __m256i *)(tmp + idx + 40)));
      carry12 = _mm256_add_epi64(
          carry12, _mm256_loadu_si256((const __m256i *)(tmp + idx + 44)));
      carry13 = _mm256_add_epi64(
          carry13, _mm256_loadu_si256((const __m256i *)(tmp + idx + 48)));
      carry14 = _mm256_add_epi64(
          carry14, _mm256_loadu_si256((const __m256i *)(tmp + idx + 52)));
      carry15 = _mm256_add_epi64(
          carry15, _mm256_loadu_si256((const __m256i *)(tmp + idx + 56)));
      carry16 = _mm256_add_epi64(
          carry16, _mm256_loadu_si256((const __m256i *)(tmp + idx + 60)));

      _mm256_storeu_si256((__m256i *)(tmp + idx),
                          _mm256_and_si256(carry1, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 32),
                          _mm256_and_si256(carry9, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 4), carry2);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 8), carry3);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 12), carry4);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 16), carry5);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 20), carry6);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 24), carry7);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 28), carry8);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 36), carry10);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 40), carry11);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 44), carry12);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 48), carry13);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 52), carry14);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 56), carry15);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 60), carry16);
      carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
      carry9 = _mm256_srli_epi64(carry9, UINT_NUM_BITS);
    }

    int idx = j - offset_right + precision;
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx),
        _mm256_add_epi64(carry1,
                         _mm256_loadu_si256((const __m256i *)(tmp + idx))));
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx + 32),
        _mm256_add_epi64(
            carry9, _mm256_loadu_si256((const __m256i *)(tmp + idx + 32))));
  }

  unsigned long carry = 0;
  for (int i = 0; i < 2 * precision; i++) {
    carry += tmp[i];
    tmp[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

void big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_8x(
    BigInteger *left, const BigInteger right, const int precision) {
  int left_size = left->data.size;
  int right_size = right.data.size;
  unsigned long *tmp = (unsigned long *)calloc(2 * precision, 8);
  int offset_left = left_size - precision;
  int offset_right = right_size - precision;
  __m256i bit_maskv = _mm256_set1_epi64x(bit_mask);
  int j = offset_right;
  for (; j < right_size - 63; j += 64) {
    __m256i carry1 = _mm256_set1_epi64x(0);
    __m256i ro1 = _mm256_loadu_si256((const __m256i *)(right.data.bits + j));
    __m256i ro2 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 4));
    __m256i ro3 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 8));
    __m256i ro4 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 12));
    __m256i ro5 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 16));
    __m256i ro6 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 20));
    __m256i ro7 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 24));
    __m256i ro8 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 28));
    __m256i ro9 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 32));
    __m256i ro10 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 36));
    __m256i ro11 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 40));
    __m256i ro12 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 44));
    __m256i ro13 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 48));
    __m256i ro14 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 52));
    __m256i ro15 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 56));
    __m256i ro16 =
        _mm256_loadu_si256((const __m256i *)(right.data.bits + j + 60));
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      __m256i lo = _mm256_set1_epi64x(left->data.bits[i]);

      __m256i mul_res1 = _mm256_mul_epu32(lo, ro1);
      __m256i mul_res2 = _mm256_mul_epu32(lo, ro2);
      __m256i mul_res3 = _mm256_mul_epu32(lo, ro3);
      __m256i mul_res4 = _mm256_mul_epu32(lo, ro4);
      __m256i mul_res5 = _mm256_mul_epu32(lo, ro5);
      __m256i mul_res6 = _mm256_mul_epu32(lo, ro6);
      __m256i mul_res7 = _mm256_mul_epu32(lo, ro7);
      __m256i mul_res8 = _mm256_mul_epu32(lo, ro8);
      __m256i mul_res9 = _mm256_mul_epu32(lo, ro9);
      __m256i mul_res10 = _mm256_mul_epu32(lo, ro10);
      __m256i mul_res11 = _mm256_mul_epu32(lo, ro11);
      __m256i mul_res12 = _mm256_mul_epu32(lo, ro12);
      __m256i mul_res13 = _mm256_mul_epu32(lo, ro13);
      __m256i mul_res14 = _mm256_mul_epu32(lo, ro14);
      __m256i mul_res15 = _mm256_mul_epu32(lo, ro15);
      __m256i mul_res16 = _mm256_mul_epu32(lo, ro16);

      __m256i carry2 = mul_res2;
      __m256i carry3 = mul_res3;
      __m256i carry4 = mul_res4;
      __m256i carry5 = mul_res5;
      __m256i carry6 = mul_res6;
      __m256i carry7 = mul_res7;
      __m256i carry8 = mul_res8;
      __m256i carry9 = mul_res9;
      __m256i carry10 = mul_res10;
      __m256i carry11 = mul_res11;
      __m256i carry12 = mul_res12;
      __m256i carry13 = mul_res13;
      __m256i carry14 = mul_res14;
      __m256i carry15 = mul_res15;
      __m256i carry16 = mul_res16;

      carry1 = _mm256_add_epi64(carry1, mul_res1);
      carry1 = _mm256_add_epi64(
          carry1, _mm256_loadu_si256((const __m256i *)(tmp + idx)));
      carry2 = _mm256_add_epi64(
          carry2, _mm256_loadu_si256((const __m256i *)(tmp + idx + 4)));
      carry3 = _mm256_add_epi64(
          carry3, _mm256_loadu_si256((const __m256i *)(tmp + idx + 8)));
      carry4 = _mm256_add_epi64(
          carry4, _mm256_loadu_si256((const __m256i *)(tmp + idx + 12)));
      carry5 = _mm256_add_epi64(
          carry5, _mm256_loadu_si256((const __m256i *)(tmp + idx + 16)));
      carry6 = _mm256_add_epi64(
          carry6, _mm256_loadu_si256((const __m256i *)(tmp + idx + 20)));
      carry7 = _mm256_add_epi64(
          carry7, _mm256_loadu_si256((const __m256i *)(tmp + idx + 24)));
      carry8 = _mm256_add_epi64(
          carry8, _mm256_loadu_si256((const __m256i *)(tmp + idx + 28)));
      carry9 = _mm256_add_epi64(
          carry9, _mm256_loadu_si256((const __m256i *)(tmp + idx + 32)));
      carry10 = _mm256_add_epi64(
          carry10, _mm256_loadu_si256((const __m256i *)(tmp + idx + 36)));
      carry11 = _mm256_add_epi64(
          carry11, _mm256_loadu_si256((const __m256i *)(tmp + idx + 40)));
      carry12 = _mm256_add_epi64(
          carry12, _mm256_loadu_si256((const __m256i *)(tmp + idx + 44)));
      carry13 = _mm256_add_epi64(
          carry13, _mm256_loadu_si256((const __m256i *)(tmp + idx + 48)));
      carry14 = _mm256_add_epi64(
          carry14, _mm256_loadu_si256((const __m256i *)(tmp + idx + 52)));
      carry15 = _mm256_add_epi64(
          carry15, _mm256_loadu_si256((const __m256i *)(tmp + idx + 56)));
      carry16 = _mm256_add_epi64(
          carry16, _mm256_loadu_si256((const __m256i *)(tmp + idx + 60)));

      _mm256_storeu_si256((__m256i *)(tmp + idx),
                          _mm256_and_si256(carry1, bit_maskv));
      _mm256_storeu_si256((__m256i *)(tmp + idx + 4), carry2);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 8), carry3);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 12), carry4);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 16), carry5);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 20), carry6);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 24), carry7);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 28), carry8);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 32), carry9);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 36), carry10);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 40), carry11);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 44), carry12);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 48), carry13);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 52), carry14);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 56), carry15);
      _mm256_storeu_si256((__m256i *)(tmp + idx + 60), carry16);
      carry1 = _mm256_srli_epi64(carry1, UINT_NUM_BITS);
    }

    int idx = j - offset_right + precision;
    _mm256_storeu_si256(
        (__m256i *)(tmp + idx),
        _mm256_add_epi64(carry1,
                         _mm256_loadu_si256((const __m256i *)(tmp + idx))));
  }

  unsigned long carry = 0;
  for (int i = 0; i < 2 * precision; i++) {
    carry += tmp[i];
    tmp[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }

  for (; j < right_size; j++) {
    unsigned long carry = 0;
    for (int i = offset_left; i < left_size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += left->data.bits[i] * right.data.bits[j] + tmp[idx];
      tmp[idx] = carry & bit_mask;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = carry & bit_mask;
  }

  left->sign *= right.sign;
  left->data.size = precision;
  int offset = precision;
  for (int i = 2 * precision - 1; i >= precision; i--) {
    if (tmp[i] != 0) {
      break;
    } else {
      offset--;
    }
  }
  memcpy(left->data.bits, tmp + offset, precision * 8);
  free(tmp); // release temporal variable
}

BigInteger big_integer_add_trailing_zeros(const BigInteger bi, int num) {
  BigInteger ans;
  ans.sign = bi.sign;
  ans.data.size = bi.data.size + num;
  ans.data.capacity = 2 * ans.data.size;
  ans.data.bits = (unsigned long *)calloc(ans.data.capacity, UINT_NUM_BYTES);
  memcpy(ans.data.bits + num, bi.data.bits, bi.data.size * UINT_NUM_BYTES);
  return ans;
}

void big_integer_add_trailing_zeros_inplace(BigInteger *bi, int num) {
  if (bi->data.capacity >= bi->data.size + num) {
    // no allocation
    for (int i = bi->data.size - 1; i >= 0; i--) {
      bi->data.bits[i + num] = bi->data.bits[i];
    }
    for (int i = 0; i < num; i++) {
      bi->data.bits[i] = 0;
    }
    bi->data.size += num;
  } else {
    // need allocation
    unsigned long *res =
        (unsigned long *)calloc(2 * (bi->data.size + num), UINT_NUM_BYTES);
    memcpy(res + num, bi->data.bits, bi->data.size * UINT_NUM_BYTES);
    free(bi->data.bits);
    bi->data.bits = res;
    bi->data.size += num;
    bi->data.capacity = 2 * bi->data.size;
  }
}

void big_integer_sum_2(const BigInteger op1, const BigInteger op2,
                       const int precision, BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;

  unsigned long sum = 0;
  for (int i = 0; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_3(const BigInteger op1, const BigInteger op2,
                       const BigInteger op3, const int precision,
                       BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;

  unsigned long sum = 0;
  for (int i = 0; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_3_unfold_1x(const BigInteger op1, const BigInteger op2,
                                 const BigInteger op3, const int precision,
                                 BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;

  __m256i sum1;

  int i = 0;
  for (; i < precision - 3; i += 4) {
    sum1 = _mm256_loadu_si256((const __m256i *)(data1 + i));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data2 + i)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data3 + i)));

    _mm256_storeu_si256((__m256i *)(res_data + i), sum1);
  }

  unsigned long carry = 0;
  for (int i = 0; i < precision; i++) {
    carry += res_data[i];
    res_data[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }
  res_data[precision] += carry;

  unsigned long sum = 0;
  for (; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_3_unfold_2x(const BigInteger op1, const BigInteger op2,
                                 const BigInteger op3, const int precision,
                                 BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;

  __m256i sum1;
  __m256i sum2;

  int i = 0;
  for (; i < precision - 7; i += 8) {
    sum1 = _mm256_loadu_si256((const __m256i *)(data1 + i));
    sum2 = _mm256_loadu_si256((const __m256i *)(data1 + i + 4));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data2 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data2 + i + 4)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data3 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data3 + i + 4)));

    _mm256_storeu_si256((__m256i *)(res_data + i), sum1);
    _mm256_storeu_si256((__m256i *)(res_data + i + 4), sum2);
  }

  unsigned long carry = 0;
  for (int i = 0; i < precision; i++) {
    carry += res_data[i];
    res_data[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }
  res_data[precision] += carry;

  unsigned long sum = 0;
  for (; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_3_unfold_4x(const BigInteger op1, const BigInteger op2,
                                 const BigInteger op3, const int precision,
                                 BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;

  __m256i sum1;
  __m256i sum2;
  __m256i sum3;
  __m256i sum4;

  int i = 0;
  for (; i < precision - 15; i += 16) {
    sum1 = _mm256_loadu_si256((const __m256i *)(data1 + i));
    sum2 = _mm256_loadu_si256((const __m256i *)(data1 + i + 4));
    sum3 = _mm256_loadu_si256((const __m256i *)(data1 + i + 8));
    sum4 = _mm256_loadu_si256((const __m256i *)(data1 + i + 12));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data2 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data2 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data2 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data2 + i + 12)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data3 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data3 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data3 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data3 + i + 12)));

    _mm256_storeu_si256((__m256i *)(res_data + i), sum1);
    _mm256_storeu_si256((__m256i *)(res_data + i + 4), sum2);
    _mm256_storeu_si256((__m256i *)(res_data + i + 8), sum3);
    _mm256_storeu_si256((__m256i *)(res_data + i + 12), sum4);
  }

  unsigned long carry = 0;
  for (int i = 0; i < precision; i++) {
    carry += res_data[i];
    res_data[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }
  res_data[precision] += carry;

  unsigned long sum = 0;
  for (; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_3_unfold_8x(const BigInteger op1, const BigInteger op2,
                                 const BigInteger op3, const int precision,
                                 BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;

  __m256i sum1;
  __m256i sum2;
  __m256i sum3;
  __m256i sum4;
  __m256i sum5;
  __m256i sum6;
  __m256i sum7;
  __m256i sum8;

  int i = 0;
  for (; i < precision - 31; i += 32) {
    sum1 = _mm256_loadu_si256((const __m256i *)(data1 + i));
    sum2 = _mm256_loadu_si256((const __m256i *)(data1 + i + 4));
    sum3 = _mm256_loadu_si256((const __m256i *)(data1 + i + 8));
    sum4 = _mm256_loadu_si256((const __m256i *)(data1 + i + 12));
    sum5 = _mm256_loadu_si256((const __m256i *)(data1 + i + 16));
    sum6 = _mm256_loadu_si256((const __m256i *)(data1 + i + 20));
    sum7 = _mm256_loadu_si256((const __m256i *)(data1 + i + 24));
    sum8 = _mm256_loadu_si256((const __m256i *)(data1 + i + 28));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data2 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data2 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data2 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data2 + i + 12)));
    sum5 = _mm256_add_epi64(
        sum5, _mm256_loadu_si256((const __m256i *)(data2 + i + 16)));
    sum6 = _mm256_add_epi64(
        sum6, _mm256_loadu_si256((const __m256i *)(data2 + i + 20)));
    sum7 = _mm256_add_epi64(
        sum7, _mm256_loadu_si256((const __m256i *)(data2 + i + 24)));
    sum8 = _mm256_add_epi64(
        sum8, _mm256_loadu_si256((const __m256i *)(data2 + i + 28)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data3 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data3 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data3 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data3 + i + 12)));
    sum5 = _mm256_add_epi64(
        sum5, _mm256_loadu_si256((const __m256i *)(data3 + i + 16)));
    sum6 = _mm256_add_epi64(
        sum6, _mm256_loadu_si256((const __m256i *)(data3 + i + 20)));
    sum7 = _mm256_add_epi64(
        sum7, _mm256_loadu_si256((const __m256i *)(data3 + i + 24)));
    sum8 = _mm256_add_epi64(
        sum8, _mm256_loadu_si256((const __m256i *)(data3 + i + 28)));

    _mm256_storeu_si256((__m256i *)(res_data + i), sum1);
    _mm256_storeu_si256((__m256i *)(res_data + i + 4), sum2);
    _mm256_storeu_si256((__m256i *)(res_data + i + 8), sum3);
    _mm256_storeu_si256((__m256i *)(res_data + i + 12), sum4);
    _mm256_storeu_si256((__m256i *)(res_data + i + 16), sum5);
    _mm256_storeu_si256((__m256i *)(res_data + i + 20), sum6);
    _mm256_storeu_si256((__m256i *)(res_data + i + 24), sum7);
    _mm256_storeu_si256((__m256i *)(res_data + i + 28), sum8);
  }

  unsigned long carry = 0;
  for (int i = 0; i < precision; i++) {
    carry += res_data[i];
    res_data[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }
  res_data[precision] += carry;

  unsigned long sum = 0;
  for (; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_5(const BigInteger op1, const BigInteger op2,
                       const BigInteger op3, const BigInteger op4,
                       const BigInteger op5, const int precision,
                       BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;
  unsigned long *data5 = op5.data.bits;

  unsigned long sum = 0;
  for (int i = 0; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    sum += data5[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_5_unfold_1x(const BigInteger op1, const BigInteger op2,
                                 const BigInteger op3, const BigInteger op4,
                                 const BigInteger op5, const int precision,
                                 BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;
  unsigned long *data5 = op5.data.bits;

  __m256i sum1;

  int i = 0;
  for (; i < precision - 3; i += 4) {
    sum1 = _mm256_loadu_si256((const __m256i *)(data1 + i));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data2 + i)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data3 + i)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data4 + i)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data5 + i)));

    _mm256_storeu_si256((__m256i *)(res_data + i), sum1);
  }

  unsigned long carry = 0;
  for (int i = 0; i < precision; i++) {
    carry += res_data[i];
    res_data[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }
  res_data[precision] += carry;

  unsigned long sum = 0;
  for (; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    sum += data5[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_5_unfold_2x(const BigInteger op1, const BigInteger op2,
                                 const BigInteger op3, const BigInteger op4,
                                 const BigInteger op5, const int precision,
                                 BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;
  unsigned long *data5 = op5.data.bits;

  __m256i sum1;
  __m256i sum2;

  int i = 0;
  for (; i < precision - 7; i += 8) {
    sum1 = _mm256_loadu_si256((const __m256i *)(data1 + i));
    sum2 = _mm256_loadu_si256((const __m256i *)(data1 + i + 4));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data2 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data2 + i + 4)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data3 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data3 + i + 4)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data4 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data4 + i + 4)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data5 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data5 + i + 4)));

    _mm256_storeu_si256((__m256i *)(res_data + i), sum1);
    _mm256_storeu_si256((__m256i *)(res_data + i + 4), sum2);
  }

  unsigned long carry = 0;
  for (int i = 0; i < precision; i++) {
    carry += res_data[i];
    res_data[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }
  res_data[precision] += carry;

  unsigned long sum = 0;
  for (; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    sum += data5[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_5_unfold_4x(const BigInteger op1, const BigInteger op2,
                                 const BigInteger op3, const BigInteger op4,
                                 const BigInteger op5, const int precision,
                                 BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;
  unsigned long *data5 = op5.data.bits;

  __m256i sum1;
  __m256i sum2;
  __m256i sum3;
  __m256i sum4;

  int i = 0;
  for (; i < precision - 7; i += 8) {
    sum1 = _mm256_loadu_si256((const __m256i *)(data1 + i));
    sum2 = _mm256_loadu_si256((const __m256i *)(data1 + i + 4));
    sum3 = _mm256_loadu_si256((const __m256i *)(data1 + i + 8));
    sum4 = _mm256_loadu_si256((const __m256i *)(data1 + i + 12));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data2 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data2 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data2 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data2 + i + 12)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data3 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data3 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data3 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data3 + i + 12)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data4 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data4 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data4 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data4 + i + 12)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data5 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data5 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data5 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data5 + i + 12)));

    _mm256_storeu_si256((__m256i *)(res_data + i), sum1);
    _mm256_storeu_si256((__m256i *)(res_data + i + 4), sum2);
    _mm256_storeu_si256((__m256i *)(res_data + i + 8), sum3);
    _mm256_storeu_si256((__m256i *)(res_data + i + 12), sum4);
  }

  unsigned long carry = 0;
  for (int i = 0; i < precision; i++) {
    carry += res_data[i];
    res_data[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }
  res_data[precision] += carry;

  unsigned long sum = 0;
  for (; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    sum += data5[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_5_unfold_8x(const BigInteger op1, const BigInteger op2,
                                 const BigInteger op3, const BigInteger op4,
                                 const BigInteger op5, const int precision,
                                 BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;
  unsigned long *data5 = op5.data.bits;

  __m256i sum1;
  __m256i sum2;
  __m256i sum3;
  __m256i sum4;
  __m256i sum5;
  __m256i sum6;
  __m256i sum7;
  __m256i sum8;

  int i = 0;
  for (; i < precision - 7; i += 8) {
    sum1 = _mm256_loadu_si256((const __m256i *)(data1 + i));
    sum2 = _mm256_loadu_si256((const __m256i *)(data1 + i + 4));
    sum3 = _mm256_loadu_si256((const __m256i *)(data1 + i + 8));
    sum4 = _mm256_loadu_si256((const __m256i *)(data1 + i + 12));
    sum5 = _mm256_loadu_si256((const __m256i *)(data1 + i + 16));
    sum6 = _mm256_loadu_si256((const __m256i *)(data1 + i + 20));
    sum7 = _mm256_loadu_si256((const __m256i *)(data1 + i + 24));
    sum8 = _mm256_loadu_si256((const __m256i *)(data1 + i + 28));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data2 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data2 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data2 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data2 + i + 12)));
    sum5 = _mm256_add_epi64(
        sum5, _mm256_loadu_si256((const __m256i *)(data2 + i + 16)));
    sum6 = _mm256_add_epi64(
        sum6, _mm256_loadu_si256((const __m256i *)(data2 + i + 20)));
    sum7 = _mm256_add_epi64(
        sum7, _mm256_loadu_si256((const __m256i *)(data2 + i + 24)));
    sum8 = _mm256_add_epi64(
        sum8, _mm256_loadu_si256((const __m256i *)(data2 + i + 28)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data3 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data3 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data3 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data3 + i + 12)));
    sum5 = _mm256_add_epi64(
        sum5, _mm256_loadu_si256((const __m256i *)(data3 + i + 16)));
    sum6 = _mm256_add_epi64(
        sum6, _mm256_loadu_si256((const __m256i *)(data3 + i + 20)));
    sum7 = _mm256_add_epi64(
        sum7, _mm256_loadu_si256((const __m256i *)(data3 + i + 24)));
    sum8 = _mm256_add_epi64(
        sum8, _mm256_loadu_si256((const __m256i *)(data3 + i + 28)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data4 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data4 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data4 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data4 + i + 12)));
    sum5 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data4 + i + 16)));
    sum6 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data4 + i + 20)));
    sum7 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data4 + i + 24)));
    sum8 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data4 + i + 28)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data5 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data5 + i + 4)));
    sum3 = _mm256_add_epi64(
        sum3, _mm256_loadu_si256((const __m256i *)(data5 + i + 8)));
    sum4 = _mm256_add_epi64(
        sum4, _mm256_loadu_si256((const __m256i *)(data5 + i + 12)));
    sum5 = _mm256_add_epi64(
        sum5, _mm256_loadu_si256((const __m256i *)(data5 + i + 16)));
    sum6 = _mm256_add_epi64(
        sum6, _mm256_loadu_si256((const __m256i *)(data5 + i + 20)));
    sum7 = _mm256_add_epi64(
        sum7, _mm256_loadu_si256((const __m256i *)(data5 + i + 24)));
    sum8 = _mm256_add_epi64(
        sum8, _mm256_loadu_si256((const __m256i *)(data5 + i + 28)));

    _mm256_storeu_si256((__m256i *)(res_data + i), sum1);
    _mm256_storeu_si256((__m256i *)(res_data + i + 4), sum2);
    _mm256_storeu_si256((__m256i *)(res_data + i + 8), sum3);
    _mm256_storeu_si256((__m256i *)(res_data + i + 12), sum4);
    _mm256_storeu_si256((__m256i *)(res_data + i + 16), sum5);
    _mm256_storeu_si256((__m256i *)(res_data + i + 20), sum6);
    _mm256_storeu_si256((__m256i *)(res_data + i + 24), sum7);
    _mm256_storeu_si256((__m256i *)(res_data + i + 28), sum8);
  }

  unsigned long carry = 0;
  for (int i = 0; i < precision; i++) {
    carry += res_data[i];
    res_data[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }
  res_data[precision] += carry;

  unsigned long sum = 0;
  for (; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    sum += data5[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum(const BigInteger op[], int index_start, int index_end,
                     const int precision, BigInteger *res) {
  res->sign = op[0].sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;

  unsigned long sum = 0;
  for (int i = 0; i < precision; i++) {
    for (int j = index_start; j < index_end; j++) {
      sum += op[j].data.bits[i];
    }
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_4(const BigInteger op1, const BigInteger op2,
                       const BigInteger op3, const BigInteger op4,
                       const int precision, BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;

  unsigned long sum = 0;
  for (int i = 0; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_8(const BigInteger op1, const BigInteger op2,
                       const BigInteger op3, const BigInteger op4,
                       const BigInteger op5, const BigInteger op6,
                       const BigInteger op7, const BigInteger op8,
                       const int precision, BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;
  unsigned long *data5 = op5.data.bits;
  unsigned long *data6 = op6.data.bits;
  unsigned long *data7 = op7.data.bits;
  unsigned long *data8 = op8.data.bits;

  unsigned long sum = 0;
  for (int i = 0; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    sum += data5[i];
    sum += data6[i];
    sum += data7[i];
    sum += data8[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_12(const BigInteger op1, const BigInteger op2,
                        const BigInteger op3, const BigInteger op4,
                        const BigInteger op5, const BigInteger op6,
                        const BigInteger op7, const BigInteger op8,
                        const BigInteger op9, const BigInteger op10,
                        const BigInteger op11, const BigInteger op12,
                        const int precision, BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;
  unsigned long *data5 = op5.data.bits;
  unsigned long *data6 = op6.data.bits;
  unsigned long *data7 = op7.data.bits;
  unsigned long *data8 = op8.data.bits;
  unsigned long *data9 = op9.data.bits;
  unsigned long *data10 = op10.data.bits;
  unsigned long *data11 = op11.data.bits;
  unsigned long *data12 = op12.data.bits;

  unsigned long sum = 0;
  for (int i = 0; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    sum += data5[i];
    sum += data6[i];
    sum += data7[i];
    sum += data8[i];
    sum += data9[i];
    sum += data10[i];
    sum += data11[i];
    sum += data12[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_16(const BigInteger op1, const BigInteger op2,
                        const BigInteger op3, const BigInteger op4,
                        const BigInteger op5, const BigInteger op6,
                        const BigInteger op7, const BigInteger op8,
                        const BigInteger op9, const BigInteger op10,
                        const BigInteger op11, const BigInteger op12,
                        const BigInteger op13, const BigInteger op14,
                        const BigInteger op15, const BigInteger op16,
                        const int precision, BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;
  unsigned long *data5 = op5.data.bits;
  unsigned long *data6 = op6.data.bits;
  unsigned long *data7 = op7.data.bits;
  unsigned long *data8 = op8.data.bits;
  unsigned long *data9 = op9.data.bits;
  unsigned long *data10 = op10.data.bits;
  unsigned long *data11 = op11.data.bits;
  unsigned long *data12 = op12.data.bits;
  unsigned long *data13 = op13.data.bits;
  unsigned long *data14 = op14.data.bits;
  unsigned long *data15 = op15.data.bits;
  unsigned long *data16 = op16.data.bits;

  unsigned long sum = 0;
  for (int i = 0; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    sum += data5[i];
    sum += data6[i];
    sum += data7[i];
    sum += data8[i];
    sum += data9[i];
    sum += data10[i];
    sum += data11[i];
    sum += data12[i];
    sum += data13[i];
    sum += data14[i];
    sum += data15[i];
    sum += data16[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_8_unfold_1x(const BigInteger op1, const BigInteger op2,
                                 const BigInteger op3, const BigInteger op4,
                                 const BigInteger op5, const BigInteger op6,
                                 const BigInteger op7, const BigInteger op8,
                                 const int precision, BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;
  unsigned long *data5 = op5.data.bits;
  unsigned long *data6 = op6.data.bits;
  unsigned long *data7 = op7.data.bits;
  unsigned long *data8 = op8.data.bits;

  __m256i sum1;

  int i = 0;
  for (; i < precision - 3; i += 4) {
    sum1 = _mm256_loadu_si256((const __m256i *)(data1 + i));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data2 + i)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data3 + i)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data4 + i)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data5 + i)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data6 + i)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data7 + i)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data8 + i)));

    _mm256_storeu_si256((__m256i *)(res_data + i), sum1);
  }

  unsigned long carry = 0;
  for (int i = 0; i < precision; i++) {
    carry += res_data[i];
    res_data[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }
  res_data[precision] += carry;

  unsigned long sum = 0;
  for (; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    sum += data5[i];
    sum += data6[i];
    sum += data7[i];
    sum += data8[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

void big_integer_sum_8_unfold_2x(const BigInteger op1, const BigInteger op2,
                                 const BigInteger op3, const BigInteger op4,
                                 const BigInteger op5, const BigInteger op6,
                                 const BigInteger op7, const BigInteger op8,
                                 const int precision, BigInteger *res) {
  res->sign = op1.sign;
  res->data.size = precision;

  unsigned long *res_data = res->data.bits;
  unsigned long *data1 = op1.data.bits;
  unsigned long *data2 = op2.data.bits;
  unsigned long *data3 = op3.data.bits;
  unsigned long *data4 = op4.data.bits;
  unsigned long *data5 = op5.data.bits;
  unsigned long *data6 = op6.data.bits;
  unsigned long *data7 = op7.data.bits;
  unsigned long *data8 = op8.data.bits;

  __m256i sum1;
  __m256i sum2;

  int i = 0;
  for (; i < precision - 7; i += 8) {
    sum1 = _mm256_loadu_si256((const __m256i *)(data1 + i));
    sum2 = _mm256_loadu_si256((const __m256i *)(data1 + i + 4));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data2 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data2 + i + 4)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data3 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data3 + i + 4)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data4 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data4 + i + 4)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data5 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data5 + i + 4)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data6 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data6 + i + 4)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data7 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data7 + i + 4)));

    sum1 = _mm256_add_epi64(sum1,
                            _mm256_loadu_si256((const __m256i *)(data8 + i)));
    sum2 = _mm256_add_epi64(
        sum2, _mm256_loadu_si256((const __m256i *)(data8 + i + 4)));

    _mm256_storeu_si256((__m256i *)(res_data + i), sum1);
    _mm256_storeu_si256((__m256i *)(res_data + i + 4), sum2);
  }

  unsigned long carry = 0;
  for (int i = 0; i < precision; i++) {
    carry += res_data[i];
    res_data[i] = carry & bit_mask;
    carry >>= UINT_NUM_BITS;
  }
  res_data[precision] += carry;

  unsigned long sum = 0;
  for (; i < precision; i++) {
    sum += data1[i];
    sum += data2[i];
    sum += data3[i];
    sum += data4[i];
    sum += data5[i];
    sum += data6[i];
    sum += data7[i];
    sum += data8[i];
    res_data[i] = sum & bit_mask;
    sum >>= UINT_NUM_BITS;
  }

  if (sum) {
    res_data[precision] = sum;
  }

  if (res_data[precision]) {
    memmove(res_data, res_data + 1, precision * UINT_NUM_BYTES);
  }
}

#ifdef DEBUG
void big_integer_dump(const BigInteger bigInt) {
  printf("BigInteger:\n");
  printf("Sign: %d\n", (int)bigInt.sign);
  printf("Data: { ");
  if (bigInt.data.size > 0) {
    int i;
    for (i = 0; i < (bigInt.data.size - 1); i++)
      printf("%lu, ", bigInt.data.bits[i]);
    printf("%lu ", bigInt.data.bits[bigInt.data.size - 1]);
  }
  printf("}\n");
  printf("Length: %d\n", bigInt.data.size);
}
#endif