#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
    BigInteger right;
    left.sign = 1;
    left.data.size = 2 * n;
    left.data.capacity = 2 * n + 1;
    left.data.bits = malloc((2 * n + 1) * 8);
    right.sign = 1;
    right.data.size = 2 * n;
    right.data.capacity = 2 * n + 1;
    right.data.bits = malloc((2 * n + 1) * 8);

    for (int j = 0; j < 2 * n; j++) {
      left.data.bits[j] = rand();
      right.data.bits[j] = rand();
    }

    int n_30bits = (int)ceil((double)n * 32 / 30);
    int n_29bits = (int)ceil((double)n * 32 / 29);

    double cycles_mul_precision_vector_16x_unfold = 0;
    double cycles_mul_precision_vector_16x_unfold_1x = 0;
    double cycles_mul_precision_vector_16x_unfold_2x = 0;
    double cycles_mul_precision_vector_16x_unfold_4x = 0;
    double cycles_mul_precision_vector_16x_unfold_8x = 0;
    cycles_mul_precision_vector_16x_unfold += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unflod(&left, right, n);
    cycles_mul_precision_vector_16x_unfold_1x += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_1x(&left, right, n_30bits);
    cycles_mul_precision_vector_16x_unfold_2x += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_2x(&left, right, n_30bits);
    cycles_mul_precision_vector_16x_unfold_4x += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_4x(&left, right, n_29bits);
    cycles_mul_precision_vector_16x_unfold_8x += rdtsc_big_integer_multiply_inplace_fixed_precision_verter_16x_unfold_reduce_intop_8x(&left, right, n_29bits);

    printf("%d %.2lf %.2lf %.2lf %.2lf %.2lf\n", 
    n, 
    cycles_mul_precision_vector_16x_unfold,
    cycles_mul_precision_vector_16x_unfold_1x,
    cycles_mul_precision_vector_16x_unfold_2x,
    cycles_mul_precision_vector_16x_unfold_4x,
    cycles_mul_precision_vector_16x_unfold_8x
    );


    big_integer_destroy(&left);
    big_integer_destroy(&right);
  }

  return 0;
}