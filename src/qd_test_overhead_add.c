#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "quad_double.h"
#include "../test/tsc_x86.h"

#define CYCLES_REQUIRED 1e8
#define REP_COUNT 1

// add

double rdtsc_qd_add(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_inline_x2(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_inline_x2(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_inline_x2(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_inline_x3(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_inline_x3(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_inline_x3(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_inline_x4(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_inline_x4(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_inline_x4(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_inline_x6(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_inline_x6(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_inline_x6(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_inline_x8(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_inline_x8(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_inline_x8(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_inline_x2_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_inline_x2_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_inline_x2_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_inline_x3_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_inline_x3_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_inline_x3_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_inline_x4_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_inline_x4_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_inline_x4_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_inline_x6_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_inline_x6_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_inline_x6_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_qd_add_inplace_vec_inline_x8_overhead(qd_arr left, qd_arr right) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      qd_arr_add_inplace_vec_inline_x8_overhead(left, right);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    qd_arr_add_inplace_vec_inline_x8_overhead(left, right);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

int main() {
  // testing
  FILE *f;
  
  for (int i = 3; i <= 17; i++) { //17 -> 8MB out of L3
    int n = 1 << i;
    srand(12);
    qd_arr left = qd_arr_create_random_aligned(n, -1, 1);
    qd_arr right = qd_arr_create_random_aligned(n, -1e-10, 1e-10);

    double cycles_qd_add = 0;
    double cycles_qd_add_inplace = 0;
    double cycles_qd_add_inplace_vec = 0;
    double cycles_qd_add_inplace_vec_inline_x2 = 0;
    double cycles_qd_add_inplace_vec_inline_x3 = 0;
    double cycles_qd_add_inplace_vec_inline_x4 = 0;
    double cycles_qd_add_inplace_vec_inline_x6 = 0;
    double cycles_qd_add_inplace_vec_inline_x8 = 0;
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_add += rdtsc_qd_add(left,right);
      cycles_qd_add -= rdtsc_qd_add_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_add_inplace += rdtsc_qd_add_inplace(left,right);
      cycles_qd_add_inplace -= rdtsc_qd_add_inplace_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_add_inplace_vec += rdtsc_qd_add_inplace_vec(left,right);
      cycles_qd_add_inplace_vec -= rdtsc_qd_add_inplace_vec_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_add_inplace_vec_inline_x2 += rdtsc_qd_add_inplace_vec_inline_x2(left,right);
      cycles_qd_add_inplace_vec_inline_x2 -= rdtsc_qd_add_inplace_vec_inline_x2_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_add_inplace_vec_inline_x3 += rdtsc_qd_add_inplace_vec_inline_x3(left,right);
      cycles_qd_add_inplace_vec_inline_x3 -= rdtsc_qd_add_inplace_vec_inline_x3_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_add_inplace_vec_inline_x4 += rdtsc_qd_add_inplace_vec_inline_x4(left,right);
      cycles_qd_add_inplace_vec_inline_x4 -= rdtsc_qd_add_inplace_vec_inline_x4_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_add_inplace_vec_inline_x6 += rdtsc_qd_add_inplace_vec_inline_x6(left,right);
      cycles_qd_add_inplace_vec_inline_x6 -= rdtsc_qd_add_inplace_vec_inline_x6_overhead(left,right);
    }
    for (int j = 0; j < REP_COUNT; j++) {
      cycles_qd_add_inplace_vec_inline_x8 += rdtsc_qd_add_inplace_vec_inline_x8(left,right);
      cycles_qd_add_inplace_vec_inline_x8 -= rdtsc_qd_add_inplace_vec_inline_x8_overhead(left,right);
    }
    printf("%d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n",
    n, 
    cycles_qd_add / REP_COUNT, 
    cycles_qd_add_inplace / REP_COUNT,
    cycles_qd_add_inplace_vec / REP_COUNT,
    cycles_qd_add_inplace_vec_inline_x2 / REP_COUNT,
    cycles_qd_add_inplace_vec_inline_x3 / REP_COUNT,
    cycles_qd_add_inplace_vec_inline_x4 / REP_COUNT,
    cycles_qd_add_inplace_vec_inline_x6 / REP_COUNT,
    cycles_qd_add_inplace_vec_inline_x8 / REP_COUNT);
    f = fopen("qd_add_noOH_speed.data", "a");
    fprintf(f, "%d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n",
    n, 
    cycles_qd_add / REP_COUNT, 
    cycles_qd_add_inplace / REP_COUNT,
    cycles_qd_add_inplace_vec / REP_COUNT,
    cycles_qd_add_inplace_vec_inline_x2 / REP_COUNT,
    cycles_qd_add_inplace_vec_inline_x3 / REP_COUNT,
    cycles_qd_add_inplace_vec_inline_x4 / REP_COUNT,
    cycles_qd_add_inplace_vec_inline_x6 / REP_COUNT,
    cycles_qd_add_inplace_vec_inline_x8 / REP_COUNT);
    fclose(f);

    f = fopen("qd_add_noOH_perf.data", "a");
    fprintf(f, "%d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n",
    n, 
    63 * n / (cycles_qd_add / REP_COUNT), 
    63 * n / (cycles_qd_add_inplace / REP_COUNT),
    63 * n / (cycles_qd_add_inplace_vec / REP_COUNT),
    63 * n / (cycles_qd_add_inplace_vec_inline_x2 / REP_COUNT),
    63 * n / (cycles_qd_add_inplace_vec_inline_x3 / REP_COUNT),
    63 * n / (cycles_qd_add_inplace_vec_inline_x4 / REP_COUNT),
    63 * n / (cycles_qd_add_inplace_vec_inline_x6 / REP_COUNT),
    63 * n / (cycles_qd_add_inplace_vec_inline_x8 / REP_COUNT));
    fclose(f);

    qd_destroy_aligned(left);
    qd_destroy_aligned(right);
  }
  return 0;
}