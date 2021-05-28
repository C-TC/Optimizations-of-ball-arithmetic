#ifndef QUAD_DOUBLE_H
#define QUAD_DOUBLE_H

#include <math.h>

/* 
basic version based on  paper Yozo Hida, Xiaoye S. Li, and David H. Bailey. 2001. 
                        Algorithms for Quad-Double Precision Floating Point Arithmetic. 
                        In Proceedings of the 15th IEEE Symposium on Computer Arithmetic
                        (ARITH '01). IEEE Computer Society, USA, 155.

                        and github repo (unofficial) https://github.com/scibuilder/QD

 */

#define _QD_SPLITTER 134217729.0               // = 2^27 + 1
#define _QD_SPLIT_THRESH 6.69692879491417e+299 // = 2^996

typedef struct quad_double
{
  double d[4];
} quad_double;

quad_double quad_double_create_from_double(double d0, double d1, double d2, double d3);
void print_qd(quad_double qd, const char* msg);

quad_double qd_add_qd(quad_double *a, quad_double *b);
quad_double qd_sub_qd(quad_double *a, quad_double *b);
quad_double qd_mul_qd(quad_double *a, quad_double *b);
quad_double qd_div_qd(quad_double *a, quad_double *b);



/* Computes fl(a+b) and err(a+b).  Assumes |a| >= |b|. */
inline double quick_two_sum(double a, double b, double *err)
{
  double s = a + b;
  *err = b - (s - a);
  return s;
}

/* Computes fl(a-b) and err(a-b).  Assumes |a| >= |b| */
inline double quick_two_diff(double a, double b, double *err)
{
  double s = a - b;
  *err = (a - s) - b;
  return s;
}

/* Computes fl(a+b) and err(a+b).  */
inline double two_sum(double a, double b, double *err)
{
  double s = a + b;
  double bb = s - a;
  *err = (a - (s - bb)) + (b - bb);
  return s;
}

inline void three_sum(double *a, double *b, double *c) {
  double t1, t2, t3;
  t1 = two_sum(*a, *b, &t2);
  *a  = two_sum(*c, t1, &t3);
  *b  = two_sum(t2, t3, c);
}

inline void three_sum2(double *a, double *b, double *c) {
  double t1, t2, t3;
  t1 = two_sum(*a, *b, &t2);
  *a  = two_sum(*c, t1, &t3);
  *b = t2 + t3;
}

/* Computes fl(a-b) and err(a-b).  */
inline double two_diff(double a, double b, double *err)
{
  double s = a - b;
  double bb = s - a;
  *err = (a - (s - bb)) - (b + bb);
  return s;
}

#ifndef QD_FMS
/* Computes high word and lo word of a */
inline void split(double a, double *hi, double *lo)
{
  double temp;
  if (a > _QD_SPLIT_THRESH || a < -_QD_SPLIT_THRESH)
  {
    a *= 3.7252902984619140625e-09; // 2^-28
    temp = _QD_SPLITTER * a;
    *hi = temp - (temp - a);
    *lo = a - *hi;
    *hi *= 268435456.0; // 2^28
    *lo *= 268435456.0; // 2^28
  }
  else
  {
    temp = _QD_SPLITTER * a;
    *hi = temp - (temp - a);
    *lo = a - *hi;
  }
}
#endif

/* Computes fl(a*b) and err(a*b). */
inline double two_prod(double a, double b, double *err)
{
#ifdef QD_FMS
  double p = a * b;
  err = QD_FMS(a, b, p);
  return p;
#else
  double a_hi, a_lo, b_hi, b_lo;
  double p = a * b;
  split(a, &a_hi, &a_lo);
  split(b, &b_hi, &b_lo);
  *err = ((a_hi * b_hi - p) + a_hi * b_lo + a_lo * b_hi) + a_lo * b_lo;
  return p;
#endif
}

/* Computes fl(a*a) and err(a*a).  Faster than the above method. */
inline double two_sqr(double a, double *err)
{
#ifdef QD_FMS
  double p = a * a;
  err = QD_FMS(a, a, p);
  return p;
#else
  double hi, lo;
  double q = a * a;
  split(a, &hi, &lo);
  *err = ((hi * hi - q) + 2.0 * hi * lo) + lo * lo;
  return q;
#endif
}

inline void renorm4(double *c0, double *c1,
                   double *c2, double *c3)
{
  double s0, s1, s2 = 0.0, s3 = 0.0;

  if (isinf(*c0))
    return;

  s0 = quick_two_sum(*c2, *c3, c3);
  s0 = quick_two_sum(*c1, s0, c2);
  *c0 = quick_two_sum(*c0, s0, c1);

  s0 = *c0;
  s1 = *c1;
  if (s1 != 0.0)
  {
    s1 = quick_two_sum(s1, *c2, &s2);
    if (s2 != 0.0)
      s2 = quick_two_sum(s2, *c3, &s3);
    else
      s1 = quick_two_sum(s1, *c3, &s2);
  }
  else
  {
    s0 = quick_two_sum(s0, *c2, &s1);
    if (s1 != 0.0)
      s1 = quick_two_sum(s1, *c3, &s2);
    else
      s0 = quick_two_sum(s0, *c3, &s1);
  }

  *c0 = s0;
  *c1 = s1;
  *c2 = s2;
  *c3 = s3;
}

inline void renorm5(double *c0, double *c1,
                   double *c2, double *c3, double *c4)
{
  double s0, s1, s2 = 0.0, s3 = 0.0;

  if (isinf(*c0))
    return;

  s0 = quick_two_sum(*c3, *c4, c4);
  s0 = quick_two_sum(*c2, s0, c3);
  s0 = quick_two_sum(*c1, s0, c2);
  *c0 = quick_two_sum(*c0, s0, c1);

  s0 = *c0;
  s1 = *c1;

  if (s1 != 0.0)
  {
    s1 = quick_two_sum(s1, *c2, &s2);
    if (s2 != 0.0)
    {
      s2 = quick_two_sum(s2, *c3, &s3);
      if (s3 != 0.0)
        s3 += *c4;
      else
        s2 = quick_two_sum(s2, *c4, &s3);
    }
    else
    {
      s1 = quick_two_sum(s1, *c3, &s2);
      if (s2 != 0.0)
        s2 = quick_two_sum(s2, *c4, &s3);
      else
        s1 = quick_two_sum(s1, *c4, &s2);
    }
  }
  else
  {
    s0 = quick_two_sum(s0, *c2, &s1);
    if (s1 != 0.0)
    {
      s1 = quick_two_sum(s1, *c3, &s2);
      if (s2 != 0.0)
        s2 = quick_two_sum(s2, *c4, &s3);
      else
        s1 = quick_two_sum(s1, *c4, &s2);
    }
    else
    {
      s0 = quick_two_sum(s0, *c3, &s1);
      if (s1 != 0.0)
        s1 = quick_two_sum(s1, *c4, &s2);
      else
        s0 = quick_two_sum(s0, *c4, &s1);
    }
  }

  *c0 = s0;
  *c1 = s1;
  *c2 = s2;
  *c3 = s3;
}

#endif