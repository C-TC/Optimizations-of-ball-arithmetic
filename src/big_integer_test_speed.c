#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "big_integer.h"
#include "../test/tsc_x86.h"

#define CYCLES_REQUIRED 1e8

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

double rdtsc_big_integer_multiply_inplace_fixed_precision_unfold(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_unflod(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_unflod(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision_verter_1x_unflod(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_verter_1x_unfold(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_verter_1x_unfold(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision_verter_2x_unflod(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_verter_2x_unfold(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_verter_2x_unfold(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision_verter_4x_unflod(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_verter_4x_unfold(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_verter_4x_unfold(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision_verter_8x_unflod(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_verter_8x_unfold(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_verter_8x_unfold(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision_verter_12x_unflod(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_verter_12x_unfold(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_verter_12x_unfold(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unflod(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_verter_16x_unfold(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_verter_16x_unfold(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_1x(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_1x(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_1x(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_2x(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_2x(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_2x(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_4x(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_4x(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_4x(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_8x(BigInteger *left,
                                                          BigInteger right,
                                                          int precision) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_8x(left, right, precision);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_8x(left, right, precision);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}


int main(){

  for (int i = 3; i <= 16; i++) {
    int n = 1 << i;

    BigInteger left;
    BigInteger left2;
    BigInteger left3;
    BigInteger left4;
    BigInteger left5;
    BigInteger left6;
    BigInteger left7;
    BigInteger left8;
    BigInteger right;
    left.sign = 1;
    left.data.size = n;
    left.data.capacity = n + 1;
    left.data.bits = malloc((n + 1) * 8);
    left2.sign = 1;
    left2.data.size = n;
    left2.data.capacity = n + 1;
    left2.data.bits = malloc((n + 1) * 8);
    left3.sign = 1;
    left3.data.size = n;
    left3.data.capacity = n + 1;
    left3.data.bits = malloc((n + 1) * 8);
    left4.sign = 1;
    left4.data.size = n;
    left4.data.capacity = n + 1;
    left4.data.bits = malloc((n + 1) * 8);
    left5.sign = 1;
    left5.data.size = n;
    left5.data.capacity = n + 1;
    left5.data.bits = malloc((n + 1) * 8);
    left6.sign = 1;
    left6.data.size = n;
    left6.data.capacity = n + 1;
    left6.data.bits = malloc((n + 1) * 8);
    left7.sign = 1;
    left7.data.size = n;
    left7.data.capacity = n + 1;
    left7.data.bits = malloc((n + 1) * 8);
    left8.sign = 1;
    left8.data.size = n;
    left8.data.capacity = n + 1;
    left8.data.bits = malloc((n + 1) * 8);
    right.sign = 1;
    right.data.size = n;
    right.data.capacity = n + 1;
    right.data.bits = malloc((n + 1) * 8);

    // memset(left.data.bits, 1, 8 * n);
    // memset(right.data.bits, 1, 8 * n);
    for (int j = 0; j < n; j++) {
      left.data.bits[j] = rand();
      right.data.bits[j] = rand();
    }

    memcpy(left2.data.bits, left.data.bits, 8 * n);
    memcpy(left3.data.bits, left.data.bits, 8 * n);
    memcpy(left4.data.bits, left.data.bits, 8 * n);
    memcpy(left5.data.bits, left.data.bits, 8 * n);
    memcpy(left6.data.bits, left.data.bits, 8 * n);
    memcpy(left7.data.bits, left.data.bits, 8 * n);
    memcpy(left8.data.bits, left.data.bits, 8 * n);


    double cycles_mul_naive = 0;
    double cycles_mul_precision = 0;
    double cycles_mul_precision_unfold = 0;
    double cycles_mul_precision_vector_1x_unfold = 0;
    double cycles_mul_precision_vector_2x_unfold = 0;
    double cycles_mul_precision_vector_4x_unfold = 0;
    double cycles_mul_precision_vector_8x_unfold = 0;
    double cycles_mul_precision_vector_12x_unfold = 0;
    double cycles_mul_precision_vector_16x_unfold = 0;
    cycles_mul_naive += rdtsc_big_integer_multiply(left, right);
    cycles_mul_precision += rdtsc_big_integer_multiply_inplace_fixed_precision(&left, right, n);
    cycles_mul_precision_unfold += rdtsc_big_integer_multiply_inplace_fixed_precision_unfold(&left3, right, n);
    cycles_mul_precision_vector_1x_unfold += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_1x_unflod(&left3, right, n);
    cycles_mul_precision_vector_2x_unfold += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_2x_unflod(&left4, right, n);
    cycles_mul_precision_vector_4x_unfold += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_4x_unflod(&left5, right, n);
    cycles_mul_precision_vector_8x_unfold += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_8x_unflod(&left6, right, n);
    cycles_mul_precision_vector_12x_unfold += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_12x_unflod(&left7, right, n);
    cycles_mul_precision_vector_16x_unfold += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unflod(&left8, right, n);
    printf("%d %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf\n", 
    n, 
    cycles_mul_naive, 
    cycles_mul_precision, 
    cycles_mul_precision_unfold,
    cycles_mul_precision_vector_1x_unfold,
    cycles_mul_precision_vector_2x_unfold,
    cycles_mul_precision_vector_4x_unfold,
    cycles_mul_precision_vector_8x_unfold,
    cycles_mul_precision_vector_12x_unfold,
    cycles_mul_precision_vector_16x_unfold
    );

/*
    double cycles_mul_precision_vector_16x_unfold = 0;
    double cycles_mul_precision_vector_16x_unfold_1x = 0;
    double cycles_mul_precision_vector_16x_unfold_2x = 0;
    double cycles_mul_precision_vector_16x_unfold_4x = 0;
    double cycles_mul_precision_vector_16x_unfold_8x = 0;
    cycles_mul_precision_vector_16x_unfold += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unflod(&left, right, n);
    cycles_mul_precision_vector_16x_unfold_1x += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_1x(&left2, right, n);
    cycles_mul_precision_vector_16x_unfold_2x += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_2x(&left3, right, n);
    cycles_mul_precision_vector_16x_unfold_4x += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_4x(&left4, right, n);
    cycles_mul_precision_vector_16x_unfold_8x += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_8x(&left5, right, n);

    printf("%d %.2lf %.2lf %.2lf %.2lf %.2lf\n", 
    n, 
    cycles_mul_precision_vector_16x_unfold,
    cycles_mul_precision_vector_16x_unfold_1x,
    cycles_mul_precision_vector_16x_unfold_2x,
    cycles_mul_precision_vector_16x_unfold_4x,
    cycles_mul_precision_vector_16x_unfold_8x
    );
*/

    big_integer_destroy(&left);
    big_integer_destroy(&left2);
    big_integer_destroy(&left3);
    big_integer_destroy(&left4);
    big_integer_destroy(&left5);
    big_integer_destroy(&left6);
    big_integer_destroy(&left7);
    big_integer_destroy(&left8);
    big_integer_destroy(&right);
  }

  return 0;
}