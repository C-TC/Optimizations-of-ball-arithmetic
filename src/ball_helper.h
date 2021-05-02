#ifndef BALL_HELPER_H
#define BALL_HELPER_H
#include "big_float_helper.h"
#include "ball.h"
Ball ball_create(BigFloat, double);
void ball_destory(Ball *);
Ball double_to_ball(double);
double ball_to_double(Ball);
void ball_print(Ball);
#endif
