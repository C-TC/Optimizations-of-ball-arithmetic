#include "ball.h"
#include <stdio.h>
Ball ball_add(Ball lo, Ball ro) {
    Ball ans;
    ans.center = big_float_add(lo.center,ro.center);
    ans.radius = lo.radius + ro.radius;
    return ans;
}
Ball ball_multiply(Ball lo, Ball ro) {
    Ball ans;
    ans.center = big_float_multiply(lo.center,ro.center);
    //ans.radius = dl*r + dr*l + l*r
    ans.radius = big_float_to_double(lo.center) * ro.radius + big_float_to_double(ro.center) * lo.radius + lo.radius * ro.radius;
    return ans;
}
Ball ball_div(Ball lo, Ball ro) {
    Ball ans;
    //TODO: what about ro = 0? how to define infinity in bigfloat?
    ans.center = big_float_div(lo.center,ro.center);
    double anscentervalue = big_float_to_double(ans.center);
    double rovalue = big_float_to_double(ro.center);
    // TODO: optimize this
    // ans.radius = l/r + l*dr/r^2 + dl/r + dl*dr/r^2
    ans.radius = anscentervalue + anscentervalue / big_float_to_double(ro.center) * ro.radius + lo.radius / rovalue + lo.radius * ro.radius / rovalue / rovalue;
    return ans;
}
int main() {
    printf("hello world\n");
}