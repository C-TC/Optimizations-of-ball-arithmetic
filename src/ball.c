#include "ball.h"
#include <stdio.h>
#include <stdlib.h>
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
    BigInteger tmp_lo = big_integer_create(1);
    BigInteger tmp_ro = big_integer_create(3);
    BigFloat lo = big_float_create(tmp_lo, 1);
    BigFloat ro = big_float_create(tmp_ro, 1);
    BigFloat ans = big_float_div(lo, ro);
    big_float_print(ans);
    printf("The power of the ans is %lld\n", ans.power);
    free(tmp_lo.data.bits);
    free(tmp_ro.data.bits);
    free(ans.mantissa.data.bits);

    double testdata = -1e300;
    BigFloat test1 = double_to_big_float(testdata);
    printf("input double: %e\n",testdata);
    big_float_print(test1);
    printf("transformed back to double: %e\n",big_float_to_double(test1));
    big_float_destroy(&test1);

}
