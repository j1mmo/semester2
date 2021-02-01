#pragma once
#define _USE_MATH_DEFINES
#include <math.h> // remember to add _USE_MATH_DEFINES to the project settings
#include <memory.h>


class Matrix
{
public:
    static void setIdentity(float* matrix);

    static void multiplyMatrix(float* result, const float* lhs, const float* rhs);

    static void translate(float* matrix, const float tx, const float ty, const float tz);

    static void scale(float* matrix, const float sx, const float sy, const float sz);

    static void rotateX(float* matrix, const float degs);

    static void rotateY(float* matrix, const float degs);

    static void rotateZ(float* matrix, const float degs);

    static void setOrtho(float* matrix, float left, float right, float bottom, float top, float near, float far);

    static void setFrustum(float* matrix, float left, float right, float bottom, float top, float _near, float _far);

    static void setLookAt(float* matrix, float* eye, float* centre, float* up);
    
    static void normaliseVec3(float* vector);
    
    static void crossProductVec3(float* product,float* vec1, float* vec2);
    
    static float dotProductVec3(float* vec1, float* vec2);

    static float degreesToRadians(const float degrees);
};

inline void Matrix::setFrustum(float* matrix, float left, float right, float bottom, float top, float _near, float _far)
{
    memset(matrix, 0, sizeof(float) * 16);
    matrix[0] = (2 * _near) / (right - left);
    matrix[5] = (2 * _near) / (top - bottom);
    matrix[8] = (right + left) / (right - left);
    matrix[9] = (top + bottom) / (top - bottom);
    matrix[10] = (-(_far + _near) / (_far - _near));
    matrix[11] = -1;
    matrix[14] = (-2 * _far * _near) / (_far - _near);
}

inline void Matrix::setLookAt(float* matrix, float* eye, float* centre, float* up)
{
    setIdentity(matrix);
    float F[] = {0,0,0}, S[] = { 0,0,0 }, U[] = {0,0,0}, T[] = { 0,0,0 };

    T[0] = up[0];
    T[1] = up[1];
    T[2] = up[2];

    F[0] = centre[0] - eye[0];
    F[1] = centre[1] - eye[1];
    F[2] = centre[2] - eye[2];

    normaliseVec3(F);
    normaliseVec3(T);
    
    crossProductVec3(F, T, S);
    normaliseVec3(S);
    crossProductVec3(S, F, U);
    normaliseVec3(U);

    matrix[0] = S[0];
    matrix[4] = S[1];
    matrix[8] = S[2];

    matrix[1] = U[0];
    matrix[5] = U[1];
    matrix[9] = U[2];

    matrix[2] = -F[0];
    matrix[6] = -F[1];
    matrix[10] = -F[2];

    S[0] *= -1;
    S[1] *= -1;
    S[2] *= -1;

    U[0] *= -1;
    U[1] *= -1;
    U[2] *= -1;
    
    matrix[12] = dotProductVec3(S, eye);
    matrix[13] = dotProductVec3(U, eye);
    matrix[14] = dotProductVec3(F, eye);
}

inline float Matrix::dotProductVec3(float* vec1, float* vec2)
{
    return (vec1[0] * vec2[0]) + (vec1[1] * vec2[1]) + (vec1[2] * vec2[2]);
}
inline void Matrix::crossProductVec3(float* a, float* b, float* product)
{
    product[0] = a[1] * b[2] - a[2] * b[1];
    product[1] = a[2] * b[0] - a[0] * b[2];
    product[2] = a[0] * b[1] - a[1] * b[0];
}
inline void Matrix::normaliseVec3(float* a)
{
    float magnitude = sqrtf((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]));
    a[0] /= magnitude;
    a[1] /= magnitude;
    a[2] /= magnitude;
}
inline float Matrix::degreesToRadians(const float degrees)
{
    return degrees * (float)(M_PI / 180.0);
}

inline void Matrix::setIdentity(float* matrix)
{
    memset(matrix, 0, sizeof(float) * 16);
    matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1;
}

inline void Matrix::multiplyMatrix(float* result, const float* lhs, const float* rhs)
{
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result[row + 4 * col] = 0;
            for (int traverse = 0; traverse < 4; traverse++) {
                result[row + 4 * col] += lhs[row + 4 * traverse] * rhs[traverse + 4 * col];
            }
        }
    }
}

inline void Matrix::translate(float* matrix, const float tx, const float ty, const float tz)
{
    matrix[12] += (matrix[0] * tx) + (matrix[4] * ty) + (matrix[8] * tz);
    matrix[13] += (matrix[1] * tx) + (matrix[5] * ty) + (matrix[9] * tz);
    matrix[14] += (matrix[2] * tx) + (matrix[6] * ty) + (matrix[10] * tz);
    matrix[15] += (matrix[3] * tx) + (matrix[7] * ty) + (matrix[11] * tz);
}

inline void Matrix::scale(float* matrix, const float sx, const float sy, const float sz)
{
    for (size_t i = 0; i < 4; i++)
    {
        matrix[i] *= sx;
        matrix[i + 4] *= sy;
        matrix[i + 8] *= sz;
    }
}

inline void Matrix::rotateX(float* matrix, const float degs)
{
    const float
        radians = degreesToRadians(degs),
        cos_theta = (float)cos(radians),
        sin_theta = (float)sin(radians);
    float original[8];

    for (size_t i = 0; i < 4; i++)
    {
        original[i] = matrix[i + 4];
        original[i + 4] = matrix[i + 8];
    }

    for (size_t i = 0; i < 4; i++)
    {
        matrix[i + 4] = original[i] * cos_theta - original[i + 4] * (sin_theta);
        matrix[i + 8] = original[i] * sin_theta + original[i + 4] * cos_theta;
    }
}

inline void Matrix::rotateY(float* matrix, const float degs)
{
    const float
        radians = degreesToRadians(degs),
        cos_theta = cos(radians),
        sin_theta = sin(radians);
    float original[8];

    for (size_t i = 0; i < 4; i++)
    {
        original[i] = matrix[i];
        original[i + 4] = matrix[i + 8];
    }
    for (size_t i = 0; i < 4; i++)
    {
        matrix[i] = original[i] * cos_theta + original[i + 4] * sin_theta;
        matrix[i + 8] = original[i + 4] * cos_theta - original[i] * sin_theta;
    }
}

inline void Matrix::rotateZ(float* matrix, const float degs)
{
    const float
        radians = degreesToRadians(degs),
        cos_theta = cos(radians),
        sin_theta = sin(radians);
    float original[8];

    for (size_t i = 0; i < 8; i++)
    {
        original[i] = matrix[i];
    }
    for (size_t i = 0; i < 4; i++)
    {
        matrix[i] = original[i] * cos_theta + original[i + 4] * sin_theta;
        matrix[i + 4] = original[i + 4] * cos_theta - original[i] * sin_theta;
    }
}

inline void Matrix::setOrtho(float* matrix, float left, float right, float bottom, float top, float _near, float _far)
{
    memset(matrix, 0, sizeof(float) * 16);
    matrix[0] = 2 / (right - left);
    matrix[5] = 2 / (top - bottom);
    matrix[10] = -2 / (_far - _near);
    matrix[12] = -((right + left) / (right - left));
    matrix[13] = -((top + bottom) / (top - bottom));
    matrix[14] = -((_far + _near) / (_far - _near));
    matrix[15] = 1;
}
