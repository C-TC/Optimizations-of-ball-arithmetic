#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "big_integer.h"
#include "../test/tsc_x86.h"

#define CYCLES_REQUIRED 1e8


double rdtsc_big_integer_sum_2(
  const BigInteger op1, const BigInteger op2,
  int precision, BigInteger* res
) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_2(
        op1, op2,
        precision, res
      );
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
      big_integer_sum_2(
        op1, op2,
        precision, res
      );
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_4(
  const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, 
  int precision, BigInteger* res
) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_4(
        op1, op2, op3, op4,
        precision, res
      );
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
      big_integer_sum_4(
        op1, op2, op3, op4,
        precision, res
      );
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_8(
  const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, 
  const BigInteger op5, const BigInteger op6, const BigInteger op7, const BigInteger op8, 
  int precision, BigInteger* res
) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_8(
        op1, op2, op3, op4,
        op5, op6, op7, op8,
        precision, res
      );
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
      big_integer_sum_8(
        op1, op2, op3, op4,
        op5, op6, op7, op8,
        precision, res
      );
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_12(
  const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, 
  const BigInteger op5, const BigInteger op6, const BigInteger op7, const BigInteger op8, 
  const BigInteger op9, const BigInteger op10, const BigInteger op11, const BigInteger op12, 
  int precision, BigInteger* res
) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_12(
        op1, op2, op3, op4,
        op5, op6, op7, op8,
        op9, op10, op11, op12,
        precision, res
      );
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
      big_integer_sum_12(
        op1, op2, op3, op4,
        op5, op6, op7, op8,
        op9, op10, op11, op12,
        precision, res
      );
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_16(
  const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, 
  const BigInteger op5, const BigInteger op6, const BigInteger op7, const BigInteger op8, 
  const BigInteger op9, const BigInteger op10, const BigInteger op11, const BigInteger op12, 
  const BigInteger op13, const BigInteger op14, const BigInteger op15, const BigInteger op16, 
  int precision, BigInteger* res
) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_16(
        op1, op2, op3, op4,
        op5, op6, op7, op8,
        op9, op10, op11, op12,
        op13, op14, op15, op16,
        precision, res
      );
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
      big_integer_sum_16(
        op1, op2, op3, op4,
        op5, op6, op7, op8,
        op9, op10, op11, op12,
        op13, op14, op15, op16,
        precision, res
      );
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_8_unfold_1x(
  const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, 
  const BigInteger op5, const BigInteger op6, const BigInteger op7, const BigInteger op8, 
  int precision, BigInteger* res
) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_8_unfold_1x(
        op1, op2, op3, op4,
        op5, op6, op7, op8,
        precision, res
      );
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
      big_integer_sum_8_unfold_1x(
        op1, op2, op3, op4,
        op5, op6, op7, op8,
        precision, res
      );
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}

double rdtsc_big_integer_sum_8_unfold_2x(
  const BigInteger op1, const BigInteger op2, const BigInteger op3, const BigInteger op4, 
  const BigInteger op5, const BigInteger op6, const BigInteger op7, const BigInteger op8, 
  int precision, BigInteger* res
) {
  int i, num_runs = 1;
  myInt64 cycles;
  myInt64 start;

  while (num_runs < (1 << 14)) {
    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
      big_integer_sum_8_unfold_2x(
        op1, op2, op3, op4,
        op5, op6, op7, op8,
        precision, res
      );
    }
    cycles = stop_tsc(start);
    if (cycles >= CYCLES_REQUIRED)
      break;
    num_runs *= 2;
  }

  start = start_tsc();
  for (i = 0; i < num_runs; ++i) {
      big_integer_sum_8_unfold_2x(
        op1, op2, op3, op4,
        op5, op6, op7, op8,
        precision, res
      );
  }
  cycles = stop_tsc(start);
  return (double)cycles / num_runs;
}


int main(){
  for (int i = 3; i <= 19; i++) {
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


    double cycles[] = {0, 0, 0, 0, 0, 0, 0};
    cycles[0] += rdtsc_big_integer_sum_2(
      ops[0], ops[1],
      ops[0].data.size, &res      
    );
    cycles[1] += rdtsc_big_integer_sum_4(
      ops[0], ops[1], ops[2], ops[3],
      ops[0].data.size, &res      
    );
    cycles[2] += rdtsc_big_integer_sum_8(
      ops[0], ops[1], ops[2], ops[3],
      ops[4], ops[5], ops[6], ops[7],
      ops[0].data.size, &res      
    );
    cycles[3] += rdtsc_big_integer_sum_12(
      ops[0], ops[1], ops[2], ops[3],
      ops[4], ops[5], ops[6], ops[7],
      ops[8], ops[9], ops[10], ops[11],
      ops[0].data.size, &res      
    );
    cycles[4] += rdtsc_big_integer_sum_16(
      ops[0], ops[1], ops[2], ops[3],
      ops[4], ops[5], ops[6], ops[7],
      ops[8], ops[9], ops[10], ops[11],
      ops[12], ops[13], ops[14], ops[15], 
      ops[0].data.size, &res      
    );
    cycles[5] += rdtsc_big_integer_sum_8_unfold_1x(
      ops[0], ops[1], ops[2], ops[3],
      ops[4], ops[5], ops[6], ops[7],
      ops[0].data.size, &res      
    );
    cycles[6] += rdtsc_big_integer_sum_8_unfold_2x(
      ops[0], ops[1], ops[2], ops[3],
      ops[4], ops[5], ops[6], ops[7],
      ops[0].data.size, &res      
    );

    printf("%d %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf\n", 
    n, 
    cycles[0],
    cycles[1],
    cycles[2],
    cycles[3],
    cycles[4],
    cycles[5],
    cycles[6]
    );
    for(int i=0;i<num_op;i++){
      big_integer_destroy(ops + i);
    }
    big_integer_destroy(&res);
  }

  return 0;
}