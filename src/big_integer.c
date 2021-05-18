/*
** big_integer.c
** Code largely based on this repo: <https://github.com/andreazevedo/biginteger>
** Modifications: support for arbitrary large numbers, bigint multiplication
**/

#include "big_integer.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

const int UINT_NUM_BYTES = (sizeof(unsigned int));
const int UINT_NUM_BITS = (sizeof(unsigned int) * 8);

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
void big_integer_deepcopy_inplace(const BigInteger other, BigInteger *this);
BigIntegerData big_integer_deepcopy_data(const BigIntegerData other);
void big_integer_deepcopy_data_inplace(const BigIntegerData other,
                                       BigIntegerData *this);
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
                                                  const int,
                                                  int *);
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
  // TODO: this is a designed choice to be discussed
  bigIntData.capacity = 2 * size;
  bigIntData.bits =
      (unsigned int *)malloc(bigIntData.capacity * UINT_NUM_BYTES);
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
  // TODO: this is a designed choice to be discussed
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
    printf("%10u\t", bigIntData.bits[i]);
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
  unsigned int *bits =
      (unsigned int *)malloc(pBigIntData->capacity * UINT_NUM_BYTES);
  memcpy(bits, pBigIntData->bits, pBigIntData->size * UINT_NUM_BYTES);
  free(pBigIntData->bits);
  pBigIntData->bits = bits;
  big_integer_clear_trash_data(pBigIntData);
  return;
}

BigInteger big_integer_deepcopy(const BigInteger other) {
  BigInteger this;
  this.sign = other.sign;
  this.data.capacity = other.data.capacity;
  this.data.size = other.data.size;
  this.data.bits = (unsigned int *)malloc(this.data.capacity * UINT_NUM_BYTES);
  memcpy(this.data.bits, other.data.bits, this.data.capacity * UINT_NUM_BYTES);
  return this;
}

void big_integer_deepcopy_inplace(const BigInteger other, BigInteger *this) {
  this->sign = other.sign;
  if (this->data.capacity < other.data.size) {
    assert(other.data.capacity > other.data.size);
    big_integer_resize_data(&this->data, other.data.capacity);
  }
  this->data.size = other.data.size;
  memcpy(this->data.bits, other.data.bits, this->data.size * UINT_NUM_BYTES);
  big_integer_clear_trash_data(&this->data);
  return;
}

BigIntegerData big_integer_deepcopy_data(const BigIntegerData other) {
  BigIntegerData this;
  this.capacity = other.capacity;
  this.size = other.size;
  this.bits = (unsigned int *)malloc(this.capacity * UINT_NUM_BYTES);
  memcpy(this.bits, other.bits, this.capacity * UINT_NUM_BYTES);
  return this;
}

void big_integer_deepcopy_data_inplace(const BigIntegerData other,
                                       BigIntegerData *this) {
  if (this->capacity < other.size) {
    assert(other.capacity > other.size);
    big_integer_resize_data(this, other.capacity);
  }
  this->size = other.size;
  memcpy(this->bits, other.bits, this->size * UINT_NUM_BYTES);
  big_integer_clear_trash_data(this);
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

  result.bits = (unsigned int *)malloc(capacity * UINT_NUM_BYTES);
  result.capacity = capacity;

  unsigned long long sum = 0;
  int i;
  for (i = 0; i < size; ++i) {
    sum += (unsigned long long)left.bits[i] + right.bits[i];
    result.bits[i] = (unsigned int)sum;
    sum >>= UINT_NUM_BITS;
  }

  if (sum > 0) {
    result.bits[i] = (unsigned int)sum;
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
    // TODO: this is a design choice: make the result as large as its operands.
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

  unsigned long long sum = 0;
  int i;
  for (i = 0; i < size; ++i) {
    if (i < left.size) {
      sum += (unsigned long long)left.bits[i];
    }
    if (i < right.size) {
      sum += (unsigned long long)right.bits[i];
    }
    // printf("left: %u, right: %u, sum: %llu\n", left.bits[i], right.bits[i],
    // sum); sum += (unsigned long long)left.bits[i] + right.bits[i];
    pResult->bits[i] = (unsigned int)sum;
    sum >>= UINT_NUM_BITS;
  }

  if (sum > 0) {
    pResult->bits[i] = (unsigned int)sum;
    i++;
  }

  pResult->size = i;
  big_integer_clear_trash_data(pResult);

  assert(pResult->size <= pResult->capacity);
  if (pResult->size == pResult->capacity) {
    // hope this won't happen much in a addition
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

  result.bits = (unsigned int *)calloc(capacity, UINT_NUM_BYTES);
  result.capacity = capacity;

  unsigned long long borrow = 0;
  int i;
  for (i = 0; i < size; ++i) {
    /* what happens here is that, if left is less than right, borrow will become
       "negative" (not really because it is unsigned), and the bit pattern for
       that is the 1's complement (complementing it to get to 0), which is
       exactly the remainder of this term in the subtraction. */
    borrow = (unsigned long long)left.bits[i] - right.bits[i] - borrow;

    result.bits[i] = (unsigned int)borrow;

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
    // TODO: this is a design choice: make the result as large as its operands.
    // make sure overflow won't happen
    assert(capacity > size);
    big_integer_resize_data(pResult, capacity);
  }

  unsigned long long borrow = 0;
  int i;
  for (i = 0; i < size; ++i) {
    /* what happens here is that, if left is less than right, borrow will become
       "negative" (not really because it is unsigned), and the bit pattern for
       that is the 1's complement (complementing it to get to 0), which is
       exactly the remainder of this term in the subtraction. */
    borrow = (unsigned long long)left.bits[i] - right.bits[i] - borrow;

    pResult->bits[i] = (unsigned int)borrow;

    /* here we just want the first 1 after removing the lower order term */
    borrow = (borrow >> UINT_NUM_BITS) & 1;
  }

  // assume bits[size] to bits[capacity-1] is clean (set to 0)
  big_integer_normalize_from(pResult, i);

  return;
}

void big_integer_increment_data(BigIntegerData *pBigIntData,
                                const unsigned int value) {
  unsigned long long carry = value;
  int i = 0;
  while (carry > 0) {
    carry += (unsigned long long)pBigIntData->bits[i];
    pBigIntData->bits[i] = (unsigned int)carry;
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
  unsigned long long borrow = value;
  int i = 0;
  while (borrow > 0) {
    borrow = (unsigned long long)pBigIntData->bits[i] - borrow;
    pBigIntData->bits[i] = (unsigned int)borrow;
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
    // TODO: not sure if this is correct and the most efficient way to extract
    // the lower 32 bits pResult->bits[i] = product & 0x00000000ffffffff;
    pResult->bits[i] = (unsigned int)product;
    // get upper 32 bits
    product >>= UINT_NUM_BITS;
  }

  // i is just left.size
  if (product > 0) {
    pResult->bits[i] = (unsigned int)product;
    pResult->size = i + 1;
  } else {
    pResult->size = i;
  }

  // TODO: skip this as we won't visit that area?
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
  result.bits = (unsigned int *)calloc(capacity * 2, UINT_NUM_BYTES);
  result.capacity = capacity * 2;
  result.size = 0;

  tmpResult.bits = (unsigned int *)calloc(capacity * 2, UINT_NUM_BYTES);
  tmpResult.capacity = capacity * 2;
  tmpResult.size = 1;

  mulResult.bits = (unsigned int *)calloc(capacity * 2, UINT_NUM_BYTES);
  mulResult.capacity = capacity * 2;
  mulResult.size = 0;

  int i;
  for (i = 0; i < right.size; i++) {
    big_integer_multiply_data_with_uint(left_copy, right.bits[i], &mulResult);
    // for (j = 1; j <= right.bits[i]; j++) {
    //   // this is memory leaking!
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
  result.bits = (unsigned int *)calloc(capacity * 2, UINT_NUM_BYTES);
  result.capacity = capacity * 2;
  result.size = 0;

  unsigned long carry = 0;
  for (int i = 0; i < right.size; ++i) {
    // big_integer_multiply_data_with_uint(left_copy, right.bits[i],
    // &mulResult);
    carry = 0;
    for (int j = 0; j < left.size; ++j) {
      carry += (unsigned long)left.bits[j] * right.bits[i] + result.bits[i + j];
      result.bits[i + j] = (unsigned int)carry;
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

void big_integer_multiply_data_two_operands_opt(BigIntegerData* pLeft,
                                                const BigIntegerData right) {
  BigIntegerData left_copy = big_integer_deepcopy_data(*pLeft);
  // TODO: merge this resize and set to zero together
  if (pLeft->capacity <= pLeft->size + right.size) {
      big_integer_resize_data(pLeft, pLeft->capacity*2);
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
      carry +=
          (unsigned long)left_copy.bits[j] * right.bits[i] + pLeft->bits[i + j];
      pLeft->bits[i + j] = (unsigned int)carry;
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
  // TODO: this is a design choice
  if (pResult->capacity < 2 * size) {
    assert(capacity > size);
    big_integer_resize_data(pResult, 2 * capacity);
  }
  big_integer_clear_trash_data(pResult);

  BigIntegerData tmpResult, mulResult;
  // tmpResult.bits = (unsigned int *)calloc(capacity * 2, UINT_NUM_BYTES);
  tmpResult.bits = (unsigned int *)calloc(capacity, UINT_NUM_BYTES);
  tmpResult.capacity = left.size + 1;
  tmpResult.size = 1;
  // mulResult.bits = (unsigned int *)calloc(capacity * 2, UINT_NUM_BYTES);
  mulResult.bits = (unsigned int *)calloc(capacity, UINT_NUM_BYTES);
  mulResult.capacity = left.size + 1;
  mulResult.size = 0;

  // big_integer_print_data(*pResult, "*pResult: ");
  int i;
  for (i = 0; i < right.size; i++) {
    big_integer_multiply_data_with_uint(left_copy, right.bits[i], &mulResult);
    // big_integer_print_data(tmpResult, "tmpResult: ");
    // for (j = 1; j <= right.bits[i]; j++) {
    //   // this is memory leaking!
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
    pHigh->bits = (unsigned int *)malloc(pHigh->capacity * UINT_NUM_BYTES);
    memcpy(pHigh->bits, source.bits + mid, pHigh->size * UINT_NUM_BYTES);
    big_integer_clear_trash_data(pHigh);
    // printf("mid: %d, bits: %u, bits+mid: %u\n", mid, *(source.bits),
    // *(source.bits+mid)); big_integer_print_data(*pHigh, "pHigh: ");
  } else {
    // set pHigh to zero
    // TODO: escape this next time?
    pHigh->size = 1;
    pHigh->capacity = 2;
    pHigh->bits = (unsigned int *)calloc(2, UINT_NUM_BYTES);
  }
  pLow->size = mid;
  pLow->capacity = mid + 1;
  pLow->bits = (unsigned int *)malloc(pLow->capacity * UINT_NUM_BYTES);
  memcpy(pLow->bits, source.bits, mid * UINT_NUM_BYTES);
  big_integer_clear_trash_data(pLow);
  // big_integer_print_data(*pLow, "pLow: ");

  return;
}

/* PUBLIC FUNCTIONS IMPLEMENTATION */
BigInteger big_integer_create(long long value) {
  BigInteger bigInt;
  bigInt.data.bits = (unsigned int *)malloc(sizeof(unsigned int) * 4);
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
      bigInt.data.bits[bigInt.data.size++] = (unsigned int)uValue;
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
      uValue = (unsigned long long)-value;
    } else {
      pBigInt->sign = 1;
      uValue = (unsigned long long)value;
    }

    pBigInt->data.size = 0;
    while (uValue > 0) {
      pBigInt->data.bits[pBigInt->data.size++] = (unsigned int)uValue;
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
      (unsigned int *)malloc(bigInt.data.capacity * UINT_NUM_BYTES);
  for (i = 0; i < size; ++i) {
    ret = fscanf(*ppFile, "%u", &bigInt.data.bits[i]);
    assert(ret == 1);
  }
  big_integer_clear_trash_data(&bigInt.data);
  return bigInt;
}

void big_integer_output_to_file(const BigInteger bigInt, FILE **ppFile) {
  // TODO: remove this check
  if (bigInt.sign == 0) {
    assert(bigInt.data.size == 1);
    assert(bigInt.data.bits[0] == 0);
  }

  fprintf(*ppFile, "%d\t%d\t", bigInt.sign, bigInt.data.size);
  int i;
  for (i = 0; i < bigInt.data.size; ++i) {
    fprintf(*ppFile, "%u\t\t\t", bigInt.data.bits[i]);
  }
  fprintf(*ppFile, "\n");
  return;
}

void big_integer_print(const BigInteger bigInt, const char *msg) {
  // TODO: remove this check
  if (bigInt.sign == 0) {
    assert(bigInt.data.size == 1);
    assert(bigInt.data.bits[0] == 0);
  }

  printf("%s\n", msg);
  printf(
      "The sign of the big int is %d\nThe size(length) of the big int is %d\n",
      bigInt.sign, bigInt.data.size);
  printf("The data is: \n");
  int i;
  for (i = 0; i < bigInt.data.size; ++i) {
    printf("%u\t\t\t", bigInt.data.bits[i]);
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
    printf("sign: %d; size: %d, bits[0]: %u\n", bigInt.sign, bigInt.data.size,
           bigInt.data.bits[0]);
    printf(
        "cannot convert this bigint to an int: not in the range of an int\n");
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
    printf("cannot convert this bigint to a long int: not in the range of a "
           "long int");
    exit(EXIT_FAILURE);
  }

  unsigned long long result = 0;
  int i = 0;
  for (i = 0; i < bigInt.data.size; ++i) {
    result |= ((unsigned long long)bigInt.data.bits[i]) << (UINT_NUM_BITS * i);
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
    big_integer_subtract_inplace_fixed_precision_data(left->data, right.data,
                                                      &left->data, precision,
                                                      carried);
  } else {
    // right > left
    BigIntegerData result;
    result.bits = malloc(precision * UINT_NUM_BYTES);
    result.capacity = precision;
    result.size = precision;
    big_integer_subtract_inplace_fixed_precision_data(right.data, left->data,
                                                      &result, precision, carried);
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
  unsigned long long borrow = 0;
  int offset = left.size - precision;
  for (int i = offset; i < left.size && i < right.size; i++) {
    /* what happens here is that, if left is less than right, borrow will become
       "negative" (not really because it is unsigned), and the bit pattern for
       that is the 1's complement (complementing it to get to 0), which is
       exactly the remainder of this term in the subtraction. */
    borrow = (unsigned long long)left.bits[i] - right.bits[i] - borrow;
    result->bits[i] = (unsigned int)borrow;
    /* here we just want the first 1 after removing the lower order term */
    borrow = (borrow >> UINT_NUM_BITS) & 1;
  }
  // get number of zeros ahead, useful in bigfloat add
  *carried = 0;
  for (int i = left.size -1; i >= offset; i--) {
    if (result->bits[i] == 0) {
      (*carried)++;
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
  unsigned long long sum = 0;
  int offset = left->size - precision;
  for (int i = offset; i < left->size && i < right.size; i++) {
    sum += (unsigned long long)left->bits[i] + right.bits[i];
    left->bits[i] = (unsigned int)sum;
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
    left->bits[left->size - 1] = (unsigned int)sum;
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
                                                  const int precision) {
  if (left->sign == 0 || right.sign == 0) {
    // answer is 0
    left->sign = 0;
    left->data.size = precision;
    memset(left->data.bits, 0, precision * UINT_NUM_BYTES);
  }
  unsigned int *tmp = calloc(2 * precision, UINT_NUM_BYTES);
  int offset_left = left->data.size - precision;
  int offset_right = right.data.size - precision;
  for (int j = offset_right; j < right.data.size; j++) {
    unsigned long long carry = 0;
    for (int i = offset_left; i < left->data.size; i++) {
      int idx = j - offset_right + i - offset_left;
      carry += (unsigned long long)left->data.bits[i] * right.data.bits[j] +
               tmp[idx];
      tmp[idx] = (unsigned int)carry;
      carry >>= UINT_NUM_BITS;
    }
    int idx = j - offset_right + precision;
    carry += tmp[idx];
    tmp[idx] = (unsigned int)carry;
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
  memmove(left->data.bits, tmp + offset, precision * UINT_NUM_BYTES);
  free(tmp); // release temporal variable
}

BigInteger big_integer_add_trailing_zeros(const BigInteger bi, int num) {
  BigInteger ans;
  ans.sign = bi.sign;
  ans.data.size = bi.data.size + num;
  ans.data.capacity = 2 * ans.data.size;
  ans.data.bits = (unsigned int *)calloc(ans.data.capacity, UINT_NUM_BYTES);
  memcpy(ans.data.bits + num, bi.data.bits, bi.data.size * UINT_NUM_BYTES);
  return ans;
}

void big_integer_add_trailing_zeros_inplace(BigInteger *bi, int num) {
  if(bi->data.capacity >= bi->data.size + num) {
    // no allocation
    for(int i = bi->data.size - 1; i >= 0; i--) {
      bi->data.bits[i + num] = bi->data.bits[i];
    }
    for(int i = 0; i < num; i++) {
      bi->data.bits[i] = 0;
    }
    bi->data.size += num;
  } else {
    // need allocation
    unsigned int *res = (unsigned int *)calloc(2 * (bi->data.size + num), UINT_NUM_BYTES);
    memcpy(res + num, bi->data.bits, bi->data.size * UINT_NUM_BYTES);
    free(bi->data.bits);
    bi->data.bits = res;
    bi->data.size += num;
    bi->data.capacity = 2 * bi->data.size;
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
      printf("%u, ", bigInt.data.bits[i]);
    printf("%u ", bigInt.data.bits[bigInt.data.size - 1]);
  }
  printf("}\n");
  printf("Length: %d\n", bigInt.data.size);
}
#endif