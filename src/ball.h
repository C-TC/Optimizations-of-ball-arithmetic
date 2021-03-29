#ifndef BALL_H
#define BALL_H
#include "bigfloat.h"
typedef struct Ball {
    BigFloat *center;
    double radius;
}Ball;
Ball *ball_add(Ball *, Ball *, Ball *);
Ball *ball_multiply(Ball *, Ball *, Ball *);
Ball *ball_div(Ball *, Ball *, Ball *);
#endif
