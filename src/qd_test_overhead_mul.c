#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "quad_double.h"
#include "../test/tsc_x86.h"

#define CYCLES_REQUIRED 1e8
#define REP_COUNT 1

// mul

double rdtsc_qd_mul(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_inplace(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_inplace(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_inplace(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_inplace_inline_vec(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_inplace_inline_vec(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_inplace_inline_vec(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_inplace_inline_vec_x2(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_inplace_inline_vec_x2(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_inplace_inline_vec_x2(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_inplace_inline_vec_x3(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_inplace_inline_vec_x3(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_inplace_inline_vec_x3(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_inplace_inline_vec_x4(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_inplace_inline_vec_x4(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_inplace_inline_vec_x4(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_inplace_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_inplace_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_inplace_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_inplace_inline_vec_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_inplace_inline_vec_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_inplace_inline_vec_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_inplace_inline_vec_x2_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_inplace_inline_vec_x2_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_inplace_inline_vec_x2_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_inplace_inline_vec_x3_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_inplace_inline_vec_x3_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_inplace_inline_vec_x3_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_mul_inplace_inline_vec_x4_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_mul_inplace_inline_vec_x4_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_mul_inplace_inline_vec_x4_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

int main() {
  // testing
  FILE *f;
  
  for (int i = 3; i <= 17; i++) { //17 -> 8MB out of L3
    int n = 1 << i;
    srand(11);
    qd_arr left = qd_arr_create_random_aligned(n, -1, 1);
    qd_arr right = qd_arr_create_random_aligned(n, 1-1e-6, 1+1e-5);

    double cycles_qd_mul = 0;
    double cycles_qd_mul_inplace = 0;
    double cycles_qd_mul_inplace_inline_vec = 0;
    double cycles_qd_mul_inplace_inline_vec_x2 = 0;
    double cycles_qd_mul_inplace_inline_vec_x3 = 0;
    double cycles_qd_mul_inplace_inline_vec_x4 = 0;
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_mul += rdtsc_qd_mul(left,right);
      cycles_qd_mul -= rdtsc_qd_mul_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_mul_inplace += rdtsc_qd_mul_inplace(left,right);
      cycles_qd_mul_inplace -= rdtsc_qd_mul_inplace_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_mul_inplace_inline_vec += rdtsc_qd_mul_inplace_inline_vec(left,right);
      cycles_qd_mul_inplace_inline_vec -= rdtsc_qd_mul_inplace_inline_vec_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_mul_inplace_inline_vec_x2 += rdtsc_qd_mul_inplace_inline_vec_x2(left,right);
      cycles_qd_mul_inplace_inline_vec_x2 -= rdtsc_qd_mul_inplace_inline_vec_x2_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_mul_inplace_inline_vec_x3 += rdtsc_qd_mul_inplace_inline_vec_x3(left,right);
      cycles_qd_mul_inplace_inline_vec_x3 -= rdtsc_qd_mul_inplace_inline_vec_x3_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_mul_inplace_inline_vec_x4 += rdtsc_qd_mul_inplace_inline_vec_x4(left,right);
      cycles_qd_mul_inplace_inline_vec_x4 -= rdtsc_qd_mul_inplace_inline_vec_x4_overhead(left,right);
    }
    printf("%d %.2f %.2f %.2f %.2f %.2f %.2f\n",
    n, 
    cycles_qd_mul / REP_COUNT, 
    cycles_qd_mul_inplace / REP_COUNT,
    cycles_qd_mul_inplace_inline_vec / REP_COUNT,
    cycles_qd_mul_inplace_inline_vec_x2 / REP_COUNT,
    cycles_qd_mul_inplace_inline_vec_x3 / REP_COUNT,
    cycles_qd_mul_inplace_inline_vec_x4 / REP_COUNT);
    f = fopen("qd_mul_noOH_speed.data", "a");
    fprintf(f, "%d %.2f %.2f %.2f %.2f %.2f %.2f\n",
    n, 
    cycles_qd_mul / REP_COUNT, 
    cycles_qd_mul_inplace / REP_COUNT,
    cycles_qd_mul_inplace_inline_vec / REP_COUNT,
    cycles_qd_mul_inplace_inline_vec_x2 / REP_COUNT,
    cycles_qd_mul_inplace_inline_vec_x3 / REP_COUNT,
    cycles_qd_mul_inplace_inline_vec_x4 / REP_COUNT);
    fclose(f);

    f = fopen("qd_mul_noOH_perf.data", "a");
    fprintf(f, "%d %.2f %.2f %.2f %.2f %.2f %.2f\n",
    n, 
    192 * n / (cycles_qd_mul / REP_COUNT), 
    192 * n / (cycles_qd_mul_inplace / REP_COUNT),
    192 * n / (cycles_qd_mul_inplace_inline_vec / REP_COUNT),
    192 * n / (cycles_qd_mul_inplace_inline_vec_x2 / REP_COUNT),
    192 * n / (cycles_qd_mul_inplace_inline_vec_x3 / REP_COUNT),
    192 * n / (cycles_qd_mul_inplace_inline_vec_x4 / REP_COUNT));
    fclose(f);

    qd_destroy_aligned(left);
    qd_destroy_aligned(right);
  }
  return 0;
}