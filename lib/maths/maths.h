#ifndef MATHS
#define MATHS

#include <math.h>
#include <utils/misc.h>

#ifdef __EMSCRIPTEN__
#include <wasm_simd128.h>
#else
#include <emmintrin.h>
#include <smmintrin.h>
#endif

#define nan (0.0/0.0)
#define INF INFINITY

#define pi (3.14159265358979323846264338327950)
// #define e (2.718281828)

#define lerp(a, b, t) ((a)*(1-(t)) + (b)*(t))

inline real smoothstep(real a, real b, real x)
{
    real t = clamp((x-a)/(b-a), 0.0, 1.0);
    return t*t*(3.0-2.0*t);
}

#define sq(a) ((a)*(a))

typedef float real;

#define sign(a) ((a) ? (a) > 0 ? 1 : -1 : 0)
#define sign_not_zero(a) ((a) >= 0 ? 1 : -1)

#ifndef __EMSCRIPTEN__
inline real sqrt(real a)
{
    __m128 a128 = _mm_load_ss(&a);
    //TODO: a*rsqrt(a) might be faster, but need to figure out how to handle sqrt(0) quickly
    __m128 out128 = _mm_sqrt_ss(a128);
    real out;
    _mm_store_ss(&out, out128);
    return out;
}

inline real invsqrt(real a)
{
    __m128 a128 = _mm_load_ss(&a);
    __m128 out128 = _mm_rsqrt_ss(a128);
    real out;
    _mm_store_ss(&out, out128);
    return out;
}

inline float32 abs(float32 x) //if it inlines correctly this should be 1 op
{
    //const __m128i sign_mask = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF);
    __m128 absolute_value = _mm_set_ss(x);
    absolute_value = _mm_and_ps(absolute_value, _mm_castsi128_ps(_mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF)));//set the sign bit to 0 to get the absolute value

    float32 out;
    _mm_store_ss(&out, absolute_value);
    return out;
}

// inline int abs(int x) {
//     return x >= 0 ? x : -x;
// }

// inline float32 floor(float32 x)
// {
//     __m128 floor_x = _mm_set_ss(x);

//     // floor_x = _mm_round_ps(floor_x, _MM_FROUND_TO_NEG_INF);//TODO: use roundps when available
//     // floor_x = _mm_cvtepi32_ps(_mm_cvttps_epi32(floor_x));
//     floor_x = _mm_floor_ps(floor_x);//TODO: use roundps when available

//     float32 out;
//     _mm_store_ss(&out, floor_x);
//     return out;
// }

#define def_minmax_abs(type)                                            \
    type min_abs(type a, type  b) {return (abs(a) < abs(b)) ? a : b;}   \
    type max_abs(type a, type  b) {return (abs(a) > abs(b)) ? a : b;}

def_minmax_abs(int)
def_minmax_abs(real)

#else //indef __EMSCRIPTEN
inline real invsqrt(real a)
{
    return 1.0/sqrt(a); //TODO: actually use web asm instructions
}
#endif //indef __EMSCRIPTEN__

//TODO: more accurate trig functions?

inline float32 fast_cos(float32 x)
{//TODO: optimize!(more)
    x = abs(x);
    x -= floor(x);
    x -= 0.5;
    x = abs(x);
    x -= 0.25;
    //x is converted to a triangle wave since the polynomial approxomation only works between -pi/4 and pi/4 radians
    float32 out = -32.0;
    out *= x;
    out *= x;
    out += 6.0;
    out *= x;
    return out;//x*(6.0-32.0*x*x);
}

inline float32 fast_sin(float32 x)
{
    return fast_cos(x - 0.25);
}
//macro to define simultaneous assignment and equality operators (+=, *=, etc.)
#define eq_op(op, rtype, atype, btype) rtype operator op##=(atype& a, btype b){return a=a op b;};

//---------------faster compilation testing, somewhat faster but less convinient---------------
// #define define_square_matrix(type, N) define_square_matrix_helper(type, N, CONCAT(CONCAT(type, _), N), CONCAT(CONCAT(CONCAT(type, _), N), CONCAT(x, N)), CONCAT(identity, N))
// #define define_square_matrix_helper(type, N, type_N, type_NxN, identityN) \
//     struct type_NxN                                                     \
//     {                                                                   \
//         union                                                           \
//         {                                                               \
//             struct                                                      \
//             {                                                           \
//                 type_N columns[N];                                      \
//             };                                                          \
//             type data[N*N];                                             \
//         };                                                              \
//                                                                         \
//         type_N& operator[](int i)                                       \
//         {                                                               \
//             return columns[i];                                          \
//         }                                                               \
//     };                                                                  \
//                                                                         \
//     type_NxN identityN(int lambda)                                           \
//     {                                                                   \
//         type_NxN Lambda = {};                                                \
//         for(int i = 0; i < N; i++) Lambda[i][i] = lambda;                         \
//         return Lambda;                                                       \
//     }                                                                   \
//                                                                         \
//     type_NxN operator*(type c, type_NxN A)                              \
//     {                                                                   \
//         type_NxN cA;                                                    \
//         for(int i = 0; i < N*N; i++) cA.data[i] = c*A.data[i];          \
//         return cA;                                                      \
//     }                                                                   \
//                                                                         \
//     type_NxN operator*(type_NxN A, type c)                              \
//     {                                                                   \
//         return c*A;                                                     \
//     }                                                                   \
//                                                                         \
//     type_NxN operator/(type_NxN A, type c)                              \
//     {                                                                   \
//         return (1.0/c)*A;                                               \
//     }                                                                   \
//                                                                         \
//     type_NxN operator+(type_NxN A, type_NxN B)                          \
//     {                                                                   \
//         type_NxN sum;                                                   \
//         for(int i = 0; i < N; i++) sum[i] = A[i]+B[i];                  \
//         return sum;                                                     \
//     }                                                                   \
//                                                                         \
//     type_NxN operator-(type_NxN A, type_NxN B)                          \
//     {                                                                   \
//         type_NxN sum;                                                   \
//         for(int i = 0; i < N; i++) sum[i] = A[i]-B[i];                  \
//         return sum;                                                     \
//     }                                                                   \
//                                                                         \
//     type_NxN transpose(type_NxN A)                                      \
//     {                                                                   \
//         type_NxN At;                                                    \
//         for(int i = 0; i < N; i++)                                      \
//             for(int j = 0; j < N; j++)                                  \
//                 At[j][i] = A[i][j];                                     \
//         return At;                                                      \
//     }                                                                   \
//                                                                         \
//     type_N operator*(type_NxN A, type_N x)                              \
//     {                                                                   \
//         type_N result = {};                                             \
//         for(int i = 0; i < N; i++) result += A[i]*x[i];                 \
//         return result;                                                  \
//     }                                                                   \
//                                                                         \
//     type_N operator*(type_N x, type_NxN A)                              \
//     {                                                                   \
//         type_NxN At = transpose(A);                                     \
//         return At*x;                                                    \
//     }                                                                   \
//                                                                         \
//     type_NxN operator*(type_NxN A, type_NxN B)                          \
//     {                                                                   \
//         type_NxN product;                                               \
//         for(int i = 0; i < N; i++) product[i] = A*B[i];                 \
//         return product;                                                 \
//     }                                                                   \
//                                                                         \
//     eq_op(+, type_NxN, type_NxN, type_NxN);                             \
//     eq_op(-, type_NxN, type_NxN, type_NxN);                             \
//     eq_op(*, type_NxN, type_NxN, type_NxN);                             \
//     eq_op(*, type_NxN, type_NxN, type);                                 \
//     eq_op(/, type_NxN, type_NxN, type);                                 \
//                                                                         \
//     type tr(type_NxN A)                                                 \
//     {                                                                   \
//         type trace = 0;                                                 \
//         for(int i = 0; i < N; i++) trace += A[i][i];                    \
//         return trace;                                                   \
//     }                                                                   \
//                                                                         \
//     type det(type_NxN A)                                                \
//     {                                                                   \
//         type numerator = 1;                                             \
//         type denominator = 1;                                           \
//                                                                         \
//         for(int i = 0; i < N-1; i++)                                    \
//             for(int j = i+1; j < N; j++)                                \
//             {                                                           \
//                 type sub_scale = A[j][i];                               \
//                 A[j] *= A[i][i];                                        \
//                 denominator *= A[i][i];                                 \
//                 A[j] -= sub_scale*A[i];                                 \
//             }                                                           \
//                                                                         \
//         for(int i = 0; i < N; i++) numerator *= A[i][i];                \
//                                                                         \
//         return numerator/denominator;                                   \
//     }                                                                   \
//                                                                         \
//     type_NxN inverse(type_NxN A)                                        \
//     {                                                                   \
//         type_NxN Ai = identityN(1);                                     \
//                                                                         \
//         for(int i = 0; i < N-1; i++)                                    \
//             for(int j = i+1; j < N; j++)                                \
//             {                                                           \
//                 type sub_scale = A[j][i];                               \
//                 A[j] *= A[i][i];                                        \
//                 Ai[j] *= A[i][i];                                       \
//                                                                         \
//                 A[j] -= sub_scale*A[i];                                 \
//                 Ai[j] -= sub_scale*Ai[i];                               \
//             }                                                           \
//                                                                         \
//         for(int i = N-1; i > 0; i--)                                    \
//             for(int j = i-1; j >= 0; j--)                               \
//             {                                                           \
//                 type sub_scale = A[j][i];                               \
//                 A[j] *= A[i][i];                                        \
//                 Ai[j] *= A[i][i];                                       \
//                                                                         \
//                 A[j] -= sub_scale*A[i];                                 \
//                 Ai[j] -= sub_scale*Ai[i];                               \
//             }                                                           \
//                                                                         \
//         for(int i = 0; i < N; i++)                                      \
//         {                                                               \
//             Ai[i] /= A[i][i];                                           \
//         }                                                               \
//                                                                         \
//         return Ai;                                                      \
//     }

//TODO: make way to specify custom procedures for specific optimization

#define type uint8
#define N 2
#include "math/generic_vector.h"

#define type uint8
#define N 3
#include "math/generic_vector.h"

#define type uint8
#define N 4
#include "math/generic_vector.h"

#if 1
#define type int
#define N 2
#include "math/generic_vector.h"
#else
#include "math/int_2_vector.h"
#endif

#define type uint
#define N 2
#include "math/generic_vector.h"

#define type uint
#define N 3
#include "math/generic_vector.h"

#define type uint
#define N 4
#include "math/generic_vector.h"

#define type int
#define N 3
#include "math/generic_vector.h"

#define type int8
#define N 2
#include "math/generic_vector.h"

#define type int16
#define N 2
#include "math/generic_vector.h"

#define NO_HAND_SIMD
#define FLOATING_POINT_TYPE
#define type real
#define N 2
#include "math/generic_vector.h"

real_2 operator*(real a, int_2 x)
{
    return {a*x.x, a*x.y};
}

real_2 operator*(int_2 x, real a)
{
    return {a*x.x, a*x.y};
}

#define FLOATING_POINT_TYPE
#define type real
#define N 2
#include "math/generic_square_matrix.h"

#define FLOATING_POINT_TYPE
#define type real
#define N 3
#include "math/generic_vector.h"

#define FLOATING_POINT_TYPE
#define type real
#define N 4
#include "math/generic_vector.h"

#define FLOATING_POINT_TYPE
#define type real
#define N 3
#include "math/generic_square_matrix.h"

#define inf INFINITY

#define type real
#define N 4
#include "math/generic_square_matrix.h"

#define type real
#define N 3
#define M 4
#include "math/generic_rectangular_matrix.h"

//complex numbers
#define IS_COMPLEX
#define type real
#define N 2
#define type_N complex
#include "math/generic_vector.h"
#undef IS_COMPLEX

real_2 conjugate(real_2 a)
{
    return {a.x, -a.y};
}

complex conjugate(complex a)
{
    return {a.x, -a.y};
}

complex operator*(complex a, complex b)
{
    return {a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x};
}

complex operator/(real a, complex b)
{
    return (a/normsq(b))*conjugate(b);
}

complex operator/(complex a, complex b)
{
    return (a/normsq(b))*conjugate(b);
}

complex as_complex(real_2 x)
{
    return {x.x, x.y};
}

eq_op(*, complex, complex, complex);
eq_op(/, complex, complex, complex);

#define FLOATING_POINT_TYPE
#define COMPLEX_TYPE
#define type complex
#define N 2
#include "math/generic_vector.h"

#define FLOATING_POINT_TYPE
#define COMPLEX_TYPE
#define type complex
#define N 3
#include "math/generic_vector.h"

#define FLOATING_POINT_TYPE
#define COMPLEX_TYPE
#define type complex
#define N 4
#include "math/generic_vector.h"

#define FLOATING_POINT_TYPE
#define COMPLEX_TYPE
#define type complex
#define N 2
#include "math/generic_square_matrix.h"

#define FLOATING_POINT_TYPE
#define COMPLEX_TYPE
#define type complex
#define N 4
#include "math/generic_square_matrix.h"

// define_square_matrix(real, 3)
// define_square_matrix(real, 4)

// #include "square_matrix_4x4.h"

#define type real
#define N 4
#define x0 r
#define x1 i
#define x2 j
#define x3 k
#define IS_COMPLEX
#define type_N quaternion
#include "math/generic_vector.h"
#undef IS_COMPLEX

const real_4 zero_4 = {0,0,0,0};
const real_3 zero_3 = {0,0,0};
const real_2 zero_2 = {0,0};

uint8_4 clamped_add(uint8_4 a, uint8_4 b)
{
    return {
        clamp((int)a.x+b.x, 0, 255),
        clamp((int)a.y+b.y, 0, 255),
        clamp((int)a.z+b.z, 0, 255),
        clamp((int)a.w+b.w, 0, 255),
    };
}

int_2 int_cast(real_2 a)
{
    return {a.x, a.y};
}

int_3 int_cast(real_3 a)
{
    return {a.x, a.y, a.z};
}

uint_2 uint_cast(real_2 a)
{
    return {a.x, a.y};
}

int8_2 int8_cast(int_2 a)
{
    return {a.x, a.y};
}

int16_2 int16_cast(real_2 a)
{
    return {a.x, a.y};
}

uint8_4 uint8_cast(real_4 a)
{
    return {a.x, a.y, a.z, a.w};
}

uint_4 uint_cast(real_4 a)
{
    return {a.x, a.y, a.z, a.w};
}

real_2 real_cast(int_2 a)
{
    return {a.x, a.y};
}

real_2 real_cast(uint_2 a)
{
    return {a.x, a.y};
}

real_3 real_cast(int_3 a)
{
    return {a.x, a.y, a.z};
}

real_4 real_cast(uint8_4 a)
{
    return {a.x, a.y, a.z, a.w};
}

real_4 from_srgb(real_4 a)
{
    real inv_gamma = 1.0/2.2;
    return {pow(a.x, inv_gamma),
            pow(a.y, inv_gamma),
            pow(a.z, inv_gamma),
            pow(a.w, inv_gamma),};
}

int axes_product(int_2 a)
{
    return a.x*a.y;
}

int axes_product(int_3 a)
{
    return a.x*a.y*a.z;
}

int axes_sum(int_3 a)
{
    return a.x+a.y+a.z;
}

int_2 abs_per_axis(int_2 a)
{
    return {abs(a.x), abs(a.y)};
}

int_3 abs_per_axis(int_3 a)
{
    return {abs(a.x), abs(a.y), abs(a.z)};
}

real_2 abs_per_axis(real_2 a)
{
    return {abs(a.x), abs(a.y)};
}

real_3 abs_per_axis(real_3 a)
{
    return {abs(a.x), abs(a.y), abs(a.z)};
}

real_2 round_per_axis(real_2 a)
{
    return {round(a.x), round(a.y)};
}

real_2 floor_per_axis(real_2 a)
{
    return {floor(a.x), floor(a.y)};
}

real_2 ceil_per_axis(real_2 a)
{
    return {ceil(a.x), ceil(a.y)};
}

real_3 multiply_per_axis(real_3 a, real_3 b)
{
    return {a.x*b.x, a.y*b.y, a.z*b.z};
}

real_3 floor_per_axis(real_3 a)
{
    return {floor(a.x), floor(a.y), floor(a.z)};
}

real_3 round_per_axis(real_3 a)
{
    return {round(a.x), round(a.y), round(a.z)};
}

real_3 lerp_per_axis(real_3 a, real_3 b, real_3 t)
{
    return {lerp(a.x,b.x,t.x), lerp(a.y,b.y,t.y), lerp(a.z,b.z,t.z)};
}

real_2 perp(real_2 a)
{
    return {-a.y, a.x};
}

real_3 perp_xy(real_3 a)
{
    return {a.y, -a.x, a.z};
}

real_3 cross(real_3 a, real_3 b)
{
    return {a.y*b.z-a.z*b.y,
            a.z*b.x-a.x*b.z,
            a.x*b.y-a.y*b.x};
}

real_3 pad_3(real_2 a)
{
    return {a.x, a.y, 0};
}

real_3 pad_3(real_2 a, real z)
{
    return {a.x, a.y, z};
}

int_3 pad_3(int_2 a)
{
    return {a.x, a.y, 0};
}

int_3 pad_3(int_2 a, int z)
{
    return {a.x, a.y, z};
}

real_4 pad_4(real_3 a)
{
    return {a.x, a.y, a.z, 0.0};
}

real_4 pad_4(real_3 a, real pad_value)
{
    return {a.x, a.y, a.z, pad_value};
}

real cross(real_2 a, real_2 b)
{
    return a.x*b.y-a.y*b.x;
}

/* multiplies two 2d vectors in R^2 as if they were complex numbers
 * i.e. the result is a vector with the magnitude |a||b|,
 * and an angle with the x axis that is the sum of the original angles*/
real_2 complexx(real_2 a, real_2 b)
{
    return {a.x*b.x-a.y*b.y,
            a.x*b.y+a.y*b.x};
}

real_3 complexx_xy(real_3 a, real_2 b)
{
    return {a.x*b.x-a.y*b.y,
            a.x*b.y+a.y*b.x,
            a.z};
}

real_3 complexx_xy(real_2 b, real_3 a)
{
    return {a.x*b.x-a.y*b.y,
            a.x*b.y+a.y*b.x,
            a.z};
}

quaternion operator*(quaternion a, quaternion b)
{
    return {a.r*b.r-a.i*b.i-a.j*b.j-a.k*b.k,
            a.r*b.i+a.i*b.r+a.j*b.k-a.k*b.j,
            a.r*b.j+a.j*b.r+a.k*b.i-a.i*b.k,
            a.r*b.k+a.k*b.r+a.i*b.j-a.j*b.i};
}

quaternion conjugate(quaternion q)
{
    return {q.r, -q.i, -q.j, -q.k};
}

quaternion axis_to_quaternion(real_3 axis)
{
    real half_angle = norm(axis)/2;
    if(half_angle <= 0.0001) return {1,0,0,0};
    real_3 axis_hat = normalize(axis);
    real s = sin(half_angle);
    real c = cos(half_angle);
    return {c, s*axis_hat.x, s*axis_hat.y, s*axis_hat.z};
}

quaternion get_rotation_between(real_3 old_dir, real_3 new_dir)
{
    real_3 sine = cross(new_dir, old_dir);
    real cosine = dot(old_dir, new_dir);
    real_3 sin_dir = {0,0,1};
    if(normsq(sine) > sq(0.001)) sin_dir = normalize(sine);
    real_3 sin_half = sqrt(clamp(0.5f*(1.0f-cosine), 0.0f, 1.0f))*sin_dir;
    real cos_half = sqrt(clamp(0.5f*(1.0f+cosine), 0.0f, 1.0f));

    return {cos_half, -sin_half.x, -sin_half.y, -sin_half.z};
}

real_3 apply_rotation(quaternion q, real_3 p)
{
    quaternion p_quat = {0, p.x, p.y, p.z};
    quaternion out = q*p_quat*conjugate(q);
    return {out.i, out.j, out.k};
}

real_3x3 apply_rotation(quaternion q, real_3x3 p)
{
    return {apply_rotation(q, p[0]), apply_rotation(q, p[1]), apply_rotation(q, p[2])};
}

float32 randf(uint32 * seed)
{
    union
    {
        uint32 iout;
        float32 fout;
    };

    *seed ^= *seed<<13;
    *seed ^= *seed>>17;
    *seed ^= *seed<<5;

    iout = ((*seed) >> 9) | 0x3F800000;

    return fout - 1.0;
}

uint32 randui(uint32 * seed)
{
    // *seed = ((uint64)*seed*16807 % 0xFFFFFFFFF);
    // *seed = ((uint64)*seed*48271 % 0xFFFFFFFFF);

    // *seed = ~*seed;
    *seed ^= *seed<<13;
    *seed ^= *seed>>17;
    *seed ^= *seed<<5;
    return *seed;
}

int rand(uint32 * seed, int a, int b)
{
    return a + randui(seed)%(b-a);
}

real randf(uint32 * seed, real a, real b)
{
    return a + randf(seed)*(b-a);
}

real_2 randf_2(uint32 * seed, real_2 a, real_2 b)
{
    return {randf(seed, a.x, b.x), randf(seed, a.y, b.y)};
}

real_3 randf_3(uint32 * seed, real_3 a, real_3 b)
{
    return {randf(seed, a.x, b.x), randf(seed, a.y, b.y), randf(seed, a.z, b.z)};
}

float32 rand_normal(uint32* seed)
{
    real u = randf(seed);
    if(u == 0) return 0; //TODO:figure out if there is a better way, this adds a 1/2^24 tiny bias towards 0
    return sqrt(pi/8)*log(u/(1-u));

    // static real x = 0;
    // static real y = 0;
    // static real s_factor = 0;
    // static int count = 0; //counts whether to generate a new pair or not

    // real out;
    // if(count)
    // {
    //     out = y*s_factor;
    // }
    // else
    // {
    //     real s_sq = 0;
    //     do
    //     {
    //         //TODO: can probably simd this
    //         x = 2*randf(seed)-1.0;
    //         y = 2*randf(seed)-1.0;
    //         s_sq = sq(x)+sq(y);
    //     } while(s_sq > 1.0);
    //     s_factor = sqrt(-2*log(s_sq)/s_sq);
    //     // real t = s_sq-0.5;
    //     // s_factor = 1.66511 - 4.06735*t + 5.5693*t*t - 10.6728*t*t*t;
    //     out = x*s_factor;
    // }
    // count = !count;
    // return out;
}

real_2 rand_normal_2(uint32* seed)
{
    return {rand_normal(seed), rand_normal(seed)};
}

real_3 rand_normal_3(uint32* seed)
{
    return {rand_normal(seed), rand_normal(seed), rand_normal(seed)};
}

real_3x3 point_moment(real_3 r)
{
        real_3x3 I = real_identity_3(normsq(r));
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                I[i][j] += r[i]*r[j];
        return I;
}

#endif
