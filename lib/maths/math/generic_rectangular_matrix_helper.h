#define type_R CONCAT(CONCAT(type, _), R)
#define type_C CONCAT(CONCAT(type, _), C)

#ifndef type_RxC
#define type_RxC CONCAT(CONCAT(CONCAT(type, _), R), CONCAT(x, C))
#endif
#ifndef type_CxR
#define type_CxR CONCAT(CONCAT(CONCAT(type, _), C), CONCAT(x, R))
#endif

type_RxC operator*(type c, type_RxC A)
{
    type_RxC cA;
    for(int i = 0; i < R*C; i++) cA.data[i] = c*A.data[i];
    return cA;
}

type_RxC operator*(type_RxC A, type c)
{
    return c*A;
}

type_RxC operator/(type_RxC A, type c)
{
    return (1.0/c)*A;
}

type_RxC operator+(type_RxC A, type_RxC B)
{
    type_RxC sum;
    for(int i = 0; i < R; i++) sum[i] = A[i]+B[i];
    return sum;
}

type_RxC operator-(type_RxC A, type_RxC B)
{
    type_RxC sum;
    for(int i = 0; i < R; i++) sum[i] = A[i]-B[i];
    return sum;
}

eq_op(+, type_RxC, type_RxC, type_RxC);
eq_op(-, type_RxC, type_RxC, type_RxC);
eq_op(*, type_RxC, type_RxC, type);
eq_op(/, type_RxC, type_RxC, type);

type_CxR transpose(type_RxC A)
{
    type_CxR At;
    for(int i = 0; i < C; i++)
        for(int j = 0; j < R; j++)
            At[j][i] = A[i][j];
    return At;
}

type_R operator*(type_RxC A, type_C x)
{
    type_R result = {};
    for(int i = 0; i < C; i++) result += A[i]*x[i];
    return result;
}

#undef type_R
#undef type_C
#undef type_RxC
#undef type_CxR
#undef R
#undef C
