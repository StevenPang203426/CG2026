/*
 * Matrix4f 文件说明（中文注释版）
 * - 定义 4x4 齐次变换矩阵，是三维图形中最核心的变换数据结构。
 * - 包含平移、旋转、缩放、观察矩阵（lookAt）和投影矩阵构造函数。
 * - 提供 2x2/3x3 子矩阵操作，服务于行列式与逆矩阵实现。
 * - 接口兼容 OpenGL 数据格式（列主序 + float 指针转换）。
 * - 支持透视/正交/无穷远投影，并兼顾 OpenGL 与 DirectX 深度约定。
 */

#ifndef MATRIX4F_H
#define MATRIX4F_H

#include <cstdio>

class Matrix2f;
class Matrix3f;
class Quat4f;
class Vector3f;
class Vector4f;

// 4x4 Matrix, stored in column major order (OpenGL style)
class Matrix4f
{
public:
    // Fill a 4x4 matrix with "fill".  Default to 0.
// 中文注释：函数 Matrix4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    explicit Matrix4f(float fill = 0.f);
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    Matrix4f(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33);

    // setColumns = true ==> sets the columns of the matrix to be [v0 v1 v2 v3]
    // otherwise, sets the rows
// 中文注释：函数 Matrix4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    Matrix4f(const Vector4f& v0, const Vector4f& v1, const Vector4f& v2, const Vector4f& v3, bool setColumns = true);

// 中文注释：函数 Matrix4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    Matrix4f(const Matrix4f& rm); // copy constructor
// 中文注释：重载运算符 operator =，用于该类型的算术/访问语义，保持表达式写法自然。
    Matrix4f& operator = (const Matrix4f& rm); // assignment operator
// 中文注释：重载运算符 operator/=，用于该类型的算术/访问语义，保持表达式写法自然。
    Matrix4f& operator/=(float d);
    // no destructor necessary

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    const float& operator () (int i, int j) const;
// 中文注释：重载运算符 operator()，用于该类型的算术/访问语义，保持表达式写法自然。
    float& operator () (int i, int j);

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    Vector4f getRow(int i) const;
// 中文注释：设置函数，将输入值写入当前对象对应位置。
    void setRow(int i, const Vector4f& v);

    // get column j (mod 4)
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    Vector4f getCol(int j) const;
// 中文注释：设置函数，将输入值写入当前对象对应位置。
    void setCol(int j, const Vector4f& v);

    // gets the 2x2 submatrix of this matrix to m
    // starting with upper left corner at (i0, j0)
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    Matrix2f getSubmatrix2x2(int i0, int j0) const;

    // gets the 3x3 submatrix of this matrix to m
    // starting with upper left corner at (i0, j0)
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    Matrix3f getSubmatrix3x3(int i0, int j0) const;

    // sets a 2x2 submatrix of this matrix to m
    // starting with upper left corner at (i0, j0)
// 中文注释：设置函数，将输入值写入当前对象对应位置。
    void setSubmatrix2x2(int i0, int j0, const Matrix2f& m);

    // sets a 3x3 submatrix of this matrix to m
    // starting with upper left corner at (i0, j0)
// 中文注释：设置函数，将输入值写入当前对象对应位置。
    void setSubmatrix3x3(int i0, int j0, const Matrix3f& m);

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    float determinant() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    Matrix4f inverse(bool* pbIsSingular = NULL, float epsilon = 0.f) const;

// 中文注释：代数变换函数，执行求逆、转置、共轭或映射变换。
    void transpose();
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    Matrix4f transposed() const;

    // ---- Utility ----
// 中文注释：函数 float*，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    operator float* (); // automatic type conversion for GL
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    operator const float* () const; // automatic type conversion for GL

// 中文注释：调试输出函数，将当前对象内容格式化打印到标准输出。
    void print();

// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f ones();
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f identity();
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f translation(float x, float y, float z);
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f translation(const Vector3f& rTranslation);
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f rotateX(float radians);
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f rotateY(float radians);
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f rotateZ(float radians);
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f rotation(const Vector3f& rDirection, float radians);
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f scaling(float sx, float sy, float sz);
// 中文注释：函数 uniformScaling，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    static Matrix4f uniformScaling(float s);
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f lookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up);
// 中文注释：函数 orthographicProjection，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    static Matrix4f orthographicProjection(float width, float height, float zNear, float zFar, bool directX = false);
// 中文注释：函数 orthographicProjection，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    static Matrix4f orthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar, bool directX = false);
// 中文注释：函数 perspectiveProjection，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    static Matrix4f perspectiveProjection(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar, bool directX = false);
// 中文注释：函数 perspectiveProjection，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    static Matrix4f perspectiveProjection(float fovYRadians, float aspect, float zNear, float zFar, bool directX = false);
// 中文注释：函数 infinitePerspectiveProjection，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    static Matrix4f infinitePerspectiveProjection(float fLeft, float fRight, float fBottom, float fTop, float fZNear, bool directX = false);

    // Returns the rotation matrix represented by a quaternion
    // uses a normalized version of q
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f rotation(const Quat4f& q);

    // returns an orthogonal matrix that's a uniformly distributed rotation
    // given u[i] is a uniformly distributed random number in [0,1]
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
    static Matrix4f randomRotation(float u0, float u1, float u2);

private:

    float m_elements[16];

};

// Matrix-Vector multiplication
// 4x4 * 4x1 ==> 4x1
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector4f operator * (const Matrix4f& m, const Vector4f& v);

// Matrix-Matrix multiplication
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Matrix4f operator * (const Matrix4f& x, const Matrix4f& y);

// Scalar multiplication 
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Matrix4f operator * (const Matrix4f& m, float f);
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Matrix4f operator * (float f, const Matrix4f& m);


#endif // MATRIX4F_H
