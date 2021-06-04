#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <random>
#include <string>
#include <time.h>
#include <vector>

#include "big_integer.h"
#include "big_integer_add.h"

void rand_uint(unsigned long *m, int n, int seed) {
  // std::random_device rd;
  std::default_random_engine generator(seed);
  std::uniform_int_distribution<unsigned long> distribution(0, 0xFFFFFFFF);
  for (int i = 0; i < n; ++i) {
    m[i] = distribution(generator);
  }
}

void initialize(BigInteger *pBigInt, int sign, int size, int capacity,
                int seed) {
  pBigInt->sign = sign;
  pBigInt->data.size = size;
  pBigInt->data.capacity = capacity;
  pBigInt->data.bits = (unsigned long *)calloc(capacity, sizeof(unsigned long));
  rand_uint(pBigInt->data.bits, size, seed);
  // for (int i = 0; i < size; ++i) {
  //   // pBigInt->data.bits[i] = (unsigned long)rand() % UINT_MAX;
  //   // assert(pBigInt->data.bits[i] <= UINT_MAX);
  //   pBigInt->data.bits[i] = UINT_MAX;
  // }
}

typedef void (*comp_func)(const BigInteger, const BigInteger, BigInteger *);

std::vector<comp_func> userFuncs;
std::vector<std::string> funcNames;
std::vector<int> funcIntOps;
int numFuncs = 0;

void add_function(comp_func f, std::string name, int intops) {
  userFuncs.push_back(f);
  funcNames.push_back(name);
  funcIntOps.push_back(intops);
  numFuncs += 1;
}

void register_functions() {
  add_function(big_integer_add_inplace_inline, "inplace", 0);
  add_function(big_integer_add_inplace_inline_intrinsics, "inplace_intrinsincs",
               0);
}

int main() {
  BigInteger left1, left2, right1, right2;
  BigInteger gt1, gt2, gt3, gt4, gt5, gt6;
  BigInteger result1, result2, result3, result4, result5, result6;
  int size = 1 << 3;
  initialize(&left1, 1, size, size + 4, 0);
  initialize(&left2, -1, size, size + 4, 1);
  initialize(&right1, 1, size, size + 4, 2);
  initialize(&right2, -1, size, size + 4, 3);
  initialize(&gt1, 1, size + 4, size + 8, 4);
  initialize(&gt2, 1, size + 4, size + 8, 4);
  initialize(&gt3, 1, size + 4, size + 8, 4);
  initialize(&gt4, 1, size + 4, size + 8, 4);
  initialize(&gt5, 1, size + 4, size + 8, 4);
  initialize(&gt6, 1, size + 4, size + 8, 4);
  initialize(&result1, 1, size + 4, size + 8, 5);
  initialize(&result2, 1, size + 4, size + 8, 5);
  initialize(&result3, 1, size + 4, size + 8, 5);
  initialize(&result4, 1, size + 4, size + 8, 5);
  initialize(&result5, 1, size + 4, size + 8, 5);
  initialize(&result6, 1, size + 4, size + 8, 5);
  big_integer_add_inplace(left1, right1, &gt1);
  big_integer_add_inplace(left1, right2, &gt2);
  big_integer_add_inplace(left2, right1, &gt3);
  big_integer_add_inplace(left2, right2, &gt4);
  big_integer_add_inplace(right2, left1, &gt5);
  big_integer_add_inplace(right1, left2, &gt6);

  register_functions();
  for (int i = 0; i < numFuncs; ++i) {
    comp_func f = userFuncs[i];
    f(left1, right1, &result1);
    assert(big_integer_compare(gt1, result1) == 0);
    f(left1, right2, &result2);
    // big_integer_print(left1, "left1");
    // big_integer_print(right1, "right1");
    // big_integer_print(result1, "result1");
    // big_integer_print(left1, "left1");
    // big_integer_print(right2, "right2");
    // big_integer_print(gt2, "gt2: ");
    // big_integer_print(result2, "result2: ");
    assert(big_integer_compare(gt2, result2) == 0);
    f(left2, right1, &result3);
    assert(big_integer_compare(gt3, result3) == 0);
    f(left2, right2, &result4);
    assert(big_integer_compare(gt4, result4) == 0);
    f(right2, left1, &result5);
    assert(big_integer_compare(gt5, result5) == 0);
    f(right1, left2, &result6);
    assert(big_integer_compare(gt6, result6) == 0);
    std::cout << funcNames[i] << " verified!" << std::endl;
  }
  printf("all tests passed\n");

  // big_integer_add_inplace(left1, right1, &result1);
  // big_integer_add_inplace_inline(left1, right1, &result2);
  // big_integer_print(left1, "left1: ");
  // big_integer_print(right1, "right1: ");
  // big_integer_print(result1, "result1: ");
  // big_integer_print(result2, "result2: ");
  // assert(big_integer_compare(result1, result2) == 0);
  // big_integer_add_inplace(left1, right2, &result1);
  // big_integer_add_inplace_inline(left1, right2, &result2);
  // assert(big_integer_compare(result1, result2) == 0);
  // big_integer_add_inplace(left2, right1, &result1);
  // big_integer_add_inplace_inline(left2, right1, &result2);
  // assert(big_integer_compare(result1, result2) == 0);
  // big_integer_add_inplace(left2, right2, &result1);
  // big_integer_add_inplace_inline(left2, right2, &result2);
  // assert(big_integer_compare(result1, result2) == 0);

  // big_integer_set(0, &result1);
  // big_integer_set(0, &result2);
  // big_integer_add_inplace(left1, right1, &result1);
  // big_integer_add_inplace_inline_unfold_1x(left1, right1, &result2);
  // assert(big_integer_compare(result1, result2) == 0);

  // big_integer_set(0, &result1);
  // big_integer_set(0, &result2);
  // big_integer_add_inplace(left1, right1, &result1);
  // big_integer_add_inplace_inline_unfold_1x_intrinsics(left1, right1,
  // &result2); assert(big_integer_compare(result1, result2) == 0);

  // big_integer_set(0, &result1);
  // big_integer_set(0, &result2);
  // big_integer_add_inplace(left1, right1, &result1);
  // big_integer_add_inplace_inline_unfold_2x_intrinsics(left1, right1,
  // &result2); assert(big_integer_compare(result1, result2) == 0);
}
