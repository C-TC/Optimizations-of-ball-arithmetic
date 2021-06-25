#include "ball.h"
#include "ball_helper.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// #include "tsc_x86.h"
Ball ball_add(Ball lo, Ball ro) {
  Ball ans;
  ans.center = big_float_add(lo.center, ro.center);
  ans.radius = lo.radius + ro.radius;
  return ans;
}
Ball ball_sub(Ball lo, Ball ro) {
  Ball ans;
  ans.center = big_float_sub(lo.center, ro.center);
  ans.radius = lo.radius + ro.radius;
  return ans;
}
Ball ball_multiply(Ball lo, Ball ro) {
  Ball ans;
  ans.center = big_float_multiply(lo.center, ro.center);
  // ans.radius = dl*r + dr*l + l*r
  ans.radius = fabs(big_float_to_double(lo.center)) * ro.radius +
               fabs(big_float_to_double(ro.center)) * lo.radius +
               lo.radius * ro.radius;
  return ans;
}
Ball ball_div(Ball lo, Ball ro) {
  Ball ans;
  // TODO: what about ro = 0? how to define infinity in bigfloat?
  ans.center = big_float_div(lo.center, ro.center);
  double ans_center_abs_value = fabs(big_float_to_double(ans.center));
  double ro_abs_value = fabs(big_float_to_double(ro.center));
  // TODO: optimize this
  // ans.radius = l/r + l*dr/r^2 + dl/r + dl*dr/r^2
  ans.radius =
      ans_center_abs_value +
      ans_center_abs_value / fabs(big_float_to_double(ro.center)) * ro.radius +
      lo.radius / ro_abs_value +
      lo.radius * ro.radius / ro_abs_value / ro_abs_value;
  return ans;
}

void ball_add_inplace_fixed_precision(Ball *lo, Ball ro, const int precision) {
  big_float_add_inplace_fixed_precision(&lo->center, ro.center, precision);
  lo->radius = lo->radius + ro.radius;
}
void ball_multiply_inplace_fixed_precision(Ball *lo, Ball ro,
                                           const int precision) {
  big_float_mul_inplace_fixed_precision(&lo->center, ro.center, precision);
  lo->radius = fabs(big_float_to_double(lo->center)) * ro.radius +
               fabs(big_float_to_double(ro.center)) * lo->radius +
               lo->radius * ro.radius;
}

Ball ball_add_quad_double(Ball lo, Ball ro) {
  Ball ans;
  ans.radius = lo.radius + ro.radius;

  Ball *op_large;
  Ball *op_small;
  if (lo.center.power >= ro.center.power) {
    op_large = &lo;
    op_small = &ro;
  } else {
    op_large = &ro;
    op_small = &lo;
  }

  int pow_diff = op_large->center.power - op_small->center.power;
  if (pow_diff >= 8 || pow_diff >= op_small->center.mantissa.data.size) {
    ans.center = big_float_deep_copy(op_large->center);
    return ans;
  }

  if (op_small->center.mantissa.sign == 0) {
    ans.center = big_float_deep_copy(op_large->center);
    return ans;
  }

  ans.center.power = op_large->center.power;
  ans.center.mantissa.sign = op_large->center.mantissa.sign;
  ans.center.mantissa.data.size = 8;
  ans.center.mantissa.data.capacity = 9;
  ans.center.mantissa.data.bits = calloc(9, 4);
  if (op_large->center.mantissa.sign == op_small->center.mantissa.sign) {
    // add
    int offset_large = op_large->center.mantissa.data.size - 8;
    int offset_small = op_small->center.mantissa.data.size - 8 + pow_diff;

    unsigned long long sum = 0;
    for (int i = offset_large; i < op_large->center.mantissa.data.size;
         i++, offset_small++) {
      if (i >= 0) {
        sum += op_large->center.mantissa.data.bits[i];
      }
      if (offset_small < op_small->center.mantissa.data.size &&
          offset_small >= 0) {
        sum += op_small->center.mantissa.data.bits[offset_small];
      }
      ans.center.mantissa.data.bits[i - offset_large] = (unsigned int)sum;
      sum >>= 32;
    }

    if (sum != 0) {
      // carry over
      memmove(ans.center.mantissa.data.bits, ans.center.mantissa.data.bits + 1,
              28);
      ans.center.mantissa.data.bits[7] = (unsigned int)sum;
      ans.center.power++;
    }
  } else {
    // sub
    int offset_large = op_large->center.mantissa.data.size - 8;
    int offset_small = op_small->center.mantissa.data.size - 8 + pow_diff;
    if (offset_large < 0) {
      offset_small -= offset_large;
      offset_large = 0;
    }

    if (pow_diff == 0) {
      // check which one has larger absolute value
      int compRes = 0;
      for (int k = 1; k <= 8; k++) {
        int data1 = 0;
        int data2 = 0;
        if (op_large->center.mantissa.data.size - k >= 0) {
          data1 = op_large->center.mantissa.data
                      .bits[op_large->center.mantissa.data.size - k];
        }
        if (op_small->center.mantissa.data.size - k >= 0) {
          data2 = op_small->center.mantissa.data
                      .bits[op_small->center.mantissa.data.size - k];
        }
        if (data1 > data2) {
          compRes = 1;
          break;
        } else if (data1 < data2) {
          compRes = -1;
          break;
        }
      }
      if (compRes < 0) {
        // swap them
        Ball *tmp = op_large;
        op_large = op_small;
        op_small = tmp;
      }
    }
    ans.center.mantissa.sign = op_large->center.mantissa.sign;

    unsigned long long borrow = 0;
    for (int i = offset_large; i < op_large->center.mantissa.data.size;
         i++, offset_small++) {
      /* what happens here is that, if left is less than right, borrow will
      become "negative" (not really because it is unsigned), and the bit pattern
      for that is the 1's complement (complementing it to get to 0), which is
      exactly the remainder of this term in the subtraction. */
      borrow = (borrow >> 32) & 1;
      int data1 = 0;
      int data2 = 0;
      if (i >= 0) {
        data1 = op_large->center.mantissa.data.bits[i];
      }
      if (offset_small < op_small->center.mantissa.data.size &&
          offset_small >= 0) {
        data2 = op_small->center.mantissa.data.bits[offset_small];
      }
      borrow = (unsigned long long)data1 - data2 - borrow;

      ans.center.mantissa.data.bits[i - offset_large] = (unsigned int)borrow;
      /* here we just want the first 1 after removing the lower order term */
    }

    if (borrow == 0) {
      ans.center.power--;
    }
  }
  // remove leading zeros
  for (int i = 7; i >= 0; i--) {
    if (ans.center.mantissa.data.bits[i] == 0) {
      ans.center.mantissa.data.size--;
    } else {
      break;
    }
  }

  return ans;
}

Ball ball_multiply_quad_double(Ball lo, Ball ro) {
  Ball ans;

  if (lo.center.mantissa.sign == 0 || ro.center.mantissa.sign == 0) {
    // one of the input center is 0
    ans.center.power = 0;
    ans.center.mantissa.sign = 0;
    ans.center.mantissa.data.size = 1;
    ans.center.mantissa.data.capacity = 2;
    ans.center.mantissa.data.bits = calloc(2, 4);
  } else {
    unsigned int *tmp = calloc(16, 4);
    int offset_tmp;

    offset_tmp = lo.center.mantissa.data.size - 8;
    int offset_left = offset_tmp >= 0 ? offset_tmp : 0;
    offset_tmp = ro.center.mantissa.data.size - 8;
    int offset_right = offset_tmp >= 0 ? offset_tmp : 0;

    for (int j = offset_right; j < ro.center.mantissa.data.size; j++) {
      unsigned long long carry = 0;
      for (int i = offset_left; i < lo.center.mantissa.data.size; i++) {
        int idx = j - offset_right + i - offset_left;
        carry += (unsigned long long)lo.center.mantissa.data.bits[i] *
                     ro.center.mantissa.data.bits[j] +
                 tmp[idx];
        tmp[idx] = (unsigned int)carry;
        carry >>= 32;
      }
      int idx = j - offset_right + 8;
      carry += tmp[idx];
      tmp[idx] = (unsigned int)carry;
    }

    ans.center.power = lo.center.power + ro.center.power;
    ans.center.mantissa.sign =
        lo.center.mantissa.sign * ro.center.mantissa.sign;
    ans.center.mantissa.data.size = 8;
    ans.center.mantissa.data.capacity = 9;
    ans.center.mantissa.data.bits = calloc(9, 4);

    int offset = 8;
    for (int i = 15; i >= 8; i--) {
      if (tmp[i] != 0) {
        break;
      } else {
        offset--;
      }
    }
    memmove(ans.center.mantissa.data.bits, tmp + offset, 32);
    for (int i = 7; i >= 0; i--) {
      if (ans.center.mantissa.data.bits[i] == 0) {
        ans.center.mantissa.data.size--;
      } else {
        break;
      }
    }

    if (lo.center.mantissa.data.size + ro.center.mantissa.data.size !=
        ans.center.mantissa.data.size) {
      ans.center.power--;
    }

    free(tmp); // release temporal variable
  }

  // ans.radius = dl*r + dr*l + l*r
  ans.radius = fabs(big_float_to_double(lo.center)) * ro.radius +
               fabs(big_float_to_double(ro.center)) * lo.radius +
               lo.radius * ro.radius;
  return ans;
}

typedef enum { BALL_ADD, BALL_SUB, BALL_MUL, BALL_DIV } test_type_ball;
void test_ball(double lo, double ro, test_type_ball t) {
  /*
printf("\n\n----------------ball ");
switch (t) {
case BALL_ADD:
  printf("add");
  break;
case BALL_SUB:
  printf("sub");
  break;
case BALL_MUL:
  printf("mul");
  break;
case BALL_DIV:
  printf("div");
  break;
default:
  exit(-1);
}
printf(" routine----------------\n");
*/
  Ball b1 = double_to_ball(lo);
  Ball b2 = double_to_ball(ro);
  // printf("input 1: %e\n", lo);
  // ball_print(b1);
  // printf("input 2: %e\n", ro);
  // ball_print(b2);
  Ball b3;
  switch (t) {
  case BALL_ADD:
    for (int i = 0; i < (1 << 20); ++i) {
      b3 = ball_add(b1, b2);
    }
    break;
  case BALL_SUB:
    for (int i = 0; i < (1 << 20); ++i) {
      b3 = ball_sub(b1, b2);
    }
    break;
  case BALL_MUL:
    for (int i = 0; i < (1 << 20); ++i) {
      b3 = ball_multiply(b1, b2);
    }
    break;
  case BALL_DIV:
    for (int i = 0; i < (1 << 20); ++i) {
      b3 = ball_div(b1, b2);
    }
    break;
  default:
    exit(-1);
  }
  /*
  printf("output: \n");
  ball_print(b3);
  printf("transformed back to double: %e\n", ball_to_double(b3));
  switch (t) {
  case BALL_ADD:
    printf("reference: %e\n", lo + ro);
    break;
  case BALL_SUB:
    printf("reference: %e\n", lo - ro);
    break;
  case BALL_MUL:
    printf("reference: %e\n", lo * ro);
    break;
  case BALL_DIV:
    printf("reference: %e\n", lo / ro);
    break;
  default:
    exit(-1);
  }
  */
  ball_destory(&b1);
  ball_destory(&b2);
  ball_destory(&b3);
}

int main() {
  srand(time(NULL));
  // double fd1 = (double)rand() / RAND_MAX * rand();
  // double fd2 = (double)rand() / RAND_MAX * rand();
  // BigFloat f1 = double_to_big_float(fd1);
  // BigFloat f2 = double_to_big_float(fd2);
  // BigFloat f3;
  // for (int i = 0; i < (1 << 6); ++i) {
  //   // f3 = big_float_add(f1, f2);
  //   // f3 = big_float_multiply(f1, f2);
  //   f3 = big_float_div(f1, f2);
  // }
  // big_float_print(f3);

  double bd1 = (double)rand() / RAND_MAX * rand();
  double bd2 = (double)rand() / RAND_MAX * rand();
  Ball b1 = double_to_ball(bd1);
  Ball b2 = double_to_ball(bd2);
  Ball b3;
  for (int i = 0; i < (1 << 6); ++i) {
    // b3 = ball_add(b1, b2);
    b3 = ball_div(b1, b2);
    // b3 = ball_div(b1, b2);
  }
  // ball_print(b3);

  // double testdata = -1e300;
  // BigFloat test1 = double_to_big_float(testdata);
  // printf("input double: %e\n",testdata);
  // big_float_print(test1);
  // printf("transformed back to double: %e\n",big_float_to_double(test1));
  // big_float_destroy(&test1);

  // double testdata2 = -1e-300;
  // Ball test2 = double_to_ball(testdata2);
  // printf("input double: %e\n",testdata2);
  // ball_print(test2);
  // printf("transformed back to double:
  // %e\n",big_float_to_double(test2.center)); ball_destory(&test2);

  // double testdata3 = -1e-300;
  // Ball test3 = double_to_ball(testdata3);
  // printf("input double: %e\n",testdata3);
  // ball_print(test3);
  // printf("transformed back to double: %e\n",ball_to_double(test3));
  // ball_destory(&test3);

  // for (int i = 0; i < (1 << 20); ++i) {
  // test_ball(1.0e3, 2.1e2, BALL_ADD);
  // }
  // test_ball(1.0e3,2.1e2,BALL_ADD);
  // test_ball(-1.0e3,1e3,BALL_ADD);
  // test_ball(1.0e300,2.1e301,BALL_ADD);

  // test_ball(1.0e3,2.1e2,BALL_MUL);
  // test_ball(-1.0e3,1e3,BALL_MUL);
  // test_ball(1.0e30,2.1e50,BALL_MUL);

  // test_ball(1.0e3,2.1e2,BALL_DIV);
  // test_ball(-1.0e3,1e3,BALL_DIV);
  // test_ball(1.0e30,2.1e50,BALL_DIV);

  // double testdata1 = 9.999e102,testdata2=-1e90;
  // int precision = 2;
  // BigFloat test1 = double_to_big_float(testdata1),test2 =
  // double_to_big_float(testdata2); big_float_print(test1);
  // big_float_print(test2);
  // big_float_add_inplace_fixed_precision(&test1,test2,precision);
  // big_float_print(test1);
  // printf("transformed back to double: %e\n",big_float_to_double(test1));
  // big_float_destroy(&test1);
  // big_float_destroy(&test2);
}
