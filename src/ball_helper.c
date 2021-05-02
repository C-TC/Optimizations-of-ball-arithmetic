#include "ball_helper.h"

Ball ball_create(BigFloat center, double radius) {
    Ball ans;
    ans.center = center;
    ans.radius = radius;
    return ans;
}

void ball_destory(Ball *tmp) {
    big_float_destroy(&tmp->center);
}

Ball double_to_ball(double data) {
    double_cast d_c;
    d_c.d = data;
    // assume data is not NAN or INF
    // Consider: zero(as a special case of denormal) or denormal or normal
    BigFloat center = double_to_big_float(data);
    double_cast radius;
    radius.parts.sign = 0;
    radius.parts.mantissa = 1;
    radius.parts.exponent = d_c.parts.exponent;
    double rad = radius.d;
    Ball res =ball_create(center, rad);
    return res;    
}

double ball_to_double(Ball b) {
    return 0.0;
}

void ball_print(Ball b) {
    printf("center: ");
    big_float_print(b.center);
    printf("radius: +- %e\n",b.radius);
}