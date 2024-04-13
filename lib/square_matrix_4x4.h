//testing compile time of this vs generic_square_matrix.h
struct real_4x4
{
    union
    {
        struct
        {
            real_4 columns[4];
        };
        real data[4*4];
    };

    real_4& operator[](int i)
    {
        return columns[i];
    }
};

real_4x4 identity4(int lambda)
{
    real_4x4 Lambda = {};
    for(int i = 0; i < 4; i++) Lambda[i][i] = lambda;
    return Lambda;
}

real_4x4 operator*(real c, real_4x4 A)
{
    real_4x4 cA;
    for(int i = 0; i < 4*4; i++) cA.data[i] = c*A.data[i];
    return cA;
}

real_4x4 operator*(real_4x4 A, real c)
{
    return c*A;
}

real_4x4 operator/(real_4x4 A, real c)
{
    return (1.0/c)*A;
}

real_4x4 operator+(real_4x4 A, real_4x4 B)
{
    real_4x4 sum;
    for(int i = 0; i < 4; i++) sum[i] = A[i]+B[i];
    return sum;
}

real_4x4 operator-(real_4x4 A, real_4x4 B)
{
    real_4x4 sum;
    for(int i = 0; i < 4; i++) sum[i] = A[i]-B[i];
    return sum;
}

real_4x4 transpose(real_4x4 A)
{
    real_4x4 At;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            At[j][i] = A[i][j];
    return At;
}

real_4 operator*(real_4x4 A, real_4 x)
{
    real_4 result = {};
    for(int i = 0; i < 4; i++) result += A[i]*x[i];
    return result;
}

real_4 operator*(real_4 x, real_4x4 A)
{
    real_4x4 At = transpose(A);
    return At*x;
}

//TODO: this stuff is computationally efficient, but it's technically some wierd transpose then multiply thing, maybe rename these to reflect this
real_4x4 operator*(real_4x4 A, real_4x4 B)
{
    real_4x4 product;
    for(int i = 0; i < 4; i++) product[i] = A*B[i];
    return product;
}

eq_op(+, real_4x4, real_4x4, real_4x4);
eq_op(-, real_4x4, real_4x4, real_4x4);
eq_op(*, real_4x4, real_4x4, real_4x4);
eq_op(*, real_4x4, real_4x4, real);
eq_op(/, real_4x4, real_4x4, real);

real tr(real_4x4 A)
{
    real trace = 0;
    for(int i = 0; i < 4; i++) trace += A[i][i];
    return trace;
}

real det(real_4x4 A)
{
    real numerator = 1;
    real denominator = 1;

    //use column operations to make A an upper triagular matrix
    for(int i = 0; i < 4-1; i++)
        for(int j = i+1; j < 4; j++)
        {
            real sub_scale = A[j][i];
            A[j] *= A[i][i];
            denominator *= A[i][i];
            A[j] -= sub_scale*A[i];
        }

    //take the determinant of the upper tringular matrix
    for(int i = 0; i < 4; i++) numerator *= A[i][i];

    return numerator/denominator;
}

//find the inverse of a matrix using Gauss-Jordan elimination
//TODO: this is probably not the most numerically stable matrix inversion algorithm
real_4x4 inverse(real_4x4 A)
{
    real_4x4 Ai = identity4(1);

    //reduce to upper triagular matrix
    for(int i = 0; i < 4-1; i++)
        for(int j = i+1; j < 4; j++)
        {
            real sub_scale = A[j][i];
            A[j] *= A[i][i];
            Ai[j] *= A[i][i];

            A[j] -= sub_scale*A[i];
            Ai[j] -= sub_scale*Ai[i];
        }

    //reduce to diagional matrix
    for(int i = 4-1; i > 0; i--)
        for(int j = i-1; j >= 0; j--)
        {
            real sub_scale = A[j][i];
            A[j] *= A[i][i];
            Ai[j] *= A[i][i];

            A[j] -= sub_scale*A[i];
            Ai[j] -= sub_scale*Ai[i];
        }

    //reduce to identity matrix
    for(int i = 0; i < 4; i++)
    {
        //we don't need to update A anymore, since we will just throw it out after this step
        Ai[i] /= A[i][i];
    }

    return Ai;
}
