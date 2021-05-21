#ifndef BALL_H
#define BALL_H
#include "big_float.h"
#include "big_float_helper.h"
#include "big_integer.h"
typedef struct Ball {
    BigFloat center;
    double radius;
} Ball;
Ball ball_add(Ball, Ball);
Ball ball_multiply(Ball, Ball);
Ball ball_div(Ball, Ball);

void ball_add_inplace_fixed_precision(Ball *, Ball, const int);
void ball_multiply_inplace_fixed_precision(Ball *, Ball, const int);

/*
    32(bit) * 8 = 256(bit)
    precision = 8
*/
Ball ball_multiply_quad_double(Ball, Ball);
Ball ball_add_quad_double(Ball, Ball);
#endif
