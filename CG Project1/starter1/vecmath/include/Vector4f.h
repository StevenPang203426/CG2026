/*
 * Vector4f 文件说明（中文注释版）
 * - 定义四维向量，可用于齐次坐标、颜色（RGBA）或四元数分量承载。
 * - 提供二维/三维/四维分量重排接口，适合图形计算中的快捷表达。
 * - 支持长度、归一化与 homogenize（齐次除法）操作。
 * - 提供点积与线性插值工具函数。
 * - 提供分量级与标量级运算符，便于批量数学计算。
 */

#ifndef VECTOR_4F_H
#define VECTOR_4F_H

class Vector2f;
class Vector3f;

class Vector4f
{
public:

// 中文注释：函数 Vector4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	explicit Vector4f( float f = 0.f );
// 中文注释：函数 Vector4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector4f( float fx, float fy, float fz, float fw );
// 中文注释：函数 Vector4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector4f( float buffer[ 4 ] );

// 中文注释：函数 Vector4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector4f( const Vector2f& xy, float z, float w );
// 中文注释：函数 Vector4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector4f( float x, const Vector2f& yz, float w );
// 中文注释：函数 Vector4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector4f( float x, float y, const Vector2f& zw );
// 中文注释：函数 Vector4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector4f( const Vector2f& xy, const Vector2f& zw );

// 中文注释：函数 Vector4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector4f( const Vector3f& xyz, float w );
// 中文注释：函数 Vector4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector4f( float x, const Vector3f& yzw );

	// copy constructors
// 中文注释：函数 Vector4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector4f( const Vector4f& rv );

	// assignment operators
// 中文注释：重载运算符 operator =，用于该类型的算术/访问语义，保持表达式写法自然。
	Vector4f& operator = ( const Vector4f& rv );

	// no destructor necessary

	// returns the ith element
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	const float& operator [] ( int i ) const;
// 中文注释：重载运算符 operator[]，用于该类型的算术/访问语义，保持表达式写法自然。
	float& operator [] ( int i );

// 中文注释：分量访问/重排函数，用于快速读取或组合向量分量。
	float& x();
// 中文注释：分量访问/重排函数，用于快速读取或组合向量分量。
	float& y();
// 中文注释：分量访问/重排函数，用于快速读取或组合向量分量。
	float& z();
// 中文注释：分量访问/重排函数，用于快速读取或组合向量分量。
	float& w();

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float x() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float y() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float z() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float w() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f xy() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f yz() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f zw() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f wx() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f xyz() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f yzw() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f zwx() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f wxy() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f xyw() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f yzx() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f zwy() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f wxz() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float abs() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float absSquared() const;
// 中文注释：归一化相关函数；原地版本会修改对象，非原地版本返回新对象。
	void normalize();
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector4f normalized() const;

	// if v.z != 0, v = v / v.w
// 中文注释：齐次坐标处理函数，用于执行齐次除法并返回规范化坐标。
	void homogenize();
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector4f homogenized() const;

// 中文注释：函数 negate，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	void negate();

	// ---- Utility ----
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	operator const float* () const; // automatic type conversion for OpenGL
// 中文注释：函数 float*，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	operator float* (); // automatic type conversion for OpenG
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	void print() const; 

// 中文注释：度量计算函数，返回长度/行列式/点积/叉积等标量或向量结果。
	static float dot( const Vector4f& v0, const Vector4f& v1 );
// 中文注释：插值相关函数，用于在两个或多个姿态/向量之间平滑过渡。
	static Vector4f lerp( const Vector4f& v0, const Vector4f& v1, float alpha );

private:

	float m_elements[ 4 ];

};

// component-wise operators
// 中文注释：重载运算符 operator +，用于该类型的算术/访问语义，保持表达式写法自然。
Vector4f operator + ( const Vector4f& v0, const Vector4f& v1 );
// 中文注释：重载运算符 operator -，用于该类型的算术/访问语义，保持表达式写法自然。
Vector4f operator - ( const Vector4f& v0, const Vector4f& v1 );
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector4f operator * ( const Vector4f& v0, const Vector4f& v1 );
// 中文注释：重载运算符 operator /，用于该类型的算术/访问语义，保持表达式写法自然。
Vector4f operator / ( const Vector4f& v0, const Vector4f& v1 );

// unary negation
// 中文注释：重载运算符 operator -，用于该类型的算术/访问语义，保持表达式写法自然。
Vector4f operator - ( const Vector4f& v );

// multiply and divide by scalar
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector4f operator * ( float f, const Vector4f& v );
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector4f operator * ( const Vector4f& v, float f );
// 中文注释：重载运算符 operator /，用于该类型的算术/访问语义，保持表达式写法自然。
Vector4f operator / ( const Vector4f& v, float f );

// 中文注释：重载运算符 operator ==，用于该类型的算术/访问语义，保持表达式写法自然。
bool operator == ( const Vector4f& v0, const Vector4f& v1 );
// 中文注释：重载运算符 operator !=，用于该类型的算术/访问语义，保持表达式写法自然。
bool operator != ( const Vector4f& v0, const Vector4f& v1 );

#endif // VECTOR_4F_H
