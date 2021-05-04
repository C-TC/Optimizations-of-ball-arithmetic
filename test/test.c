#include "../src/big_integer.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #ifndef LLONG_MAX
//     #define LLONG_MAX	0x7FFFFFFFFFFFFFFF
// #endif
// #ifndef LLONG_MIN
//     #define LLONG_MIN	(-LLONG_MAX-1)
// #endif

void test_create() {
  BigInteger bigInt;

  bigInt = big_integer_create(0);
  assert(big_integer_to_int(bigInt) == 0);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(17);
  assert(big_integer_to_int(bigInt) == 17);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-17);
  assert(big_integer_to_int(bigInt) == -17);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(INT_MAX);
  assert(big_integer_to_int(bigInt) == INT_MAX);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-INT_MAX);
  assert(big_integer_to_int(bigInt) == -INT_MAX);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(INT_MIN);
  assert(big_integer_to_int(bigInt) == INT_MIN);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create((long long)INT_MAX + INT_MAX + 5);
  assert(big_integer_to_long_long(bigInt) == 4294967299);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create((long long)UINT_MAX + UINT_MAX);
  assert(big_integer_to_long_long(bigInt) == (long long)UINT_MAX + UINT_MAX);
  big_integer_destroy(&bigInt);
}

void test_set() {
  BigInteger bigInt;
  bigInt.data.bits = NULL;
  bigInt.data.capacity = 0;
  bigInt.data.size = 0;

  big_integer_set(0, &bigInt);
  assert(big_integer_to_int(bigInt) == 0);

  big_integer_set(17, &bigInt);
  assert(big_integer_to_int(bigInt) == 17);

  big_integer_set(-17, &bigInt);
  assert(big_integer_to_int(bigInt) == -17);

  big_integer_set(INT_MAX, &bigInt);
  assert(big_integer_to_int(bigInt) == INT_MAX);

  big_integer_set(-INT_MAX, &bigInt);
  assert(big_integer_to_int(bigInt) == -INT_MAX);

  big_integer_set(INT_MIN, &bigInt);
  assert(big_integer_to_int(bigInt) == INT_MIN);

  big_integer_set((long long)INT_MAX + INT_MAX + 5, &bigInt);
  assert(big_integer_to_long_long(bigInt) == 4294967299);

  big_integer_set((long long)UINT_MAX + UINT_MAX, &bigInt);
  assert(big_integer_to_long_long(bigInt) == (long long)UINT_MAX + UINT_MAX);

  big_integer_destroy(&bigInt);
}

void test_input_output() {
  BigInteger bigInt;
  FILE *in = fopen("data/simple_add.txt", "r");
  FILE *out = fopen("data/copy_simple_add.txt", "w");
  for (int i = 0; i < 18; ++i) {
    bigInt = big_integer_create_from_file(&in);
    big_integer_output_to_file(bigInt, &out);
    big_integer_destroy(&bigInt);
  }

  fclose(in);
  fclose(out);
}

// test for big_integer_destroy is done via valgrind for memory leak
void test_to_int() {
  BigInteger bigInt;
  long result;

  bigInt = big_integer_create(0);
  result = big_integer_to_int(bigInt);
  assert(result == 0);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(35);
  result = big_integer_to_int(bigInt);
  assert(result == 35);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-26);
  result = big_integer_to_int(bigInt);
  assert(result == -26);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(INT_MAX);
  result = big_integer_to_int(bigInt);
  assert(result == INT_MAX);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(INT_MIN);
  result = big_integer_to_int(bigInt);
  assert(result == INT_MIN);
  big_integer_destroy(&bigInt);

  FILE *in = fopen("data/simple_add.txt", "r");

  bigInt = big_integer_create_from_file(&in);
  // overflow...
  // result = big_integer_to_int(bigInt);
  // assert(result == 3626764237);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  // overflow...
  // result = big_integer_to_int(bigInt);
  // assert(result == -2488626091);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  result = big_integer_to_int(bigInt);
  assert(result == 1138138146);
  big_integer_destroy(&bigInt);

  fclose(in);
}

void test_to_long_long() {
  BigInteger bigInt;
  long long result;

  bigInt = big_integer_create(0);
  result = big_integer_to_long_long(bigInt);
  assert(result == 0);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(35);
  result = big_integer_to_long_long(bigInt);
  assert(result == 35);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-26);
  result = big_integer_to_long_long(bigInt);
  assert(result == -26);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(LLONG_MAX);
  result = big_integer_to_long_long(bigInt);
  assert(result == LLONG_MAX);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(LLONG_MIN);
  result = big_integer_to_long_long(bigInt);
  assert(result == LLONG_MIN);
  big_integer_destroy(&bigInt);

  FILE *in = fopen("data/simple_add.txt", "r");

  bigInt = big_integer_create_from_file(&in);
  result = big_integer_to_long_long(bigInt);
  assert(result == 3626764237);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  result = big_integer_to_long_long(bigInt);
  assert(result == -2488626091);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  result = big_integer_to_long_long(bigInt);
  assert(result == 1138138146);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  // overflow...
  // result = big_integer_to_long_long(bigInt);
  // assert(result == -10688567668993751422);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  // overflow...
  // result = big_integer_to_long_long(bigInt);
  // assert(result == 15308084094301570617);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  result = big_integer_to_long_long(bigInt);
  assert(result == 4619516425307819195);
  big_integer_destroy(&bigInt);

  fclose(in);
}

void test_compare() {
  BigInteger left;
  BigInteger right;

  left = big_integer_create(10);
  right = big_integer_create(9);
  assert(big_integer_compare(left, right) > 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(9);
  right = big_integer_create(10);
  assert(big_integer_compare(left, right) < 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(10);
  right = big_integer_create(10);
  assert(big_integer_compare(left, right) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-9);
  right = big_integer_create(-10);
  assert(big_integer_compare(left, right) > 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-10);
  right = big_integer_create(-9);
  assert(big_integer_compare(left, right) < 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-10);
  right = big_integer_create(-10);
  assert(big_integer_compare(left, right) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(10);
  right = big_integer_create(-10);
  assert(big_integer_compare(left, right) > 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-10);
  right = big_integer_create(10);
  assert(big_integer_compare(left, right) < 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create((long long)INT_MAX + INT_MAX);
  right = big_integer_create((long long)INT_MAX + 3);
  assert(big_integer_compare(left, right) > 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create((long long)INT_MAX + 3);
  right = big_integer_create((long long)INT_MAX + INT_MAX);
  assert(big_integer_compare(left, right) < 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create((long long)INT_MAX + INT_MAX);
  right = big_integer_create((long long)INT_MAX + INT_MAX);
  assert(big_integer_compare(left, right) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  FILE *in = fopen("data/simple_add.txt", "r");
  int results[9] = {1, 1, 1, 1, -1, -1, 1, 1, 1};
  for (int i = 0; i < 9; ++i) {
    left = big_integer_create_from_file(&in);
    right = big_integer_create_from_file(&in);
    assert(big_integer_compare(left, right) == results[i]);
    big_integer_destroy(&left);
    big_integer_destroy(&right);
  }
  fclose(in);
};

void test_add() {
  BigInteger left;
  BigInteger right;
  BigInteger result;
  BigInteger answer;

  left = big_integer_create(17);
  right = big_integer_create(3);
  result = big_integer_add(left, right);
  assert(big_integer_to_int(result) == 20);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(INT_MAX);
  right = big_integer_create(5);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) == ((long long)INT_MAX + 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(1);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX + 1));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(5);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX + 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(UINT_MAX);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX + UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create((long long)UINT_MAX + UINT_MAX);
  right = big_integer_create(UINT_MAX);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) ==
         ((long long)UINT_MAX + UINT_MAX + UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-17);
  right = big_integer_create(-3);
  result = big_integer_add(left, right);
  assert(big_integer_to_int(result) == -20);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-1);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX - 1));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-5);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX - 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX - UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) ==
         (-(long long)UINT_MAX - UINT_MAX - UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(5);
  right = big_integer_create(0);
  result = big_integer_add(left, right);
  assert(big_integer_to_int(result) == 5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(0);
  right = big_integer_create(5);
  result = big_integer_add(left, right);
  assert(big_integer_to_int(result) == 5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(0);
  right = big_integer_create(0);
  result = big_integer_add(left, right);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-5);
  right = big_integer_create(0);
  result = big_integer_add(left, right);
  assert(big_integer_to_int(result) == -5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(0);
  right = big_integer_create(-5);
  result = big_integer_add(left, right);
  assert(big_integer_to_int(result) == -5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(18);
  right = big_integer_create(-3);
  result = big_integer_add(left, right);
  assert(big_integer_to_int(result) == 15);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-18);
  right = big_integer_create(+3);
  result = big_integer_add(left, right);
  assert(big_integer_to_int(result) == -15);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(+UINT_MAX);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) == -(long long)UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create((long long)UINT_MAX + UINT_MAX + UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  result = big_integer_add(left, right);
  assert(big_integer_to_long_long(result) == (long long)UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  FILE *in = fopen("data/simple_add.txt", "r");
  for (int i = 0; i < 6; ++i) {
    left = big_integer_create_from_file(&in);
    right = big_integer_create_from_file(&in);
    answer = big_integer_create_from_file(&in);
    result = big_integer_add(left, right);
    assert(big_integer_compare(result, answer) == 0);
    big_integer_destroy(&left);
    big_integer_destroy(&right);
    big_integer_destroy(&result);
    big_integer_destroy(&answer);
  }
  fclose(in);
};

void test_add_inplace() {
  BigInteger left;
  BigInteger right;
  BigInteger result;
  BigInteger answer;
  result.data.size = 0;
  result.data.bits = NULL;
  result.data.capacity = 0;
  answer.data.size = 0;
  answer.data.bits = NULL;
  answer.data.capacity = 0;

  left = big_integer_create(17);
  right = big_integer_create(3);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 20);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(INT_MAX);
  right = big_integer_create(5);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == ((long long)INT_MAX + 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(1);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX + 1));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(5);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX + 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(UINT_MAX);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX + UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create((long long)UINT_MAX + UINT_MAX);
  right = big_integer_create(UINT_MAX);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) ==
         ((long long)UINT_MAX + UINT_MAX + UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-17);
  right = big_integer_create(-3);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_int(result) == -20);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-1);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX - 1));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-5);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX - 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX - UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) ==
         (-(long long)UINT_MAX - UINT_MAX - UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(5);
  right = big_integer_create(0);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(0);
  right = big_integer_create(5);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(0);
  right = big_integer_create(0);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-5);
  right = big_integer_create(0);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_int(result) == -5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(0);
  right = big_integer_create(-5);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_int(result) == -5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(18);
  right = big_integer_create(-3);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 15);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-18);
  right = big_integer_create(+3);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_int(result) == -15);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(+UINT_MAX);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == -(long long)UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create((long long)UINT_MAX + UINT_MAX + UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  big_integer_add_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == (long long)UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  FILE *in = fopen("data/simple_add.txt", "r");
  for (int i = 0; i < 6; ++i) {
    left = big_integer_create_from_file(&in);
    right = big_integer_create_from_file(&in);
    answer = big_integer_create_from_file(&in);
    big_integer_add_inplace(left, right, &result);
    assert(big_integer_compare(result, answer) == 0);
    big_integer_destroy(&left);
    big_integer_destroy(&right);
    big_integer_destroy(&answer);
  }

  big_integer_destroy(&result);
  fclose(in);
};

void test_subtract() {
  BigInteger left;
  BigInteger right;
  BigInteger result;
  BigInteger answer;

  left = big_integer_create(18);
  right = big_integer_create(3);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_int(result) == 15);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(INT_MAX);
  right = big_integer_create(5);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) == ((long long)INT_MAX - 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(1);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX - 1));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(5);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX - 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(UINT_MAX);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create((long long)UINT_MAX + UINT_MAX);
  right = big_integer_create(UINT_MAX);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-18);
  right = big_integer_create(-3);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_int(result) == -15);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-1);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX + 1));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-5);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX + 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(5);
  right = big_integer_create(0);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_int(result) == 5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(0);
  right = big_integer_create(5);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_int(result) == -5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(0);
  right = big_integer_create(0);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-5);
  right = big_integer_create(0);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_int(result) == -5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(0);
  right = big_integer_create(-5);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_int(result) == 5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(18);
  right = big_integer_create(-3);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_int(result) == 21);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-18);
  right = big_integer_create(+3);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_int(result) == -21);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(+UINT_MAX);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) ==
         -(long long)UINT_MAX - UINT_MAX - UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create((long long)UINT_MAX + UINT_MAX + UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  result = big_integer_subtract(left, right);
  assert(big_integer_to_long_long(result) ==
         (long long)UINT_MAX + UINT_MAX + UINT_MAX + UINT_MAX + UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  FILE *in = fopen("data/simple_sub.txt", "r");
  for (int i = 0; i < 6; ++i) {
    left = big_integer_create_from_file(&in);
    right = big_integer_create_from_file(&in);
    answer = big_integer_create_from_file(&in);
    result = big_integer_subtract(left, right);
    assert(big_integer_compare(result, answer) == 0);
    big_integer_destroy(&left);
    big_integer_destroy(&right);
    big_integer_destroy(&result);
    big_integer_destroy(&answer);
  }
  fclose(in);
};

void test_subtract_inplace() {
  BigInteger left;
  BigInteger right;
  BigInteger result;
  BigInteger answer;
  result.data.size = 0;
  result.data.bits = NULL;
  result.data.capacity = 0;
  answer.data.size = 0;
  answer.data.bits = NULL;
  answer.data.capacity = 0;

  left = big_integer_create(18);
  right = big_integer_create(3);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 15);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(INT_MAX);
  right = big_integer_create(5);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == ((long long)INT_MAX - 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(1);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX - 1));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(5);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX - 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(UINT_MAX);
  right = big_integer_create(UINT_MAX);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create((long long)UINT_MAX + UINT_MAX);
  right = big_integer_create(UINT_MAX);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == ((long long)UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-18);
  right = big_integer_create(-3);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_int(result) == -15);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-1);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX + 1));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-5);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX + 5));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == (-(long long)UINT_MAX));
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(5);
  right = big_integer_create(0);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(0);
  right = big_integer_create(5);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_int(result) == -5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(0);
  right = big_integer_create(0);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-5);
  right = big_integer_create(0);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_int(result) == -5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(0);
  right = big_integer_create(-5);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 5);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(18);
  right = big_integer_create(-3);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 21);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-18);
  right = big_integer_create(+3);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_int(result) == -21);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(+UINT_MAX);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) ==
         -(long long)UINT_MAX - UINT_MAX - UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create((long long)UINT_MAX + UINT_MAX + UINT_MAX);
  right = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  big_integer_subtract_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) ==
         (long long)UINT_MAX + UINT_MAX + UINT_MAX + UINT_MAX + UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  FILE *in = fopen("data/simple_sub.txt", "r");
  for (int i = 0; i < 6; ++i) {
    left = big_integer_create_from_file(&in);
    right = big_integer_create_from_file(&in);
    answer = big_integer_create_from_file(&in);
    big_integer_subtract_inplace(left, right, &result);
    assert(big_integer_compare(result, answer) == 0);
    big_integer_destroy(&left);
    big_integer_destroy(&right);
    big_integer_destroy(&answer);
  }
  big_integer_destroy(&result);
  fclose(in);
};

void test_multiply() {
  BigInteger left;
  BigInteger right;
  BigInteger result;
  BigInteger answer;

  left = big_integer_create(0);
  right = big_integer_create(12);
  result = big_integer_multiply(left, right);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(0);
  right = big_integer_create(-5);
  result = big_integer_multiply(left, right);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(12);
  right = big_integer_create(0);
  result = big_integer_multiply(left, right);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-5);
  right = big_integer_create(0);
  result = big_integer_multiply(left, right);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(1);
  right = big_integer_create(117);
  result = big_integer_multiply(left, right);
  assert(big_integer_to_int(result) == 117);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-1);
  right = big_integer_create(117);
  result = big_integer_multiply(left, right);
  assert(big_integer_to_int(result) == -117);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-11);
  right = big_integer_create(15);
  result = big_integer_multiply(left, right);
  assert(big_integer_to_int(result) == -165);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(1);
  right = big_integer_create(1);
  result = big_integer_multiply(left, right);
  assert(big_integer_to_int(result) == 1);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-1);
  right = big_integer_create(-1);
  result = big_integer_multiply(left, right);
  assert(big_integer_to_int(result) == 1);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(1);
  result = big_integer_multiply(left, right);
  assert(big_integer_to_long_long(result) == -(long long)UINT_MAX - UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  FILE *in = fopen("data/simple_mul.txt", "r");
  for (int i = 0; i < 6; ++i) {
    left = big_integer_create_from_file(&in);
    right = big_integer_create_from_file(&in);
    answer = big_integer_create_from_file(&in);
    result = big_integer_multiply(left, right);
    // big_integer_print(result, "result: ");
    // big_integer_print(answer, "answer: ");
    assert(big_integer_compare(result, answer) == 0);
    big_integer_destroy(&left);
    big_integer_destroy(&right);
    big_integer_destroy(&result);
    big_integer_destroy(&answer);
    // printf("mul done [%d/6]\n", i + 1);
  }
  fclose(in);
}

void test_multiply_opt() {
  BigInteger left;
  BigInteger right;
  BigInteger result;
  BigInteger answer;

  left = big_integer_create(0);
  right = big_integer_create(12);
  result = big_integer_multiply_opt(left, right);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(0);
  right = big_integer_create(-5);
  result = big_integer_multiply_opt(left, right);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(12);
  right = big_integer_create(0);
  result = big_integer_multiply_opt(left, right);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-5);
  right = big_integer_create(0);
  result = big_integer_multiply_opt(left, right);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(1);
  right = big_integer_create(117);
  result = big_integer_multiply_opt(left, right);
  assert(big_integer_to_int(result) == 117);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-1);
  right = big_integer_create(117);
  result = big_integer_multiply_opt(left, right);
  assert(big_integer_to_int(result) == -117);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-11);
  right = big_integer_create(15);
  result = big_integer_multiply_opt(left, right);
  assert(big_integer_to_int(result) == -165);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(1);
  right = big_integer_create(1);
  result = big_integer_multiply_opt(left, right);
  assert(big_integer_to_int(result) == 1);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-1);
  right = big_integer_create(-1);
  result = big_integer_multiply_opt(left, right);
  assert(big_integer_to_int(result) == 1);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(1);
  result = big_integer_multiply_opt(left, right);
  assert(big_integer_to_long_long(result) == -(long long)UINT_MAX - UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);

  FILE *in = fopen("data/simple_mul.txt", "r");
  for (int i = 0; i < 6; ++i) {
    left = big_integer_create_from_file(&in);
    right = big_integer_create_from_file(&in);
    answer = big_integer_create_from_file(&in);
    result = big_integer_multiply_opt(left, right);
    // big_integer_print(result, "result: ");
    // big_integer_print(answer, "answer: ");
    assert(big_integer_compare(result, answer) == 0);
    big_integer_destroy(&left);
    big_integer_destroy(&right);
    big_integer_destroy(&result);
    big_integer_destroy(&answer);
    // printf("mul done [%d/6]\n", i + 1);
  }
  fclose(in);
}

void test_multiply_two_operands_opt() {
  BigInteger left;
  BigInteger right;
  BigInteger answer;
  left = big_integer_create(0);
  right = big_integer_create(12);
  big_integer_multiply_two_operands_opt(&left, right);
  assert(big_integer_to_int(left) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(0);
  right = big_integer_create(-5);
  big_integer_multiply_two_operands_opt(&left, right);
  assert(big_integer_to_int(left) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(12);
  right = big_integer_create(0);
  big_integer_multiply_two_operands_opt(&left, right);
  assert(big_integer_to_int(left) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-5);
  right = big_integer_create(0);
  big_integer_multiply_two_operands_opt(&left, right);
  assert(big_integer_to_int(left) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(1);
  right = big_integer_create(117);
  big_integer_multiply_two_operands_opt(&left, right);
  // big_integer_print(left, "left: ");
  assert(big_integer_to_int(left) == 117);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-1);
  right = big_integer_create(117);
  big_integer_multiply_two_operands_opt(&left, right);
  assert(big_integer_to_int(left) == -117);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-11);
  right = big_integer_create(15);
  big_integer_multiply_two_operands_opt(&left, right);
  assert(big_integer_to_int(left) == -165);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(1);
  right = big_integer_create(1);
  big_integer_multiply_two_operands_opt(&left, right);
  assert(big_integer_to_int(left) == 1);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-1);
  right = big_integer_create(-1);
  big_integer_multiply_two_operands_opt(&left, right);
  assert(big_integer_to_int(left) == 1);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(1);
  big_integer_multiply_two_operands_opt(&left, right);
  // big_integer_print(left, "left");
  // printf("%d\n", left.data.size);
  // printf("%lld\t%lld\n", big_integer_to_long_long(left), -(long long)UINT_MAX - UINT_MAX);
  assert(big_integer_to_long_long(left) == -(long long)UINT_MAX - UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  FILE *in = fopen("data/simple_mul.txt", "r");
  for (int i = 0; i < 6; ++i) {
    left = big_integer_create_from_file(&in);
    right = big_integer_create_from_file(&in);
    answer = big_integer_create_from_file(&in);
    big_integer_multiply_two_operands_opt(&left, right);
    // big_integer_print(left, "left: ");
    // big_integer_print(answer, "answer: ");
    assert(big_integer_compare(left, answer) == 0);
    big_integer_destroy(&left);
    big_integer_destroy(&right);
    big_integer_destroy(&answer);
    // printf("mul done [%d/6]\n", i + 1);
  }
  fclose(in);
}


void test_multiply_inplace() {
  BigInteger left;
  BigInteger right;
  BigInteger result;
  BigInteger answer;
  result.data.size = 0;
  result.data.capacity = 0;
  result.data.bits = NULL;
  answer.data.size = 0;
  answer.data.capacity = 0;
  answer.data.bits = NULL;

  left = big_integer_create(0);
  right = big_integer_create(12);
  big_integer_multiply_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(0);
  right = big_integer_create(-5);
  big_integer_multiply_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(12);
  right = big_integer_create(0);
  big_integer_multiply_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-5);
  right = big_integer_create(0);
  big_integer_multiply_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 0);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(1);
  right = big_integer_create(117);
  big_integer_multiply_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 117);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-1);
  right = big_integer_create(117);
  big_integer_multiply_inplace(left, right, &result);
  assert(big_integer_to_int(result) == -117);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-11);
  right = big_integer_create(15);
  big_integer_multiply_inplace(left, right, &result);
  assert(big_integer_to_int(result) == -165);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(1);
  right = big_integer_create(1);
  big_integer_multiply_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 1);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-1);
  right = big_integer_create(-1);
  big_integer_multiply_inplace(left, right, &result);
  assert(big_integer_to_int(result) == 1);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  left = big_integer_create(-(long long)UINT_MAX - UINT_MAX);
  right = big_integer_create(1);
  big_integer_multiply_inplace(left, right, &result);
  assert(big_integer_to_long_long(result) == -(long long)UINT_MAX - UINT_MAX);
  big_integer_destroy(&left);
  big_integer_destroy(&right);

  FILE *in = fopen("data/simple_mul.txt", "r");
  for (int i = 0; i < 6; ++i) {
    left = big_integer_create_from_file(&in);
    right = big_integer_create_from_file(&in);
    answer = big_integer_create_from_file(&in);
    big_integer_multiply_inplace(left, right, &result);
    // big_integer_print(result, "result: ");
    // big_integer_print(answer, "answer: ");
    assert(big_integer_compare(result, answer) == 0);
    big_integer_destroy(&left);
    big_integer_destroy(&right);
    big_integer_destroy(&answer);
    // printf("mul done [%d/6]\n", i + 1);
  }
  big_integer_destroy(&result);
  fclose(in);
}

void test_multiply_karatsuba() {
  BigInteger left;
  BigInteger right;
  BigInteger result;
  BigInteger answer;

  FILE *in = fopen("data/simple_mul.txt", "r");
  for (int i = 0; i < 6; ++i) {
    left = big_integer_create_from_file(&in);
    right = big_integer_create_from_file(&in);
    answer = big_integer_create_from_file(&in);
    result = big_integer_multiply_karatsuba(left, right);
    // big_integer_print(result, "result: ");
    // big_integer_print(answer, "answer: ");
    assert(big_integer_compare(result, answer) == 0);
    big_integer_destroy(&left);
    big_integer_destroy(&right);
    big_integer_destroy(&result);
    big_integer_destroy(&answer);
    // printf("mul done [%d/6]\n", i + 1);
  }
  fclose(in);
}

void test_increment() {
  BigInteger bigInt;

  bigInt = big_integer_create(10);
  big_integer_increment(&bigInt, 5);
  assert(big_integer_to_long_long(bigInt) == 15);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-5);
  big_integer_increment(&bigInt, 10);
  assert(big_integer_to_long_long(bigInt) == 5);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(0);
  big_integer_increment(&bigInt, 0);
  assert(big_integer_to_long_long(bigInt) == 0);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(0);
  big_integer_increment(&bigInt, 3);
  assert(big_integer_to_long_long(bigInt) == 3);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-15);
  big_integer_increment(&bigInt, 15);
  assert(big_integer_to_long_long(bigInt) == 0);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(UINT_MAX);
  big_integer_increment(&bigInt, 1);
  assert(big_integer_to_long_long(bigInt) == (long long)UINT_MAX + 1);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(UINT_MAX);
  big_integer_increment(&bigInt, UINT_MAX);
  assert(big_integer_to_long_long(bigInt) == (long long)UINT_MAX + UINT_MAX);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create((long long)UINT_MAX + UINT_MAX);
  big_integer_increment(&bigInt, 15);
  assert(big_integer_to_long_long(bigInt) ==
         (long long)UINT_MAX + UINT_MAX + 15);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-(long long)UINT_MAX - 15);
  big_integer_increment(&bigInt, 15);
  assert(big_integer_to_long_long(bigInt) == -(long long)UINT_MAX);
  big_integer_destroy(&bigInt);

  FILE *in = fopen("data/simple_add.txt", "r");

  long long result;
  bigInt = big_integer_create_from_file(&in);
  big_integer_increment(&bigInt, 123456);
  result = big_integer_to_long_long(bigInt);
  assert(result == 3626764237 + 123456);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  big_integer_increment(&bigInt, 789101112);
  result = big_integer_to_long_long(bigInt);
  assert(result == -2488626091 + 789101112);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  big_integer_increment(&bigInt, 1357911);
  result = big_integer_to_long_long(bigInt);
  assert(result == 1138138146 + 1357911);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  // overflow...
  // result = big_integer_to_long_long(bigInt);
  // assert(result == -10688567668993751422);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  // overflow...
  // result = big_integer_to_long_long(bigInt);
  // assert(result == 15308084094301570617);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  big_integer_increment(&bigInt, 24681012);
  result = big_integer_to_long_long(bigInt);
  assert(result == 4619516425307819195 + 24681012);
  big_integer_destroy(&bigInt);

  fclose(in);
};

void test_decrement() {
  BigInteger bigInt;

  bigInt = big_integer_create(10);
  big_integer_decrement(&bigInt, 5);
  assert(big_integer_to_long_long(bigInt) == 5);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-5);
  big_integer_decrement(&bigInt, 10);
  assert(big_integer_to_long_long(bigInt) == -15);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(0);
  big_integer_decrement(&bigInt, 0);
  assert(big_integer_to_long_long(bigInt) == 0);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(0);
  big_integer_decrement(&bigInt, 3);
  assert(big_integer_to_long_long(bigInt) == -3);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(15);
  big_integer_decrement(&bigInt, 15);
  assert(big_integer_to_long_long(bigInt) == 0);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(UINT_MAX);
  big_integer_decrement(&bigInt, 1);
  assert(big_integer_to_long_long(bigInt) == (long long)UINT_MAX - 1);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(UINT_MAX);
  big_integer_decrement(&bigInt, UINT_MAX);
  assert(big_integer_to_long_long(bigInt) == 0);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-(long long)UINT_MAX);
  big_integer_decrement(&bigInt, UINT_MAX);
  assert(big_integer_to_long_long(bigInt) == -(long long)UINT_MAX - UINT_MAX);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create((long long)UINT_MAX + UINT_MAX);
  big_integer_decrement(&bigInt, 15);
  assert(big_integer_to_long_long(bigInt) ==
         (long long)UINT_MAX + UINT_MAX - 15);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-(long long)UINT_MAX - 15);
  big_integer_decrement(&bigInt, 15);
  assert(big_integer_to_long_long(bigInt) == -(long long)UINT_MAX - 30);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create(-(long long)UINT_MAX + 15);
  big_integer_decrement(&bigInt, 15);
  assert(big_integer_to_long_long(bigInt) == -(long long)UINT_MAX);
  big_integer_destroy(&bigInt);

  FILE *in = fopen("data/simple_add.txt", "r");

  long long result;
  bigInt = big_integer_create_from_file(&in);
  big_integer_decrement(&bigInt, 123456);
  result = big_integer_to_long_long(bigInt);
  assert(result == 3626764237 - 123456);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  big_integer_decrement(&bigInt, 789101112);
  result = big_integer_to_long_long(bigInt);
  assert(result == -2488626091 - 789101112);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  big_integer_decrement(&bigInt, 1357911);
  result = big_integer_to_long_long(bigInt);
  assert(result == 1138138146 - 1357911);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  // overflow...
  // result = big_integer_to_long_long(bigInt);
  // assert(result == -10688567668993751422);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  // overflow...
  // result = big_integer_to_long_long(bigInt);
  // assert(result == 15308084094301570617);
  big_integer_destroy(&bigInt);

  bigInt = big_integer_create_from_file(&in);
  big_integer_decrement(&bigInt, 24681012);
  result = big_integer_to_long_long(bigInt);
  assert(result == 4619516425307819195 - 24681012);
  big_integer_destroy(&bigInt);

  fclose(in);
};

// void test_performance() {
//   int NUM_ITERATIONS = 10000000;
//
//   clock_t start;
//   clock_t end;
//   double cpuTime;
//
//   long long l;
//   start = clock();
//   BigInteger sumBigInt = big_integer_create(0);
//   for (l = 1; l <= NUM_ITERATIONS; ++l) {
//     big_integer_increment(&sumBigInt, (unsigned int)l);
//   }
//   end = clock();
//   cpuTime = ((double)(end - start)) / CLOCKS_PER_SEC;
//   printf("BigInteger made %d increment operations in %f seconds.\n",
//          NUM_ITERATIONS, cpuTime);
//   printf("BigInteger last value was: %lld\n",
//          big_integer_to_long_long(sumBigInt));
//   big_integer_destroy(&sumBigInt);
//
//   double d;
//   start = clock();
//   double sumDouble = 0;
//   for (d = 1; d <= NUM_ITERATIONS; ++d)
//     sumDouble += d;
//   end = clock();
//   cpuTime = ((double)(end - start)) / CLOCKS_PER_SEC;
//   printf("Double made %d increment operations in %f seconds.\n",
//   NUM_ITERATIONS,
//          cpuTime);
//   printf("Double last value was: %f\n", sumDouble);
// };
//
// void test_performance_factorial() {
//   int NUM_ITERATIONS = 20;
//
//   clock_t start;
//   clock_t end;
//   double cpuTime;
//
//   long long l;
//   start = clock();
//   BigInteger resBigInt = big_integer_create(1);
//   BigInteger facBigInt = big_integer_create(1);
//   for (l = 1; l <= NUM_ITERATIONS; ++l) {
//     // this is memory leaking!
//     resBigInt = big_integer_multiply(resBigInt, facBigInt);
//     big_integer_increment(&facBigInt, 1);
//   }
//   end = clock();
//   cpuTime = ((double)(end - start)) / CLOCKS_PER_SEC;
//   printf("BigInteger did factorial(%d) in %f seconds.\n", NUM_ITERATIONS,
//          cpuTime);
//   printf("BigInteger last value was: %lld\n",
//          big_integer_to_long_long(resBigInt));
//   big_integer_destroy(&resBigInt);
//   big_integer_destroy(&facBigInt);
//
//   double d;
//   start = clock();
//   double resDouble = 1;
//   double facDouble = 1;
//   for (d = 1; d <= NUM_ITERATIONS; ++d) {
//     resDouble *= facDouble;
//     facDouble++;
//   }
//   end = clock();
//   cpuTime = ((double)(end - start)) / CLOCKS_PER_SEC;
//   printf("Double did factorial(%d) in %f seconds.\n", NUM_ITERATIONS,
//   cpuTime); printf("Double last value was: %f\n", resDouble);
// };

int main(int argc, const char **argv) {
  test_create();
  printf("test_create pass\n");
  test_set();
  printf("test_set pass\n");
  test_input_output();
  printf("test input & output with file finished\n");
  printf("need to verify with diff -bB data/simple_add.txt "
         "data/copy_simple_add.txt \n");
  test_to_int();
  printf("test_to_int pass\n");
  test_to_long_long();
  printf("test_to_long_long pass\n");
  test_compare();
  printf("test_compare pass\n");
  test_add();
  printf("test_add pass\n");
  test_add_inplace();
  printf("test_add_inplace pass\n");
  test_subtract();
  printf("test_subtract pass\n");
  test_subtract_inplace();
  printf("test_subtract_inplace pass\n");
  test_multiply();
  printf("test_multiply pass\n");
  test_multiply_opt();
  printf("test_multiply_opt pass\n");
  test_multiply_two_operands_opt();
  printf("test_multiply_two_operands_opt pass\n");
  test_multiply_inplace();
  printf("test_multiply_inplace pass\n");
  test_multiply_karatsuba();
  printf("test_multiply_karatsuba pass\n");
  test_increment();
  printf("test_increment pass\n");
  test_decrement();
  printf("test_decrement pass\n");

  // test_performance();
  // printf("test_performance done\n");
  // test_performance_factorial();
  // printf("test_performance_factorial done\n");

  printf("\nAll tests passed!\n");
  return EXIT_SUCCESS;
};