#include <immintrin.h>
#include <smmintrin.h>

#pragma pack(push, 4)
struct int_2
{
    union
    {
        struct
        {
            int x;
            int y;
        };
        int data[2];
        __m64 data64;
    };

    int& operator[](int i)
    {
        return data[i];
    }
};
#pragma pack(pop)

inline int_2 operator*(int a, int_2 x)
{
    __m128i x128 = _mm_loadu_si64(&x);
    __m128i a128 = _mm_set_epi32(0, 0, a, a);
    __m128i out128 = _mm_mullo_epi32(a128, x128);
    int_2 out;
    _mm_storeu_si64(&out, out128);
    return out;
}

inline int_2 operator*(int_2 x, int a)
{
    return a*x;
}

inline int_2 operator/(int_2 x, int a)
{
    int_2 x_a;
    for(int i = 0; i < 2; i++) x_a[i] = x[i]/a;
    return x_a;
}

int_2 operator+(int_2 a, int_2 b)
{
    return (int_2) {.data64=_mm_add_pi32(a.data64, b.data64)};
}

int_2 operator-(int_2 a, int_2 b)
{
    return (int_2) {.data64=_mm_sub_pi32(a.data64, b.data64)};
}

int_2 operator-(int_2 a)
{
    return (int_2) {.data64=_mm_sub_pi32((__m64) {0}, a.data64)};
}

bool operator==(int_2 a, int_2 b)
{
    return (uint64) a.data64 == (uint64) b.data64;
}

bool operator!=(int_2 a, int_2 b)
{
    return (uint64) a.data64 != (uint64) b.data64;
}

int dot(int_2 a, int_2 b)
{
    __m128i a128 = _mm_loadu_si64(&a);
    __m128i b128 = _mm_loadu_si64(&b);
    __m128i out128 = _mm_mullo_epi32(a128, b128);
    int_2 out;
    _mm_storeu_si64(&out, out128);
    return out.x+out.y;
}

real norm(int_2 a)
{
    return sqrtf(dot(a, a));
}

int normsq(int_2 a)
{
    return (dot(a, a));
}

int_2 normalize(int_2 a)
{
    return invsqrt(dot(a, a))*a;
}

int_2 normalize_or_zero(int_2 a)
{
    int a_sq = dot(a,a);
    if(a_sq == 0) return {0};
    return invsqrt(a_sq)*a;
}

int_2 proj(int_2 a, int_2 b)
{
    int_2 bhat = normalize(b);
    return bhat*dot(a, bhat);
}

int_2 rej(int_2 a, int_2 b)
{
    return a - proj(a, b);
}

eq_op(+, int_2, int_2, int_2);
eq_op(-, int_2, int_2, int_2);
eq_op(*, int_2, int_2, int);
eq_op(/, int_2, int_2, int);
