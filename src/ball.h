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
#endif
