/*
 * Matrix3f 文件说明（中文注释版）
 * - 定义 3x3 矩阵，使用列主序布局，适用于法线变换与旋转缩放组合。
 * - 支持子矩阵提取与写回（2x2），用于代数余子式和逆矩阵计算。
 * - 提供 rotateX/Y/Z、任意轴旋转、缩放等静态构造接口。
 * - 支持与四元数互操作（由单位四元数生成旋转矩阵）。
 * - 重载矩阵-向量、矩阵-矩阵与标量乘法，便于图形管线中的链式计算。
 */

#ifndef MATRIX3F_H
#define MATRIX3F_H

#include <cstdio>

class Matrix2f;
class Quat4f;
class Vector3f;

// 3x3 Matrix, stored in column major order (OpenGL style)
class Matrix3f
{
public:

    // Fill a 3x3 matrix with "fill", default to 0.
// 中文注释：函数 Matrix3f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Matrix3f( float fill = 0.f );
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Matrix3f( float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22 );

	// setColumns = true ==> sets the columns of the matrix to be [v0 v1 v2]
	// otherwise, sets the rows
// 中文注释：函数 Matrix3f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Matrix3f( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, bool setColumns = true );

// 中文注释：函数 Matrix3f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Matrix3f( const Matrix3f& rm ); // copy constructor
// 中文注释：重载运算符 operator =，用于该类型的算术/访问语义，保持表达式写法自然。
	Matrix3f& operator = ( const Matrix3f& rm ); // assignment operator
	// no destructor necessary

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	const float& operator () ( int i, int j ) const;
// 中文注释：重载运算符 operator()，用于该类型的算术/访问语义，保持表达式写法自然。
	float& operator () ( int i, int j );

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f getRow( int i ) const;
// 中文注释：设置函数，将输入值写入当前对象对应位置。
	void setRow( int i, const Vector3f& v );

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f getCol( int j ) const;
// 中文注释：设置函数，将输入值写入当前对象对应位置。
	void setCol( int j, const Vector3f& v );

	// gets the 2x2 submatrix of this matrix to m
	// starting with upper left corner at (i0, j0)
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Matrix2f getSubmatrix2x2( int i0, int j0 ) const;

	// sets a 2x2 submatrix of this matrix to m
	// starting with upper left corner at (i0, j0)
// 中文注释：设置函数，将输入值写入当前对象对应位置。
	void setSubmatrix2x2( int i0, int j0, const Matrix2f& m );

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float determinant() const;
	// TODO: invert in place as well
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Matrix3f inverse( bool* pbIsSingular = NULL, float epsilon = 0.f ) const;

// 中文注释：代数变换函数，执行求逆、转置、共轭或映射变换。
	void transpose();
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Matrix3f transposed() const;

	// ---- Utility ----
// 中文注释：函数 float*，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	operator float* (); // automatic type conversion for GL
// 中文注释：调试输出函数，将当前对象内容格式化打印到标准输出。
	void print();

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	static float determinant3x3( float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22 );

// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix3f ones();
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix3f identity();
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix3f rotateX( float radians );
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix3f rotateY( float radians );
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix3f rotateZ( float radians );
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix3f scaling( float sx, float sy, float sz );
// 中文注释：函数 uniformScaling，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	static Matrix3f uniformScaling( float s );
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix3f rotation( const Vector3f& rDirection, float radians );

	// Returns the rotation matrix represented by a unit quaternion
	// if q is not normalized, it it normalized first
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix3f rotation( const Quat4f& rq );

private:

	float m_elements[ 9 ];

};

// Matrix-Vector multiplication
// 3x3 * 3x1 ==> 3x1
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector3f operator * ( const Matrix3f& m, const Vector3f& v );

// Matrix-Matrix multiplication
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Matrix3f operator * ( const Matrix3f& x, const Matrix3f& y );

// Scalar multiplication 
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Matrix3f operator * (const Matrix3f& m, float f);
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Matrix3f operator * (float f, const Matrix3f& m);


#endif // MATRIX3F_H
