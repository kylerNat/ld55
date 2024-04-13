/* including this file defines matrices of type type and size NxM or MxN,
 */

/*TODO: as needed, diagonalization/jordanlization, exponential, finding eigenvectors, any other operations*/

#define type_N CONCAT(CONCAT(type, _), N)
#define type_M CONCAT(CONCAT(type, _), M)

#ifndef type_NxM
#define type_NxM CONCAT(CONCAT(CONCAT(type, _), N), CONCAT(x, M))
#endif
#ifndef type_MxN
#define type_MxN CONCAT(CONCAT(CONCAT(type, _), M), CONCAT(x, N))
#endif
#define type_NxN CONCAT(CONCAT(CONCAT(type, _), N), CONCAT(x, N))
#define type_MxM CONCAT(CONCAT(CONCAT(type, _), M), CONCAT(x, M))

struct type_NxM
{
    union
    {
        struct
        {
            type_N columns[M];
        };
        type data[N*M];
    };

    type_N& operator[](int i)
    {
        return columns[i];
    }
};

struct type_MxN
{
    union
    {
        struct
        {
            type_M columns[N];
        };
        type data[M*N];
    };

    type_M& operator[](int i)
    {
        return columns[i];
    }
};

#define R N
#define C M
#include "generic_rectangular_matrix_helper.h"

#define R M
#define C N
#include "generic_rectangular_matrix_helper.h"

type_M operator*(type_N x, type_NxM A)
{
    type_MxN At = transpose(A);
    return At*x;
}

type_N operator*(type_M x, type_MxN A)
{
    type_NxM At = transpose(A);
    return At*x;
}

type_NxN operator*(type_NxM A, type_MxN B)
{
    type_NxN product;
    for(int i = 0; i < N; i++) product[i] = A*B[i];
    return product;
}

type_MxM operator*(type_MxN A, type_NxM B)
{
    type_MxM product;
    for(int i = 0; i < M; i++) product[i] = A*B[i];
    return product;
}

#undef type_NxM
#undef type_MxN
#undef type_N
#undef type
#undef N
#undef M
