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
