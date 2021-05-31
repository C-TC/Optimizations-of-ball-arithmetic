#ifndef QUAD_DOUBLE_H
#define QUAD_DOUBLE_H

#include <math.h>
#include <immintrin.h>
/* 
basic version based on  paper Yozo Hida, Xiaoye S. Li, and David H. Bailey. 2001. 
                        Algorithms for Quad-Double Precision Floating Point Arithmetic. 
                        In Proceedings of the 15th IEEE Symposium on Computer Arithmetic
                        (ARITH '01). IEEE Computer Society, USA, 155.

                        and github repo (unofficial) https://github.com/scibuilder/QD

 */

#define _QD_SPLITTER 134217729.0               // = 2^27 + 1
#define _QD_SPLIT_THRESH 6.69692879491417e+299 // = 2^996
#define ALIGNMENT 32
typedef struct quad_double
{
  double d[4];
} quad_double;

typedef struct qd_arr
{
  int size;
  double *d0;
  double *d1;
  double *d2;
  double *d3;
}qd_arr;

quad_double quad_double_create_from_double(double d0, double d1, double d2, double d3);
qd_arr qd_arr_create_from_double_arr(double *data, int size);
qd_arr qd_arr_create_random(int size, double min, double max);
qd_arr qd_arr_create_random_aligned(int size, double min, double max);
void qd_destroy(qd_arr qda);
void qd_destroy_aligned(qd_arr qda);
void print_qd(quad_double qd, const char* msg);
void print_qd_arr(qd_arr qda, int num, const char* msg);

quad_double qd_add_qd(quad_double *a, quad_double *b);
quad_double qd_sub_qd(quad_double *a, quad_double *b);
quad_double qd_mul_qd(quad_double *a, quad_double *b);
quad_double qd_div_qd(quad_double *a, quad_double *b);

qd_arr qd_arr_add(qd_arr lo, qd_arr ro);
qd_arr qd_arr_sub(qd_arr lo, qd_arr ro);
qd_arr qd_arr_mul(qd_arr lo, qd_arr ro);
qd_arr qd_arr_div(qd_arr lo, qd_arr ro);


void qd_arr_add_inplace(qd_arr lo, qd_arr ro);
void qd_arr_add_inplace_inline(qd_arr lo, qd_arr ro);
void qd_arr_add_inplace_vec(qd_arr lo, qd_arr ro);
void qd_arr_add_inplace_vec_inline(qd_arr lo, qd_arr ro);
//reorder useless
void qd_arr_add_inplace_vec_inline_reorder(qd_arr lo, qd_arr ro);
void qd_arr_add_inplace_vec_inline_x4(qd_arr lo, qd_arr ro);

/* Computes fl(a+b) and err(a+b).  Assumes |a| >= |b|. */
inline double quick_two_sum(double a, double b, double *err)
{
  double s = a + b;
  *err = b - (s - a);
  return s;
}
/*  
tmp_s = in1 + in2;
tmp0 = tmp_s - in1;
err = in2 - tmp0;
out = tmp_s;
 */

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
/* 
tmp_s=a+b;
tmp_bb=tmp_s-a;
err = (a - (tmp_s - tmp_bb)) + (b - tmp_bb);
out = tmp_s;
 */

inline __m256d two_sum_vec(__m256d a, __m256d b, __m256d *err)
{
  __m256d s = _mm256_add_pd(a, b);
  __m256d bb = _mm256_sub_pd(s, a);
  *err = _mm256_add_pd(_mm256_sub_pd(a, _mm256_sub_pd(s, bb)), _mm256_sub_pd(b, bb));
  return s;
}
/*   
tmp_s = _mm256_add_pd(in1, in2);
tmp_bb = _mm256_sub_pd(tmp_s, in1);
tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
tmp1 = _mm256_sub_pd(in2, tmp_bb);
tmp0 = _mm256_sub_pd(in1, tmp0);
err = _mm256_add_pd(tmp0, tmp1);
out = tmp_s;
*/

inline void three_sum(double *a, double *b, double *c) {
  double t1, t2, t3;
  t1 = two_sum(*a, *b, &t2);
  *a  = two_sum(*c, t1, &t3);
  *b  = two_sum(t2, t3, c);
}
/* 
tmp_s=a+b;
tmp_bb=tmp_s-a;
tmp_t2 = (a - (tmp_s - tmp_bb)) + (b - tmp_bb);
tmp_t1 = tmp_s;
tmp_s=c+tmp_t1;
tmp_bb=tmp_s-c;
tmp_t3 = (c - (tmp_s - tmp_bb)) + (tmp_t1 - tmp_bb);
a = tmp_s;
tmp_s=tmp_t2+tmp_t3;
tmp_bb=tmp_s-tmp_t2;
c = (tmp_t2 - (tmp_s - tmp_bb)) + (tmp_t3 - tmp_bb);
b = tmp_s;
 */
inline void three_sum_inline(double *a, double *b, double *c) {
  double t1, t2, t3;
  double tmp_s,tmp_bb;

  double val_a=*a,val_b=*b,val_c=*c;

  //t1 = two_sum(val_a, val_b, &t2);
  tmp_s=val_a+val_b;
  tmp_bb=tmp_s-val_a;
  t2 = (val_a - (tmp_s - tmp_bb)) + (val_b - tmp_bb);
  t1 = tmp_s;
  //val_a  = two_sum(val_c, t1, &t3);
  tmp_s=val_c+t1;
  tmp_bb=tmp_s-val_c;
  t3 = (val_c - (tmp_s - tmp_bb)) + (t1 - tmp_bb);
  val_a = tmp_s;
  //val_b  = two_sum(t2, t3, val_c);
  tmp_s=t2+t3;
  tmp_bb=tmp_s-t2;
  val_c = (t2 - (tmp_s - tmp_bb)) + (t3 - tmp_bb);
  val_b = tmp_s;

  *a=val_a;*b=val_b;*c=val_c;
}

inline void three_sum_vec(__m256d *a, __m256d *b, __m256d *c) {
  __m256d t1, t2, t3;
  t1 = two_sum_vec(*a, *b, &t2);
  *a  = two_sum_vec(*c, t1, &t3);
  *b  = two_sum_vec(t2, t3, c);
}
/* 
tmp_s = _mm256_add_pd(in1, in2);
tmp_bb = _mm256_sub_pd(tmp_s, in1);
tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
tmp1 = _mm256_sub_pd(in2, tmp_bb);
tmp0 = _mm256_sub_pd(in1, tmp0);
tmp_t2 = _mm256_add_pd(tmp0, tmp1);
tmp_t1 = tmp_s;
tmp_s = _mm256_add_pd(in3, tmp_t1);
tmp_bb = _mm256_sub_pd(tmp_s, in3);
tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
tmp0 = _mm256_sub_pd(in3, tmp0);
tmp_t3 = _mm256_add_pd(tmp0, tmp1);
in1 = tmp_s;
tmp_s = _mm256_add_pd(tmp_t2, tmp_t3);
tmp_bb = _mm256_sub_pd(tmp_s, tmp_t2);
tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
tmp1 = _mm256_sub_pd(tmp_t3, tmp_bb);
tmp0 = _mm256_sub_pd(tmp_t2, tmp0);
in3 = _mm256_add_pd(tmp0, tmp1);
in2 = tmp_s;
*/

inline void three_sum2(double *a, double *b, double *c) {
  double t1, t2, t3;
  t1 = two_sum(*a, *b, &t2);
  *a  = two_sum(*c, t1, &t3);
  *b = t2 + t3;
}
/* 
tmp_s = _mm256_add_pd(in1, in2);
tmp_bb = _mm256_sub_pd(tmp_s, in1);
tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
tmp1 = _mm256_sub_pd(in2, tmp_bb);
tmp0 = _mm256_sub_pd(in1, tmp0);
tmp_t2 = _mm256_add_pd(tmp0, tmp1);
tmp_t1 = tmp_s;
tmp_s = _mm256_add_pd(in3, tmp_t1);
tmp_bb = _mm256_sub_pd(tmp_s, in3);
tmp0 = _mm256_sub_pd(tmp_s, tmp_bb);
tmp1 = _mm256_sub_pd(tmp_t1, tmp_bb);
tmp0 = _mm256_sub_pd(in3, tmp0);
tmp_t3 = _mm256_add_pd(tmp0, tmp1);
in1 = tmp_s;
in2 = _mm256_add_pd(tmp_t2, tmp_t3);
 */

inline void three_sum2_inline(double *a, double *b, double *c) {
  double t1, t2, t3;
  double tmp_s,tmp_bb;

  double val_a=*a,val_b=*b,val_c=*c;

  //t1 = two_sum(*a, *b, &t2);
  tmp_s=val_a+val_b;
  tmp_bb=tmp_s-val_a;
  t2 = (val_a - (tmp_s - tmp_bb)) + (val_b - tmp_bb);
  t1 = tmp_s;
  //*a  = two_sum(*c, t1, &t3);
  tmp_s=val_c+t1;
  tmp_bb=tmp_s-val_c;
  t3 = (val_c - (tmp_s - tmp_bb)) + (t1 - tmp_bb);
  val_a = tmp_s;
  //*b = t2 + t3;
  val_b=t2+t3;

  *a=val_a;*b=val_b;*c=val_c;
}

inline void three_sum2_vec(__m256d *a, __m256d *b, __m256d *c) {
  __m256d t1, t2, t3;
  t1 = two_sum_vec(*a, *b, &t2);
  *a  = two_sum_vec(*c, t1, &t3);
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

inline void renorm5_inline(double *c0, double *c1,
                   double *c2, double *c3, double *c4)
{
  double s0, s1, s2 = 0.0, s3 = 0.0;
  double in_c0 = *c0;
  double in_c1 = *c1;
  double in_c2 = *c2;
  double in_c3 = *c3;
  double in_c4 = *c4;
  double tmp_s, tmp0;
  if (isinf(in_c0))
    return;

  //s0 = quick_two_sum(*c3, *c4, c4);
  tmp_s = in_c3 + in_c4;
  tmp0 = tmp_s - in_c3;
  in_c4 = in_c4 - tmp0;
  s0 = tmp_s;
  //s0 = quick_two_sum(*c2, s0, c3);
  tmp_s = in_c2 + s0;
  tmp0 = tmp_s - in_c2;
  in_c3 = s0 - tmp0;
  s0 = tmp_s;
  //s0 = quick_two_sum(*c1, s0, c2);
  tmp_s = in_c1 + s0;
  tmp0 = tmp_s - in_c1;
  in_c2 = s0 - tmp0;
  s0 = tmp_s;
  //*c0 = quick_two_sum(*c0, s0, c1);
  tmp_s = in_c0 + s0;
  tmp0 = tmp_s - in_c0;
  in_c1 = s0 - tmp0;
  in_c0 = tmp_s;

  //s0 = *c0;
  s0 = in_c0;
  //s1 = *c1;
  s1 = in_c1;

  if (s1 != 0.0)
  {
    //s1 = quick_two_sum(s1, *c2, &s2);
    tmp_s = s1 + in_c2;
    tmp0 = tmp_s - s1;
    s2 = in_c2 - tmp0;
    s1 = tmp_s;
    if (s2 != 0.0)
    {
      //s2 = quick_two_sum(s2, *c3, &s3);
      tmp_s = s2 + in_c3;
      tmp0 = tmp_s - s2;
      s3 = in_c3 - tmp0;
      s2 = tmp_s;
      if (s3 != 0.0) {
        //s3 += *c4;
        s3 += in_c4;
      }
      else {
        //s2 = quick_two_sum(s2, *c4, &s3);
        tmp_s = s2 + in_c4;
        tmp0 = tmp_s - s2;
        s3 = in_c4 - tmp0;
        s2 = tmp_s;
      }
    }
    else
    {
      //s1 = quick_two_sum(s1, *c3, &s2);
      tmp_s = s1 + in_c3;
      tmp0 = tmp_s - s1;
      s2 = in_c3 - tmp0;
      s1 = tmp_s;
      if (s2 != 0.0) {
        //s2 = quick_two_sum(s2, *c4, &s3);
        tmp_s = s2 + in_c4;
        tmp0 = tmp_s - s2;
        s3 = in_c4 - tmp0;
        s2 = tmp_s;
      }
      else {
        //s1 = quick_two_sum(s1, *c4, &s2);
        tmp_s = s1 + in_c4;
        tmp0 = tmp_s - s1;
        s2 = in_c4 - tmp0;
        s1 = tmp_s;
      }
    }
  }
  else
  {
    //s0 = quick_two_sum(s0, *c2, &s1);
    tmp_s = s0 + in_c2;
    tmp0 = tmp_s - s0;
    s1 = in_c2 - tmp0;
    s0 = tmp_s;
    if (s1 != 0.0)
    {
      //s1 = quick_two_sum(s1, *c3, &s2);
      tmp_s = s1 + in_c3;
      tmp0 = tmp_s - s1;
      s2 = in_c3 - tmp0;
      s1 = tmp_s;
      if (s2 != 0.0) {
        //s2 = quick_two_sum(s2, *c4, &s3);
        tmp_s = s2 + in_c4;
        tmp0 = tmp_s - s2;
        s3 = in_c4 - tmp0;
        s2 = tmp_s;
      }
      else {
        //s1 = quick_two_sum(s1, *c4, &s2);
        tmp_s = s1 + in_c4;
        tmp0 = tmp_s - s1;
        s2 = in_c4 - tmp0;
        s1 = tmp_s;
      }
    }
    else
    {
      //s0 = quick_two_sum(s0, *c3, &s1);
      tmp_s = s0 + in_c3;
      tmp0 = tmp_s - s0;
      s1 = in_c3 - tmp0;
      s0 = tmp_s;
      if (s1 != 0.0) {
        //s1 = quick_two_sum(s1, *c4, &s2);
        tmp_s = s1 + in_c4;
        tmp0 = tmp_s - s1;
        s2 = in_c4 - tmp0;
        s1 = tmp_s;
      }
      else {
        //s0 = quick_two_sum(s0, *c4, &s1);
        tmp_s = s0 + in_c4;
        tmp0 = tmp_s - s0;
        s1 = in_c4 - tmp0;
        s0 = tmp_s;
      }
    }
  }

  *c0 = s0;
  *c1 = s1;
  *c2 = s2;
  *c3 = s3;
}
#endif