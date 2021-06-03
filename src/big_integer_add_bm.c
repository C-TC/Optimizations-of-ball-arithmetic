#include "../test/tsc_x86.h"
#include "big_integer.h"
#include "big_integer_add.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define CYCLES_REQUIRED 1e8

double rdtsc_big_integer_add_inplace(const BigInteger left,
                                     const BigInteger right,
                                     BigInteger *pResult) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_add_inplace(left, right, pResult);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_add_inplace(left, right, pResult);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_add_inplace_inline(const BigInteger left,
                                            const BigInteger right,
                                            BigInteger *pResult) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_add_inplace_inline(left, right, pResult);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_add_inplace_inline(left, right, pResult);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_add_inplace_inline_unfold_1x(const BigInteger left,
                                                      const BigInteger right,
                                                      BigInteger *pResult) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_add_inplace_inline_unfold_1x(left, right, pResult);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_add_inplace_inline_unfold_1x(left, right, pResult);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_add_inplace_inline_unfold_1x_intrinsics(
    const BigInteger left, const BigInteger right, BigInteger *pResult) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_add_inplace_inline_unfold_1x_intrinsics(left, right, pResult);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_add_inplace_inline_unfold_1x_intrinsics(left, right, pResult);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_add_inplace_inline_unfold_2x_intrinsics(
    const BigInteger left, const BigInteger right, BigInteger *pResult) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_add_inplace_inline_unfold_2x_intrinsics(left, right, pResult);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_add_inplace_inline_unfold_2x_intrinsics(left, right, pResult);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

int main() {
  srand(0);

  for (int i = 3; i <= 22; i++) {
    int n = 1 << i;

    BigInteger left, left1, left2, left3, left4, left5, left6;
    BigInteger right, right1, right2, right3, right4, right5, right6;
    BigInteger result1, result2, result3, result4, result5, result6;
    left.sign = 1;
    left.data.size = n;
    left.data.capacity = n + 1;
    left.data.bits = (unsigned long *)malloc((n + 1) * 8);
    left1.sign = 1;
    left1.data.size = n;
    left1.data.capacity = n + 1;
    left1.data.bits = (unsigned long *)malloc((n + 1) * 8);
    left2.sign = 1;
    left2.data.size = n;
    left2.data.capacity = n + 1;
    left2.data.bits = (unsigned long *)malloc((n + 1) * 8);
    left3.sign = 1;
    left3.data.size = n;
    left3.data.capacity = n + 1;
    left3.data.bits = (unsigned long *)malloc((n + 1) * 8);
    left4.sign = 1;
    left4.data.size = n;
    left4.data.capacity = n + 1;
    left4.data.bits = (unsigned long *)malloc((n + 1) * 8);
    left5.sign = 1;
    left5.data.size = n;
    left5.data.capacity = n + 1;
    left5.data.bits = (unsigned long *)malloc((n + 1) * 8);
    left6.sign = 1;
    left6.data.size = n;
    left6.data.capacity = n + 1;
    left6.data.bits = (unsigned long *)malloc((n + 1) * 8);
    right.sign = 1;
    right.data.size = n;
    right.data.capacity = n + 1;
    right.data.bits = (unsigned long *)malloc((n + 1) * 8);
    right1.sign = 1;
    right1.data.size = n;
    right1.data.capacity = n + 1;
    right1.data.bits = (unsigned long *)malloc((n + 1) * 8);
    right2.sign = 1;
    right2.data.size = n;
    right2.data.capacity = n + 1;
    right2.data.bits = (unsigned long *)malloc((n + 1) * 8);
    right3.sign = 1;
    right3.data.size = n;
    right3.data.capacity = n + 1;
    right3.data.bits = (unsigned long *)malloc((n + 1) * 8);
    right4.sign = 1;
    right4.data.size = n;
    right4.data.capacity = n + 1;
    right4.data.bits = (unsigned long *)malloc((n + 1) * 8);
    right5.sign = 1;
    right5.data.size = n;
    right5.data.capacity = n + 1;
    right5.data.bits = (unsigned long *)malloc((n + 1) * 8);
    right6.sign = 1;
    right6.data.size = n;
    right6.data.capacity = n + 1;
    right6.data.bits = (unsigned long *)malloc((n + 1) * 8);

    result1.data.capacity = n + 2;
    result1.data.bits = (unsigned long *)malloc((n + 2) * 8);
    result2.data.capacity = n + 2;
    result2.data.bits = (unsigned long *)malloc((n + 2) * 8);
    result3.data.capacity = n + 2;
    result3.data.bits = (unsigned long *)malloc((n + 2) * 8);
    result4.data.capacity = n + 2;
    result4.data.bits = (unsigned long *)malloc((n + 2) * 8);
    result5.data.capacity = n + 2;
    result5.data.bits = (unsigned long *)malloc((n + 2) * 8);
    result6.data.capacity = n + 2;
    result6.data.bits = (unsigned long *)malloc((n + 2) * 8);

    for (int j = 0; j < n; j++) {
      left.data.bits[j] = rand();
      right.data.bits[j] = rand();
    }

    memcpy(left1.data.bits, left.data.bits, n * 8);
    memcpy(left2.data.bits, left.data.bits, n * 8);
    memcpy(left3.data.bits, left.data.bits, n * 8);
    memcpy(right1.data.bits, left.data.bits, n * 8);
    memcpy(right2.data.bits, left.data.bits, n * 8);
    memcpy(right3.data.bits, left.data.bits, n * 8);

    double cycles_naive = 0.0;
    double cycles_inline = 0.0;
    double cycles_unfold_1x = 0.0;
    double cycles_unfold_1x_intrinsics = 0.0;
    double cycles_unfold_2x_intrinsics = 0.0;

    int num_repeat = 5;
    for (int j = 0; j < num_repeat; ++j) {
      cycles_naive += rdtsc_big_integer_add_inplace(left1, right1, &result1);
      cycles_inline +=
          rdtsc_big_integer_add_inplace_inline(left2, right2, &result2);
      cycles_unfold_1x += rdtsc_big_integer_add_inplace_inline_unfold_1x(
          left3, right3, &result3);
      cycles_unfold_1x_intrinsics +=
          rdtsc_big_integer_add_inplace_inline_unfold_1x_intrinsics(
              left4, right4, &result4);
      cycles_unfold_2x_intrinsics +=
          rdtsc_big_integer_add_inplace_inline_unfold_2x_intrinsics(
              left5, right5, &result5);
    }
    cycles_naive /= num_repeat;
    cycles_inline /= num_repeat;
    cycles_unfold_1x /= num_repeat;
    cycles_unfold_1x_intrinsics /= num_repeat;
    cycles_unfold_2x_intrinsics /= num_repeat;
    printf("%10d\t%10.2lf\t%10.2lf\t%10.2lf\t%10.2lf\t%10.2lf\n", n,
           cycles_naive, cycles_inline, cycles_unfold_1x,
           cycles_unfold_1x_intrinsics, cycles_unfold_2x_intrinsics);
  }
  return 0;
}