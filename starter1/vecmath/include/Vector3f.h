/*
 * Vector3f 文件说明（中文注释版）
 * - 定义三维浮点向量，适用于位置、方向、法线等图形学语义。
 * - 提供点积、叉积、长度、归一化和齐次坐标相关工具。
 * - 支持二维/三维 swizzle 组合，方便构造子向量。
 * - 包含线性插值与 Catmull-Rom 三次插值，常用于路径与动画。
 * - 预置 ZERO/UP/RIGHT/FORWARD 常量，统一空间方向约定。
 */

#ifndef VECTOR_3F_H
#define VECTOR_3F_H

class Vector2f;

class Vector3f
{
public:

	static const Vector3f ZERO;
	static const Vector3f UP;
	static const Vector3f RIGHT;
	static const Vector3f FORWARD;

// 中文注释：函数 Vector3f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    explicit Vector3f( float f = 0.f );
// 中文注释：函数 Vector3f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    Vector3f( float x, float y, float z );

// 中文注释：函数 Vector3f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector3f( const Vector2f& xy, float z );
// 中文注释：函数 Vector3f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Vector3f( float x, const Vector2f& yz );

	// copy constructors
// 中文注释：函数 Vector3f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
    Vector3f( const Vector3f& rv );

	// assignment operators
// 中文注释：重载运算符 operator =，用于该类型的算术/访问语义，保持表达式写法自然。
    Vector3f& operator = ( const Vector3f& rv );

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

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float x() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float y() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float z() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f xy() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f xz() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f yz() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f xyz() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f yzx() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f zxy() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float abs() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
    float absSquared() const;

// 中文注释：归一化相关函数；原地版本会修改对象，非原地版本返回新对象。
	void normalize();
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f normalized() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector2f homogenized() const;

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
	Vector3f& operator += ( const Vector3f& v );
// 中文注释：重载运算符 operator -=，用于该类型的算术/访问语义，保持表达式写法自然。
	Vector3f& operator -= ( const Vector3f& v );
// 中文注释：重载运算符 operator *=，用于该类型的算术/访问语义，保持表达式写法自然。
  Vector3f& operator *= ( float f );
// 中文注释：重载运算符 operator /=，用于该类型的算术/访问语义，保持表达式写法自然。
  Vector3f& operator /= (float f );

// 中文注释：度量计算函数，返回长度/行列式/点积/叉积等标量或向量结果。
  static float dot( const Vector3f& v0, const Vector3f& v1 );
// 中文注释：度量计算函数，返回长度/行列式/点积/叉积等标量或向量结果。
	static Vector3f cross( const Vector3f& v0, const Vector3f& v1 );
    
    // computes the linear interpolation between v0 and v1 by alpha \in [0,1]
	// returns v0 * ( 1 - alpha ) * v1 * alpha
// 中文注释：插值相关函数，用于在两个或多个姿态/向量之间平滑过渡。
	static Vector3f lerp( const Vector3f& v0, const Vector3f& v1, float alpha );

	// computes the cubic catmull-rom interpolation between p0, p1, p2, p3
    // by t \in [0,1].  Guarantees that at t = 0, the result is p0 and
    // at p1, the result is p2.
// 中文注释：插值相关函数，用于在两个或多个姿态/向量之间平滑过渡。
	static Vector3f cubicInterpolate( const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, float t );

private:

	float m_elements[ 3 ];

};

// component-wise operators
// 中文注释：重载运算符 operator +，用于该类型的算术/访问语义，保持表达式写法自然。
Vector3f operator + ( const Vector3f& v0, const Vector3f& v1 );
// 中文注释：重载运算符 operator -，用于该类型的算术/访问语义，保持表达式写法自然。
Vector3f operator - ( const Vector3f& v0, const Vector3f& v1 );
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector3f operator * ( const Vector3f& v0, const Vector3f& v1 );
// 中文注释：重载运算符 operator /，用于该类型的算术/访问语义，保持表达式写法自然。
Vector3f operator / ( const Vector3f& v0, const Vector3f& v1 );

// unary negation
// 中文注释：重载运算符 operator -，用于该类型的算术/访问语义，保持表达式写法自然。
Vector3f operator - ( const Vector3f& v );

// multiply and divide by scalar
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector3f operator * ( float f, const Vector3f& v );
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Vector3f operator * ( const Vector3f& v, float f );
// 中文注释：重载运算符 operator /，用于该类型的算术/访问语义，保持表达式写法自然。
Vector3f operator / ( const Vector3f& v, float f );


// 中文注释：重载运算符 operator ==，用于该类型的算术/访问语义，保持表达式写法自然。
bool operator == ( const Vector3f& v0, const Vector3f& v1 );
// 中文注释：重载运算符 operator !=，用于该类型的算术/访问语义，保持表达式写法自然。
bool operator != ( const Vector3f& v0, const Vector3f& v1 );

#endif // VECTOR_3F_H
