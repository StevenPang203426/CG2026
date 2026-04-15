/*
 * Vector2f 文件说明（中文注释版）
 * - 定义二维浮点向量，覆盖基础代数运算与几何工具函数。
 * - 提供长度、归一化、点积、二维法向（normal）与二维叉积扩展接口。
 * - 支持 swizzle 访问（xy/yx/xx/yy），便于快速重组分量。
 * - 暴露 OpenGL 兼容指针转换，减少渲染调用中的数据拷贝。
 * - 包含常用常量 ZERO/UP/RIGHT，用于提高调用可读性。
 */

#ifndef VECTOR_2F_H
#define VECTOR_2F_H

#include <cmath>

class Vector3f;

class Vector2f
{
public:
    
    static const Vector2f ZERO;
	static const Vector2f UP;
	static const Vector2f RIGHT;

// 中文注释：函数 Vector2f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    explicit Vector2f( float f = 0.f );
// 中文注释：函数 Vector2f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    Vector2f( float x, float y );

	// copy constructors
// 中文注释：函数 Vector2f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    Vector2f( const Vector2f& rv );

	// assignment operators
// 中文注释：重载运算符 operator =，用于该类型的算术/访问语义，保持表达式写法自然。
	Vector2f& operator = ( const Vector2f& rv );

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

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float x() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float y() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    Vector2f xy() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f yx() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f xx() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f yy() const;

	// returns ( -y, x )
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    Vector2f normal() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    float abs() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    float absSquared() const;
// 中文注释：归一化相关函数；原地版本会修改对象，非原地版本返回新对象。
    void normalize();
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    Vector2f normalized() const;

// 中文注释：函数 negate，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    void negate();

	// ---- Utility ----
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    operator const float* () const; // automatic type conversion for OpenGL 
// 中文注释：函数 float*，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    operator float* (); // automatic type conversion for OpenGL 
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	void print() const;

// 中文注释：重载运算符 operator +=，用于该类型的算术/访问语义，保持表达式写法自然。
	Vector2f& operator += ( const Vector2f& v );
// 中文注释：重载运算符 operator -=，用于该类型的算术/访问语义，保持表达式写法自然。
	Vector2f& operator -= ( const Vector2f& v );
// 中文注释：重载运算符 operator *=，用于该类型的算术/访问语义，保持表达式写法自然。
	Vector2f& operator *= ( float f );

// 中文注释：度量计算函数，返回长度/行列式/点积/叉积等标量或向量结果。
    static float dot( const Vector2f& v0, const Vector2f& v1 );

// 中文注释：度量计算函数，返回长度/行列式/点积/叉积等标量或向量结果。
	static Vector3f cross( const Vector2f& v0, const Vector2f& v1 );

	// returns v0 * ( 1 - alpha ) * v1 * alpha
// 中文注释：插值相关函数，用于在两个或多个姿态/向量之间平滑过渡。
	static Vector2f lerp( const Vector2f& v0, const Vector2f& v1, float alpha );

private:

	float m_elements[2];

};

// component-wise operators
// 中文注释：重载运算符 operator +，用于该类型的算术/访问语义，保持表达式写法自然。
Vector2f operator + ( const Vector2f& v0, const Vector2f& v1 );
// 中文注释：重载运算符 operator -，用于该类型的算术/访问语义，保持表达式写法自然。
Vector2f operator - ( const Vector2f& v0, const Vector2f& v1 );
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector2f operator * ( const Vector2f& v0, const Vector2f& v1 );
// 中文注释：重载运算符 operator /，用于该类型的算术/访问语义，保持表达式写法自然。
Vector2f operator / ( const Vector2f& v0, const Vector2f& v1 );

// unary negation
// 中文注释：重载运算符 operator -，用于该类型的算术/访问语义，保持表达式写法自然。
Vector2f operator - ( const Vector2f& v );

// multiply and divide by scalar
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector2f operator * ( float f, const Vector2f& v );
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector2f operator * ( const Vector2f& v, float f );
// 中文注释：重载运算符 operator /，用于该类型的算术/访问语义，保持表达式写法自然。
Vector2f operator / ( const Vector2f& v, float f );

// 中文注释：重载运算符 operator ==，用于该类型的算术/访问语义，保持表达式写法自然。
bool operator == ( const Vector2f& v0, const Vector2f& v1 );
// 中文注释：重载运算符 operator !=，用于该类型的算术/访问语义，保持表达式写法自然。
bool operator != ( const Vector2f& v0, const Vector2f& v1 );

#endif // VECTOR_2F_H
