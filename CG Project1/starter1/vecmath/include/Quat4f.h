/*
 * Quat4f 文件说明（中文注释版）
 * - 定义四元数类型 q = w + x*i + y*j + z*k，用于表达三维旋转。
 * - 提供归一化、共轭、求逆、对数映射与指数映射等核心运算。
 * - 支持轴角与旋转矩阵之间的互相转换。
 * - 提供 lerp/slerp/squad/cubicInterpolate 等插值方法，适合动画过渡。
 * - 提供随机旋转构造，便于采样与蒙特卡洛相关应用。
 */

#ifndef QUAT4F_H
#define QUAT4F_H

class Vector3f;
class Vector4f;

#include "Matrix3f.h"

class Quat4f
{
public:

	static const Quat4f ZERO;
	static const Quat4f IDENTITY;

// 中文注释：函数 Quat4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Quat4f();

	// q = w + x * i + y * j + z * k
// 中文注释：函数 Quat4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Quat4f( float w, float x, float y, float z );
		
// 中文注释：函数 Quat4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Quat4f( const Quat4f& rq ); // copy constructor
// 中文注释：重载运算符 operator =，用于该类型的算术/访问语义，保持表达式写法自然。
	Quat4f& operator = ( const Quat4f& rq ); // assignment operator
	// no destructor necessary

	// returns a quaternion with 0 real part
// 中文注释：函数 Quat4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Quat4f( const Vector3f& v );

	// copies the components of a Vector4f directly into this quaternion
// 中文注释：函数 Quat4f，用于完成该数学类型的构造、访问或运算逻辑；参数与返回值语义见签名。
	Quat4f( const Vector4f& v );

	// returns the ith element
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	const float& operator [] ( int i ) const;
// 中文注释：重载运算符 operator[]，用于该类型的算术/访问语义，保持表达式写法自然。
	float& operator [] ( int i );

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float w() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float x() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float y() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float z() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector3f xyz() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Vector4f wxyz() const;

// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float abs() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	float absSquared() const;
// 中文注释：归一化相关函数；原地版本会修改对象，非原地版本返回新对象。
	void normalize();
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Quat4f normalized() const;

// 中文注释：代数变换函数，执行求逆、转置、共轭或映射变换。
	void conjugate();
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Quat4f conjugated() const;

// 中文注释：代数变换函数，执行求逆、转置、共轭或映射变换。
	void invert();
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Quat4f inverse() const;

	// log and exponential maps
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Quat4f log() const;
// 中文注释：该函数用于当前数学类型的接口操作（构造、访问、运算或工具）；具体参数和返回语义以函数签名为准。
	Quat4f exp() const;
	
	// returns unit vector for rotation and radians about the unit vector
// 中文注释：获取函数，返回当前对象中的目标分量、行列或子结构。
	Vector3f getAxisAngle( float* radiansOut );

	// sets this quaternion to be a rotation of fRadians about v = < fx, fy, fz >, v need not necessarily be unit length
// 中文注释：设置函数，将输入值写入当前对象对应位置。
	void setAxisAngle( float radians, const Vector3f& axis );

	// ---- Utility ----
// 中文注释：调试输出函数，将当前对象内容格式化打印到标准输出。
	void print();
 
	 // quaternion dot product (a la vector)
// 中文注释：度量计算函数，返回长度/行列式/点积/叉积等标量或向量结果。
	static float dot( const Quat4f& q0, const Quat4f& q1 );	
	
	// linear (stupid) interpolation
// 中文注释：插值相关函数，用于在两个或多个姿态/向量之间平滑过渡。
	static Quat4f lerp( const Quat4f& q0, const Quat4f& q1, float alpha );

	// spherical linear interpolation
// 中文注释：插值相关函数，用于在两个或多个姿态/向量之间平滑过渡。
	static Quat4f slerp( const Quat4f& a, const Quat4f& b, float t, bool allowFlip = true );
	
	// spherical quadratic interoplation between a and b at point t
	// given quaternion tangents tanA and tanB (can be computed using squadTangent)	
// 中文注释：插值相关函数，用于在两个或多个姿态/向量之间平滑过渡。
	static Quat4f squad( const Quat4f& a, const Quat4f& tanA, const Quat4f& tanB, const Quat4f& b, float t );

// 中文注释：插值相关函数，用于在两个或多个姿态/向量之间平滑过渡。
	static Quat4f cubicInterpolate( const Quat4f& q0, const Quat4f& q1, const Quat4f& q2, const Quat4f& q3, float t );

	// Log-difference between a and b, used for squadTangent
	// returns log( a^-1 b )	
// 中文注释：代数变换函数，执行求逆、转置、共轭或映射变换。
	static Quat4f logDifference( const Quat4f& a, const Quat4f& b );

	// Computes a tangent at center, defined by the before and after quaternions
	// Useful for squad()
// 中文注释：插值相关函数，用于在两个或多个姿态/向量之间平滑过渡。
	static Quat4f squadTangent( const Quat4f& before, const Quat4f& center, const Quat4f& after );		

// 中文注释：旋转表示转换函数，在轴角、矩阵与四元数之间进行转换。
	static Quat4f fromRotationMatrix( const Matrix3f& m );

// 中文注释：旋转表示转换函数，在轴角、矩阵与四元数之间进行转换。
	static Quat4f fromRotatedBasis( const Vector3f& x, const Vector3f& y, const Vector3f& z );

	// returns a unit quaternion that's a uniformly distributed rotation
	// given u[i] is a uniformly distributed random number in [0,1]
	// taken from Graphics Gems II
// 中文注释：几何构造函数，用于创建旋转/平移/缩放/投影等变换矩阵或姿态。
	static Quat4f randomRotation( float u0, float u1, float u2 );

private:

	float m_elements[ 4 ];

};

// 中文注释：重载运算符 operator +，用于该类型的算术/访问语义，保持表达式写法自然。
Quat4f operator + ( const Quat4f& q0, const Quat4f& q1 );
// 中文注释：重载运算符 operator -，用于该类型的算术/访问语义，保持表达式写法自然。
Quat4f operator - ( const Quat4f& q0, const Quat4f& q1 );
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Quat4f operator * ( const Quat4f& q0, const Quat4f& q1 );
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Quat4f operator * ( float f, const Quat4f& q );
// 中文注释：重载运算符 operator *，用于该类型的算术/访问语义，保持表达式写法自然。
Quat4f operator * ( const Quat4f& q, float f );

#endif // QUAT4F_H
