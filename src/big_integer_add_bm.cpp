#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <random>
#include <string>
#include <time.h>
#include <vector>

#include "../test/tsc_x86.h"
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
  add_function(big_integer_add_inplace, "inplace", 0);
  add_function(big_integer_add_inplace_inline, "inline", 0);
  add_function(big_integer_add_inplace_inline_intrinsics, "intrinsics", 0);
  add_function(big_integer_add_inplace_inline_intrinsics_v2, "intrinsics_v2",
               0);
  add_function(big_integer_add_inplace_inline_intrinsics_v3, "intrinsics_v3",
               0);
  add_function(big_integer_add_inplace_inline_intrinsics_v4, "intrinsics_v4",
               0);
}

#define CYCLES_REQUIRED 1e8
#define NR 32
#define CYCLES_REQUIRED 1e8
#define REP 5

#define WARM_CACHE

double perf_test(comp_func f, std::string name, int intops, int n) {
  double cycles = 0;
  long num_runs = 100;
  double multiplier = 1;
  myInt64 start, end;

  BigInteger left, right, result;
  initialize(&left, 1, n, n + 4, 0);
  initialize(&right, 1, n, n + 4, 1);
  initialize(&result, 1, n, n + 4, 2);
  do {
    num_runs = num_runs * multiplier;
    start = start_tsc();
    for (int i = 0; i < num_runs; i++) {
      f(left, right, &result);
    }
    end = stop_tsc(start);

    cycles = (double)end;
    multiplier = (CYCLES_REQUIRED) / (cycles);
  } while (multiplier > 2);

  double total_cycles = 0;
#ifndef WARM_CACHE
  // printf("cold cache scenario\n");
  std::vector<BigInteger> lefts(num_runs), rights(num_runs), results(num_runs);
  for (int i = 0; i < num_runs; ++i) {
    initialize(&lefts[i], 1, n, n + 4, i * 3);
    initialize(&rights[i], 1, n, n + 4, i * 3 + 1);
    initialize(&results[i], 1, n, n + 4, i * 3 + 2);
  }
  for (int j = 0; j < REP; j++) {
    start = start_tsc();
    for (int i = 0; i < num_runs; ++i) {
      f(lefts[i], rights[i], &results[i]);
    }
    end = stop_tsc(start);

    cycles = ((double)end) / num_runs;
    total_cycles += cycles;
  }
  total_cycles /= REP;
  for (int i = 0; i < num_runs; ++i) {
    big_integer_destroy(&lefts[i]);
    big_integer_destroy(&rights[i]);
    big_integer_destroy(&results[i]);
  }
#else
  // printf("warm cache scenario\n");
  // BigInteger left, right, result;
  // initialize(&left, 1, n, n + 4, 0);
  // initialize(&right, 1, n, n + 4, 1);
  // initialize(&result, 1, n, n + 4, 2);
  for (int j = 0; j < REP; j++) {
    start = start_tsc();
    for (int i = 0; i < num_runs; ++i) {
      f(left, right, &result);
    }
    end = stop_tsc(start);

    cycles = ((double)end) / num_runs;
    total_cycles += cycles;
  }
  total_cycles /= REP;
  big_integer_destroy(&left);
  big_integer_destroy(&right);
  big_integer_destroy(&result);
#endif
  return total_cycles;
}

int main() {
  register_functions();
  // for (int j = 0; j < numFuncs; ++j) {
  //   std::cout << funcNames[j] << "  ";
  // }
  // std::cout << std::endl;
  for (int i = 3; i <= 20; i++) {
    int n = 1 << i;
    printf("%16d  ", n);
    for (int j = 0; j < numFuncs; ++j) {
      double cycles = perf_test(userFuncs[j], funcNames[j], 0, n);
      printf("%10.2f  ", cycles);
    }
    printf("\n");
  }
  return 0;
}