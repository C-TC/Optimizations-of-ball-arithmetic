#include "../src/big_integer.h"
#include "../src/big_float.h"
#include "tsc_x86.h"
#include <stdio.h>
#include <stdlib.h>

#define CYCLES_REQUIRED 1e8
#define MAX_FUNC 100
#define REP_COUNT 1

double rdtsc_big_integer_add(BigInteger left, BigInteger right){
    int i, num_runs = 1;
    myInt64 cycles;
    myInt64 start;

    while(num_runs < (1 << 14)){
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            big_integer_add(left, right);
        }
        cycles = stop_tsc(start);
        if(cycles >= CYCLES_REQUIRED) break;
        num_runs *= 2;
    }

    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
        big_integer_add(left, right);
    }
    cycles = stop_tsc(start);
    return (double)cycles / num_runs;
}

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

double rdtsc_big_float_add(BigFloat left, BigFloat right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_float_add(left,right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_float_add(left,right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_float_mul(BigFloat left, BigFloat right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_float_multiply(left,right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_float_multiply(left,right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_float_div(BigFloat left, BigFloat right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_float_div(left,right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_float_div(left,right);
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
    BigFloat l,r;
    l.mantissa = left;
    l.power = 0;
    r.mantissa = right;
    r.power = 0;

    double cycles_bi_add = 0;
    double cycles_bi_mul = 0;
    double cycles_bf_add = 0;
    double cycles_bf_mul = 0;
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_bf_add += rdtsc_big_float_add(l, r);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_bf_mul += rdtsc_big_float_mul(l, r);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_bi_add += rdtsc_big_integer_add(left, right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_bi_mul += rdtsc_big_integer_multiply(left, right);
    }
    printf("%d %.2lf %.2lf %.2lf  %.2lf\n", n, cycles_bi_add / REP_COUNT,
           cycles_bf_add / REP_COUNT, cycles_bi_mul / REP_COUNT,
           cycles_bf_mul / REP_COUNT);
  }
  return 0;
}