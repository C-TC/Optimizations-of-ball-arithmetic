#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "big_integer.h"
#include "../test/tsc_x86.h"

#define CYCLES_REQUIRED 1e8

double rdtsc_big_integer_sum_2(BigInteger op1, BigInteger op2, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_2(op1, op2, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_2(op1, op2, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_3(BigInteger op1, BigInteger op2, BigInteger op3, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_3(op1, op2, op3, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_3(op1, op2, op3, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_3_unfold_1x(BigInteger op1, BigInteger op2, BigInteger op3, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_3_unfold_1x(op1, op2, op3, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_3_unfold_1x(op1, op2, op3, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_3_unfold_2x(BigInteger op1, BigInteger op2, BigInteger op3, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_3_unfold_2x(op1, op2, op3, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_3_unfold_2x(op1, op2, op3, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_3_unfold_4x(BigInteger op1, BigInteger op2, BigInteger op3, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_3_unfold_4x(op1, op2, op3, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_3_unfold_4x(op1, op2, op3, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_3_unfold_8x(BigInteger op1, BigInteger op2, BigInteger op3, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_3_unfold_8x(op1, op2, op3, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_3_unfold_8x(op1, op2, op3, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_5(BigInteger op1, BigInteger op2, BigInteger op3, BigInteger op4, BigInteger op5, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_5(op1, op2, op3, op4, op5, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_5(op1, op2, op3, op4, op5, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_5_unfold_1x(BigInteger op1, BigInteger op2, BigInteger op3, BigInteger op4, BigInteger op5, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_5_unfold_1x(op1, op2, op3, op4, op5, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_5_unfold_1x(op1, op2, op3, op4, op5, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_5_unfold_2x(BigInteger op1, BigInteger op2, BigInteger op3, BigInteger op4, BigInteger op5, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_5_unfold_2x(op1, op2, op3, op4, op5, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_5_unfold_2x(op1, op2, op3, op4, op5, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_5_unfold_4x(BigInteger op1, BigInteger op2, BigInteger op3, BigInteger op4, BigInteger op5, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_5_unfold_4x(op1, op2, op3, op4, op5, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_5_unfold_4x(op1, op2, op3, op4, op5, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_5_unfold_8x(BigInteger op1, BigInteger op2, BigInteger op3, BigInteger op4, BigInteger op5, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_5_unfold_8x(op1, op2, op3, op4, op5, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum_5_unfold_8x(op1, op2, op3, op4, op5, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum(BigInteger op[], int index_start, int index_end, int precision, BigInteger* res) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum(op, index_start, index_end, precision, res);
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
    big_integer_sum(op, index_start, index_end, precision, res);
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}


int main(){
  for (int i = 3; i <= 16; i++) {
    int n = 1 << i;

    int num_op = 16;
    BigInteger *ops = malloc(sizeof(BigInteger) * num_op);
    for(int i=0;i<num_op;i++){
      ops[i].sign = 1;
      ops[i].data.size = n;
      ops[i].data.capacity = n + 1;
      ops[i].data.bits = malloc((n + 1) * 8);
    }
    BigInteger res;
    res.sign = 1;
    res.data.size = n;
    res.data.capacity = n + 1;
    res.data.bits = malloc((n + 1) * 8);

    for (int j = 0; j < n; j++) {
      for(int i=0;i<num_op;i++){
        ops[i].data.bits[j] = rand();
      }
    }


    double cycles_add = 0;
    double cycles[] = {0, 0, 0, 0, 0};
    cycles_add += rdtsc_big_integer_sum_2(ops[0], ops[1], ops[0].data.size, &res);
    cycles[0] += rdtsc_big_integer_sum(ops, 0, 2, ops[0].data.size, &res);
    cycles[1] += rdtsc_big_integer_sum(ops, 0, 4, ops[0].data.size, &res);
    cycles[2] += rdtsc_big_integer_sum(ops, 0, 8, ops[0].data.size, &res);
    cycles[3] += rdtsc_big_integer_sum(ops, 0, 12, ops[0].data.size, &res);
    cycles[4] += rdtsc_big_integer_sum(ops, 0, 16, ops[0].data.size, &res);

    printf("%d %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf\n", 
    n, 
    cycles_add,
    cycles[0],
    cycles[1],
    cycles[2],
    cycles[3],
    cycles[4]
    );
    for(int i=0;i<num_op;i++){
      big_integer_destroy(ops + i);
    }
    big_integer_destroy(&res);
  }

  return 0;
}