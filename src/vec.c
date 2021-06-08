#include "vec.h"
#include <stdlib.h>     /* malloc, free, rand */


Vec vec_add(Vec l, Vec r) {
    Vec res;
    res.size = l.size;
    res.data = (Ball *)malloc(l.size * sizeof(Ball));
    for (int i = 0; i < res.size; ++i) {
        res.data[i] = ball_add(l.data[i], r.data[i]); 
    }
    return res;
}
Vec vec_add_vec(Vec l, Vec r) {
    Vec res;
    res.size = l.size;
    res.data = (Ball *)malloc(l.size * sizeof(Ball));
    BallVec balll, ballr, ballres;
    for (int i = 0; i < res.size; i += 4) {
        balll.foo = l.data[i];
        balll.bar = l.data[i + 1];
        balll.baz = l.data[i + 2];
        balll.foobar = l.data[i + 3];
        ballr.foo = r.data[i];
        ballr.bar = r.data[i + 1];
        ballr.baz = r.data[i + 2];
        ballr.foobar = r.data[i + 3];
        ballres = ball_add_vec(balll, ballr);
        res.data[i] = ballres.foo;
        res.data[i + 1] = ballres.bar;
        res.data[i + 2] = ballres.baz;
        res.data[i + 3] = ballres.foobar; 
    }
    return res;
}
