/* including this file defines matrices of type type and size NxN,
 */

/*TODO: as needed, diagonalization/jordanlization, exponential, finding eigenvectors, any other operations*/

#ifndef type_N
#define type_N CONCAT(CONCAT(type, _), N)
#endif

#ifndef type_NxN
#define type_NxN CONCAT(CONCAT(CONCAT(type, _), N), CONCAT(x, N))
#endif

struct type_NxN
{
    union
    {
        struct
        {
            type_N columns[N];
        };
        type data[N*N];
    };

    type_N& operator[](int i)
    {
        return columns[i];
    }
};

#define type_identity_N CONCAT(CONCAT(type, _identity_), N)
type_NxN type_identity_N(type lambda)
{
    type_NxN Lambda = {};
    for(int i = 0; i < N; i++) Lambda[i][i] = lambda;
    return Lambda;
}

type_NxN operator*(type c, type_NxN A)
{
    type_NxN cA;
    for(int i = 0; i < N*N; i++) cA.data[i] = c*A.data[i];
    return cA;
}

type_NxN operator*(type_NxN A, type c)
{
    return c*A;
}

type_NxN operator/(type_NxN A, type c)
{
    return (1.0/c)*A;
}

#ifdef COMPLEX_TYPE
type_NxN operator*(real c, type_NxN A)
{
    type_NxN cA;
    for(int i = 0; i < N*N; i++) cA.data[i] = c*A.data[i];
    return cA;
}

type_NxN operator*(type_NxN A, real c)
{
    return c*A;
}

type_NxN operator/(type_NxN A, real c)
{
    return (1.0/c)*A;
}

eq_op(*, type_NxN, type_NxN, real);
eq_op(/, type_NxN, type_NxN, real);
#endif

type_NxN operator+(type_NxN A, type_NxN B)
{
    type_NxN sum;
    for(int i = 0; i < N; i++) sum[i] = A[i]+B[i];
    return sum;
}

type_NxN operator-(type_NxN A, type_NxN B)
{
    type_NxN sum;
    for(int i = 0; i < N; i++) sum[i] = A[i]-B[i];
    return sum;
}

type_NxN transpose(type_NxN A)
{
    type_NxN At;
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            At[j][i] = A[i][j];
    return At;
}

type_N operator*(type_NxN A, type_N x)
{
    type_N result = {};
    for(int i = 0; i < N; i++) result += A[i]*x[i];
    return result;
}

type_N operator*(type_N x, type_NxN A)
{
    type_NxN At = transpose(A);
    return At*x;
}

type_NxN operator*(type_NxN A, type_NxN B)
{
    type_NxN product;
    for(int i = 0; i < N; i++) product[i] = A*B[i];
    return product;
}

eq_op(+, type_NxN, type_NxN, type_NxN);
eq_op(-, type_NxN, type_NxN, type_NxN);
eq_op(*, type_NxN, type_NxN, type_NxN);
eq_op(*, type_NxN, type_NxN, type);
eq_op(/, type_NxN, type_NxN, type);

type_NxN outer_product(type_N a, type_N b)
{
    type_NxN product;
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            product[i][j] = a[j]*b[i];
    return product;
}


type tr(type_NxN A)
{
    type trace = {0};
    for(int i = 0; i < N; i++) trace += A[i][i];
    return trace;
}

type det(type_NxN A)
{
    type numerator = {1};
    type denominator = {1};

    //use column operations to make A an upper triagular matrix
    for(int i = 0; i < N-1; i++)
        for(int j = i+1; j < N; j++)
        {
            type sub_scale = A[j][i];
            A[j] *= A[i][i];
            denominator *= A[i][i];
            A[j] -= sub_scale*A[i];
        }

    //take the determinant of the upper tringular matrix
    for(int i = 0; i < N; i++) numerator *= A[i][i];

    return numerator/denominator;
}

#ifndef COMPLEX_TYPE
type_NxN abs(type_NxN a)
{
    type_NxN out;
    for(int i = 0; i < N*N; i++)
        out.data[i] = abs(a.data[i]);
    return out;
}
#endif

#if N == 2
type_NxN inverse(type_NxN A)
{
    return (1.0/det(A))*((type_NxN){
            A[1][1], -A[0][1],
            -A[1][0], A[0][0]
        });
}
#elif N == 3
type_NxN inverse(type_NxN A)
{
    return (1.0/det(A))*((type_NxN){
            A[1][1]*A[2][2]-A[2][1]*A[1][2], A[2][1]*A[0][2]-A[0][1]*A[2][2], A[0][1]*A[1][2]-A[1][1]*A[0][2],
            A[2][0]*A[1][2]-A[1][0]*A[2][2], A[0][0]*A[2][2]-A[2][0]*A[0][2], A[1][0]*A[0][2]-A[0][0]*A[1][2],
            A[1][0]*A[2][1]-A[2][0]*A[1][1], A[2][0]*A[0][1]-A[0][0]*A[2][1], A[0][0]*A[1][1]-A[1][0]*A[0][1],
        });
}
#else
//find the inverse of a matrix using Gauss-Jordan elimination
//TODO: this is probably not the most numerically stable matrix inversion algorithm
type_NxN inverse(type_NxN A)
{
    type_NxN Ai = type_identity_N({1});

    //reduce to upper triagular matrix
    for(int i = 0; i <= N-1; i++)
    {
        Ai[i] /= A[i][i];
        A[i] /= A[i][i];
        for(int j = i+1; j < N; j++)
        {
            type sub_scale = A[j][i];

            A[j] -= sub_scale*A[i];
            Ai[j] -= sub_scale*Ai[i];
        }
    }

    //reduce to diagional matrix
    for(int i = N-1; i > 0; i--)
        for(int j = i-1; j >= 0; j--)
        {
            type sub_scale = A[j][i];

            A[j] -= sub_scale*A[i];
            Ai[j] -= sub_scale*Ai[i];
        }

    return Ai;
}
#endif

// //find the inverse of a matrix using Gauss-Jordan elimination
// //TODO: this is probably not the most numerically stable matrix inversion algorithm
// type_NxN inverse(type_NxN A)
// {
//     type_NxN Ai = type_identity_N({1});

//     //reduce to upper triagular matrix
//     for(int i = 0; i < N-1; i++)
//         for(int j = i+1; j < N; j++)
//         {
//             type sub_scale = A[j][i];
//             A[j] *= A[i][i];
//             Ai[j] *= A[i][i];

//             A[j] -= sub_scale*A[i];
//             Ai[j] -= sub_scale*Ai[i];
//         }

//     //reduce to diagional matrix
//     for(int i = N-1; i > 0; i--)
//         for(int j = i-1; j >= 0; j--)
//         {
//             type sub_scale = A[j][i];
//             A[j] *= A[i][i];
//             Ai[j] *= A[i][i];

//             A[j] -= sub_scale*A[i];
//             Ai[j] -= sub_scale*Ai[i];
//         }

//     //reduce to identity matrix
//     for(int i = 0; i < N; i++)
//     {
//         //we don't need to update A anymore, since we will just throw it out after this step
//         Ai[i] /= A[i][i];
//     }

//     return Ai;
// }

#undef type_identity_N
#undef type_NxN
#undef type_N
#undef type
#undef N
#undef COMPLEX_TYPE
