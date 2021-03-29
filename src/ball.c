#include "ball.h"
#include "bigfloat.h"
Ball *ball_add(Ball *lo, Ball *ro, Ball *res) {
    if (!res) res = malloc(sizeof(Ball));
    res->radius = lo->radius + ro->radius;
    res->center = bigfloat_add(lo->center, ro->center, NULL);
    return res;
}
Ball *ball_multiply(Ball *lo, Ball *ro, Ball *res) {
    if (!res) res = malloc(sizeof(Ball));
    return res;
}
Ball *ball_div(Ball *lo, Ball *ro, Ball *res) {
    if (!res) res = malloc(sizeof(Ball));
    return res;
}
