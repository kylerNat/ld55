/* abusing the preprocessor because we can
 *
 * including this file defines vectors of type type and size N,
 */

#ifndef x0
#define x0 x
#endif
#ifndef x1
#define x1 y
#endif
#ifndef x2
#define x2 z
#endif
#ifndef x3
#define x3 w
#endif

#ifndef x01
#define x01 CONCAT(x0, x1)
#endif

#ifndef x12
#define x12 CONCAT(x1, x2)
#endif

#ifndef x012
#define x012 CONCAT(x01, x2)
#endif

#ifndef x123
#define x123 CONCAT(CONCAT(x1, x2), x3)
#endif

#ifndef x201
#define x201 CONCAT(x2, x01)
#endif

#ifndef x120
#define x120 CONCAT(x12, x0)
#endif

#ifndef type_N
#define type_N CONCAT(CONCAT(type, _), N)
#endif

#ifdef PACKING
#pragma pack(push, PACKING)
#endif
struct type_N
{
    union
    {
        #if N <= 4
        struct
        {
            type x0;
            type x1;
            #if N > 2
            type x2;
            #if N > 3
            type x3;
            #endif
            #endif
        };
        #if N > 2
        #define type_2 CONCAT(CONCAT(type, _), 2)
        struct
        {
            type_2 x01;
        };
        struct
        {
            type __x0;
            type_2 x12;
        };
        #endif
        #if N > 3
        #define type_3 CONCAT(CONCAT(type, _), 3)
        struct
        {
            type_3 x012;
        };
        struct
        {
            type _x0;
            type_3 x123;
        };
        #endif
        #endif
        type data[N];
    };

    type& operator[](int i)
    {
        return data[i];
    }

    #if N >= 3
    type_N x201()
    {
        return {data[2], data[0], data[1]};
    }

    type_N x120()
    {
        return {data[1], data[2], data[0]};
    }
    #endif
};
#ifdef PACKING
#pragma pack(pop)
#endif

#if N==2 && defined(FLOATING_POINT_TYPE) && !defined(COMPLEX_TYPE) && !defined(NO_HAND_SIMD)
type_N operator*(type a, type_N x)
{
    __m128 a128 = _mm_load_ps1(&a);
    __m128 x128 = _mm_load_sd((double*) &x);
    __m128 p128 = _mm_mul_ps(a128, x128);
    alignas(16) type_N out;
    _mm_store_sd((double*)&out, p128);
    return out;
}
#else
type_N operator*(type a, type_N x)
{
    type_N ax;
    for(int i = 0; i < N; i++) ax[i] = a*x[i];
    return ax;
}
#endif

type_N operator*(type_N x, type a)
{
    return a*x;
}

type_N operator/(type_N x, type a)
{
    #ifdef FLOATING_POINT_TYPE
    return (1.0/a)*x;
    #else
    type_N x_a;
    for(int i = 0; i < N; i++) x_a[i] = x[i]/a;
    return x_a;
    #endif
}

#ifdef COMPLEX_TYPE
type_N operator*(real a, type_N x)
{
    type_N ax;
    for(int i = 0; i < N; i++) ax[i] = a*x[i];
    return ax;
}

type_N operator*(type_N x, real a)
{
    return a*x;
}

type_N operator/(type_N x, real a)
{
    return (1.0/a)*x;
}

eq_op(*, type_N, type_N, real);
eq_op(/, type_N, type_N, real);
#endif

#if N==2 && defined(FLOATING_POINT_TYPE) && !defined(COMPLEX_TYPE) && !defined(NO_HAND_SIMD)
type_N operator+(type_N a, type_N b)
{
    __m128 a128 = _mm_load_sd((double*) &a);
    __m128 b128 = _mm_load_sd((double*) &b);
    __m128 p128 = _mm_add_ps(a128, b128);
    alignas(16) type_N out;
    _mm_store_sd((double*) &out, p128);
    return out;
}

type_N operator-(type_N a, type_N b)
{
    __m128 a128 = _mm_load_sd((double*) &a);
    __m128 b128 = _mm_load_sd((double*) &b);
    __m128 p128 = _mm_sub_ps(a128, b128);
    alignas(16) type_N out;
    _mm_store_sd((double*) &out, p128);
    return out;
}
#else
type_N operator+(type_N a, type_N b)
{
    type_N sum;
    for(int i = 0; i < N; i++) sum[i] = a[i]+b[i];
    return sum;
}

type_N operator-(type_N a, type_N b)
{
    type_N sum;
    for(int i = 0; i < N; i++) sum[i] = a[i]-b[i];
    return sum;
}
#endif

type_N operator+(type_N a, type b)
{
    type_N sum;
    for(int i = 0; i < N; i++) sum[i] = a[i]+b;
    return sum;
}

type_N operator+(type a, type_N b)
{
    type_N sum;
    for(int i = 0; i < N; i++) sum[i] = a+b[i];
    return sum;
}

type_N operator-(type_N a, type b)
{
    type_N sum;
    for(int i = 0; i < N; i++) sum[i] = a[i]-b;
    return sum;
}

type_N operator-(type a, type_N b)
{
    type_N sum;
    for(int i = 0; i < N; i++) sum[i] = a-b[i];
    return sum;
}

type_N operator-(type_N a)
{
    type_N out;
    for(int i = 0; i < N; i++) out.data[i] = -a[i];
    return out;
}

bool operator==(type_N a, type_N b)
{
    for(int i = 0; i < N; i++) if(a[i] != b[i]) return false;
    return true;
}

bool operator!=(type_N a, type_N b)
{
    for(int i = 0; i < N; i++) if(a[i] != b[i]) return true;
    return false;
}

#if N==2 && defined(FLOATING_POINT_TYPE) && !defined(COMPLEX_TYPE) && !defined(NO_HAND_SIMD)
type dot(type_N a, type_N b)
{
    __m128 a128 = _mm_load_sd((double*) &a);
    __m128 b128 = _mm_load_sd((double*) &b);
    __m128 p128 = _mm_mul_ps(a128, b128);
    p128 = _mm_hadd_ps(p128, p128);
    alignas(16) type out;
    _mm_store_ss(&out, p128);
    return out;
}
#else
type dot(type_N a, type_N b)
{
    type product = {0};
    for(int i = 0; i < N; i++) product += a[i]*b[i];
    return product;
}
#endif

#ifndef COMPLEX_TYPE
real norm(type_N a)
{
    return sqrtf(dot(a, a));
}

type normsq(type_N a)
{
    return (dot(a, a));
}

type_N normalize(type_N a)
{
    return invsqrt(dot(a, a))*a;
}

type_N normalize_or_zero(type_N a)
{
    type a_sq = dot(a,a);
    if(a_sq <= 0.000001f) return {0};
    return invsqrt(a_sq)*a;
}

type_N normalize_or_fallback(type_N a, type_N fallback)
{
    type a_sq = dot(a,a);
    if(a_sq <= 0.000001f) return fallback;
    return invsqrt(a_sq)*a;
}


type_N proj(type_N a, type_N b)
{
    type_N bhat = normalize(b);
    return bhat*dot(a, bhat);
}

type_N rej(type_N a, type_N b)
{
    return a - proj(a, b);
}

#if N==2 && defined(FLOATING_POINT_TYPE) && !defined(COMPLEX_TYPE) && !defined(NO_HAND_SIMD)
type_N multiply_components(type_N a, type_N b)
{
    __m128 a128 = _mm_load_sd((double*) &a);
    __m128 b128 = _mm_load_sd((double*) &b);
    __m128 p128 = _mm_mul_ps(a128, b128);
    alignas(16) type_N out;
    _mm_store_sd((double*) &out, p128);
    return out;
}

type_N divide_components(type_N a, type_N b)
{
    __m128 a128 = _mm_load_sd((double*) &a);
    __m128 b128 = _mm_load_sd((double*) &b);
    __m128 p128 = _mm_div_ps(a128, b128);
    alignas(16) type_N out;
    _mm_store_sd((double*) &out, p128);
    return out;
}
#else
type_N multiply_components(type_N a, type_N b)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = a[i]*b[i];
    return out;
}

type_N divide_components(type_N a, type_N b)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = a[i]/b[i];
    return out;
}
#endif

#ifndef IS_COMPLEX
type_N operator*(type_N a, type_N b)
{
    return multiply_components(a, b);
}

type_N operator/(type_N a, type_N b)
{
    return divide_components(a, b);
}
#endif

type_N invert_components(type_N a)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = 1.0/a[i];
    return out;
}

type_N min_per_axis(type_N a, type_N b)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = min(a[i], b[i]);
    return out;
}

bool all_less_than(type_N a, type_N b)
{
    for(int i = 0; i < N; i++)
        if(a[i] >= b[i]) return false;
    return true;
}

bool all_less_than_eq(type_N a, type_N b)
{
    for(int i = 0; i < N; i++)
        if(a[i] > b[i]) return false;
    return true;
}

type_N max_per_axis(type_N a, type_N b)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = max(a[i], b[i]);
    return out;
}

type_N clamp_per_axis(type_N x, type a, type b)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = clamp(x[i], a, b);
    return out;
}

type_N clamp_per_axis(type_N x, type_N a, type_N b)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = clamp(x[i], a[i], b[i]);
    return out;
}

type_N radius_min(type_N x, type a)
{
    type xsq = normsq(x);
    if(xsq > sq(a)) return a*x*invsqrt(xsq);
    return x;
}

type_N sign_per_axis(type_N a)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = sign(a[i]);
    return out;
}

type_N sign_not_zero_per_axis(type_N a)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = sign_not_zero(a[i]);
    return out;
}

type_N sin_per_axis(type_N a)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = sin(a[i]);
    return out;
}

type_N pow_per_axis(type_N a, type b)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = pow(a[i], b);
    return out;
}

#ifdef FLOATING_POINT_TYPE
type_N mod_per_axis(type_N a, type_N b)
{
    type_N out;
    for(int i = 0; i < N; i++) out[i] = fmodf(a[i], b[i]);
    return out;
}
#endif

#endif

eq_op(+, type_N, type_N, type_N);
eq_op(-, type_N, type_N, type_N);
eq_op(*, type_N, type_N, type);
eq_op(/, type_N, type_N, type);

#undef type_N
#undef type
#undef N
#undef x0
#undef x1
#undef x2
#undef x3
#undef FLOATING_POINT_TYPE
#undef COMPLEX_TYPE
#undef PACKING
