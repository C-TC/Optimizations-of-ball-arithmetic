#include "../src/big_integer.h"
#include "tsc_x86.h"
#include <stdio.h>
#include <stdlib.h>

#define CYCLES_REQUIRED 1e8
#define MAX_FUNC 100
#define REP_COUNT 1

double rdtsc_big_integer_multiply(BigInteger left, BigInteger right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_opt(BigInteger left, BigInteger right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_opt(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_opt(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_two_operands_opt(BigInteger left,
                                                   BigInteger right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_two_operands_opt(&left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_two_operands_opt(&left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

int main() {
  // testing
  for (int i = 3; i <= 12; i++) {
    int n = 1 << i;

    BigInteger left;
    BigInteger right;
    left.sign = 1;
    left.data.size = n;
    left.data.capacity = n + 1;
    left.data.bits = malloc((n + 1) * sizeof(unsigned int));
    right.sign = 1;
    right.data.size = n;
    right.data.capacity = n + 1;
    right.data.bits = malloc((n + 1) * sizeof(unsigned int));
    for (int j = 0; j < n; j++) {
      left.data.bits[j] = rand();
      right.data.bits[j] = rand();
    }

    double cycles_mul = 0;
    double cycles_mul_opt = 0;
    double cycles_mul_two_operands_opt = 0;
    double cycles_mul_precision = 0;
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_mul += rdtsc_big_integer_multiply(left, right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_mul_opt += rdtsc_big_integer_multiply_opt(left, right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_mul_two_operands_opt +=
          rdtsc_big_integer_multiply_two_operands_opt(left, right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_mul_precision +=
          rdtsc_big_integer_multiply_inplace_fixed_precision(&left, right, n);
    }
    printf("%10d %16.2lf %16.2lf %16.2lf %16.2lf\n", n, cycles_mul / REP_COUNT,
           cycles_mul_opt / REP_COUNT, cycles_mul_two_operands_opt / REP_COUNT,
           cycles_mul_precision / REP_COUNT);
  }
  return 0;
}