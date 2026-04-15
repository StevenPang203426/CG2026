/*
 * Matrix2f 文件说明（中文注释版）
 * - 定义 2x2 浮点矩阵类型，底层采用列主序存储，方便直接对接 OpenGL。
 * - 提供常见线性代数操作：行列访问、行列式、逆矩阵、转置。
 * - 提供图形学常用构造：单位矩阵、全 1 矩阵、二维旋转矩阵。
 * - 通过 operator float* 可将矩阵数据直接传递到图形 API。
 * - 文件中的全局运算符负责标量/向量/矩阵乘法，保证表达式书写自然。
 */

#ifndef MATRIX2F_H
#define MATRIX2F_H

#include <cstdio>

class Vector2f;

// 2x2 Matrix, stored in column major order (OpenGL style)
class Matrix2f
{
public:

    // Fill a 2x2 matrix with "fill", default to 0.
// 中文注释：函数 Matrix2f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	explicit Matrix2f( float fill = 0.f );
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Matrix2f( float m00, float m01,
		float m10, float m11 );

	// setColumns = true ==> sets the columns of the matrix to be [v0 v1]
	// otherwise, sets the rows
// 中文注释：函数 Matrix2f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Matrix2f( const Vector2f& v0, const Vector2f& v1, bool setColumns = true );

// 中文注释：函数 Matrix2f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Matrix2f( const Matrix2f& rm ); // copy constructor
// 中文注释：重载运算符 operator =，用于该类型的算术/访问语义，保持表达式写法自然。
	Matrix2f& operator = ( const Matrix2f& rm ); // assignment operator
	// no destructor necessary

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	const float& operator () ( int i, int j ) const;
// 中文注释：重载运算符 operator()，用于该类型的算术/访问语义，保持表达式写法自然。
	float& operator () ( int i, int j );

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f getRow( int i ) const;
// 中文注释：设置函数，将输入值写入当前对象对应位置。
	void setRow( int i, const Vector2f& v );

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f getCol( int j ) const;
// 中文注释：设置函数，将输入值写入当前对象对应位置。
	void setCol( int j, const Vector2f& v );

// 中文注释：度量计算函数，返回长度/行列式/点积/叉积等标量或向量结果。
	float determinant();
// 中文注释：代数变换函数，执行求逆、转置、共轭或映射变换。
	Matrix2f inverse( bool* pbIsSingular = NULL, float epsilon = 0.f );

// 中文注释：代数变换函数，执行求逆、转置、共轭或映射变换。
	void transpose();
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Matrix2f transposed() const;

	// ---- Utility ----
// 中文注释：函数 float*，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	operator float* (); // automatic type conversion for GL
// 中文注释：调试输出函数，将当前对象内容格式化打印到标准输出。
	void print();

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	static float determinant2x2( float m00, float m01,
		float m10, float m11 );

// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix2f ones();
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix2f identity();
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Matrix2f rotation( float degrees );

private:

	float m_elements[ 4 ];

};

// Scalar-Matrix multiplication
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Matrix2f operator * ( float f, const Matrix2f& m );
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Matrix2f operator * ( const Matrix2f& m, float f );

// Matrix-Vector multiplication
// 2x2 * 2x1 ==> 2x1
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector2f operator * ( const Matrix2f& m, const Vector2f& v );

// Matrix-Matrix multiplication
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Matrix2f operator * ( const Matrix2f& x, const Matrix2f& y );

#endif // MATRIX2F_H
