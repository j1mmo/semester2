#pragma once
#include "Matrix.h"
#include <assert.h>

struct LIGHT_HANDLES
{
    int direction, halfplane, ambient, diffuse, specular;
};

struct MATERIAL_HANDLES
{
    int ambient, diffuse, specular, shininess;
};



class RenderingContext
{
public:
    RenderingContext();

    void initModelMatrix(bool resetdepth);
    void pushModelMatrix();
    void popModelMatrix();

    void updateMVPs();

    // These all assume the transformation happens to the top-most matrix in the stack (model view)
    void translate(const float x, const float y, const float z);
    void scale(const float x, const float y, const float z);
    void rotateX(const float degs);
    void rotateY(const float degs);
    void rotateZ(const float degs);
    //void changeShader(int index);

    static const int
    MAX_MODEL_DEPTH = 32;
    int glprogram,
        poshandle,
        normalhandle,
        mvhandle,
        mvphandle;

    float viewmatrix[16], projectionmatrix[16],
        mvmatrix[16], mvpmatrix[16];

    LIGHT_HANDLES lighthandles;
    MATERIAL_HANDLES materialhandles;
private:
    int depth;
    float modelmatrix[16 * MAX_MODEL_DEPTH];
};

// This constructor is done for you
inline RenderingContext::RenderingContext() :
    depth(0), glprogram(-1), poshandle(-1), mvphandle(-1)
{
    Matrix::setIdentity(viewmatrix);
    Matrix::setIdentity(projectionmatrix);
    Matrix::setIdentity(modelmatrix);
    Matrix::setIdentity(mvmatrix);
    Matrix::setIdentity(mvpmatrix);
}

inline void RenderingContext::initModelMatrix(bool resetdepth)
{
    if (resetdepth)
        depth = 0;
    Matrix::setIdentity(modelmatrix + depth);
}

inline void RenderingContext::pushModelMatrix()
{
    assert((depth / 16) < (MAX_MODEL_DEPTH - 1));
    memcpy(modelmatrix + depth + 16, modelmatrix + depth, sizeof(float) * 16);
    depth += 16;
}

inline void RenderingContext::popModelMatrix()
{
    assert(depth > 0);
    depth -= 16;
}

inline void RenderingContext::translate(const float x, const float y, const float z)
{
    Matrix::translate(modelmatrix + depth, x, y, z);
}

inline void RenderingContext::scale(const float x, const float y, const float z)
{
    Matrix::scale(modelmatrix + depth, x, y, z);
}

inline void RenderingContext::rotateX(const float degs)
{
    Matrix::rotateX(modelmatrix + depth, degs);
}

inline void RenderingContext::rotateY(const float degs)
{
    Matrix::rotateY(modelmatrix + depth, degs);
}

inline void RenderingContext::rotateZ(const float degs)
{
    Matrix::rotateZ(modelmatrix + depth, degs);
}

inline void RenderingContext::updateMVPs()
{
    Matrix::multiplyMatrix(mvmatrix, viewmatrix, modelmatrix + depth);
    Matrix::multiplyMatrix(mvpmatrix, projectionmatrix, mvmatrix);
}

